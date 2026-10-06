"""Rank routines that are ready to recreate.

Ready: every static callee is already recreated, no dynamic calls or jumps
out, observed as a call target. Each is scored by its glue burden: the
registers it modifies that are live after it returns at its observed call
sites (the outputs the glue must rebuild), then by how often it runs.

  python tools/recomp/port_candidates.py [--profile build/recomp/profile3000.json] [-n 40]
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GEN = ROOT / "port/recomp/generated"
sys.path.insert(0, str(Path(__file__).resolve().parent))
import liveness  # noqa: E402
from port_info import instructions  # noqa: E402
from native_call_graph import native_entries


def modified(entry: str) -> set[str]:
    out: set[str] = set()
    for line in instructions(entry):
        text = line.split(": ", 1)[1]
        du = liveness.def_use(text)
        if du is None:
            return set(liveness.REGS)
        uses, defs, _, _, _ = du
        out |= defs
        # Partial writes (read-modify-write) also modify the register.
        for token in uses:
            if token in liveness.REGS and re.search(rf"\b{token[:2]}\b", text.split(",")[-1]):
                out.add(token)
    return out


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--profile", type=Path, default=ROOT / "build/recomp/profile3000.json")
    ap.add_argument("-n", type=int, default=40)
    args = ap.parse_args()
    profile = json.loads(args.profile.read_text()) if args.profile.is_file() else {}
    edges = json.loads((GEN / "recomp_edges.json").read_text())
    graph = {f["entry"]: f for f in json.loads((GEN / "recomp_graph.json").read_text())}
    done = set(re.findall(r"\{0x([0-9A-F]{6})", (ROOT / "port/game/glue/ports.c").read_text()))
    done |= native_entries(list(graph.values()), done).keys()
    live = {a: (int(r, 16), int(h, 16), int(f, 16)) for a, r, h, f in re.findall(
        r"\{0x([0-9A-F]+), 0x([0-9A-F]+), 0x([0-9A-F]+), 0x([0-9A-F]+)\}",
        (GEN / "recomp_liveness.c").read_text())}
    sites: dict[str, set[str]] = {}
    for r, c in edges:
        sites.setdefault(c, set()).add(r)
    rows = []
    for e, f in graph.items():
        if e in done or e not in sites:
            continue
        k = f["kinds"]
        if f["dynamic_calls"] or f["jumps_out"] or "interp" in k or "undecodable" in k:
            continue
        if not all(c in done for c in f["calls"]):
            continue
        tokens = set()
        for r in sites[e]:
            m = live.get(r)
            if m is None:
                tokens |= set(liveness.REGS)
                continue
            regs, high, _ = m
            tokens |= {t for i, t in enumerate(liveness.DLO + liveness.AREGS) if regs >> i & 1}
            tokens |= {t for i, t in enumerate(liveness.DHI) if high >> i & 1}
        burden = sorted((modified(e) & tokens) - {"A7"})
        rows.append((len(burden), -profile.get(e, 0), e, f["instructions"], burden))
    rows.sort()
    for n, calls, e, insns, burden in rows[:args.n]:
        print(f"${e}  insns {insns:4}  calls {-calls:6}  burden {n:2}  {' '.join(burden)}")
    print(f"{len(rows)} ready")


if __name__ == "__main__":
    main()
