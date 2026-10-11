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
STREAM_GATE = 0xC2AF40
STREAM_START = 0xC2AF46
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


def signed_long(data: bytes, absolute: int) -> int:
    offset = absolute - 0xC00000
    return int.from_bytes(data[offset:offset + 4], "big", signed=True)


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
        f"- Direct `$C2AF00` packet entries: {report['direct_packet_entries']}",
        f"- Direct inline/alternate selections: {report['packet_stream_selection']['inline']} inline, "
        f"{report['packet_stream_selection']['alternate']} alternate",
        f"- Completed transform batches at `$C2AFE2`: {report['transform_batches']}",
        f"- Exact immutable coordinate pairs consumed: {report['consumed_pairs']}",
        "",
        "## Transform batches",
        "",
        "| Trace frame | Pair-address range | Direct packet header | Pairs | `$C2FF48` before next pair transform |",
        "| ---: | --- | --- | ---: | ---: |",
    ]
    for row in report["batches"]:
        packet = (f"`{row['packet']} / {row['first_longword']}`"
                  if row["packet"] else "not entered directly in this batch")
        lines.append(
            f"| {row['frame']} | `{row['first_pair']}`--`{row['last_pair']}` | {packet} | "
            f"{len(row['consumed_pairs'])} | {row['polygon_submissions']} |")
    lines.extend([
        "",
        "The JSON companion retains every exact consumed signed pair and its static address.",
        "",
        "## Direct packet stream selections",
        "",
        "`$C2AF40` chooses the inline source at `header + 4` when `D7` is zero, otherwise the header longword.",
        "",
        "| Frame | Header | `A4` live-base pointer | `4(A4)` live term | Inline stream | Header pointer | `D7` | Selected stream | Route |",
        "| ---: | --- | --- | ---: | --- | --- | ---: | --- | --- |",
    ])
    for row in report["packet_streams"]:
        lines.append(
            f"| {row['frame']} | `{row['header']}` | `{row['live_base']}` | "
            f"{row['live_term']} | `{row['inline_stream']}` | "
            f"`{row['alternate_stream']}` | {row['d7']} | `{row['selected_stream']}` | "
            f"{row['route']} |")
    lines.extend([
        "",
        "`A4` is captured at the direct `$C2AF00` entry, before the routine reads `4(A4)` "
        "as its per-packet projection-origin input.  The table records that live dataflow "
        "without treating the value as a packet placement record or a global map coordinate.",
        "",
        "The same coordinate pairs are available as grouped `l` primitives in the "
        "[OBJ inspection export](../exports/run003_m_map_static_pair_batches.obj) for run003 only. "
        "It writes `(source_x, 0, source_y)` solely as a viewer convention, with no faces, "
        "path closure, global placement, or game-axis semantics inferred.",
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
    entries = {index for index, row in enumerate(trace)
               if row["pc"] == ENTRY and index and trace[index - 1]["pc"] == ENTRY_PRELUDE}
    packet_streams = []
    for entry_index in sorted(entries):
        entry = trace[entry_index]
        header = entry["registers"]["a3"] & 0xFFFFFF
        if not SOURCE_START <= header <= SOURCE_END - 8:
            raise ValueError(f"packet header outside segment {SOURCE_SEGMENT}: {address(header)}")
        end = next((cursor for cursor in sorted(entries) if cursor > entry_index), len(trace))
        gate_index = next((cursor for cursor in range(entry_index, end)
                           if trace[cursor]["pc"] == STREAM_GATE), None)
        stream_index = next((cursor for cursor in range(entry_index, end)
                             if trace[cursor]["pc"] == STREAM_START), None)
        if gate_index is None or stream_index is None:
            raise ValueError(f"packet {address(header)} lacks C2AF40/C2AF46 stream selection")
        alternate = long(slow, header)
        live_base = entry["registers"]["a4"] & 0xFFFFFF
        if not 0xC00000 <= live_base <= 0xC7FFFB:
            raise ValueError(f"packet {address(header)} has live base outside slow RAM: "
                             f"{address(live_base)}")
        selected = trace[stream_index]["registers"]["a3"] & 0xFFFFFF
        inline = header + 4
        if selected == inline:
            route = "inline"
        elif selected == alternate:
            route = "alternate"
        else:
            raise ValueError(f"packet {address(header)} selected unexpected stream {address(selected)}")
        packet_streams.append({"frame": entry["frame"], "trace_index": entry["index"],
                               "header": address(header), "inline_stream": address(inline),
                               "alternate_stream": address(alternate),
                               "live_base": address(live_base),
                               "live_term": signed_long(slow, live_base + 4),
                               "d7": trace[gate_index]["registers"]["d7"] & 0xFFFF,
                               "selected_stream": address(selected), "route": route})
    stages = [index for index, row in enumerate(trace) if row["pc"] == DISPLAY_STAGE]
    batches = []
    previous_stage = 0
    for item, stage_index in enumerate(stages):
        start = previous_stage
        end = stage_index + 1
        direct_entry = next((cursor for cursor in range(start, end) if cursor in entries), None)
        entry = trace[direct_entry] if direct_entry is not None else None
        source = entry["registers"]["a3"] & 0xFFFFFF if entry else None
        if source is not None and not SOURCE_START <= source <= SOURCE_END - 8:
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
        if not pairs:
            previous_stage = end
            continue
        next_pair = next((cursor for cursor in range(end, len(trace))
                          if trace[cursor]["pc"] == PAIR_X), len(trace))
        batches.append({
            "trace_index": trace[stage_index]["index"],
            "frame": trace[stage_index]["frame"],
            "source_segment": SOURCE_SEGMENT,
            "packet": address(source) if source is not None else None,
            "first_longword": f"${long(slow, source):08X}" if source is not None else None,
            "following_words": ([f"${word(slow, source + offset):04X}" for offset in (4, 6)]
                                if source is not None else None),
            "consumed_pairs": pairs,
            "first_pair": pairs[0]["address"],
            "last_pair": pairs[-1]["address"],
            "display_stage_return": address(trace[stage_index]["registers"]["a3"]),
            "polygon_submissions": sum(row["pc"] == POLYGON_SUBMIT
                                       for row in trace[end:next_pair]),
        })
        previous_stage = end
    report = {
        "scope": "completed C2AF9C/C2AF9E transform batches in the bounded map-page trace",
        "trace": str(args.trace),
        "slow": str(args.slow),
        "source_segment": SOURCE_SEGMENT,
        "source_range": f"{address(SOURCE_START)}-{address(SOURCE_END - 1)}",
        "direct_packet_entries": len(packet_streams),
        "packet_stream_selection": {"inline": sum(row["route"] == "inline" for row in packet_streams),
                                    "alternate": sum(row["route"] == "alternate" for row in packet_streams)},
        "packet_streams": packet_streams,
        "transform_batches": len(batches),
        "consumed_pairs": sum(len(row["consumed_pairs"]) for row in batches),
        "batches": batches,
        "qualification": (
            "Each row ends at a traced C2AFE2 display-stage entry and retains the preceding C2AF9C/C2AF9E "
            "fixed-point source-pair reads since the prior display-stage entry. Every such pair is in verified "
            "original segment 68. Only batches with an observed direct C2AEFC-to-C2AF00 entry expose a packet header; "
            "the first batch begins after the trace starts. C4BFxx, C4B9xx, and C4B3xx outputs are mutable workspaces and are not "
            "exported as source geometry. This is an input-to-renderer inventory for the prepared map page, not "
            "a complete terrain mesh, a global coordinate system, or a coastline-pixel ownership map."
        ),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    write_report(args.output, report)
    print(json.dumps({key: report[key] for key in ("direct_packet_entries", "transform_batches", "consumed_pairs")}))


if __name__ == "__main__":
    main()
