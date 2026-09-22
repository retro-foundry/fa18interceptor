"""Inventory immutable pair packets entering the map polygon transform path."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ENTRY_PRELUDE = 0xC2AEFC
ENTRY = 0xC2AF00
PAIR_X = 0xC2AF9C
PAIR_Y = 0xC2AF9E
DISPLAY_STAGE = 0xC2AFE2
POLYGON_SUBMIT = 0xC2FF48
SOURCE_SEGMENT = 68
SOURCE_START = 0xC42CA8
SOURCE_END = 0xC444F8


def address(value: int) -> str:
    return f"${value & 0xFFFFFF:06X}"


def word(data: bytes, absolute: int, *, signed: bool = False) -> int:
    offset = absolute - 0xC00000
    return int.from_bytes(data[offset:offset + 2], "big", signed=signed)


def long(data: bytes, absolute: int) -> int:
    offset = absolute - 0xC00000
    return int.from_bytes(data[offset:offset + 4], "big")


def write_report(output: Path, report: dict[str, object]) -> None:
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# Map polygon static-packet inventory",
        "",
        "Classification: **bounded immutable input to map-page polygon transforms**.",
        "",
        report["qualification"],
        "",
        f"- Original source segment: {report['source_segment']} `{report['source_range']}`",
        f"- Direct `$C2AF00` packet entries: {report['packet_entries']}",
        f"- Entries completing at `$C2AFE2`: {report['display_stage_returns']}",
        f"- Exact immutable coordinate pairs consumed: {report['consumed_pairs']}",
        "",
        "## Packets",
        "",
        "| Trace frame | Static packet | Raw first longword | Following words | Consumed coordinate-pairs | `$C2FF48` before next packet |",
        "| ---: | --- | --- | --- | ---: | ---: |",
    ]
    for row in report["packets"]:
        lines.append(
            f"| {row['frame']} | `{row['packet']}` | `{row['first_longword']}` | "
            f"`{row['following_words']}` | {len(row['consumed_pairs'])} | {row['polygon_submissions']} |")
    lines.extend([
        "",
        "The JSON companion retains every exact consumed signed pair and its static address.",
        "",
    ])
    output.with_suffix(".md").write_text("\n".join(lines), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, required=True)
    parser.add_argument("--slow", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--replace", action="store_true",
                        help="replace existing generated JSON and Markdown")
    args = parser.parse_args()
    markdown = args.output.with_suffix(".md")
    if (args.output.exists() or markdown.exists()) and not args.replace:
        raise FileExistsError(args.output)
    if args.replace:
        for path in (args.output, markdown):
            if path.exists():
                path.unlink()
    trace = [json.loads(line) for line in args.trace.read_text(encoding="utf-8").splitlines()]
    slow = args.slow.read_bytes()
    if len(slow) != 0x80000:
        raise ValueError(f"expected a 512 KiB slow-RAM image, got {len(slow)} bytes")
    entries = [index for index, row in enumerate(trace)
               if row["pc"] == ENTRY and index and trace[index - 1]["pc"] == ENTRY_PRELUDE]
    packets = []
    for item, start in enumerate(entries):
        end = entries[item + 1] if item + 1 < len(entries) else len(trace)
        entry = trace[start]
        source = entry["registers"]["a3"] & 0xFFFFFF
        if not SOURCE_START <= source <= SOURCE_END - 8:
            raise ValueError(f"packet source outside segment {SOURCE_SEGMENT}: {address(source)}")
        pairs = []
        for cursor in range(start, end - 1):
            row = trace[cursor]
            if row["pc"] != PAIR_X or trace[cursor + 1]["pc"] != PAIR_Y:
                continue
            pair_source = row["registers"]["a3"] & 0xFFFFFF
            if not SOURCE_START <= pair_source <= SOURCE_END - 4:
                raise ValueError(f"pair source outside segment {SOURCE_SEGMENT}: {address(pair_source)}")
            pairs.append({"address": address(pair_source),
                          "xy": [word(slow, pair_source, signed=True),
                                 word(slow, pair_source + 2, signed=True)]})
        stage = next((trace[cursor] for cursor in range(start, end)
                      if trace[cursor]["pc"] == DISPLAY_STAGE), None)
        packets.append({
            "trace_index": entry["index"],
            "frame": entry["frame"],
            "source_segment": SOURCE_SEGMENT,
            "packet": address(source),
            "first_longword": f"${long(slow, source):08X}",
            "following_words": [f"${word(slow, source + offset):04X}" for offset in (4, 6)],
            "consumed_pairs": pairs,
            "display_stage_return": (address(stage["registers"]["a3"])
                                     if stage is not None else None),
            "polygon_submissions": sum(row["pc"] == POLYGON_SUBMIT
                                       for row in trace[start:end]),
        })
    report = {
        "scope": "direct C2AEFC-to-C2AF00 entries in the bounded map-page trace",
        "trace": str(args.trace),
        "slow": str(args.slow),
        "source_segment": SOURCE_SEGMENT,
        "source_range": f"{address(SOURCE_START)}-{address(SOURCE_END - 1)}",
        "packet_entries": len(packets),
        "display_stage_returns": sum(row["display_stage_return"] is not None for row in packets),
        "consumed_pairs": sum(len(row["consumed_pairs"]) for row in packets),
        "packets": packets,
        "qualification": (
            "Each row begins at the traced direct C2AEFC-to-C2AF00 entry with A3 in verified original segment 68. "
            "The listed pairs are only those read at the C2AF9C/C2AF9E fixed-point transform pair before the "
            "next direct packet entry. C4BFxx, C4B9xx, and C4B3xx outputs are mutable workspaces and are not "
            "exported as source geometry. This is an input-to-renderer inventory for the prepared map page, not "
            "a complete terrain mesh, a global coordinate system, or a coastline-pixel ownership map."
        ),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    write_report(args.output, report)
    print(json.dumps({key: report[key] for key in ("packet_entries", "display_stage_returns", "consumed_pairs")}))


if __name__ == "__main__":
    main()
