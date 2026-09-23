"""Render the static 32x32 wide M-map selector directory as a guard/target mask."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from PIL import Image, ImageDraw


ROOT = Path(__file__).resolve().parents[1]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path,
                        default=ROOT / "analysis/data/wide_m_map_packet_directory.json")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/plots/wide_m_map_packet_directory_mask.png")
    parser.add_argument("--replace", action="store_true")
    args = parser.parse_args()
    if args.output.exists() and not args.replace:
        raise FileExistsError(args.output)
    report = json.loads(args.input.read_text(encoding="utf-8"))
    size, margin, title = 24, 80, 72
    image = Image.new("RGB", (margin + size * 32 + 20, title + margin + size * 32 + 20), "#10151c")
    draw = ImageDraw.Draw(image)
    draw.text((18, 16), "Wide M-map packet directory: static selector mask", fill="#f1f5f9")
    draw.text((18, 36), "teal = non-negative target word; grey = negative-target guard; local indices are not world coordinates", fill="#cbd5e1")
    for cell in report["cells"]:
        x, y = cell["x"], cell["y"]
        left, top = margin + x * size, title + y * size
        colour = "#55c6be" if cell["classification"] == "nonnegative_target_word" else "#4a5563"
        draw.rectangle((left, top, left + size - 1, top + size - 1), fill=colour, outline="#18212a")
    for value in range(0, 32, 4):
        draw.text((margin + value * size, title + 48), str(value), fill="#cbd5e1")
        draw.text((40, title + value * size), str(value), fill="#cbd5e1")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    image.save(args.output)
    print({"output": str(args.output), "cells": len(report["cells"])})


if __name__ == "__main__":
    main()
