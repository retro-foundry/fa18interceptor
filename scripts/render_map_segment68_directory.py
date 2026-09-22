"""Render the traced segment-68 map packet directory as a labelled cell grid."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from PIL import Image, ImageDraw


PALETTE = ("#3e92cc", "#5fa777", "#e5a84b", "#d46a82", "#9b7edb", "#56c6bd", "#d9d9d9")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    report = json.loads(args.input.read_text(encoding="utf-8"))
    cells = report["cells"]
    targets = sorted({cell["target"] for cell in cells})
    colours = {target: PALETTE[index] for index, target in enumerate(targets)}
    cell_size, margin, title = 130, 82, 72
    image = Image.new("RGB", (margin + cell_size * 8 + 18, title + margin + cell_size * 8 + 20), "#10151c")
    draw = ImageDraw.Draw(image)
    draw.text((18, 16), "run003 M-map segment-68 relative-offset directory", fill="#f1f5f9")
    draw.text((18, 36), "colour = static target; outlined cells = traced accesses", fill="#cbd5e1")
    used = {(row["x"], row["y"]) for row in report["observed_reads"]}
    for y in range(8):
        draw.text((42, title + margin + y * cell_size + cell_size // 2), str(y), fill="#cbd5e1")
        for x in range(8):
            if y == 0:
                draw.text((margin + x * cell_size + cell_size // 2, title + 48), str(x), fill="#cbd5e1")
            cell = cells[y * 8 + x]
            left, top = margin + x * cell_size, title + margin + y * cell_size
            draw.rectangle((left, top, left + cell_size - 3, top + cell_size - 3), fill=colours[cell["target"]],
                           outline="#ffffff" if (x, y) in used else "#18212a", width=4 if (x, y) in used else 1)
            draw.text((left + 8, top + 34), cell["target"], fill="#10151c")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    image.save(args.output)
    print({"targets": len(targets), "output": str(args.output)})


if __name__ == "__main__":
    main()
