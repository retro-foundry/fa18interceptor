"""Inventory traced segment-65 control bytes that select static template groups."""
from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TRACE = ROOT / "build/run033_placement_bulk_404_trace/trace.jsonl"
SLOW = ROOT / "build/run033_placement_bulk_404_trace/slow.bin"
SLOW_BASE = 0xC00000
CONTROL_READ_PC = 0xC1D338
SELECTOR_CALL_PC = 0xC1D3B4


def address(value: int) -> str:
    return f"${value:06X}"


def main() -> None:
    trace = [json.loads(line) for line in TRACE.read_text(encoding="utf-8").splitlines()]
    slow = SLOW.read_bytes()
    starts = [index for index, row in enumerate(trace) if row["pc"] == CONTROL_READ_PC]
    rows = []
    for start, end in zip(starts, starts[1:] + [len(trace)]):
        control = trace[start]
        call = next((row for row in trace[start:end] if row["pc"] == SELECTOR_CALL_PC), None)
        control_address = control["registers"]["a0"]
        rows.append({
            "frame": control["frame"],
            "trace_index": control["index"],
            "static_control_byte_address": address(control_address),
            "static_control_byte": f"${slow[control_address - SLOW_BASE]:02X}",
            "workspace_band": address(control["registers"]["a3"]),
            "selector_call_reached": call is not None,
            "static_group_selector_index": f"${call['registers']['d0'] & 0xFFFF:04X}" if call else None,
            "live_row_term": f"${call['registers']['d1'] & 0xFFFF:04X}" if call else None,
        })
    payload = {
        "authority": {
            "trace": str(TRACE.relative_to(ROOT)).replace("\\", "/"),
            "slow_snapshot": str(SLOW.relative_to(ROOT)).replace("\\", "/"),
            "control_read_pc": address(CONTROL_READ_PC),
            "selector_call_pc": address(SELECTOR_CALL_PC),
        },
        "classification": "traced_segment65_control_stream_to_static_group_selector_not_world_grid",
        "rows": rows,
    }
    json_path = ROOT / "analysis/data/workspace_band_selector_stream.json"
    markdown_path = ROOT / "analysis/data/workspace_band_selector_stream.md"
    json_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# Segment-65 workspace-band selector stream",
        "",
        "Classification: **scenario-backed static-control to group-selector dataflow**. "
        "The byte stream controls which static template groups are considered for mutable "
        "workspace bands; it is not a decoded world grid, world coordinate table, or LOD table.",
        "",
        "Authority: `build/run033_placement_bulk_404_trace/trace.jsonl`.  `$C1D338` reads "
        "a byte from the byte-stable, relocation-free segment-65 control stream.  Its local "
        "path maps through `$C411F0` and `$C1D764`, applies the observed live bounds, and on "
        "success calls `$C1D3F4` at `$C1D3B4`.  The call-time `D0` is the static-group selector "
        "index later used with `$C42390`; `D1` remains a live row term.",
        "",
        f"The bounded trace reads **{len(rows)}** control items; "
        f"**{sum(row['selector_call_reached'] for row in rows)}** reach the group selector.",
        "",
        "| Frame | static control byte | byte value | workspace band | reaches group selector | selector index | live row term |",
        "| ---: | --- | --- | --- | --- | --- | --- |",
    ]
    for row in rows:
        lines.append(
            f"| {row['frame']} | {row['static_control_byte_address']} | {row['static_control_byte']} | "
            f"{row['workspace_band']} | {row['selector_call_reached']} | "
            f"{row['static_group_selector_index'] or ''} | {row['live_row_term'] or ''} |"
        )
    lines += [
        "",
        "The same raw control-byte range yields different selector-index ranges in the two "
        "observed update phases, while the per-call row term is live state.  That establishes "
        "a static-control plus live-state selection stage, not a distance formula or a map-cell "
        "coordinate system.  See [static selector groups](static_template_selector_groups.md) "
        "for the next static-table lookup.",
        "",
    ]
    markdown_path.write_text("\n".join(lines), encoding="utf-8")
    print(f"wrote {json_path.relative_to(ROOT)} and {markdown_path.relative_to(ROOT)} "
          f"({len(rows)} control items)")


if __name__ == "__main__":
    main()
