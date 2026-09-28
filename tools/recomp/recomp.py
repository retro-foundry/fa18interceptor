"""Mechanical 68000 -> C translation of the F/A-18 Interceptor game code.

Stage A of PORT.md. Reads the loaded game image (Chip and Slow RAM from a
bridge snapshot), discovers routines by recursive descent from seed PCs, and
writes one C function per routine to port/recomp/generated/.

Decoding uses Musashi's own disassembler and opcode->handler table (via
build/recomp/dasm_helper.dll), so instruction lengths and semantics match the
reference CPU core exactly. Each instruction's data operation executes through
the Musashi handler for that opcode; control flow (branches, calls, returns,
jump targets) is native C with labels at every leader. See
port/recomp/recomp_runtime.h for the macro contract.
"""
from __future__ import annotations

import argparse
import ctypes
import json
import re
from collections import deque
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MUSASHI = ROOT / "tools/musashi"

SLOW_BASE, SLOW_SIZE = 0xC00000, 0x80000
CHIP_SIZE = 0x80000


def handler_names() -> list[str]:
    text = (MUSASHI / "m68kops.c").read_text()
    start = text.index("m68k_opcode_handler_table[] =")
    names = re.findall(r"\{(m68k_op_\w+)\s*,", text[start:])
    return names


class Decoder:
    def __init__(self, dll: Path, regions: list[tuple[int, bytes]]):
        self.lib = ctypes.CDLL(str(dll))
        self.lib.fa18_dasm_init()
        self.lib.fa18_dasm.restype = ctypes.c_uint
        self.lib.fa18_dasm.argtypes = [ctypes.c_uint32, ctypes.c_char_p]
        self.lib.fa18_handler_index.argtypes = [ctypes.c_uint]
        for base, data in regions:
            self.lib.fa18_dasm_load(ctypes.c_uint32(base), data, ctypes.c_uint32(len(data)))
        self.names = handler_names()
        self.image = {base: data for base, data in regions}
        self.cache: dict[int, tuple | None] = {}

    def word(self, a: int) -> int:
        for base, data in self.image.items():
            if base <= a < base + len(data) - 1:
                return data[a - base] << 8 | data[a - base + 1]
        raise KeyError(a)

    def decode(self, pc: int):
        if pc in self.cache:
            return self.cache[pc]
        result = None
        if pc & 1 == 0 and in_code_region(pc):
            buf = ctypes.create_string_buffer(256)
            length = self.lib.fa18_dasm(pc, buf)
            op = self.word(pc)
            index = self.lib.fa18_handler_index(op)
            name = self.names[index] if 0 <= index < len(self.names) else "illegal"
            if name not in ("m68k_op_illegal",) and 2 <= length <= 10 and in_code_region(pc + length - 1):
                text = buf.value.decode("latin1")
                result = (length, op, name, text)
        self.cache[pc] = result
        return result


def in_code_region(a: int) -> bool:
    return SLOW_BASE <= a < SLOW_BASE + SLOW_SIZE or 0 <= a < CHIP_SIZE


# Instruction classes, from the Musashi handler name.
def classify(name: str) -> str:
    base = name[len("m68k_op_"):]
    if base.startswith(("bra_",)):
        return "bra"
    if base.startswith("bsr_"):
        return "bsr"
    if re.match(r"b(hi|ls|cc|cs|ne|eq|vc|vs|pl|mi|ge|lt|gt|le)_", base):
        return "bcc"
    if re.match(r"db(t|f|hi|ls|cc|cs|ne|eq|vc|vs|pl|mi|ge|lt|gt|le)_", base):
        return "dbcc"
    if base.startswith("jmp_"):
        return "jmp"
    if base.startswith("jsr_"):
        return "jsr"
    if base.startswith(("rts_", "rtr_")):
        return "rts"
    if base.startswith(("rte_", "stop", "reset", "trap", "illegal", "1010", "1111", "bkpt")):
        return "interp"
    if base.startswith("trapv") or base.startswith("chk"):
        return "op"  # exception only on the rare path; caught by the PC check
    return "op"


def s16(v: int) -> int:
    return v - 0x10000 if v & 0x8000 else v


def s8(v: int) -> int:
    return v - 0x100 if v & 0x80 else v


