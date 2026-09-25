"""Create an exact coloured SVG of the captured run003 M-map screen region.

The output is a screen-derived vector oracle: contiguous same-colour horizontal
pixel runs become filled SVG rectangles. It preserves the visible M-map palette,
grid, fills, and markers without inventing an unproved world-coordinate or
coastline-source interpretation.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

from PIL import Image


ROOT = Path(__file__).resolve().parents[1]
LEFT, TOP, RIGHT, BOTTOM = 40, 16, 680, 216


def colour(value: tuple[int, int, int]) -> str:
    return "#" + "".join(f"{channel:02x}" for channel in value)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--screen", type=Path,
                        default=ROOT / "analysis/visuals/run003_m_map_display.png")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/visuals/run003_m_map_screen_vector.svg")
    parser.add_argument("--report", type=Path,
                        default=ROOT / "analysis/data/run003_m_map_screen_vector.json")
    args = parser.parse_args()

    screen = Image.open(args.screen).convert("RGB")
    image = screen.crop((LEFT, TOP, RIGHT, BOTTOM))
    width, height = image.size
    runs: list[tuple[int, int, int, str]] = []
    for y in range(height):
        start = 0
        current = image.getpixel((0, y))
        for x in range(1, width + 1):
            next_value = image.getpixel((x, y)) if x < width else None
            if next_value != current:
                runs.append((start, y, x - start, colour(current)))
                start, current = x, next_value

    lines = [
        '<?xml version="1.0" encoding="UTF-8"?>',
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}" shape-rendering="crispEdges">',
        '<title>Run003 captured M-map screen vector oracle</title>',
        '<desc>Exact screen-derived filled pixel runs; not a source-geometry or world-coordinate reconstruction.</desc>',
    ]
    lines.extend(f'<rect x="{x}" y="{y}" width="{run_width}" height="1" fill="{fill}"/>'
                 for x, y, run_width, fill in runs)
    lines.append('</svg>')
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("\n".join(lines) + "\n", encoding="utf-8")

    colours = sorted({fill for _, _, _, fill in runs})
    report = {
        "classification": "scenario_backed_exact_screen_derived_vector_oracle_not_source_geometry",
        "screen": str(args.screen.relative_to(ROOT)),
        "screen_rectangle": [LEFT, TOP, RIGHT, BOTTOM],
        "dimensions": [width, height],
        "filled_rectangles": len(runs),
        "colours": colours,
        "svg_sha256": hashlib.sha256(args.output.read_bytes()).hexdigest(),
        "qualification": ("Every visible pixel in the run003 M-map rectangle is represented by a same-colour filled SVG run. "
                          "This is an exact visual target, not evidence of the original immutable coastline geometry or a global world map."),
    }
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"output": str(args.output), "runs": len(runs), "colours": colours}, indent=2))


if __name__ == "__main__":
    main()
