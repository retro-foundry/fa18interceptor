"""Inventory run037 template descriptor targets located in verified Hunk 69."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SEGMENT = 69
START = 0xC44500
END = 0xC44878


def address(value: int) -> str:
    return f"${value:06X}"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--catalog", type=Path, default=ROOT / "analysis/data/run037_m_map_template_target_catalog.json")
    parser.add_argument("--handoff", type=Path, default=ROOT / "analysis/data/run037_m_map_descriptor_control_handoff.json")
    parser.add_argument("--output", type=Path, default=ROOT / "analysis/data/run037_segment69_descriptor_target_mapping.json")
    args = parser.parse_args()
    catalog = json.loads(args.catalog.read_text(encoding="utf-8"))["targets"]
    handoff = json.loads(args.handoff.read_text(encoding="utf-8"))["descriptor_control_writes"]
    writes = {row["descriptor_plus_8_field"] for row in handoff}
    rows = []
    for target in catalog:
        value = int(target["descriptor_plus_8_field"][1:], 16)
        if START <= value < END:
            rows.append({"target": target["descriptor_plus_8_field"],
                         "segment": SEGMENT, "segment_offset": f"${value - START:04X}",
                         "placement_count": target["placement_count"],
                         "static_sources": target["static_sources"],
                         "descriptors": target["descriptors"],
                         "observed_c1cc70_control_write": target["descriptor_plus_8_field"] in writes})
    rows.sort(key=lambda row: row["target"])
    payload = {
        "classification": "verified_original_hunk69_descriptor_target_subset_not_complete_mesh_or_code_classification",
        "segment": {"index": SEGMENT, "runtime_start": address(START),
                    "runtime_end_exclusive": address(END), "size_bytes": END - START,
                    "original_payload_file_offset": "$3FC8C"},
        "authority": {"runtime_mapping": "analysis/hunk_runtime_resolved.json",
                      "template_target_catalog": str(args.catalog.relative_to(ROOT)).replace("\\", "/"),
                      "control_handoff": str(args.handoff.relative_to(ROOT)).replace("\\", "/")},
        "targets": rows,
        "qualification": "Every row is a descriptor +8 field present in the run037 template-placement catalog. A field is a renderer-control candidate; it is not by itself a vertex list, terrain-cell payload, or proof that a particular control stream was rasterized.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# Verified Hunk-69 run037 descriptor targets",
        "",
        "Classification: **verified immutable descriptor-target subset**. This is neither a complete terrain mesh nor a whole-hunk code/data reclassification.",
        "",
        "Original CODE Hunk 69 is byte-exactly verified at `$C44500-$C44877` (888 bytes), including all 16 self-relocation operands. The rows below are its fields reached by the run037 terrain-template placement catalog.",
        "",
        "| target | Hunk offset | placements | static template sources | descriptors | observed `$C1CC70` write |",
        "| --- | --- | ---: | --- | --- | --- |",
    ]
    for row in rows:
        lines.append(f"| {row['target']} | {row['segment_offset']} | {row['placement_count']} | "
                     f"{', '.join(row['static_sources'])} | {', '.join(row['descriptors'])} | "
                     f"{'yes' if row['observed_c1cc70_control_write'] else 'no'} |")
    lines += ["", payload["qualification"], ""]
    args.output.with_suffix(".md").write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"targets": len(rows), "direct_control_writes": sum(row["observed_c1cc70_control_write"] for row in rows)}))


if __name__ == "__main__":
    main()
