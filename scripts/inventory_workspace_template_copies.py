"""Inventory traced static-template copies into the mutable scene workspace.

This report intentionally describes a producer-to-consumer contract, not a
terrain mesh.  It joins `$C1D488` static-stream entries with the later
`$C1DD36` workspace-cell reader in the same bounded trace.
"""
from __future__ import annotations

import json
import struct
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TRACE = ROOT / "build/run033_placement_bulk_404_trace/trace.jsonl"
SLOW = ROOT / "build/run033_placement_bulk_404_trace/slow.bin"
SLOW_BASE = 0xC00000
COPY_PC = 0xC1D488
BUILDER_HEADER_PC = 0xC1DD36


def hex_address(value: int) -> str:
    return f"${value:06X}"


def source_segment(address: int) -> int | None:
    if 0xC42290 <= address < 0xC429C8:
        return 66
    if 0xC429D0 <= address < 0xC42CA0:
        return 67
    return None


def load_trace() -> list[dict]:
    return [json.loads(line) for line in TRACE.read_text(encoding="utf-8").splitlines()]


def first_later_consumer(builders: list[dict], frame: int, cell: int) -> dict | None:
    for row in builders:
        if row["frame"] > frame and row["registers"]["a3"] == cell:
            return row
    return None


def inventory() -> list[dict]:
    trace = load_trace()
    memory = SLOW.read_bytes()
    builders = [row for row in trace if row["pc"] == BUILDER_HEADER_PC]
    rows = []
    for row in trace:
        if row["pc"] != COPY_PC:
            continue
        source = row["registers"]["a5"]
        cell = row["registers"]["a2"]
        offset = source - SLOW_BASE
        header = memory[offset]
        first_word, second_word = struct.unpack_from(">HH", memory, offset + 1)
        consumer = first_later_consumer(builders, row["frame"], cell)
        # C1D48C retains source bit 7, C1D490 stores it in the high byte of
        # the destination word, and C1D492/C1D496 retain source bits 0..6.
        workspace_header = ((header & 0x80) << 8) | (header & 0x7F)
        rows.append({
            "copy_frame": row["frame"],
            "copy_trace_index": row["index"],
            "static_source": hex_address(source),
            "source_segment": source_segment(source),
            "source_header_byte": f"${header:02X}",
            "source_words": [f"${first_word:04X}", f"${second_word:04X}"],
            "workspace_cell": hex_address(cell),
            "workspace_header_word": f"${workspace_header:04X}",
            "observed_later_builder_frame": consumer["frame"] if consumer else None,
            "observed_later_builder_trace_index": consumer["index"] if consumer else None,
        })
    return rows


def markdown(rows: list[dict]) -> str:
    consumed = [row for row in rows if row["observed_later_builder_frame"] is not None]
    lines = [
        "# Traced static-template workspace copies",
        "",
        "Classification: **scenario-backed producer-to-consumer inventory**. "
        "The rows describe static bytes copied into mutable workspace cells; "
        "they are not an extracted terrain mesh, global position table, or LOD table.",
        "",
        "Authority: `build/run033_placement_bulk_404_trace/trace.jsonl` and its "
        "frame-0 slow-RAM snapshot.  `$C1D488` supplies each static header; "
        "`$C1D4BC` copies its following two words; `$C1DD36` is the later cell-header reader.",
        "",
        f"The bounded trace has **{len(rows)}** observed static-entry copies.  "
        f"**{len(consumed)}** are later read at `$C1DD36` before the trace ends.  "
        "An absent later read means only that this three-frame trace did not reach one; "
        "it is not rejection evidence.",
        "",
        "`$C1D48C-$C1D496` transforms the source header byte rather than copying it "
        "directly: source bit 7 becomes destination word bit 15, while source bits "
        "0..6 remain the low seven bits.  The two displayed source words are copied "
        "to the next four workspace bytes by `$C1D4BC`.",
        "",
        "| Copy frame | static source | segment | header -> workspace header | copied words | workspace cell | later `$C1DD36` frame |",
        "| ---: | --- | ---: | --- | --- | --- | ---: |",
    ]
    for row in rows:
        later = "" if row["observed_later_builder_frame"] is None else str(row["observed_later_builder_frame"])
        segment = "unknown" if row["source_segment"] is None else str(row["source_segment"])
        lines.append(
            f"| {row['copy_frame']} | {row['static_source']} | {segment} | "
            f"{row['source_header_byte']} -> {row['workspace_header_word']} | "
            f"{' '.join(row['source_words'])} | {row['workspace_cell']} | {later} |"
        )
    lines += [
        "",
        "Segment 66 is verified at `$C42290-$C429C7`; segment 67 is verified at "
        "`$C429D0-$C42C9F`.  The inventory records only source addresses reached by "
        "the captured path.  It does not claim that either entire segment is terrain data.",
        "",
        "See [the single-entry copy contract](../routines/c1d442_workspace_cell_template_copy.md) "
        "for the instruction-level `$C427C1 -> $C4B270` example and "
        "[the placement builder](../routines/c1dc1c_scene_placement_record_builder.md) "
        "for the downstream coordinate calculation.",
        "",
    ]
    return "\n".join(lines)


def main() -> None:
    rows = inventory()
    payload = {
        "authority": {
            "trace": str(TRACE.relative_to(ROOT)).replace("\\", "/"),
            "slow_snapshot": str(SLOW.relative_to(ROOT)).replace("\\", "/"),
            "copy_pc": hex_address(COPY_PC),
            "later_builder_header_read_pc": hex_address(BUILDER_HEADER_PC),
        },
        "classification": "traced_static_template_to_mutable_workspace_not_map_mesh",
        "copies": rows,
    }
    json_path = ROOT / "analysis/data/workspace_template_copies.json"
    markdown_path = ROOT / "analysis/data/workspace_template_copies.md"
    json_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    markdown_path.write_text(markdown(rows), encoding="utf-8")
    consumed = sum(row["observed_later_builder_frame"] is not None for row in rows)
    print(f"wrote {json_path.relative_to(ROOT)} and {markdown_path.relative_to(ROOT)} "
          f"({len(rows)} copies, {consumed} later builder reads)")


if __name__ == "__main__":
    main()
