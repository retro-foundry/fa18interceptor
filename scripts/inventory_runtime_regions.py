"""Generate a conservative code, data, BSS, and active-display region manifest.

The original Hunk kind is the primary boundary.  A CODE Hunk is never silently
reclassified as data merely because a capture has not entered it; the secondary
classification below is only positive evidence from execution and reconstructed
references.  Known inline data is recorded separately so its parent CODE Hunk
remains executable.
"""
from __future__ import annotations

import argparse
import json
from collections import Counter
from pathlib import Path

import coverage as cov


ROOT = Path(__file__).resolve().parents[1]

# These ranges have their own evidence reports and exact byte boundaries.  Do
# not extend a range merely because neighbouring bytes look table-like.
INLINE_DATA = (
    (0xC2209C, 12, "scene_pointer_prefix",
     "analysis/data/c2209c_scene_pointer_prefix.md"),
    (0xC223A8, 1092, "scene_pointer_triplet_table",
     "analysis/data/c223a8_scene_pointer_triplets.md"),
    (0xC33258, 92, "font_compositor_offset_mask_pairs",
     "analysis/data/c33258_postflight_transform_table.md"),
    (0xC37EA0, 6, "projected_edge_list_candidate",
     "analysis/data/c37ea0_bridge_silhouette_projected_edge_list.md"),
    (0xC38B0A, 22, "projected_edge_list_candidate",
     "analysis/data/c38b0a_external_view_projected_edge_list.md"),
    (0xC35584, 6, "golden_gate_control_stream_words",
     "analysis/data/c35584_c355d8_golden_gate_control_stream.md"),
    (0xC355D8, 6, "golden_gate_control_stream_words",
     "analysis/data/c35584_c355d8_golden_gate_control_stream.md"),
    (0xC3925A, 6, "external_view_control_stream_words",
     "analysis/data/c3925a_external_view_control_stream.md"),
    (0xC3751A, 22, "projected_edge_list_candidate",
     "analysis/data/traced_runtime_edge_lists.md#c3751a"),
    (0xC393C4, 14, "projected_edge_list_candidate",
     "analysis/data/traced_runtime_edge_lists.md#c393c4"),
    (0xC3985A, 42, "projected_edge_list_candidate",
     "analysis/data/c3985a_projected_edge_list.md"),
    (0xC39886, 10, "projected_edge_list_candidate",
     "analysis/data/traced_runtime_edge_lists.md#c39886"),
    (0xC3989A, 10, "projected_edge_list_candidate",
     "analysis/data/traced_runtime_edge_lists.md#c3989a"),
    (0xC483BA, 156, "consecutive_projection_records",
     "analysis/data/c483ba_golden_gate_projection_records.md"),
)


def hex_address(value: int) -> str:
    return f"${value:06X}"


def segment_rows(inventory: dict, resolved: dict, executed: set[int], data_refs: Counter,
                 flow_refs: Counter) -> list[dict]:
    rows = []
    for segment in inventory["segments"]:
        index = str(segment["index"])
        runtime = resolved.get(index)
        row = {
            "segment": segment["index"],
            "hunk_kind": segment["kind"],
            "original_payload_file_offset": hex(segment["payload_file_offset"]),
            "size_bytes": segment["size_bytes"],
            "runtime_status": runtime["status"] if runtime else "unresolved",
        }
        if runtime:
            base = runtime["runtime_payload_base"]
            row["runtime_start"] = hex_address(base)
            row["runtime_end_exclusive"] = hex_address(base + segment["size_bytes"])
            row["runtime_bank"] = runtime.get("bank")
        if segment["kind"] == "DATA":
            row["classification"] = "initialized_data"
            row["reason"] = "Original HUNK_DATA declaration"
        elif segment["kind"] == "BSS":
            row["classification"] = "uninitialized_data"
            row["reason"] = "Original HUNK_BSS declaration"
        elif not runtime:
            row["classification"] = "code_hunk_unresolved"
            row["reason"] = "Original HUNK_CODE, no verified runtime placement"
        else:
            span = range(base, base + segment["size_bytes"])
            executed_bytes = len(executed.intersection(span))
            data_count = sum(count for address, count in data_refs.items() if address in span)
            flow_count = sum(count for address, count in flow_refs.items() if address in span)
            row.update(observed_executed_bytes=executed_bytes, reconstructed_data_references=data_count,
                       reconstructed_flow_references=flow_count)
            if executed_bytes:
                row["classification"] = "code_hunk_executed"
                row["reason"] = "Observed instruction bytes in a P-code export"
            elif flow_count:
                row["classification"] = "code_hunk_branch_target"
                row["reason"] = "Reconstructed branch/call target"
            elif data_count:
                row["classification"] = "data_in_code_hunk"
                row["reason"] = "Positive reconstructed data operands and no flow reference"
            else:
                row["classification"] = "code_hunk_unclassified"
                row["reason"] = "No positive execution, flow, or data-reference evidence"
        rows.append(row)
    return rows


