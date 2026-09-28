"""Register and flag liveness after every call site.

For each return address (the instruction after a JSR/BSR) this computes which
registers and condition flags the caller may read before it overwrites them.
SHADOW mode compares only those outputs of a recreated routine; everything
else the original leaves in registers is dead.

Data registers are tracked as two halves, low word (D0) and high word (D0h):
byte and word instructions read and write only the low half, so a caller
that works in words never observes a routine's leftover high words.

Def/use comes from the Musashi disassembly text of each instruction. The
analysis is conservative: anything it cannot classify and any path that
leaves the routine make all still-undecided registers live. It is
interprocedural: at an RTS, what is live is what the routine's own callers
read after their calls (a worklist fixpoint over static and observed call
edges); a routine with no known caller keeps everything live at its RTS.
Callee upward-exposed uses are computed per routine; a call kills the flags
(the callee's RTS leaves its own flags) and kills no registers.

Validation: `fa18_recomp --ports shadow --poison` overwrites everything
declared dead after every compared call; the run must end with the same RAM
and registers as the plain generated run.
"""
from __future__ import annotations

import re

DLO = [f"D{i}" for i in range(8)]
DHI = [f"D{i}h" for i in range(8)]
AREGS = [f"A{i}" for i in range(8)]
REGS = DLO + DHI + AREGS
FLAGS = ["X", "N", "Z", "V", "C"]
ALL = frozenset(REGS + FLAGS)
NZVC = frozenset("NZVC")
XNZVC = frozenset("XNZVC")

INDEX_RE = re.compile(r"\b([DA][0-7]|SP)(?:\.([wlWL]))?\b")
CONDITIONS = "t|f|ra|hi|ls|cc|hs|cs|lo|ne|eq|vc|vs|pl|mi|ge|lt|gt|le"


def split_operands(ops: str) -> list[str]:
    out, depth, cur = [], 0, ""
    for ch in ops:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur.strip())
            cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


def norm(r: str) -> str:
    r = r.upper()
    return "A7" if r == "SP" else r


def reglist(text: str) -> set[str]:
    regs = set()
    for part in text.split("/"):
        m = re.fullmatch(r"([DA])([0-7])(?:-([DA])([0-7]))?", part.strip(), re.IGNORECASE)
        if not m:
            return set()
        kind, a = m.group(1).upper(), int(m.group(2))
        b = int(m.group(4)) if m.group(3) else a
        regs.update(f"{kind}{i}" for i in range(a, b + 1))
    return regs


def parts(reg: str, size: str) -> set[str]:
    """Tokens of a register accessed at `size` ('b', 'w', 'l')."""
    if reg.startswith("D"):
        return {reg} if size in ("b", "w") else {reg, reg + "h"}
    return {reg}


def operand(op: str):
    """(kind, register, address-forming uses, autoinc/dec register)."""
    s = op.strip()
    if re.fullmatch(r"[DA][0-7]|SP", s, re.IGNORECASE):
        r = norm(s)
        return ("dreg" if r[0] == "D" else "areg"), r, set(), None
    if s.startswith("#"):
        return "imm", None, set(), None
    if "(" not in s:
        return "mem", None, set(), None
    uses: set[str] = set()
    inner = s[s.find("(") + 1:s.rfind(")")]
    for m in INDEX_RE.finditer(inner):
        r = norm(m.group(1))
        uses |= parts(r, (m.group(2) or "l").lower()) if r[0] == "D" else {r}
    auto = None
    m = re.fullmatch(r"-?\((A[0-7]|SP)\)\+?", s, re.IGNORECASE)
    if m and (s.startswith("-") or s.endswith("+")):
        auto = norm(m.group(1))
    return "mem", None, uses, auto


def size_of(mnemonic: str) -> str:
    m = re.search(r"\.([bwl])$", mnemonic)
    return m.group(1) if m else ""


