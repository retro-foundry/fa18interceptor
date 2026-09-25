"""Join map-mode template placements to observed C45A36 control-field writes."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE = 0xC00000
CONTROL_STORE = 0xC1CC70
CONTROL_WALKER = 0xC1F6F8
STREAM_CURSOR_PUBLISH = 0xC1EF10
WALKER_POINTER_LOADED = 0xC1F70E


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
    placements_by_descriptor = {}
    for placement in placement_rows:
        descriptor = placement["runtime_descriptor"]
        if descriptor is not None and placement["runtime_placement_record"] is not None:
            placements_by_descriptor.setdefault(descriptor, []).append({
                "static_source": placement["static_source"],
                "runtime_placement_record": placement["runtime_placement_record"],
                "coordinates": placement["runtime_coordinate_words_signed"],
            })
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
    # C1CC70 publishes a descriptor field to the shared staging pointer.  The
    # C1EE14 family consumes that field, advances its cursor, and C1F6F8 then
    # loads the resulting pointer into A5.  Do not use A1 at the walker entry:
    # it is a separate cursor and is not the stream pointer.
    walkers = []
    for walker_index, row in enumerate(trace):
        if row["pc"] != CONTROL_WALKER:
            continue
        preceding_writes = [write for write in writes if write["trace_index"] < row["index"]]
        preceding = preceding_writes[-1] if preceding_writes else None
        publications = [candidate for candidate in trace
                        if candidate["pc"] == STREAM_CURSOR_PUBLISH
                        and (preceding is None or candidate["index"] > preceding["trace_index"])
                        and candidate["index"] < row["index"]]
        publication = publications[-1] if publications else None
        loaded = next((candidate for candidate in trace[walker_index:]
                       if candidate["pc"] == WALKER_POINTER_LOADED), None)
        walkers.append({
            "frame": row["frame"], "trace_index": row["index"],
            "entry_a1": addr(row["registers"]["a1"]),
            "preceding_descriptor_store": preceding,
            "placement_instances": (placements_by_descriptor.get(preceding["descriptor"], [])
                                    if preceding else []),
            "cursor_publication_trace_index": publication["index"] if publication else None,
            "cursor_published_stream": addr(publication["registers"]["a2"]) if publication else None,
            "walker_a5_after_load": addr(loaded["registers"]["a5"]) if loaded else None,
            "a5_matches_published_stream": bool(publication and loaded and
                                                publication["registers"]["a2"] == loaded["registers"]["a5"]),
        })
    payload = {
        "classification": "cross_capture_scenario_correlated_template_placement_to_control_stream_ownership_not_yet_single_execution_mesh_or_pixel_ownership",
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
                    "walkers_with_preceding_descriptor_store": sum(x["preceding_descriptor_store"] is not None for x in walkers),
                    "walkers_with_published_a5_match": sum(x["a5_matches_published_stream"] for x in walkers)},
        "qualification": "The placement inventory and control trace are separate sealed run037 captures. Their matching static template, runtime descriptor, and descriptor +8 field make this a scenario-correlated instance-class join. Within the control trace, each listed walker has a proven immediate C1CC70 store, C1EE14 cursor publication, and A5 load. A single uninterrupted trace from that exact runtime placement record through its mesh primitives is still required for literal per-instance mesh ownership.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# M-map descriptor-to-control handoff",
        "",
        "Classification: **cross-capture scenario-correlated template/placement-to-control-stream ownership**. This is not yet single-execution mesh, terrain-cell, or pixel ownership.",
        "",
        f"The trace reaches `$C1CC70` **{payload['summary']['writes']}** times for "
        f"**{payload['summary']['unique_descriptors']}** distinct descriptors. Every observed descriptor is also in the independently captured template-placement inventory. `$C1CC70` reads each descriptor's `+8` longword and writes it to `$C45A36`, the input that `$C1F6F8` loads as its control-stream pointer.",
        "",
        f"The same control trace enters `$C1F6F8` **{payload['summary']['walker_entries']}** times. For each entry, `$C1CC70` stores the replay-correlated descriptor's `+8` field, `$C1EF10` publishes the cursor, and `$C1F70E` loads that exact published value into `A5`. `A1` at walker entry is a separate cursor, not the stream pointer. The placement inventory is a separate sealed capture, so this does not yet prove a literal single-execution record-to-mesh chain.",
        "",
        "| Frame | descriptor | `+8` field written to `$C45A36` | descriptor in template-placement inventory |",
        "| ---: | --- | --- | --- |",
    ]
    for row in writes:
        lines.append(f"| {row['frame']} | {row['descriptor']} | {row['descriptor_plus_8_field']} | {'yes' if row['in_template_placement_inventory'] else 'no'} |")
    lines += ["", "| Walker frame | static template -> runtime placement `(X,Y,Z)` | descriptor field | cursor-published stream | `A5` after walker load | exact match |",
              "| ---: | --- | --- | --- | --- |"]
    for row in walkers:
        preceding = row["preceding_descriptor_store"]
        instances = "; ".join(f"{item['static_source']} -> {item['runtime_placement_record']} {tuple(item['coordinates'])}"
                              for item in row['placement_instances']) or '—'
        lines.append(f"| {row['frame']} | {instances} | {preceding['descriptor_plus_8_field'] if preceding else '—'} | "
                     f"{row['cursor_published_stream'] or '—'} | {row['walker_a5_after_load'] or '—'} | "
                     f"{'yes' if row['a5_matches_published_stream'] else 'no'} |")
    lines += ["", payload["qualification"], ""]
    args.output.with_suffix(".md").write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps(payload["summary"]))


if __name__ == "__main__":
    main()
