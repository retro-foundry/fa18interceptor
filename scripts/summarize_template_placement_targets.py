"""Aggregate a traced static-template-to-placement inventory by descriptor +8 field.

The generic route may copy the field to C45A36, but descriptor type gates can
bypass it.  The report therefore calls it a field/target candidate rather than
an unconditional renderer submission.
"""
from __future__ import annotations

import argparse
import json
from collections import defaultdict
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--markdown", type=Path, required=True)
    args = parser.parse_args()
    input_path = args.input.resolve()
    source = json.loads(input_path.read_text(encoding="utf-8"))
    groups: dict[str, list[dict]] = defaultdict(list)
    for row in source["copies"]:
        if row["runtime_placement_record"] is not None and row["descriptor_plus_8_field"] is not None:
            groups[row["descriptor_plus_8_field"]].append(row)

    targets = []
    for field, rows in sorted(groups.items()):
        triples = [row["runtime_coordinate_words_signed"] for row in rows]
        targets.append({
            "descriptor_plus_8_field": field,
            "placement_count": len(rows),
            "static_sources": sorted({row["static_source"] for row in rows}),
            "descriptors": sorted({row["runtime_descriptor"] for row in rows}),
            "x_range": [min(value[0] for value in triples), max(value[0] for value in triples)],
            "z_range": [min(value[2] for value in triples), max(value[2] for value in triples)],
            "zero_middle_count": sum(value[1] == 0 for value in triples),
        })
    report = {
        "authority": {"input": str(input_path.relative_to(ROOT)).replace("\\", "/")},
        "classification": "scenario_backed_template_to_descriptor_field_candidates_not_complete_map_or_renderer_submission",
        "targets": targets,
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# Map-mode template placement target catalog",
        "",
        "Classification: **scenario-backed template-to-descriptor-field aggregation**. "
        "Each row groups emitted flat placements by the selected descriptor's `+8` field. "
        "The generic route uses that field as `$C45A36`, but descriptor type gates can bypass it; "
        "this is not an unconditional renderer-submission, model, or map-object claim.",
        "",
        f"Authority: `{report['authority']['input']}`. {sum(row['placement_count'] for row in targets)} "
        f"placements group into {len(targets)} descriptor-field candidates.",
        "",
        "| descriptor `+8` field | placements | static sources | descriptors | X range | Z range | zero middle |",
        "| --- | ---: | ---: | ---: | --- | --- | ---: |",
    ]
    for row in targets:
        lines.append(
            f"| {row['descriptor_plus_8_field']} | {row['placement_count']} | {len(row['static_sources'])} | "
            f"{len(row['descriptors'])} | {row['x_range'][0]}..{row['x_range'][1]} | "
            f"{row['z_range'][0]}..{row['z_range'][1]} | {row['zero_middle_count']} / {row['placement_count']} |"
        )
    lines += [
        "",
        "The accompanying JSON preserves source and descriptor membership for each target. "
        "These X/Z bounds describe only this video-hash-matched map-mode window; they do not define "
        "global coordinates, terrain elevation, map extent, or LOD.",
        "",
    ]
    args.markdown.parent.mkdir(parents=True, exist_ok=True)
    args.markdown.write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"targets": len(targets), "placements": sum(row["placement_count"] for row in targets)}))


if __name__ == "__main__":
    main()
