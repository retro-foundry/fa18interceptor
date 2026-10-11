"""Render same-header inline/alternate map packet coordinate batches as a PNG sheet."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


BACKGROUND = "#10151d"
GRID = "#394655"
TEXT = "#d8e1ea"
INLINE = "#59c7d9"
ALTERNATE = "#e56dbe"
TILE_WIDTH, TILE_HEIGHT = 460, 290
MARGIN = 35


def direct_batch(report: dict[str, object], header: str) -> dict[str, object]:
    matches = [row for row in report["batches"] if row["packet"] == header]
    if not matches:
        raise ValueError(f"missing direct batch for {header}")
    pair_sets = {tuple(tuple(pair["xy"]) for pair in row["consumed_pairs"]) for row in matches}
    if len(pair_sets) != 1:
        raise ValueError(f"non-deterministic pair batch for {header}")
    return matches[0]


def project(points: list[tuple[int, int]], bounds: tuple[int, int, int, int], origin: tuple[int, int]) -> list[tuple[int, int]]:
    minimum_x, minimum_y, maximum_x, maximum_y = bounds
    width = TILE_WIDTH - MARGIN * 2
    height = TILE_HEIGHT - 90
    span = max(maximum_x - minimum_x, maximum_y - minimum_y, 1)
    scale = min(width / span, height / span)
    used_x = (maximum_x - minimum_x) * scale
    used_y = (maximum_y - minimum_y) * scale
    left = origin[0] + MARGIN + (width - used_x) / 2
    top = origin[1] + 60 + (height - used_y) / 2
    return [(round(left + (x - minimum_x) * scale), round(top + (maximum_y - y) * scale)) for x, y in points]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--comparison", type=Path, required=True)
    parser.add_argument("--inline", type=Path, required=True)
    parser.add_argument("--alternate", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    comparison = json.loads(args.comparison.read_text(encoding="utf-8"))
    inline_report = json.loads(args.inline.read_text(encoding="utf-8"))
    alternate_report = json.loads(args.alternate.read_text(encoding="utf-8"))
    rows = comparison["comparisons"]
    columns = 2
    image = Image.new("RGB", (TILE_WIDTH * columns, TILE_HEIGHT * ((len(rows) + columns - 1) // columns)), BACKGROUND)
    draw = ImageDraw.Draw(image)
    font = ImageFont.load_default()
    for index, row in enumerate(rows):
        x0, y0 = (index % columns) * TILE_WIDTH, (index // columns) * TILE_HEIGHT
        left = [tuple(pair["xy"]) for pair in direct_batch(inline_report, row["header"])["consumed_pairs"]]
        right = [tuple(pair["xy"]) for pair in direct_batch(alternate_report, row["header"])["consumed_pairs"]]
        all_points = left + right
        minimum_x = min(point[0] for point in all_points)
        minimum_y = min(point[1] for point in all_points)
        maximum_x = max(point[0] for point in all_points)
        maximum_y = max(point[1] for point in all_points)
        inset = (x0 + MARGIN, y0 + 60, x0 + TILE_WIDTH - MARGIN, y0 + TILE_HEIGHT - 30)
        draw.rectangle(inset, outline=GRID)
        for fraction in (0.25, 0.5, 0.75):
            draw.line((inset[0] + (inset[2] - inset[0]) * fraction, inset[1], inset[0] + (inset[2] - inset[0]) * fraction, inset[3]), fill="#202b38")
            draw.line((inset[0], inset[1] + (inset[3] - inset[1]) * fraction, inset[2], inset[1] + (inset[3] - inset[1]) * fraction), fill="#202b38")
        bounds = (minimum_x, minimum_y, maximum_x, maximum_y)
        for points, colour in ((left, INLINE), (right, ALTERNATE)):
            projected = project(points, bounds, (x0, y0))
            if len(projected) > 1:
                draw.line(projected, fill=colour, width=2)
            for point in projected:
                draw.ellipse((point[0] - 2, point[1] - 2, point[0] + 2, point[1] + 2), fill=colour)
        draw.text((x0 + MARGIN, y0 + 15), f"{row['header']}  inline {len(left)}  alternate {len(right)}", fill=TEXT, font=font)
        draw.text((x0 + MARGIN, y0 + 34), "cyan = inline; magenta = alternate; common local scale", fill=TEXT, font=font)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    image.save(args.output)
    print(json.dumps({"packets": len(rows), "output": str(args.output)}))


if __name__ == "__main__":
    main()
