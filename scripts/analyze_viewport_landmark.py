"""Measure a known-colour landmark only inside the outside-world viewport.

The scan deliberately excludes the cockpit and HUD.  It reports matching
pixels and their bounding rectangle at each replay keyframe; it does not infer
which 3-D face generated them.
"""
from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

from PIL import Image


FRAME_NAME = re.compile(r"frame_(\d+)\.png$")


def parse_rgb(value: str) -> tuple[int, int, int]:
    value = value.removeprefix("#")
    if len(value) != 6:
        raise argparse.ArgumentTypeError("colour must be six hexadecimal digits")
    try:
        return tuple(int(value[offset:offset + 2], 16) for offset in range(0, 6, 2))
    except ValueError as error:
        raise argparse.ArgumentTypeError(str(error)) from error


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--images", type=Path, required=True)
    parser.add_argument("--colour", type=parse_rgb, required=True)
    parser.add_argument("--left", type=int, required=True)
    parser.add_argument("--top", type=int, required=True)
    parser.add_argument("--right", type=int, required=True, help="exclusive")
    parser.add_argument("--bottom", type=int, required=True, help="exclusive")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if not (args.left < args.right and args.top < args.bottom):
        raise ValueError("viewport bounds must have positive area")

    rows = []
    for path in sorted(args.images.glob("frame_*.png")):
        match = FRAME_NAME.match(path.name)
        if not match:
            continue
        image = Image.open(path).convert("RGB")
        if image.width < args.right or image.height < args.bottom:
            raise ValueError(f"{path} is smaller than the requested viewport")
        pixels = [(x, y)
                  for y in range(args.top, args.bottom)
                  for x in range(args.left, args.right)
                  if image.getpixel((x, y)) == args.colour]
        row = {"frame": int(match.group(1)), "pixels": len(pixels)}
        if pixels:
            xs, ys = zip(*pixels)
            row["bounds"] = {"left": min(xs), "top": min(ys),
                             "right": max(xs), "bottom": max(ys)}
        rows.append(row)
    if not rows:
        raise ValueError(f"no frame_*.png images in {args.images}")
    report = {
        "scope": "exact-colour pixels within a supplied outside-world viewport",
        "source": str(args.images),
        "colour_rgb": list(args.colour),
        "viewport": {"left": args.left, "top": args.top,
                     "right_exclusive": args.right, "bottom_exclusive": args.bottom},
        "frames": rows,
        "qualification": "A colour landmark bounds a visual interval; it does not assign pixels to a renderer face or source model.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"frames": len(rows), "matching_frames": sum(row["pixels"] > 0 for row in rows)}))


if __name__ == "__main__":
    main()