def write_markdown(manifest: dict, output: Path) -> None:
    counts = Counter(row["classification"] for row in manifest["hunk_segments"])
    initialized_bytes = sum(row["size_bytes"] for row in manifest["hunk_segments"]
                            if row["classification"] == "initialized_data")
    bss_bytes = sum(row["size_bytes"] for row in manifest["hunk_segments"]
                    if row["classification"] == "uninitialized_data")
    lines = [
        "# Runtime code/data/graphics boundary inventory",
        "",
        "This generated inventory is the separation authority.  It preserves the original",
        "Amiga Hunk declaration and adds only evidence-backed runtime classifications.",
        "A CODE hunk that has not run is **not** called data: it remains unclassified.",
        "",
        "## Result",
        "",
        f"- Original segments: {len(manifest['hunk_segments'])}.",
        f"- Initialized DATA hunks: {counts['initialized_data']} ({initialized_bytes:,} bytes); "
        f"BSS hunks: {counts['uninitialized_data']} ({bss_bytes:,} bytes).",
        f"- CODE hunks with positive data-only evidence: {counts['data_in_code_hunk']}.",
        f"- Executed CODE hunks: {counts['code_hunk_executed']}; unresolved/ambiguous CODE hunks: "
        f"{counts['code_hunk_unresolved'] + counts['code_hunk_unclassified']}.",
        "- Known visual memory is listed as mutable display targets, not immutable graphics assets.",
        "",
        "## Separation rules",
        "",
        "1. Extract `initialized_data` from original HUNK_DATA payloads and allocate `uninitialized_data` from HUNK_BSS sizes.",
        "2. Keep every `code_hunk_*` region in the executable image.  The `data_in_code_hunk` verdict only licenses treating that whole hunk as data where its positive references support it.",
        "3. Split the exact `inline_data_regions` out before disassembly; their enclosing CODE hunks stay executable.",
        "4. Keep `active_display_regions` outside both program-code and static-asset exports. They are mutable Chip-RAM renderer destinations in the named capture state.",
        "",
        "## Exact inline data boundaries",
        "",
        "| Range | Bytes | Conservative role | Evidence |",
        "| --- | ---: | --- | --- |",
    ]
    for row in manifest["inline_data_regions"]:
        lines.append(f"| `{row['start']}`–`{row['end_inclusive']}` | {row['size_bytes']} | {row['role']} | `{row['evidence']}` |")
    lines += ["", "## Original HUNK data allocation", "", "| Kind | Hunks | Bytes | Boundary authority |", "| --- | ---: | ---: | --- |", f"| Initialized DATA | {counts['initialized_data']} | {initialized_bytes:,} | Original `HUNK_DATA` declarations and payload offsets in `analysis/hunk_inventory.json` |", f"| Uninitialized BSS | {counts['uninitialized_data']} | {bss_bytes:,} | Original `HUNK_BSS` declarations and allocation sizes in `analysis/hunk_inventory.json` |", "", "Zero-length Hunk declarations are retained in the counts: they preserve segment numbering and relocation identity, but contribute no bytes to extraction/allocation.", "", "## Snapshot-backed static data", "", "| Range | Bytes | Role | Qualification |", "| --- | ---: | --- | --- |"]
    for row in manifest["runtime_static_data_regions"]:
        lines.append(f"| `{row['start']}` to `{row['end_inclusive']}` | {row['size_bytes']} | {row['role']} | {row['qualification']} |")
    lines += ["", "## Runtime display state", "", "| Range | Bytes | Classification | Evidence |", "| --- | ---: | --- | --- |"]
    for row in manifest["runtime_display_state_regions"]:
        lines.append(f"| `{row['start']}` to `{row['end_inclusive']}` | {row['size_bytes']} | {row['classification']} | `{row['evidence']}` |")
    lines += ["", "## Mutable geometry workspace", "", "| Range | Bytes | Classification | Evidence |", "| --- | ---: | --- | --- |"]
    for row in manifest["runtime_geometry_workspace_regions"]:
        lines.append(f"| `{row['start']}` to `{row['end_inclusive']}` | {row['size_bytes']} | {row['classification']} | `{row['evidence']}` |")
    lines += ["", "## Byte-stable scene-family candidates", "", "| Range | Bytes | Classification | Evidence |", "| --- | ---: | --- | --- |"]
    for row in manifest["immutable_scene_source_candidate_regions"]:
        lines.append(f"| `{row['start']}` to `{row['end_inclusive']}` | {row['size_bytes']} | {row['classification']} | `{row['evidence']}` |")
    lines += ["", "## Active display memory", "", "| Capture state | Range(s) | Classification | Evidence |", "| --- | --- | --- | --- |"]
    for row in manifest["active_display_regions"]:
        ranges = ", ".join(f"`{item['start']}–{item['end_inclusive']}`" for item in row["planes"])
        lines.append(f"| {row['capture_state']} | {ranges} | {row['classification']} | `{row['evidence']}` |")
    lines += ["", "## Blitter channel state", "",
              "`analysis/attract_cockpit_blitter_jobs.json` records the complete register state at each observed CPU `BLTSIZE` write. It is the source/destination boundary for the frame-1800 cockpit trace; do not infer an immutable source asset from an active plane pointer.",
              "", "## Immutable disk graphics", "",
              "The ADF's `pix/frnt5`, `pix/inst5`, and `pix/splsh` ILBM resources are independently inventoried in `analysis/disk_graphics_assets.md`. They can be exported verbatim by the recorded command and are separate from code and mutable display memory.",
              "The broader game-disk inventory at `analysis/disk_game_resources.md` separately hashes the executable, ILBMs, text resources, configuration, and icon metadata.",
              "", "The complete machine-readable segment list, including original payload offsets and resolved runtime addresses, is `analysis/runtime_region_manifest.json`.", ""]
    output.write_text("\n".join(lines))


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=ROOT / "analysis" / "runtime_region_manifest.json")
    parser.add_argument("--markdown", type=Path, default=ROOT / "analysis" / "runtime_region_inventory.md")
    args = parser.parse_args()
    inventory = json.loads((ROOT / "analysis" / "hunk_inventory.json").read_text())
    resolved = json.loads((ROOT / "analysis" / "hunk_runtime_resolved.json").read_text())["resolved"]
    live_streams = json.loads((ROOT / "analysis" / "live_control_streams.json").read_text())["streams"]
    executed, export_count = cov.observed_bytes()
    data_refs, flow_refs = cov.symbol_references()
    manifest = {
        "authority": {
            "hunk_inventory": "analysis/hunk_inventory.json",
            "runtime_mapping": "analysis/hunk_runtime_resolved.json",
            "pcode_exports": export_count,
        },
        "policy": "Hunk kind is authoritative; lack of execution never proves data.",
        "hunk_segments": segment_rows(inventory, resolved, executed, data_refs, flow_refs),
        "inline_data_regions": [
            {"start": hex_address(start), "end_inclusive": hex_address(start + size - 1),
             "size_bytes": size, "role": role, "evidence": evidence}
            for start, size, role, evidence in INLINE_DATA
        ] + [
            {"start": row["stream"], "end_inclusive": hex_address(int(row["stream"].removeprefix("$"), 16) + 23),
             "size_bytes": 24, "role": "live_renderer_control_stream_prefix",
             "evidence": "analysis/live_control_streams.md"}
            for row in live_streams
            if int(row["stream"].removeprefix("$"), 16) not in {start for start, _, _, _ in INLINE_DATA}
        ],
        "runtime_static_data_regions": [
            {"start": "$C3ED00", "end_inclusive": "$C41127", "size_bytes": 9256,
             "role": "preloaded_menu_and_mission_text_pool",
             "evidence": "analysis/mission_text_inventory.md",
             "qualification": "Snapshot-backed static data within a mutated CODE hunk; not a byte-identity claim for the extracted executable."},
        ],
        "runtime_display_state_regions": [
            {"start": "$C1AA9C", "end_inclusive": "$C1AB07", "size_bytes": 108,
             "classification": "mutable_palette_and_display_pointer_state_in_BSS",
             "evidence": "analysis/runtime_display_pointer_state.md",
             "qualification": "Baseline snapshot has a decoded RGB4 palette at $C1AA9C and five contiguous plane-like pointers at $C1AAF4; Copper evidence proves only the first four active in the documented display section. This lies inside original HUNK_BSS segment 2."},
        ],
        "runtime_geometry_workspace_regions": [
            {"start": "$C45630", "end_inclusive": "$C48383", "size_bytes": 11604,
             "classification": "mutable_geometry_record_workspace_in_CODE_hunk",
             "evidence": "analysis/model_geometry_boundaries.md",
             "qualification": "C21C2E selects a C47628 record here before midpoint writes. Snapshot comparisons differ from the original Hunk 71 payload in every recorded state; this is not an immutable model export."},
            {"start": "$C48390", "end_inclusive": "$C4E76B", "size_bytes": 25564,
             "classification": "mutable_geometry_working_table_in_CODE_hunk",
             "evidence": "analysis/model_geometry_boundaries.md",
             "qualification": "Projection consumers read selected triples here, while C21C2E writes midpoint-derived triples into selected records. Snapshot comparisons differ from the original Hunk 72 payload in every recorded state; this is not an immutable model export."},
        ],
        "immutable_scene_source_candidate_regions": [
            {"start": "$C34A50", "end_inclusive": "$C3555F", "size_bytes": 2832, "segment": 41},
            {"start": "$C35568", "end_inclusive": "$C361FF", "size_bytes": 3224, "segment": 42},
            {"start": "$C36208", "end_inclusive": "$C36A1B", "size_bytes": 2068, "segment": 43},
            {"start": "$C36A28", "end_inclusive": "$C3720F", "size_bytes": 2024, "segment": 44},
            {"start": "$C08718", "end_inclusive": "$C089EB", "size_bytes": 724, "segment": 45},
            {"start": "$C37218", "end_inclusive": "$C37677", "size_bytes": 1120, "segment": 46},
            {"start": "$C37680", "end_inclusive": "$C37983", "size_bytes": 772, "segment": 47},
            {"start": "$C37990", "end_inclusive": "$C37F77", "size_bytes": 1512, "segment": 49},
            {"start": "$C37F80", "end_inclusive": "$C383E3", "size_bytes": 1124, "segment": 50},
        ],
        "active_display_regions": [
            {"capture_state": "attract frame 1800", "classification": "mutable_chip_ram_display_target",
             "evidence": "analysis/cockpit_bitplane_assets.md",
             "planes": [
                 {"start": "$012BC0", "end_inclusive": "$014AFF"},
                 {"start": "$014B00", "end_inclusive": "$016A3F"},
                 {"start": "$016A40", "end_inclusive": "$01897F"},
                 {"start": "$018980", "end_inclusive": "$01A8BF"},
             ]},
            {"capture_state": "run029 frame 993/994", "classification": "mutable_chip_ram_display_target",
             "evidence": "analysis/run029_active_cockpit_bitplanes.md",
             "planes": [
                 {"start": "$04DB30", "end_inclusive": "$04FA6F"},
                 {"start": "$04FA70", "end_inclusive": "$0519AF"},
                 {"start": "$0519B0", "end_inclusive": "$0538EF"},
                 {"start": "$0538F0", "end_inclusive": "$05582F"},
                 {"start": "$055830", "end_inclusive": "$05776F"},
             ]},
        ],
        "blitter_job_inventories": [
            {"capture_state": "attract frame 1800 ten-frame trace",
             "inventory": "analysis/attract_cockpit_blitter_jobs.json",
             "interpretation": "Full Custom-register state at CPU BLTSIZE writes; active-plane pointers are destinations unless a channel role is separately proven."},
        ],
        "external_immutable_resources": [
            {"class": "ILBM_graphics", "inventory": "analysis/disk_graphics_assets.json",
             "separation": "Extractable verbatim from original ADF pix paths; never conflated with mutable Chip-RAM targets."},
            {"class": "game_disk_resources", "inventory": "analysis/disk_game_resources.json",
             "separation": "Hashes and separates executable, ILBM graphics, text resources, configuration, and icon metadata from the original ADF."},
        ],
        "unlocated_static_graphics": "No additional immutable graphics source has been tied to a live renderer path; visible planes remain mutable renderer targets.",
    }
    for row in manifest["immutable_scene_source_candidate_regions"]:
        row.update(classification="byte_stable_scene_family_candidate_in_CODE_hunk",
                   evidence="analysis/model_geometry_boundaries.md",
                   qualification="Matches original payload across the sampled states after complete relocation operands are excluded. Original HUNK_CODE is retained: this may be mixed instructions and data, not a named model export.")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(manifest, indent=2) + "\n")
    write_markdown(manifest, args.markdown)
    print(f"wrote {args.output.relative_to(ROOT)}")
    print(f"wrote {args.markdown.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
