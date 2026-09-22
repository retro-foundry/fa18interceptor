"""Render traced map static-packet pair paths for qualitative inspection."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from PIL import Image, ImageDraw


SIZE = 900
MARGIN = 56
BACKGROUND = "#10151c"
GRID = "#2b3542"
C42 = "#55d6be"
C43 = "#ffb454"


def point(value: int) -> float:
    return MARGIN + value * (SIZE - MARGIN * 2) / 4352


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--inventory", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--replace", action="store_true",
                        help="replace an existing generated image")
    args = parser.parse_args()
    if args.output.exists() and not args.replace:
        raise FileExistsError(args.output)
    if args.output.exists():
        args.output.unlink()
    report = json.loads(args.inventory.read_text(encoding="utf-8"))
    image = Image.new("RGB", (SIZE, SIZE), BACKGROUND)
    draw = ImageDraw.Draw(image)
    for tick in range(0, 4353, 512):
        x = point(tick)
        y = SIZE - point(tick)
        draw.line((x, MARGIN, x, SIZE - MARGIN), fill=GRID)
        draw.line((MARGIN, y, SIZE - MARGIN, y), fill=GRID)
        draw.text((x + 2, SIZE - MARGIN + 5), str(tick), fill="#8fa4b8")
        draw.text((4, y - 6), str(tick), fill="#8fa4b8")
    for packet in report["batches"]:
        pairs = packet["consumed_pairs"]
        if not pairs:
            continue
        colour = C42 if packet["consumed_pairs"][0]["address"].startswith("$C42") else C43
        points = [(point(pair["xy"][0]), SIZE - point(pair["xy"][1])) for pair in pairs]
        if len(points) > 1:
            draw.line(points, fill=colour, width=2)
        for x, y in points:
            draw.ellipse((x - 2, y - 2, x + 2, y + 2), fill=colour)
    draw.rectangle((MARGIN, MARGIN, SIZE - MARGIN, SIZE - MARGIN), outline="#d5e0ea")
    draw.text((MARGIN, 16), "run003 M-map static packet pairs (raw source coordinates)", fill="#f1f5f9")
    draw.text((MARGIN, 32), "teal: $C42xxx   orange: $C43xxx   paths follow observed read order; no closure inferred", fill="#cbd5e1")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    image.save(args.output)
    print({"batches": len(report["batches"]), "output": str(args.output)})


if __name__ == "__main__":
    main()