def def_use(text: str):
    """(uses, full defs, flags defined, flags used, control) for one
    instruction, or None when it cannot be classified. control is None for
    straight-line code, else 'branch', 'jump', 'call', 'ret' or 'stop'."""
    fields = text.split(None, 1)
    mnem = fields[0].lower()
    ops = split_operands(fields[1]) if len(fields) > 1 else []
    base = mnem.split(".")[0]
    size = size_of(mnem) or "l"
    uses: set[str] = set()
    defs: set[str] = set()
    fuse: set[str] = set()

    def use(op, sz=None):
        kind, reg, addr, auto = operand(op)
        uses.update(addr)
        if reg:
            uses.update(parts(reg, sz or size))

    def write(op, sz=None, read=False):
        """Destination written at size sz; a byte write keeps the word."""
        sz = sz or size
        kind, reg, addr, auto = operand(op)
        uses.update(addr)
        if reg is None:
            return
        if read or sz == "b":
            uses.update(parts(reg, sz))
        if kind == "areg":
            defs.add(reg)  # word writes to An sign-extend into the full register
        elif sz == "w":
            defs.add(reg)
        elif sz == "l":
            defs.update(parts(reg, "l"))

    def bit_operands(op_bit, op_dst):
        kind, reg, addr, auto = operand(op_dst)
        uses.update(addr)
        m = re.fullmatch(r"#\$?(-?[0-9a-fA-F]+)", op_bit.strip())
        if not m:
            use(op_bit, "w")
        if reg is None:
            return
        if m:
            n = int(m.group(1), 16) & 31
            uses.add(reg if n < 16 else reg + "h")
        else:
            uses.update(parts(reg, "l"))

    if base == "bra":
        return set(), set(), frozenset(), set(), "jump"
    if base in ("bsr", "jsr", "jmp"):
        for op in ops:
            uses.update(operand(op)[2])
        return uses, set(), frozenset(), set(), ("jump" if base == "jmp" else "call")
    if base in ("rts", "rtr", "rte"):
        return set(), set(), frozenset(), set(), "ret"
    if base in ("stop", "reset", "illegal", "trap", "trapv") or base.startswith("dc"):
        return set(), set(), frozenset(), set(), "stop"
    if re.fullmatch(rf"b({CONDITIONS})", base):
        return set(), set(), frozenset(), set(NZVC), "branch"
    if re.fullmatch(rf"db({CONDITIONS})", base):
        reg = operand(ops[0])[1]
        return {reg}, {reg}, frozenset(), set(NZVC), "branch"
    if re.fullmatch(rf"s({CONDITIONS})", base):
        write(ops[0], "b")
        return uses, defs, frozenset(), set(NZVC), None
    if base == "nop":
        return set(), set(), frozenset(), set(), None

    if base in ("move", "moveq", "movea"):
        if len(ops) != 2:
            return None
        a, b = ops[0].upper(), ops[1].upper()
        if "USP" in (a, b):
            return None
        if b in ("SR", "CCR"):
            use(ops[0], "w")
            return uses, defs, XNZVC, fuse, None
        if a in ("SR", "CCR"):
            write(ops[1], "w")
            return uses, defs, frozenset(), set(XNZVC), None
        sz = "l" if base == "moveq" else size
        use(ops[0], sz)
        write(ops[1], sz)
        return uses, defs, (frozenset() if base == "movea" else NZVC), fuse, None
    if base == "movem":
        if len(ops) != 2:
            return None
        regs = reglist(ops[0])
        if regs:
            for r in regs:
                uses.update(parts(r, size))
            uses.update(operand(ops[1])[2])
            return uses, defs, frozenset(), fuse, None
        regs = reglist(ops[1])
        if not regs:
            return None
        kind, _, addr, auto = operand(ops[0])
        uses.update(addr)
        for r in regs:
            if r != auto:
                defs.update(parts(r, "l"))  # MOVEM.W sign-extends into the full register
        return uses, defs, frozenset(), fuse, None
    if base == "lea":
        uses.update(operand(ops[0])[2])
        defs.add(operand(ops[1])[1])
        return uses, defs, frozenset(), fuse, None
    if base == "pea":
        uses.update(operand(ops[0])[2])
        uses.add("A7")
        return uses, defs, frozenset(), fuse, None
    if base == "link":
        uses.update({operand(ops[0])[1], "A7"})
        return uses, defs, frozenset(), fuse, None
    if base == "unlk":
        uses.add(operand(ops[0])[1])
        defs.add("A7")
        return uses, defs, frozenset(), fuse, None
    if base == "exg":
        for op in ops:
            r = operand(op)[1]
            uses.update(parts(r, "l"))
            defs.update(parts(r, "l"))
        return uses, defs, frozenset(), fuse, None
    if base == "swap":
        r = operand(ops[0])[1]
        uses.update(parts(r, "l"))
        defs.update(parts(r, "l"))
        return uses, defs, NZVC, fuse, None
    if base == "ext":
        r = operand(ops[0])[1]
        uses.add(r)
        defs.update(parts(r, "l") if size == "l" else {r})
        return uses, defs, NZVC, fuse, None
    if base in ("tst", "cmp", "cmpi", "cmpm", "chk"):
        for op in ops:
            use(op)
        return uses, defs, NZVC, fuse, None
    if base == "cmpa":
        use(ops[0])
        use(ops[1], "l")
        return uses, defs, NZVC, fuse, None
    if base in ("btst", "bchg", "bclr", "bset"):
        bit_operands(ops[0], ops[1])
        return uses, defs, frozenset("Z"), fuse, None
    if base == "clr":
        write(ops[0])
        return uses, defs, NZVC, fuse, None
    if base in ("not", "neg", "negx", "nbcd", "tas"):
        write(ops[0], read=True)
        if base in ("negx", "nbcd"):
            fuse.update({"X", "Z"})
        return uses, defs, (NZVC if base in ("not", "tas") else XNZVC), fuse, None
    if base in ("adda", "suba"):
        use(ops[0])
        r = operand(ops[1])[1]
        uses.add(r)
        defs.add(r)
        return uses, defs, frozenset(), fuse, None
    if base in ("add", "addi", "addq", "sub", "subi", "subq", "addx", "subx", "abcd", "sbcd"):
        use(ops[0])
        kind = operand(ops[1])[0]
        write(ops[1], read=True)
        if base in ("addx", "subx", "abcd", "sbcd"):
            fuse.update({"X", "Z"})
        return uses, defs, (frozenset() if kind == "areg" else XNZVC), fuse, None
    if base in ("and", "andi", "or", "ori", "eor", "eori"):
        if ops[1].upper() in ("SR", "CCR"):
            fuse.update(XNZVC)
            return uses, defs, XNZVC, fuse, None
        use(ops[0])
        write(ops[1], read=True)
        return uses, defs, NZVC, fuse, None
    if base in ("mulu", "muls"):
        use(ops[0], "w")
        r = operand(ops[1])[1]
        uses.add(r)
        defs.update(parts(r, "l"))
        return uses, defs, NZVC, fuse, None
    if base in ("divu", "divs"):
        use(ops[0], "w")
        r = operand(ops[1])[1]
        uses.update(parts(r, "l"))
        defs.update(parts(r, "l"))
        return uses, defs, NZVC, fuse, None
    if base in ("asl", "asr", "lsl", "lsr", "rol", "ror", "roxl", "roxr"):
        if len(ops) == 1:
            write(ops[0], "w", read=True)
        else:
            use(ops[0], "w")
            write(ops[1], read=True)
        if base in ("roxl", "roxr"):
            fuse.add("X")
        return uses, defs, (NZVC if base in ("rol", "ror") else XNZVC), fuse, None
    return None


