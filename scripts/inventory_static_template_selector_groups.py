"""Inventory the traced static selector groups that feed workspace templates.

The report follows each `$C1D3F4` helper invocation through its `$C42390`
relative-offset lookup and records the first `$C1D442` stream byte when the
branch reaches the template-copy path.  It intentionally does not assign map,
LOD, or global-coordinate semantics to the selector values.
"""
from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TRACE = ROOT / "build/run033_placement_bulk_404_trace/trace.jsonl"
SELECTOR_ENTRY_PC = 0xC1D3F4
TABLE_TARGET_READ_PC = 0xC1D406
BIT_TEST_PC = 0xC1D426
STREAM_START_PC = 0xC1D442


def address(value: int) -> str:
    return f"${value:06X}"


def load_trace() -> list[dict]:
    return [json.loads(line) for line in TRACE.read_text(encoding="utf-8").splitlines()]


def inventory() -> list[dict]:
    trace = load_trace()
    starts = [index for index, row in enumerate(trace) if row["pc"] == SELECTOR_ENTRY_PC]
    rows = []
    for start, end in zip(starts, starts[1:] + [len(trace)]):
        entry = trace[start]
        packet = trace[start:end]
        table_read = next((row for row in packet if row["pc"] == TABLE_TARGET_READ_PC), None)
        bit_test = next((row for row in packet if row["pc"] == BIT_TEST_PC), None)
        stream = next((row for row in packet if row["pc"] == STREAM_START_PC), None)
        if table_read is None:
            continue
        input_index = entry["registers"]["d0"] & 0xFFFF
        rows.append({
            "frame": entry["frame"],
            "trace_index": entry["index"],
            "workspace_band": address(entry["registers"]["a3"]),
            "table_base": "$C42390",
            "selector_index": input_index,
            "selector_table_word_address": address(0xC42390 + input_index * 2),
            "group_record": address(table_read["registers"]["a0"]),
            "bit_gate_taken": bool(bit_test and bit_test["next_pc"] != 0xC1D4DA),
            "first_template_stream_byte": address(stream["registers"]["a5"]) if stream else None,
        })
    return rows


def markdown(rows: list[dict]) -> str:
    reached = [row for row in rows if row["first_template_stream_byte"] is not None]
    lines = [
        "# Static template-selector groups",
        "",
        "Classification: **scenario-backed static selector dataflow**.  This records "
        "which `$C42390` table entries selected template streams in one bounded replay; "
        "it is not a decoded world grid, an LOD table, or a terrain mesh.",
        "",
        "Authority: `build/run033_placement_bulk_404_trace/trace.jsonl`.  At `$C1D400` "
        "the helper doubles `D0`; `$C1D402` adds the signed word at `$C42390+D0*2` "
        "to that table base; `$C1D406` tests the resulting static group record.  On the "
        "observed accepted path, `$C1D426` passes a bit gate, `$C1D43A` resolves a stream "
        "pointer, and `$C1D442` begins the byte stream consumed by the workspace copier.",
        "",
        f"The trace executes **{len(rows)}** group selections; **{len(reached)}** reach a "
        "template stream before returning.  Entries without a stream either take a gate/exit "
        "path in this trace or lack an observed stream before the next helper entry.",
        "",
        "| Frame | workspace band | selector index | table word | static group record | bit gate passed | first stream byte |",
        "| ---: | --- | ---: | --- | --- | --- | --- |",
    ]
    for row in rows:
        stream = row["first_template_stream_byte"] or ""
        lines.append(
            f"| {row['frame']} | {row['workspace_band']} | `${row['selector_index']:04X}` | "
            f"{row['selector_table_word_address']} | {row['group_record']} | "
            f"{row['bit_gate_taken']} | {stream} |"
        )
    lines += [
        "",
        "The static group records and their selected streams are a stronger upstream boundary "
        "than the mutable workspace: the same path later reaches the static-to-workspace copy "
        "contract.  The selector index is not yet tied to a player world coordinate, distance, "
        "course cell, or visual LOD state, so none of those meanings are assigned here.",
        "",
        "See [the workspace-template inventory](workspace_template_copies.md) for the copied "
        "records and their later placement outputs.",
        "",
    ]
    return "\n".join(lines)


def main() -> None:
    rows = inventory()
    payload = {
        "authority": {
            "trace": str(TRACE.relative_to(ROOT)).replace("\\", "/"),
            "selector_entry_pc": address(SELECTOR_ENTRY_PC),
            "static_table_base": "$C42390",
            "stream_start_pc": address(STREAM_START_PC),
        },
        "classification": "traced_static_template_selector_not_world_grid_or_lod_table",
        "groups": rows,
    }
    json_path = ROOT / "analysis/data/static_template_selector_groups.json"
    markdown_path = ROOT / "analysis/data/static_template_selector_groups.md"
    json_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    markdown_path.write_text(markdown(rows), encoding="utf-8")
    streams = sum(row["first_template_stream_byte"] is not None for row in rows)
    print(f"wrote {json_path.relative_to(ROOT)} and {markdown_path.relative_to(ROOT)} "
          f"({len(rows)} groups, {streams} streams)")


if __name__ == "__main__":
    main()
