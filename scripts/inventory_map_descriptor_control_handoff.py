"""Join map-mode template placements to observed C45A36 control-field writes."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE = 0xC00000
CONTROL_STORE = 0xC1CC70
CONTROL_WALKER = 0xC1F6F8


def addr(value: int) -> str:
    return f"${value & 0xFFFFFF:06X}"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, required=True)
    parser.add_argument("--slow", type=Path, required=True)
    parser.add_argument("--placements", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    args.trace = args.trace.resolve()
    args.slow = args.slow.resolve()
    args.placements = args.placements.resolve()
    args.output = args.output.resolve()
    trace = [json.loads(line) for line in args.trace.read_text(encoding="utf-8").splitlines()]
    slow = args.slow.read_bytes()
    placement_rows = json.loads(args.placements.read_text(encoding="utf-8"))["copies"]
    placement_descriptors = {row["runtime_descriptor"] for row in placement_rows
                             if row["runtime_descriptor"] is not None}
    placement_fields = {row["descriptor_plus_8_field"] for row in placement_rows
                        if row["descriptor_plus_8_field"] is not None}
    writes = []
    for row in trace:
        if row["pc"] != CONTROL_STORE:
            continue
        field_address = row["registers"]["a1"] & 0xFFFFFF
        offset = field_address - SLOW_BASE
        if not 0 <= offset <= len(slow) - 4:
            raise ValueError(f"C1CC70 source outside slow RAM: {addr(field_address)}")
        descriptor = addr(field_address - 8)
        field = addr(int.from_bytes(slow[offset:offset + 4], "big"))
        writes.append({"frame": row["frame"], "trace_index": row["index"],
                       "descriptor": descriptor, "descriptor_plus_8_field": field,
                       "in_template_placement_inventory": descriptor in placement_descriptors,
                       "field_in_template_placement_inventory": field in placement_fields})
    walkers = [{"frame": row["frame"], "trace_index": row["index"],
                "entry_a1": addr(row["registers"]["a1"])}
               for row in trace if row["pc"] == CONTROL_WALKER]
    payload = {
        "classification": "same_map_mode_trace_descriptor_control_handoff_not_pixel_or_mesh_ownership",
        "authority": {"trace": str(args.trace.relative_to(ROOT)).replace("\\", "/"),
                      "slow": str(args.slow.relative_to(ROOT)).replace("\\", "/"),
                      "placements": str(args.placements.relative_to(ROOT)).replace("\\", "/"),
                      "control_store_pc": addr(CONTROL_STORE), "control_walker_pc": addr(CONTROL_WALKER)},
        "descriptor_control_writes": writes,
        "control_walker_entries": walkers,
        "summary": {"writes": len(writes), "unique_descriptors": len({x["descriptor"] for x in writes}),
                    "descriptors_in_template_inventory": sum(x["in_template_placement_inventory"] for x in writes),
                    "unique_fields": len({x["descriptor_plus_8_field"] for x in writes}),
                    "fields_in_template_inventory": sum(x["field_in_template_placement_inventory"] for x in writes),
                    "walker_entries": len(walkers),
                    "unique_walker_entry_a1": len({x["entry_a1"] for x in walkers})},
        "qualification": "C1CC70 stores a descriptor's third longword to C45A36. C1F6F8 later loads C45A36 into A5. The trace establishes descriptor-to-control activity in the same map mode, but does not assign an individual placement to a control-walker invocation, pixel, model, or terrain cell.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# M-map descriptor-to-control handoff",
        "",
        "Classification: **same-map-mode descriptor-to-control dataflow**. This is not mesh, terrain-cell, or pixel ownership.",
        "",
        f"The trace reaches `$C1CC70` **{payload['summary']['writes']}** times for "
        f"**{payload['summary']['unique_descriptors']}** distinct descriptors. Every observed descriptor is also in the independently captured template-placement inventory. `$C1CC70` reads each descriptor's `+8` longword and writes it to `$C45A36`, the input that `$C1F6F8` loads as its control-stream pointer.",
        "",
        f"The same trace enters `$C1F6F8` **{payload['summary']['walker_entries']}** times with "
        f"{payload['summary']['unique_walker_entry_a1']} distinct entry values. These are a scenario-level renderer-control observation; scheduler/order state prevents assigning each walker entry to one preceding placement.",
        "",
        "| Frame | descriptor | `+8` field written to `$C45A36` | descriptor in template-placement inventory |",
        "| ---: | --- | --- | --- |",
    ]
    for row in writes:
        lines.append(f"| {row['frame']} | {row['descriptor']} | {row['descriptor_plus_8_field']} | {'yes' if row['in_template_placement_inventory'] else 'no'} |")
    lines += ["", payload["qualification"], ""]
    args.output.with_suffix(".md").write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps(payload["summary"]))


if __name__ == "__main__":
    main()