def build_summaries(functions: dict) -> dict[int, frozenset]:
    """Upward-exposed register uses of each routine (flags excluded)."""
    summaries: dict[int, frozenset] = {}
    active: set[int] = set()

    def summary(entry: int) -> frozenset:
        if entry in summaries:
            return summaries[entry]
        if entry in active or entry not in functions:
            return frozenset(REGS)
        active.add(entry)
        live = walk(entry, frozenset(REGS), functions[entry], summary, ret_live=frozenset())
        active.discard(entry)
        summaries[entry] = frozenset(r for r in live if r in REGS)
        return summaries[entry]

    for e in functions:
        summary(e)
    return summaries


def walk(start: int, undecided: frozenset, fn, summary, ret_live: frozenset) -> set[str]:
    """Tokens in `undecided` read on some path from `start` before being
    defined. At an RTS, `ret_live` is what the routine's callers read."""
    live: set[str] = set()
    seen: dict[int, frozenset] = {}
    work = [(start, undecided)]
    while work:
        pc, und = work.pop()
        while und:
            prev = seen.get(pc)
            if prev is not None and und <= prev:
                break
            seen[pc] = (prev or frozenset()) | und
            d = fn.insns.get(pc)
            if d is None:
                live |= und  # undecodable or outside the routine
                break
            length, op, name, text, kind, target = d
            du = def_use(text)
            if du is None:
                live |= und
                break
            uses, defs, flags_def, flags_use, control = du
            live |= und & (set(uses) | set(flags_use))
            if control == "call":
                callee = summary(target) if target is not None else frozenset(REGS)
                live |= und & callee
                und = und - set(FLAGS)
                pc += length
                continue
            if control == "ret":
                live |= und & ret_live
                break
            if control == "stop":
                live |= und
                break
            und = und - defs - flags_def
            if control == "jump":
                if target is not None and target in fn.insns:
                    pc = target
                    continue
                live |= und
                break
            if control == "branch":
                if target is not None and target in fn.insns:
                    work.append((target, und))
                else:
                    live |= und
                pc += length
                continue
            pc += length
    return live


