"""Join observed C2AD80 selector byte pairs to later C2AEFC packet headers."""
from __future__ import annotations

import argparse
import json
from collections import defaultdict
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE, TABLE = 0xC00000, 0xC29F00
SELECTOR, PACKET = 0xC2AD80, 0xC2AEFC


def address(value: int) -> str:
    return f"${value:06X}"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, action="append", required=True)
    parser.add_argument("--slow", type=Path, required=True)
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/data/m_map_selector_pair_header_join.json")
    parser.add_argument("--replace", action="store_true")
    args = parser.parse_args()
    markdown = args.output.with_suffix(".md")
    if not args.replace and (args.output.exists() or markdown.exists()):
        raise FileExistsError(args.output)
    slow = args.slow.read_bytes()
    rows: dict[int, dict] = {}
    total, reached = 0, 0
    for trace_path in args.trace:
        trace = [json.loads(line) for line in trace_path.read_text(encoding="utf-8").splitlines()]
        for index, item in enumerate(trace):
            if item["pc"] != SELECTOR:
                continue
            total += 1
            selector = item["registers"]["d2"] & 0xFFFF
            offset = TABLE + selector * 2 - SLOW_BASE
            pair = [int.from_bytes(slow[offset:offset + 1], "big", signed=True),
                    int.from_bytes(slow[offset + 1:offset + 2], "big", signed=True)]
            next_selector = next((cursor for cursor in range(index + 1, len(trace))
                                  if trace[cursor]["pc"] == SELECTOR), len(trace))
            packet_index = next((cursor for cursor in range(index + 1, next_selector)
                                 if trace[cursor]["pc"] == PACKET), None)
            row = rows.setdefault(selector, {"selector": selector, "pair": pair,
                                             "headers": defaultdict(list), "no_packet": []})
            sample = {"trace": str(trace_path.resolve().relative_to(ROOT)),
                      "frame": item["frame"], "trace_index": item["index"]}
            if packet_index is None:
                row["no_packet"].append(sample)
                continue
            header = (trace[packet_index]["registers"]["a3"] - 4) & 0xFFFFFF
            row["headers"][address(header)].append(sample)
            reached += 1
    output_rows = []
    for selector, row in sorted(rows.items()):
        output_rows.append({"selector": selector, "pair": row["pair"],
                            "headers": [{"header": header, "observations": samples}
                                        for header, samples in sorted(row["headers"].items())],
                            "no_packet_observations": row["no_packet"]})
    report = {"classification": "scenario_join_of_local_m_map_selector_pairs_to_packet_headers",
              "traces": [str(path.resolve().relative_to(ROOT)) for path in args.trace],
              "selector_entries": total, "entries_reaching_packet_header": reached,
              "entries": output_rows,
              "qualification": ("The join searches only until the next C2AD80 selector entry. A missing packet header "
                                "means this bounded trace returned/rejected before C2AEFC; it is not a claim that the "
                                "local offset has no map content. Pair components remain local selector offsets, not global axes.")}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# M-map local selector pair to packet-header join", "", "Classification: **scenario-backed local selection join**.", "", report["qualification"], "",
             f"{reached} of {total} `$C2AD80` entries reach `$C2AEFC` before the next selector entry.", "",
             "| Selector | signed pair | packet headers | no-header entries |", "| ---: | --- | --- | ---: |"]
    for row in output_rows:
        headers = ", ".join(f"`{entry['header']}` ({len(entry['observations'])})" for entry in row["headers"]) or "none"
        lines.append(f"| {row['selector']} | {tuple(row['pair'])} | {headers} | {len(row['no_packet_observations'])} |")
    lines.append("")
    markdown.write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"selector_entries": total, "joined": reached, "selectors": len(output_rows)}))


if __name__ == "__main__":
    main()
