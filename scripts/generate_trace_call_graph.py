"""Generate a dynamic Amiga-game call graph and native address-coverage ledger.

The graph contains only verified 68000 JSR/BSR transitions that pushed a return
address on the same stack.  It is intentionally a trace report, not a static
whole-program call graph or a claim that every address-linked native module is
semantically complete.
"""
import argparse
import collections
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GAME_FIRST, GAME_LAST = 0xC00000, 0xC60000
ADDRESS = re.compile(r"\$C([0-5][0-9A-F]{4})", re.I)
RANGE = re.compile(r"\$C([0-5][0-9A-F]{4})\s*[-–]\s*\$C([0-5][0-9A-F]{4})", re.I)


def native_evidence():
    exact, ranges = collections.defaultdict(set), []
    for path in (ROOT / "port").glob("*.[ch]"):
        text = path.read_text(encoding="utf8", errors="replace")
        for match in ADDRESS.finditer(text):
            exact[0xC00000 | int(match.group(1), 16)].add(path.name)
        for match in RANGE.finditer(text):
            first = 0xC00000 | int(match.group(1), 16)
            last = 0xC00000 | int(match.group(2), 16)
            if first <= last:
                ranges.append((first, last, path.name))
    return exact, ranges


def coverage(address, exact, ranges):
    paths = set(exact[address])
    paths.update(path for first, last, path in ranges if first <= address <= last)
    return sorted(paths)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True,
                        help="output stem; creates .json, .dot, and .md")
    args = parser.parse_args()
    rows = [json.loads(line) for line in args.trace.read_text(encoding="utf8").splitlines()]
    edges = collections.Counter()
    for prior, row in zip(rows, rows[1:]):
        operation = prior["asm"].split(" ", 1)[0].lower()
        if not operation.startswith(("jsr", "bsr")):
            continue
        if prior.get("next_pc") != row["pc"]:
            continue
        if row["registers"]["a7"] != ((prior["registers"]["a7"] - 4) & 0xffffffff):
            continue
        if GAME_FIRST <= prior["pc"] < GAME_LAST and GAME_FIRST <= row["pc"] < GAME_LAST:
            edges[prior["pc"], row["pc"]] += 1
    targets = sorted({target for _, target in edges})
    exact, ranges = native_evidence()
    nodes = []
    for target in targets:
        callers = {caller: calls for (caller, callee), calls in edges.items() if callee == target}
        paths = coverage(target, exact, ranges)
        nodes.append({"address": target, "label": f"${target:06X}",
                      "calls": sum(callers.values()), "callers": len(callers),
                      "native_address_evidence": paths,
                      "classification": "address_linked_native" if paths else "no_native_address_evidence"})
    ledger = {
        "classification": "verified dynamic JSR/BSR stack pushes only; native coverage means an address-linked port source reference, not semantic completeness",
        "trace": str(args.trace), "instruction_rows": len(rows),
        "verified_game_calls": sum(edges.values()), "distinct_edges": len(edges),
        "distinct_dynamic_callees": len(targets),
        "address_linked_native_callees": sum(bool(node["native_address_evidence"]) for node in nodes),
        "callees_without_native_address_evidence": sum(not bool(node["native_address_evidence"]) for node in nodes),
        "edges": [{"caller": caller, "callee": callee, "calls": calls}
                  for (caller, callee), calls in sorted(edges.items())],
        "callees": nodes,
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.with_suffix(".json").write_text(json.dumps(ledger, indent=2) + "\n", encoding="utf8")
    dot = ["digraph frame_call_graph {", "  rankdir=LR;", "  node [shape=box,fontname=Consolas];"]
    for node in nodes:
        color = "palegreen" if node["native_address_evidence"] else "lightcoral"
        dot.append(f'  n{node["address"]:06X} [label="{node["label"]}",style=filled,fillcolor={color}];')
    for (caller, callee), calls in sorted(edges.items()):
        dot.append(f'  n{caller:06X} -> n{callee:06X} [label="{calls}"];')
    dot.append("}")
    args.output.with_suffix(".dot").write_text("\n".join(dot) + "\n", encoding="utf8")
    lines = ["# Dynamic frame call graph", "",
             f"Authority: `{args.trace.as_posix()}`.", "",
             "Only observed `JSR`/`BSR` transitions with a verified 68000 stack push are edges.",
             "Green nodes have an address-linked native `port/` source reference; red nodes do not.",
             "That classification is a porting triage signal, not proof that a green routine is fully ported.", "",
             f"- Instruction rows: {ledger['instruction_rows']}",
             f"- Verified game calls: {ledger['verified_game_calls']}",
             f"- Distinct dynamic callees: {ledger['distinct_dynamic_callees']}",
             f"- Address-linked native callees: {ledger['address_linked_native_callees']}",
             f"- Callees without native address evidence: {ledger['callees_without_native_address_evidence']}", "",
             "## Callee ledger", "", "| Callee | Calls | Callers | Native address evidence |", "| --- | ---: | ---: | --- |"]
    for node in sorted(nodes, key=lambda value: (-value["calls"], value["address"])):
        paths = ", ".join(f"`port/{path}`" for path in node["native_address_evidence"]) or "—"
        lines.append(f"| `{node['label']}` | {node['calls']} | {node['callers']} | {paths} |")
    args.output.with_suffix(".md").write_text("\n".join(lines) + "\n", encoding="utf8")
    print(json.dumps({key: ledger[key] for key in ("verified_game_calls", "distinct_edges", "distinct_dynamic_callees", "address_linked_native_callees", "callees_without_native_address_evidence")}))


if __name__ == "__main__":
    main()
