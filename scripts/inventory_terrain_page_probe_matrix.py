"""Summarize comparable controlled terrain-page selector probes."""
from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PROBES = (
    ("control", "(16,16)", "unchanged"),
    ("x_plus", "(17,16)", "first component +1"),
    ("z_plus", "(16,17)", "second component +1"),
    ("mutation", "(17,17)", "both components +1"),
    ("row_zero", "(0,16)", "first component zero"),
    ("group_zero", "(16,0)", "second component zero"),
    ("group_08", "(16,8)", "second component 8"),
    ("group_0c", "(16,12)", "second component 12"),
    ("group_0e", "(16,14)", "second component 14"),
    ("group_0f", "(16,15)", "second component 15"),
    ("row_08", "(8,16)", "first component 8"),
    ("row_0c", "(12,16)", "first component 12"),
    ("row_ff", "(255,16)", "first component 255"),
)


def main() -> None:
    rows = []
    for suffix, bins, perturbation in PROBES:
        groups = json.loads((ROOT / f"analysis/data/static_template_selector_groups_origin_{suffix}.json").read_text())
        copies = json.loads((ROOT / f"analysis/data/workspace_template_copies_origin_{suffix}.json").read_text())
        active = [row for row in groups["groups"] if row["first_template_stream_byte"]]
        joined = [row for row in copies["copies"] if row["runtime_descriptor"]]
        rows.append({
            "probe": suffix,
            "origin_bins": bins,
            "perturbation": perturbation,
            "selector_calls": len(groups["groups"]),
            "selected_streams": len(active),
            "distinct_group_records": len({row["group_record"] for row in active}),
            "static_copies": len(copies["copies"]),
            "later_builder_reads": len(joined),
            "streams": sorted({row["first_template_stream_byte"] for row in active}),
        })
    payload = {
        "classification": "controlled_terrain_page_selector_probe_matrix_not_global_map_extent",
        "rows": rows,
    }
    json_path = ROOT / "analysis/data/terrain_page_probe_matrix.json"
    markdown_path = ROOT / "analysis/data/terrain_page_probe_matrix.md"
    json_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# Terrain page-selector probe matrix",
        "",
        "Classification: **controlled selector-causality summary**. Each probe restores the "
        "same state and changes only named origin-bin values at the same breakpoint. This is "
        "not a complete world-map extent or LOD table.",
        "",
        "| bins | perturbation | selector calls | selected streams | group records | static copies | later builder reads |",
        "| --- | --- | ---: | ---: | ---: | ---: | ---: |",
    ]
    for row in rows:
        lines.append(f"| {row['origin_bins']} | {row['perturbation']} | {row['selector_calls']} | "
                     f"{row['selected_streams']} | {row['distinct_group_records']} | "
                     f"{row['static_copies']} | {row['later_builder_reads']} |")
    lines += [
        "",
        "The control window selects 16 streams. The independent +1 probes demonstrate the "
        "two directory axes, while all three sampled outer bins retain the same four-stream "
        "subset. This supports a bounded active page window but cannot establish a global map "
        "edge, coordinate scale, or LOD scheme without more origin bins and a distance-controlled "
        "renderer experiment.",
        "",
        "Per-probe record and group inventories remain the authority; see "
        "[the origin mutation probe](origin_selector_mutation_probe.md).",
        "",
    ]
    markdown_path.write_text("\n".join(lines), encoding="utf-8")
    print(f"wrote {json_path.relative_to(ROOT)} and {markdown_path.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
