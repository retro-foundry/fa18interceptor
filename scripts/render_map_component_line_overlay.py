"""Overlay bounded map-component line endpoints on a map screenshot."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from PIL import Image, ImageDraw


COLOURS = ("#ff00ff", "#ffff00", "#00ffff", "#ff9900", "#ff66cc")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--image", type=Path, required=True)
    parser.add_argument("--census", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--component-count", type=int, default=4,
                        help="use this many leading bounded control rows")
    parser.add_argument("--x-origin", type=int, default=40)
    parser.add_argument("--y-origin", type=int, default=16)
    parser.add_argument("--x-scale", type=int, default=2)
    args = parser.parse_args()
    if args.component_count < 1:
        raise ValueError("component count must be positive")
    if args.output.exists():
        raise FileExistsError(args.output)
    census = json.loads(args.census.read_text(encoding="utf-8"))
    rows = census["control_to_primitive"][:args.component_count]
    image = Image.open(args.image).convert("RGB")
    draw = ImageDraw.Draw(image)
    used = []
    for index, row in enumerate(rows):
        colour = COLOURS[index % len(COLOURS)]
        used.append({"transform_input": row["transform_input"],
                     "control_entry": row["control_entry"], "colour": colour,
                     "line_count": len(row["line_endpoints"])})
        for x0, y0, x1, y1 in row["line_endpoints"]:
            draw.line(((args.x_origin + x0 * args.x_scale, args.y_origin + y0),
                       (args.x_origin + x1 * args.x_scale, args.y_origin + y1)),
                      fill=colour, width=2)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    image.save(args.output)
    print(json.dumps({"components": used, "output": str(args.output)}, indent=2))


if __name__ == "__main__":
    main()
