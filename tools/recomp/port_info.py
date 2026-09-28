"""Print what porting one routine needs: its instructions, callers, observed
call sites, and the registers/flags live after it returns (the outputs the
glue must reproduce).

  python tools/recomp/port_info.py C305AA [C2E6DA ...]
"""
from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GEN = ROOT / "port/recomp/generated"
NAMES = [f"D{i}" for i in range(8)] + [f"A{i}" for i in range(8)]


def instructions(entry: str) -> list[str]:
    head = f"int fa18_fn_{entry}(int entry)"
    for path in sorted(GEN.glob("recomp_*.c")):
        text = path.read_text()
        start = text.find(head)
        if start < 0:
            continue
        end = text.find("\n}\n", start)
        return [m.group(1) for m in re.finditer(r"/\* ([0-9A-F]{6}: .*?) \*/", text[start:end])]
    return []


def main() -> None:
    live = {a: (int(r, 16), int(h, 16), int(f, 16)) for a, r, h, f in re.findall(
        r"\{0x([0-9A-F]+), 0x([0-9A-F]+), 0x([0-9A-F]+), 0x([0-9A-F]+)\}",
        (GEN / "recomp_liveness.c").read_text())}
    edges = json.loads((GEN / "recomp_edges.json").read_text())
    graph = {f["entry"]: f for f in json.loads((GEN / "recomp_graph.json").read_text())}
    for entry in sys.argv[1:]:
        entry = entry.upper().lstrip("$")
        print(f"== ${entry}")
        for line in instructions(entry):
            print("   ", line)
        sites = sorted({r for r, c in edges if c == entry})
        for f in graph.values():
            for s, e in f["spans"]:
                pass
        regs = high = flags = 0
        unknown = []
        for r in sites:
            m = live.get(r)
            if m is None:
                unknown.append(r)
                continue
            regs |= m[0]
            high |= m[1]
            flags |= m[2]
        print(f"  observed call sites: {', '.join(sites) or 'none'}")
        if unknown:
            print(f"  sites without liveness (all live): {', '.join(unknown)}")
        print("  live after return:",
              " ".join(NAMES[i] for i in range(16) if regs >> i & 1), "|",
              " ".join(f"D{i}h" for i in range(8) if high >> i & 1), "| flags",
              "".join(x for i, x in enumerate("XNZVC") if flags >> i & 1) or "-")
        reports = sorted((ROOT / "analysis/routines").glob(f"{entry.lower()}_*.md"))
        for rep in reports:
            print(f"  report: {rep.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
