"""Compare directly observed inline and alternate map-packet transform batches."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


def direct_batches(report: dict[str, object]) -> dict[str, list[dict[str, object]]]:
    rows: dict[str, list[dict[str, object]]] = {}
    for batch in report["batches"]:
        if batch["packet"]:
            rows.setdefault(batch["packet"], []).append(batch)
    return rows


def route_headers(report: dict[str, object], route: str) -> set[str]:
    return {row["header"] for row in report["packet_streams"] if row["route"] == route}


def summarize(rows: list[dict[str, object]]) -> dict[str, object]:
    counts = {len(row["consumed_pairs"]) for row in rows}
    ranges = {(row["first_pair"], row["last_pair"]) for row in rows}
    if len(counts) != 1 or len(ranges) != 1:
        raise ValueError("repeated direct packet observations disagree")
    return {"pair_count": counts.pop(), "pair_range": list(ranges.pop()), "observations": len(rows)}


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--inline", type=Path, required=True)
    parser.add_argument("--alternate", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--replace", action="store_true")
    args = parser.parse_args()
    markdown = args.output.with_suffix(".md")
    if (args.output.exists() or markdown.exists()) and not args.replace:
        raise FileExistsError(args.output)
    inline = json.loads(args.inline.read_text(encoding="utf-8"))
    alternate = json.loads(args.alternate.read_text(encoding="utf-8"))
    inline_batches = direct_batches(inline)
    alternate_batches = direct_batches(alternate)
    shared = sorted(route_headers(inline, "inline") & route_headers(alternate, "alternate"))
    comparisons = []
    for header in shared:
        if header not in inline_batches or header not in alternate_batches:
            continue
        left = summarize(inline_batches[header])
        right = summarize(alternate_batches[header])
        comparisons.append({"header": header, "inline": left, "alternate": right,
                            "pair_count_delta": right["pair_count"] - left["pair_count"]})
    report = {
        "scope": "same-header direct map packet batches observed inline in run003 and alternate in run035",
        "inline_inventory": str(args.inline), "alternate_inventory": str(args.alternate),
        "comparisons": comparisons,
        "qualification": (
            "Each row compares the first directly entered transform batch for the same immutable packet header. "
            "Pair counts are source-coordinate consumption counts, not face counts or a global terrain complexity measure. "
            "The alternate route is dynamically proven, but these captures do not isolate physical distance from other "
            "map transition/control state, so the comparison establishes a live geometry variant rather than distance-only LOD."
        ),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# Map packet inline/alternate geometry comparison", "", report["qualification"], "",
             "| Packet header | Inline pairs | Alternate pairs | Delta | Inline pair range | Alternate pair range |",
             "| --- | ---: | ---: | ---: | --- | --- |"]
    for row in comparisons:
        left, right = row["inline"], row["alternate"]
        lines.append(f"| `{row['header']}` | {left['pair_count']} | {right['pair_count']} | "
                     f"{row['pair_count_delta']:+d} | `{left['pair_range'][0]}`--`{left['pair_range'][1]}` | "
                     f"`{right['pair_range'][0]}`--`{right['pair_range'][1]}` |")
    lines.append("")
    markdown.write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"same_header_comparisons": len(comparisons), "output": str(args.output)}))


if __name__ == "__main__":
    main()
