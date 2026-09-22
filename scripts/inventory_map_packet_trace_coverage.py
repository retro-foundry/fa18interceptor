"""Summarize immutable segment-68 packet data exercised across map traces."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


SEGMENT_START = 0xC42CA8
SEGMENT_END = 0xC444F8


def address(value: int) -> str:
    return f"${value:06X}"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--inventory", type=Path, action="append", required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--replace", action="store_true")
    args = parser.parse_args()
    markdown = args.output.with_suffix(".md")
    if (args.output.exists() or markdown.exists()) and not args.replace:
        raise FileExistsError(args.output)
    headers, streams, pairs = set(), set(), set()
    source_rows = []
    for path in args.inventory:
        report = json.loads(path.read_text(encoding="utf-8"))
        local_headers = {row["header"] for row in report["packet_streams"]}
        local_streams = {row["selected_stream"] for row in report["packet_streams"]}
        local_pairs = {row["address"] for batch in report["batches"] for row in batch["consumed_pairs"]}
        headers |= local_headers
        streams |= local_streams
        pairs |= local_pairs
        source_rows.append({"inventory": str(path), "direct_headers": len(local_headers),
                            "selected_streams": len(local_streams), "unique_pairs": len(local_pairs)})
    pair_bytes = set()
    for item in pairs:
        start = int(item[1:], 16)
        if not SEGMENT_START <= start <= SEGMENT_END - 4:
            raise ValueError(f"pair outside segment 68: {item}")
        pair_bytes.update(range(start, start + 4))
    report = {
        "scope": "immutable segment-68 map packet inputs exercised by listed bounded M-map inventories",
        "segment": {"start": address(SEGMENT_START), "end_exclusive": address(SEGMENT_END), "bytes": SEGMENT_END - SEGMENT_START},
        "sources": source_rows,
        "unique_direct_headers": sorted(headers), "unique_selected_streams": sorted(streams),
        "unique_pair_addresses": sorted(pairs), "unique_pair_bytes": len(pair_bytes),
        "pair_byte_fraction": len(pair_bytes) / (SEGMENT_END - SEGMENT_START),
        "qualification": ("Coverage counts only exact pair payload bytes reached in these bounded map traces. "
                          "It excludes packet headers, control words, unselected alternate streams, and all untraced "
                          "segment data; it is not complete terrain-map coverage."),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# Segment-68 M-map packet trace coverage", "", report["qualification"], "",
             f"- Segment: `{report['segment']['start']}`--`{address(SEGMENT_END - 1)}` ({report['segment']['bytes']} bytes)",
             f"- Unique direct packet headers: {len(headers)}",
             f"- Unique selected stream starts: {len(streams)}",
             f"- Unique consumed coordinate pairs: {len(pairs)}",
             f"- Unique exact pair-payload bytes: {len(pair_bytes)} ({report['pair_byte_fraction']:.2%} of segment bytes)", "",
             "| Inventory | Direct headers | Selected streams | Unique pairs |",
             "| --- | ---: | ---: | ---: |"]
    for row in source_rows:
        lines.append(f"| `{row['inventory']}` | {row['direct_headers']} | {row['selected_streams']} | {row['unique_pairs']} |")
    lines.extend(["", "The JSON companion retains every reached header, selected stream, and pair address.", ""])
    markdown.write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"headers": len(headers), "streams": len(streams), "pairs": len(pairs), "pair_bytes": len(pair_bytes)}))


if __name__ == "__main__":
    main()