def call_site_liveness(functions: dict, extra_callers: dict[int, set[int]] | None = None
                       ) -> dict[int, tuple[set[str], int]]:
    """{return address: (live tokens, owning routine entry)}."""
    summaries = build_summaries(functions)
    sites = []  # (ret, owning routine, callee or None)
    for entry, fn in functions.items():
        for pc, d in fn.insns.items():
            if d is not None and d[4] in ("bsr", "jsr"):
                sites.append((pc + d[0], entry, d[5]))
    callers: dict[int, set[int]] = {}
    for ret, owner, callee in sites:
        if callee is not None:
            callers.setdefault(callee, set()).add(ret)
    for callee, rets in (extra_callers or {}).items():
        callers.setdefault(callee, set()).update(rets)

    site_rets = {ret for ret, _, _ in sites}
    live_out = {e: (ALL if e not in callers or any(r not in site_rets for r in callers[e]) else frozenset())
                for e in functions}
    live_at: dict[int, frozenset] = {}
    site_owner = {ret: owner for ret, owner, _ in sites}
    sites_in: dict[int, list[int]] = {}
    for ret, owner, _ in sites:
        sites_in.setdefault(owner, []).append(ret)
    callees_of_site: dict[int, list[int]] = {}
    for callee, rets in callers.items():
        for r in rets:
            callees_of_site.setdefault(r, []).append(callee)

    def summary(e):
        return summaries.get(e, frozenset(REGS))

    # Worklist: a site depends on its owner's live-out; a routine's live-out
    # depends on the sites that call it. Sets only grow, so this terminates.
    work = list(site_owner)
    queued = set(work)
    while work:
        ret = work.pop()
        queued.discard(ret)
        owner = site_owner[ret]
        live = frozenset(walk(ret, ALL, functions[owner], summary, live_out[owner]))
        if live_at.get(ret) == live:
            continue
        live_at[ret] = live
        for callee in callees_of_site.get(ret, ()):
            if callee not in live_out:
                continue
            # A return address outside translated code (ROM, undiscovered
            # code) is not analysed: everything is live there.
            new = frozenset().union(*(live_at.get(r, frozenset()) if r in site_owner else ALL
                                      for r in callers[callee]))
            if new != live_out[callee]:
                live_out[callee] = new
                for r in sites_in.get(callee, ()):
                    if r not in queued:
                        queued.add(r)
                        work.append(r)
    return {ret: (set(live_at[ret]), owner) for ret, owner, _ in sites}


def masks(live: set[str]) -> tuple[int, int, int]:
    """(D0-D7 low words and A0-A7 as bits 0-15, D0-D7 high words, flags)."""
    regs = sum(1 << i for i, r in enumerate(DLO + AREGS) if r in live)
    high = sum(1 << i for i, r in enumerate(DHI) if r in live)
    flags = sum(1 << i for i, f in enumerate(FLAGS) if f in live)
    return regs, high, flags
