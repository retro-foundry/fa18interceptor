"""Inventory the traced static selector groups that feed workspace templates.

The report follows each `$C1D3F4` helper invocation through its `$C42390`
relative-offset lookup and records the first `$C1D442` stream byte when the
branch reaches the template-copy path.  It intentionally does not assign map,
LOD, or global-coordinate semantics to the selector values.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TRACE = ROOT / "build/run033_placement_bulk_404_trace/trace.jsonl"
SLOW = ROOT / "build/run033_placement_bulk_404_trace/slow.bin"
SLOW_BASE = 0xC00000
SELECTOR_ENTRY_PC = 0xC1D3F4
TABLE_TARGET_READ_PC = 0xC1D406
BIT_TEST_PC = 0xC1D426
STREAM_START_PC = 0xC1D442
ROW_SEARCH_CALL_PC = 0xC1D432
ROW_SEARCH_RETURN_PC = 0xC1D436


def address(value: int) -> str:
    return f"${value:06X}"


def load_trace(trace_path: Path) -> list[dict]:
    return [json.loads(line) for line in trace_path.read_text(encoding="utf-8").splitlines()]


def inventory(trace_path: Path, slow_path: Path) -> list[dict]:
    trace = load_trace(trace_path)
    slow = slow_path.read_bytes()
    starts = [index for index, row in enumerate(trace) if row["pc"] == SELECTOR_ENTRY_PC]
    rows = []
    for start, end in zip(starts, starts[1:] + [len(trace)]):
        entry = trace[start]
        packet = trace[start:end]
        table_read = next((row for row in packet if row["pc"] == TABLE_TARGET_READ_PC), None)
        bit_test = next((row for row in packet if row["pc"] == BIT_TEST_PC), None)
        stream = next((row for row in packet if row["pc"] == STREAM_START_PC), None)
        row_search_call = next((row for row in packet if row["pc"] == ROW_SEARCH_CALL_PC), None)
        row_search_return = next((row for row in packet if row["pc"] == ROW_SEARCH_RETURN_PC), None)
        if table_read is None:
            continue
        input_index = entry["registers"]["d0"] & 0xFFFF
        group_address = table_read["registers"]["a0"]
        group_header = int.from_bytes(slow[group_address - SLOW_BASE:group_address - SLOW_BASE + 2], "big")
        rows.append({
            "frame": entry.get("frame"),
            "trace_index": entry["index"],
            "workspace_band": address(entry["registers"]["a3"]),
            "table_base": "$C42390",
            "selector_index": input_index,
            "selector_table_word_address": address(0xC42390 + input_index * 2),
            "group_record": address(group_address),
            "bit_gate_taken": bool(bit_test and bit_test["next_pc"] != 0xC1D4DA),
            "row_threshold_count": group_header >> 1 if row_search_call else None,
            "live_row_search_key": f"${row_search_call['registers']['d1'] & 0xFFFF:04X}" if row_search_call else None,
            "selected_row_index": row_search_return["registers"]["d0"] & 0xFFFF if row_search_return else None,
            "first_template_stream_byte": address(stream["registers"]["a5"]) if stream else None,
        })
    return rows


def markdown(rows: list[dict], trace_path: Path) -> str:
    reached = [row for row in rows if row["first_template_stream_byte"] is not None]
    lines = [
        "# Static template-selector groups",
        "",
        "Classification: **scenario-backed static selector dataflow**.  This records "
        "which `$C42390` table entries selected template streams in one bounded replay; "
        "it is not a decoded world grid, an LOD table, or a terrain mesh.",
        "",
        f"Authority: `{trace_path.relative_to(ROOT).as_posix()}` and its slow-RAM "
        "snapshot.  At `$C1D400` "
        "the helper doubles `D0`; `$C1D402` adds the signed word at `$C42390+D0*2` "
        "to that table base; `$C1D406` tests the resulting static group record.  On the "
        "observed accepted path, `$C1D426` passes a bit gate, `$C1D43A` resolves a stream "
        "pointer, and `$C1D442` begins the byte stream consumed by the workspace copier.  "
        "Between those steps, `$C1D4E4-$C1D50C` binary-searches the group's static sorted "
        "word list using live `D1`, returning the pointer-table index used at `$C1D43A`.",
        "",
        f"The trace executes **{len(rows)}** group selections; **{len(reached)}** reach a "
        "template stream before returning.  Entries without a stream either take a gate/exit "
        "path in this trace or lack an observed stream before the next helper entry.",
        "",
        "| Frame | workspace band | selector index | table word | static group record | row thresholds / live key / selected index | first stream byte |",
        "| ---: | --- | ---: | --- | --- | --- | --- |",
    ]
    for row in rows:
        stream = row["first_template_stream_byte"] or ""
        frame = row["frame"] if row["frame"] is not None else ""
        if row["row_threshold_count"] is None:
            row_selection = ""
        else:
            row_selection = (f"{row['row_threshold_count']} / {row['live_row_search_key']} / "
                             f"{row['selected_row_index']}")
        lines.append(
            f"| {frame} | {row['workspace_band']} | `${row['selector_index']:04X}` | "
            f"{row['selector_table_word_address']} | {row['group_record']} | "
            f"{row_selection} | {stream} |"
        )
    lines += [
        "",
        "The static group records and their selected streams are a stronger upstream boundary "
        "than the mutable workspace: the same path later reaches the static-to-workspace copy "
        "contract.  The selector index is not yet tied to a player world coordinate, distance, "
        "course cell, or visual LOD state, so none of those meanings are assigned here.  The "
        "two-stage static selector plus live search key resembles a two-axis lookup structure, "
        "but resemblance is not sufficient to call it a world-map grid.",
        "",
        "See [the workspace-template inventory](workspace_template_copies.md) for the copied "
        "records and their later placement outputs.",
        "",
    ]
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--trace-directory", type=Path, default=TRACE.parent,
                        help="directory containing trace.jsonl and slow.bin")
    parser.add_argument("--output-suffix", default="",
                        help="suffix appended to both output basenames")
    args = parser.parse_args()
    trace_directory = args.trace_directory.resolve()
    trace_path = trace_directory / "trace.jsonl"
    slow_path = trace_directory / "slow.bin"
    rows = inventory(trace_path, slow_path)
    payload = {
        "authority": {
            "trace": str(trace_path.relative_to(ROOT)).replace("\\", "/"),
            "slow_snapshot": str(slow_path.relative_to(ROOT)).replace("\\", "/"),
            "selector_entry_pc": address(SELECTOR_ENTRY_PC),
            "static_table_base": "$C42390",
            "stream_start_pc": address(STREAM_START_PC),
        },
        "classification": "traced_static_template_selector_not_world_grid_or_lod_table",
        "groups": rows,
    }
    json_path = ROOT / f"analysis/data/static_template_selector_groups{args.output_suffix}.json"
    markdown_path = ROOT / f"analysis/data/static_template_selector_groups{args.output_suffix}.md"
    json_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    markdown_path.write_text(markdown(rows, trace_path), encoding="utf-8")
    streams = sum(row["first_template_stream_byte"] is not None for row in rows)
    print(f"wrote {json_path.relative_to(ROOT)} and {markdown_path.relative_to(ROOT)} "
          f"({len(rows)} groups, {streams} streams)")


if __name__ == "__main__":
    main()
