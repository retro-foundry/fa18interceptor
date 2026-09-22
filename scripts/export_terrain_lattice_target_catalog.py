"""Aggregate control-window descriptor targets by terrain selector-page membership."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def markdown(targets: list[dict]) -> str:
    lines = [
        "# Terrain selector lattice target catalog",
        "",
        "Classification: **control-window target aggregation**. Each target is reached",
        "through an exported static template record's descriptor association in the origin",
        "control window. This catalog groups selector-page membership; it is not a stable",
        "world-space instance list, terrain mesh, or LOD assignment.",
        "",
        "| Descriptor target | template records | static streams | selector-bin cells |",
        "| --- | ---: | ---: | ---: |",
    ]
    for target in targets:
        lines.append(f"| {target['target']} | {len(target['records'])} | {len(target['streams'])} | {len(target['selector_cells'])} |")
    lines += [
        "",
        "A target can occur in many cells because templates and descriptors are reusable.",
        "The separate refresh-window experiment proves the same template source can use a",
        "different descriptor/target in another context, so this aggregation must not be",
        "used as an unconditional map-object or LOD table.",
        "",
    ]
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-json", type=Path,
                        default=ROOT / "analysis/data/terrain_lattice_target_catalog.json")
    parser.add_argument("--output-markdown", type=Path,
                        default=ROOT / "analysis/data/terrain_lattice_target_catalog.md")
    args = parser.parse_args()
    payload = json.loads((ROOT / "analysis/data/terrain_lattice_stream_templates.json").read_text(encoding="utf-8"))
    catalog: dict[str, dict] = {}
    for stream in payload["streams"]:
        for record in stream["records"]:
            target = record.get("control_window_descriptor_target")
            if target is None:
                continue
            entry = catalog.setdefault(target, {"target": target, "records": [], "streams": set(), "selector_cells": set()})
            entry["records"].append({"source": record["source"], "header": record["header"],
                                     "descriptor": record["control_window_descriptor"]})
            entry["streams"].add(stream["stream"])
            entry["selector_cells"].update(tuple(cell) for cell in stream["selector_cells"])
    targets = []
    for entry in catalog.values():
        entry["records"] = sorted(entry["records"], key=lambda row: row["source"])
        entry["streams"] = sorted(entry["streams"])
        entry["selector_cells"] = [list(cell) for cell in sorted(entry["selector_cells"])]
        targets.append(entry)
    targets.sort(key=lambda row: row["target"])
    report = {"classification": "control_window_terrain_lattice_targets_not_world_mesh_or_lod",
              "source": "analysis/data/terrain_lattice_stream_templates.json", "targets": targets}
    args.output_json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    args.output_markdown.write_text(markdown(targets), encoding="utf-8")
    print(f"wrote {args.output_json.relative_to(ROOT)} and {args.output_markdown.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
