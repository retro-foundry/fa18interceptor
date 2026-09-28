"""Rank subtree roots: routines whose unported static callees (transitively)
are all clean, scored by the root's glue burden and the subtree size. Porting
the whole subtree at once needs glue only at the root.

  python tools/recomp/subtree_candidates.py [-n 30] [--max-insns 600]
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
from port_candidates import modified  # noqa: E402


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("-n", type=int, default=30)
    ap.add_argument("--max-insns", type=int, default=600)
    args = ap.parse_args()
    graph = {f["entry"]: f for f in json.loads((GEN / "recomp_graph.json").read_text())}
    edges = json.loads((GEN / "recomp_edges.json").read_text())
    done = set(re.findall(r"\{0x([0-9A-F]{6})", (ROOT / "port/game/glue/ports.c").read_text()))
    live = {a: (int(r, 16), int(h, 16)) for a, r, h, f in re.findall(
        r"\{0x([0-9A-F]+), 0x([0-9A-F]+), 0x([0-9A-F]+), 0x([0-9A-F]+)\}",
        (GEN / "recomp_liveness.c").read_text())}
    profile = json.loads((ROOT / "build/recomp/profile3000.json").read_text())
    sites = {}
    for r, c in edges:
        sites.setdefault(c, set()).add(r)

    def clean(f):
        k = f["kinds"]
        return not f["dynamic_calls"] and not f["jumps_out"] and "interp" not in k and "undecodable" not in k

    def subtree(e, seen):
        if e in done or e in seen:
            return set()
        f = graph.get(e)
        if f is None or not clean(f):
            return None
        seen.add(e)
        out = {e}
        for c in f["calls"]:
            s = subtree(c, seen)
            if s is None:
                return None
            out |= s
        return out

    rows = []
    for e in graph:
        if e in done or e not in sites:
            continue
        tree = subtree(e, set())
        if not tree or len(tree) < 2:
            continue
        insns = sum(graph[x]["instructions"] for x in tree)
        if insns > args.max_insns:
            continue
        tokens = set()
        for r in sites[e]:
            m = live.get(r)
            if m is None:
                tokens |= set(liveness.REGS)
                continue
            tokens |= {t for i, t in enumerate(liveness.DLO + liveness.AREGS) if m[0] >> i & 1}
            tokens |= {t for i, t in enumerate(liveness.DHI) if m[1] >> i & 1}
        mods = set()
        for x in tree:
            mods |= modified(x)
        burden = sorted((mods & tokens) - {"A7"})
        rows.append((len(burden), -profile.get(e, 0), e, len(tree), insns, burden, sorted(tree - {e})))
    rows.sort()
    for b, calls, e, n, insns, burden, rest in rows[:args.n]:
        print(f"${e} tree {n:2} insns {insns:4} calls {-calls:6} burden {b:2} {' '.join(burden)}  [{' '.join(rest)}]")


if __name__ == "__main__":
    main()
