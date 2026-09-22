"""Export unique bounded map-mode geometry/control components from a census."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--census", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists() or args.output.with_suffix(".md").exists():
        raise FileExistsError(args.output)
    census = json.loads(args.census.read_text(encoding="utf-8"))
    components = []
    seen = set()
    for row in census["control_to_primitive"]:
        key = (row["transform_input"], row["control_entry"])
        if key in seen:
            continue
        seen.add(key)
        components.append({
            "transform_input": row["transform_input"],
            "raw_local_triples": row["raw_triples"],
            "control_entry": row["control_entry"],
            "line_records": row["line_submissions"],
            "polygon_line_routes": row["polygon_line_routes"],
            "polygon_span_routes": row["polygon_span_routes"],
        })
    report = {
        "scope": "unique source/control components observed in the bounded run003 M-map preparation trace",
        "source_census": str(args.census),
        "component_count": len(components),
        "components": components,
        "qualification": (
            "Triples are immutable local transform inputs, not global world coordinates. "
            "Line records are bounded to the next control-walker entry; the final component's interval ends at the trace cap "
            "and can include later unrelated line work. Polygon route counts are bounded to each wrapper's observed return. "
            "This is a partial renderer-input export, not the complete terrain map, a global placement table, or LOD data."
        ),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# Partial run003 map-mode geometry export",
        "",
        report["qualification"],
        "",
        "| Transform input | Local triples | Control entry | Line records | Polygon line/span routes |",
        "| --- | --- | --- | ---: | ---: |",
    ]
    for component in components:
        lines.append(
            f"| `{component['transform_input']}` | `{component['raw_local_triples']}` | "
            f"`{component['control_entry']}` | {len(component['line_records'])} | "
            f"{component['polygon_line_routes']}/{component['polygon_span_routes']} |")
    args.output.with_suffix(".md").write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(json.dumps({"components": len(components), "output": str(args.output)}))


if __name__ == "__main__":
    main()
