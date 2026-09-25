"""Render labelled replay screenshots into a compact chronological contact sheet."""
from __future__ import annotations

import argparse
import re
from pathlib import Path

from PIL import Image, ImageDraw


FRAME_NAME = re.compile(r"frame_(\d+)\.png$")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--columns", type=int, default=5)
    parser.add_argument("--stride", type=int, default=1,
                        help="include every Nth chronological keyframe (default: 1)")
    parser.add_argument("--first-frame", type=int,
                        help="optional inclusive first replay frame")
    parser.add_argument("--last-frame", type=int,
                        help="optional inclusive last replay frame")
    parser.add_argument("--title", required=True)
    args = parser.parse_args()
    if args.columns < 1:
        parser.error("--columns must be positive")
    if args.stride < 1:
        parser.error("--stride must be positive")
    if (args.first_frame is not None and args.last_frame is not None
            and args.last_frame < args.first_frame):
        parser.error("last-frame must not precede first-frame")
    if args.output.exists():
        raise FileExistsError(args.output)
    frames = []
    for path in args.input.glob("frame_*.png"):
        matched = FRAME_NAME.fullmatch(path.name)
        if matched:
            frames.append((int(matched.group(1)), path))
    frames.sort()
    if args.first_frame is not None:
        frames = [(frame, path) for frame, path in frames if frame >= args.first_frame]
    if args.last_frame is not None:
        frames = [(frame, path) for frame, path in frames if frame <= args.last_frame]
    frames = frames[::args.stride]
    if not frames:
        raise ValueError("no frame_*.png files found")
    first = Image.open(frames[0][1]).convert("RGB")
    width, height = first.size
    label_height, margin, title_height = 18, 8, 30
    rows = (len(frames) + args.columns - 1) // args.columns
    canvas = Image.new("RGB", (args.columns * (width + margin) + margin,
                                title_height + rows * (height + label_height + margin) + margin),
                       "#10151c")
    draw = ImageDraw.Draw(canvas)
    draw.text((margin, 8), args.title, fill="#f1f5f9")
    for index, (frame, path) in enumerate(frames):
        x = margin + (index % args.columns) * (width + margin)
        y = title_height + (index // args.columns) * (height + label_height + margin)
        image = Image.open(path).convert("RGB")
        canvas.paste(image, (x, y))
        draw.text((x, y + height + 2), f"frame {frame}", fill="#cbd5e1")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    canvas.save(args.output)
    print({"frames": len(frames), "output": str(args.output)})


if __name__ == "__main__":
    main()
