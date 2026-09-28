"""Audit Musashi's 68000 cycle table against cycle-exact Engine9000 traces.

For every traced instruction, predicts Musashi's cost (table value plus its
data-dependent extras) and compares with the reference cost. Bus contention
only ever adds time, so the most common difference per opcode is the table
error. Requires build/recomp/musashi_cycles.bin (the 68000 column of
m68ki_cycles, 65536 bytes).

  python scripts/musashi_timing_audit.py build/recomp/trace*/trace.jsonl
"""
from __future__ import annotations

import collections
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TABLE = (ROOT / "build/recomp/musashi_cycles.bin").read_bytes()


def musashi_extra(op: int, row: dict) -> int | None:
    """Musashi's data-dependent cycles, or None when not modelled here."""
    regs = row["registers"]
    words = bytes.fromhex(row["bytes"])
    length = len(words)
    taken = row["next_pc"] != row["pc"] + length
    if (op & 0xF000) == 0x6000 and (op & 0xFF00) not in (0x6000, 0x6100):  # Bcc
        if taken:
            return 0
        return -2 if op & 0xFF else 2
    if (op & 0xF0F8) == 0x50C8:  # DBcc
        if not taken and row["next_pc"] == row["pc"] + 4:
            # either the condition held (no extra) or the counter expired (+2)
            dn = regs[f"d{op & 7}"] & 0xFFFF
            return 2 if dn == 0 else 0
        return -2
    if (op & 0xF0C0) == 0x50C0 and (op & 0x38) == 0:  # Scc Dn
        return None
    if (op & 0xF000) == 0xE000 and (op & 0xC0) != 0xC0:  # register shifts
        if op & 0x20:
            count = regs[f"d{(op >> 9) & 7}"] & 63
        else:
            count = ((op >> 9) & 7) or 8
        return count * 2
    if (op & 0xFB80) == 0x4880:  # MOVEM
        mask = int.from_bytes(words[2:4], "big")
        return bin(mask).count("1") * (8 if op & 0x40 else 4)
    if (op & 0xF1C0) in (0xC1C0, 0xC0C0, 0x81C0, 0x80C0):  # MULS/MULU/DIVS/DIVU
        return None
    if (op & 0xF100) == 0x0100 or (op & 0xFF00) == 0x0800:  # bit ops
        return 0
    return 0


def main() -> None:
    stats = collections.defaultdict(collections.Counter)
    names = {}
    for path in sys.argv[1:]:
        rows = [json.loads(line) for line in open(path)]
        for row, after in zip(rows, rows[1:]):
            op = int(row["bytes"][:4], 16)
            extra = musashi_extra(op, row)
            if extra is None:
                continue
            ref = (after["cycles"] - row["cycles"]) * 2
            stats[op][ref - (TABLE[op] + extra)] += 1
            names.setdefault(op, row["asm"])
    groups = collections.defaultdict(list)
    for op, counter in stats.items():
        mode, hits = counter.most_common(1)[0]
        total = sum(counter.values())
        if mode != 0:
            groups[names[op].split()[0]].append((op, mode, hits, total, dict(counter.most_common(4))))
    for name in sorted(groups):
        for op, mode, hits, total, top in sorted(groups[name]):
            print(f"{op:04X} table {TABLE[op]:3d} mode {mode:+4d} ({hits}/{total})  {names[op]:32s} {top}")


if __name__ == "__main__":
    main()