def static_target(dec: Decoder, pc: int, op: int, name: str, kind: str) -> int | None:
    """Branch/call target when it is encoded in the instruction."""
    base = name[len("m68k_op_"):]
    if kind in ("bra", "bsr", "bcc"):
        if base.endswith("_8"):
            return pc + 2 + s8(op & 0xFF)
        if base.endswith("_16"):
            return pc + 2 + s16(dec.word(pc + 2))
        return None
    if kind == "dbcc":
        return pc + 2 + s16(dec.word(pc + 2))
    if kind in ("jmp", "jsr"):
        mode = base.split("_")[-1]
        if mode == "aw":
            return s16(dec.word(pc + 2)) & 0xFFFFFF
        if mode == "al":
            return (dec.word(pc + 2) << 16 | dec.word(pc + 4)) & 0xFFFFFF
        if mode == "pcdi":
            return pc + 2 + s16(dec.word(pc + 2))
        return None
    return None


class Function:
    def __init__(self, entry: int):
        self.entry = entry
        self.insns: dict[int, tuple] = {}
        self.leaders: set[int] = {entry}
        self.calls: set[int] = set()


def discover(dec: Decoder, entry: int) -> Function:
    fn = Function(entry)
    work = deque([entry])
    while work:
        pc = work.popleft()
        if pc in fn.insns:
            continue
        d = dec.decode(pc)
        if d is None:
            fn.insns[pc] = None  # undecodable: emit an interpreter exit
            continue
        length, op, name, text = d
        kind = classify(name)
        target = static_target(dec, pc, op, name, kind)
        fn.insns[pc] = (length, op, name, text, kind, target)
        nxt = pc + length
        if kind in ("bcc", "dbcc"):
            fn.leaders.update((nxt,))
            work.append(nxt)
            if target is not None and in_code_region(target):
                fn.leaders.add(target)
                work.append(target)
        elif kind in ("bra", "jmp"):
            if target is not None and in_code_region(target):
                fn.leaders.add(target)
                work.append(target)
        elif kind in ("bsr", "jsr"):
            fn.leaders.add(nxt)
            work.append(nxt)
            if target is not None and in_code_region(target):
                fn.calls.add(target)
        elif kind == "rts":
            pass
        elif kind == "interp":
            fn.leaders.update((pc, nxt))
            work.append(nxt)
        else:
            work.append(nxt)
    return fn


def cname(pc: int) -> str:
    return f"fa18_fn_{pc:06X}"


def fn_labels(fn: Function) -> list[int]:
    return sorted(fn.leaders & set(fn.insns))


def emit_function(fn: Function, callees: dict[int, tuple[int, int]]) -> tuple[str, list[int]]:
    labels = fn_labels(fn)
    index = {pc: i for i, pc in enumerate(labels)}
    out = [f"int {cname(fn.entry)}(int entry) {{", "    switch (entry) {"]
    for pc, i in index.items():
        out.append(f"    case {i}: goto L_{pc:06X};")
    out.append("    default: return FA18_EXIT_DISPATCH;")
    out.append("    }")
    order = sorted(fn.insns)
    prev_end = None
    for pc in order:
        d = fn.insns[pc]
        if prev_end is not None and pc != prev_end:
            # Straight-line flow never falls off a gap; be explicit anyway.
            out.append(f"    REG_PC = 0x{prev_end:06X}; return FA18_EXIT_DISPATCH;")
        if pc in index:
            out.append(f"L_{pc:06X}:")
            out.append(f"    FA18_CHECK(0x{pc:06X});")
        if d is None:
            out.append(f"    FA18_INTERP(0x{pc:06X}); /* undecodable */")
            prev_end = None
            continue
        length, op, name, text, kind, target = d
        nxt = pc + length
        comment = f"/* {pc:06X}: {text} */"
        local = target is not None and target in index
        if kind == "op":
            out.append(f"    FA18_OP(0x{pc:06X}, 0x{op:04X}, 0x{nxt:06X}); {comment}")
            prev_end = nxt
        elif kind == "interp":
            out.append(f"    FA18_INTERP(0x{pc:06X}); {comment}")
            prev_end = None
        elif kind == "bcc" or kind == "dbcc":
            if local:
                out.append(f"    FA18_BCC(0x{pc:06X}, 0x{op:04X}, 0x{nxt:06X}, 0x{target:06X}, L_{target:06X}); {comment}")
            else:
                out.append(f"    FA18_BCC_OUT(0x{pc:06X}, 0x{op:04X}, 0x{nxt:06X}); {comment}")
            prev_end = nxt
        elif kind in ("bra", "jmp"):
            if local:
                out.append(f"    FA18_JUMP(0x{pc:06X}, 0x{op:04X}, 0x{target:06X}, L_{target:06X}); {comment}")
            else:
                out.append(f"    FA18_JUMP_OUT(0x{pc:06X}, 0x{op:04X}); {comment}")
            prev_end = None
        elif kind in ("bsr", "jsr"):
            callee = callees.get(target) if target is not None else None
            if callee:
                out.append(f"    FA18_CALL(0x{pc:06X}, 0x{op:04X}, 0x{nxt:06X}, {callee[0]}, {callee[1]}); {comment}")
            else:
                out.append(f"    FA18_CALL_DYNAMIC(0x{pc:06X}, 0x{op:04X}, 0x{nxt:06X}); {comment}")
            prev_end = nxt
        elif kind == "rts":
            out.append(f"    FA18_RETURN(0x{pc:06X}, 0x{op:04X}); {comment}")
            prev_end = None
    if prev_end is not None:
        out.append(f"    REG_PC = 0x{prev_end:06X}; return FA18_EXIT_DISPATCH;")
    out.append("}")
    return "\n".join(out) + "\n", labels


