"""Overlay trace-derived logical line endpoints on an Amiga map screenshot."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from PIL import Image, ImageDraw


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--image", type=Path, required=True)
    parser.add_argument("--lines", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--x-origin", type=int, default=40)
    parser.add_argument("--y-origin", type=int, default=16)
    parser.add_argument("--x-scale", type=int, default=2)
    args = parser.parse_args()
    source = json.loads(args.lines.read_text(encoding="utf-8"))
    image = Image.open(args.image).convert("RGB")
    draw = ImageDraw.Draw(image)
    for row in source["lines"]:
        points = [(args.x_origin + x * args.x_scale, args.y_origin + y)
                  for x, y in row["endpoints"]]
        draw.line(points, fill="#ff00ff", width=2)
        for x, y in points:
            draw.rectangle((x - 2, y - 2, x + 2, y + 2), fill="#ffff00")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    image.save(args.output)
    print(json.dumps({"lines": len(source["lines"]), "output": str(args.output)}))


if __name__ == "__main__":
    main()
