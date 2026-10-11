"""Create a coloured M-map review SVG with evidence-backed landmark callouts.

The land/sea fills deliberately use the completed run003 display as a visual
oracle.  The Golden Gate marker is separately tied to renderer vectors from
$C3559A/$C355D2.  Keeping those provenances separate makes this useful for
review without claiming that screen pixel runs are recovered source polygons.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from PIL import Image


ROOT = Path(__file__).resolve().parents[1]
LEFT, TOP, RIGHT, BOTTOM = 40, 16, 680, 216
GOLDEN_GATE = (97.0 * 2, 59.5)


def colour(value: tuple[int, int, int]) -> str:
    return "#" + "".join(f"{channel:02x}" for channel in value)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--screen", type=Path, default=ROOT / "analysis/visuals/run003_m_map_display.png")
    parser.add_argument("--svg", type=Path, default=ROOT / "analysis/visuals/run003_m_map_land_sea_golden_gate.svg")
    parser.add_argument("--output", type=Path, default=ROOT / "analysis/data/run003_m_map_land_sea_golden_gate.json")
    args = parser.parse_args()
    if args.svg.exists() or args.output.exists():
        raise FileExistsError("refusing to overwrite review artifact")
    image = Image.open(args.screen).convert("RGB").crop((LEFT, TOP, RIGHT, BOTTOM))
    width, height = image.size
    runs = []
    for y in range(height):
        start, current = 0, image.getpixel((0, y))
        for x in range(1, width + 1):
            candidate = image.getpixel((x, y)) if x < width else None
            if candidate != current:
                runs.append((start, y, x - start, colour(current)))
                start, current = x, candidate
    x, y = GOLDEN_GATE
    svg = [
        '<?xml version="1.0" encoding="UTF-8"?>',
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}" shape-rendering="crispEdges">',
        '<title>Run003 M-map: land, sea, and Golden Gate review</title>',
        '<desc>Land and sea are exact run003 display oracle fills. Golden Gate is a separately trace-backed vector anchor.</desc>',
    ]
    svg.extend(f'<rect x="{rx}" y="{ry}" width="{rw}" height="1" fill="{fill}"/>'
               for rx, ry, rw, fill in runs)
    svg.extend([
        '<g shape-rendering="geometricPrecision" font-family="monospace">',
        f'<circle cx="{x:g}" cy="{y:g}" r="4" fill="#e53935" stroke="#ffffff" stroke-width="1"/>',
        f'<path d="M {x + 4:g} {y - 3:g} L {x + 52:g} {y - 19:g}" stroke="#e53935" stroke-width="1.5"/>',
        f'<rect x="{x + 53:g}" y="{y - 27:g}" width="78" height="13" fill="#000000" fill-opacity=".78" stroke="#e53935" stroke-width=".5"/>',
        f'<text x="{x + 56:g}" y="{y - 18:g}" fill="#ffffff" font-size="8">GOLDEN GATE</text>',
        '</g>', '</svg>',
    ])
    args.svg.parent.mkdir(parents=True, exist_ok=True)
    args.svg.write_text("\n".join(svg) + "\n", encoding="utf-8")
    report = {
        "classification": "coloured_screen_oracle_with_separately_trace_backed_landmark_overlay",
        "screen": str(args.screen.relative_to(ROOT)), "screen_rectangle": [LEFT, TOP, RIGHT, BOTTOM],
        "dimensions": [width, height], "filled_colour_runs": len(runs),
        "landmarks": [{"name": "Golden Gate", "screen_anchor": list(GOLDEN_GATE),
                       "evidence": "$C3559A/$C355D2 traced bridge contexts"}],
        "qualification": "Land/sea fill is a visual oracle; the callout is renderer-vector evidence. This is not a static coastline-polygon export.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"runs": len(runs), "golden_gate": GOLDEN_GATE}))


if __name__ == "__main__":
    main()