def ranges(fn: Function) -> list[tuple[int, int]]:
    spans = []
    for pc in sorted(fn.insns):
        d = fn.insns[pc]
        if d is None:
            continue
        end = pc + d[0]
        if spans and spans[-1][1] == pc:
            spans[-1][1] = end
        else:
            spans.append([pc, end])
    return [tuple(s) for s in spans]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--snapshot", type=Path, default=ROOT / "build/recomp/run075_f392",
                        help="bridge output directory with chip.bin and slow.bin")
    parser.add_argument("--trace", type=Path, action="append", default=[],
                        help="bridge trace.jsonl whose PCs seed discovery")
    parser.add_argument("--seeds", type=Path, action="append", default=[],
                        help="JSON list of extra entry PCs (e.g. fallback-interpreter log)")
    parser.add_argument("--dll", type=Path, default=ROOT / "build/recomp/dasm_helper.dll")
    parser.add_argument("--out", type=Path, default=ROOT / "port/recomp/generated")
    parser.add_argument("--functions-per-file", type=int, default=150)
    args = parser.parse_args()

    chip = (args.snapshot / "chip.bin").read_bytes()
    slow = (args.snapshot / "slow.bin").read_bytes()
    dec = Decoder(args.dll, [(0, chip), (SLOW_BASE, slow)])

    seeds: list[int] = []
    for trace in args.trace:
        seen = set()
        with trace.open() as f:
            for line in f:
                pc = json.loads(line)["pc"]
                if pc not in seen and in_code_region(pc):
                    seen.add(pc)
                    seeds.append(pc)
    for path in args.seeds:
        seeds.extend(int(x, 16) if isinstance(x, str) else int(x) for x in json.loads(path.read_text()))

    functions: dict[int, Function] = {}
    covered: set[int] = set()
    pending = deque()

    def run_pending():
        while pending:
            entry = pending.popleft()
            if entry in functions or dec.decode(entry) is None:
                continue
            fn = discover(dec, entry)
            functions[entry] = fn
            covered.update(pc for pc, d in fn.insns.items() if d is not None)
            for callee in sorted(fn.calls):
                if callee not in functions:
                    pending.append(callee)

    for pc in seeds:
        if pc not in covered:
            pending.append(pc)
            run_pending()

    entry_fn = {entry: cname(entry) for entry in functions}
    args.out.mkdir(parents=True, exist_ok=True)
    for old in args.out.glob("recomp_*.c"):
        old.unlink()
    header = ['/* Generated by tools/recomp/recomp.py. Do not edit. */',
              '#define FA18_RECOMP_GENERATED', '#include "recomp_runtime.h"', '']
    decls = [f"int {entry_fn[e]}(int entry);" for e in sorted(functions)]
    (args.out / "recomp_functions.h").write_text(
        "/* Generated by tools/recomp/recomp.py. Do not edit. */\n#ifndef FA18_RECOMP_FUNCTIONS_H\n"
        "#define FA18_RECOMP_FUNCTIONS_H\n" + "\n".join(decls) + "\n#endif\n")
    entries: list[tuple[int, str, int]] = []
    spans: list[tuple[str, int, int]] = []
    ordered = sorted(functions)
    callees = {e: (i, fn_labels(functions[e]).index(e)) for i, e in enumerate(ordered)}
    for file_index in range(0, len(ordered), args.functions_per_file):
        chunk = ordered[file_index:file_index + args.functions_per_file]
        body = header + ['#include "recomp_functions.h"', '']
        for e in chunk:
            fn = functions[e]
            text, labels = emit_function(fn, callees)
            body.append(text)
            for i, pc in enumerate(labels):
                entries.append((pc, entry_fn[e], i))
            for a, b in ranges(fn):
                spans.append((entry_fn[e], a, b))
        (args.out / f"recomp_{file_index // args.functions_per_file:03d}.c").write_text("\n".join(body))

    # Entry table: one entry per address, preferring a routine's own start.
    best: dict[int, tuple[str, int]] = {}
    for pc, fname, label in entries:
        if pc not in best or (pc in functions and fname == entry_fn[pc]):
            best[pc] = (fname, label)
    fn_ids = {entry_fn[e]: i for i, e in enumerate(ordered)}
    table = header + ['#include "recomp_functions.h"', '',
                      f"const FA18RecompFunction fa18_recomp_functions[{len(ordered)}] = {{"]
    table += [f"    {{{entry_fn[e]}, 0x{e:06X}}}," for e in ordered]
    table += ["};", f"const int fa18_recomp_function_count = {len(ordered)};", "",
              f"const FA18RecompEntry fa18_recomp_entries[{len(best)}] = {{"]
    table += [f"    {{0x{pc:06X}, {fn_ids[f]}, {label}}}," for pc, (f, label) in sorted(best.items())]
    table += ["};", f"const int fa18_recomp_entry_count = {len(best)};", "",
              f"const FA18RecompSpan fa18_recomp_spans[{len(spans)}] = {{"]
    table += [f"    {{{fn_ids[f]}, 0x{a:06X}, 0x{b:06X}}}," for f, a, b in spans]
    table += ["};", f"const int fa18_recomp_span_count = {len(spans)};", ""]
    (args.out / "recomp_table.c").write_text("\n".join(table))

    instructions = len(covered)
    kinds: dict[str, int] = {}
    for fn in functions.values():
        for d in fn.insns.values():
            k = "undecodable" if d is None else d[4]
            kinds[k] = kinds.get(k, 0) + 1
    report = {"functions": len(functions), "entries": len(best), "instructions": instructions,
              "seed_pcs": len(seeds), "seed_pcs_uncovered": sum(1 for s in seeds if s not in covered),
              "instruction_kinds": kinds}
    (args.out / "recomp_manifest.json").write_text(json.dumps(report, indent=2) + "\n")

    # Per-routine call graph for porting order (tools/recomp/inventory.py).
    graph = []
    for e in ordered:
        fn = functions[e]
        kinds = {}
        for d in fn.insns.values():
            k = "undecodable" if d is None else d[4]
            kinds[k] = kinds.get(k, 0) + 1
        dynamic_calls = sum(1 for d in fn.insns.values()
                            if d is not None and d[4] in ("bsr", "jsr") and d[5] is None)
        jumps_out = sum(1 for d in fn.insns.values()
                        if d is not None and d[4] in ("bra", "jmp", "bcc", "dbcc")
                        and (d[5] is None or d[5] not in fn.insns))
        graph.append({"entry": f"{e:06X}", "instructions": sum(1 for d in fn.insns.values() if d),
                      "spans": [[f"{a:06X}", f"{b:06X}"] for a, b in ranges(fn)],
                      "calls": [f"{c:06X}" for c in sorted(fn.calls)],
                      "dynamic_calls": dynamic_calls, "jumps_out": jumps_out, "kinds": kinds})
    (args.out / "recomp_graph.json").write_text(json.dumps(graph, indent=1) + "\n")
    (args.out / "recomp_seeds.json").write_text(json.dumps(sorted(f"{s:06X}" for s in set(seeds))) + "\n")
    print(json.dumps(report))


if __name__ == "__main__":
    main()
