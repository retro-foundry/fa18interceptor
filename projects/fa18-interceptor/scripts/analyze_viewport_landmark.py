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

from analyze_cockpit_bitplanes import BYTES_PER_ROW, copper_registers, plane_pointers


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
    parser.add_argument("--chip", type=Path,
                        help="optional Chip-RAM snapshot for one scanned frame")
    parser.add_argument("--chip-frame", type=int,
                        help="replay frame represented by --chip")
    parser.add_argument("--bitmap-x-origin", type=int, default=0)
    parser.add_argument("--bitmap-y-origin", type=int, default=0)
    parser.add_argument("--bitmap-x-scale", type=int, default=1)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if not (args.left < args.right and args.top < args.bottom):
        raise ValueError("viewport bounds must have positive area")
    if bool(args.chip) != bool(args.chip_frame):
        raise ValueError("--chip and --chip-frame must be supplied together")
    if args.bitmap_x_scale < 1:
        raise ValueError("--bitmap-x-scale must be positive")

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
    if args.chip:
        target = next((row for row in rows if row["frame"] == args.chip_frame), None)
        if target is None:
            raise ValueError("--chip-frame is not one of the scanned images")
        chip = args.chip.read_bytes()
        registers = copper_registers(chip)
        pointers = plane_pointers(registers)
        image_path = args.images / f"frame_{args.chip_frame:05d}.png"
        if not image_path.exists():
            image_path = args.images / f"frame_{args.chip_frame}.png"
        image = Image.open(image_path).convert("RGB")
        indices = []
        for y in range(args.top, args.bottom):
            for x in range(args.left, args.right):
                if image.getpixel((x, y)) != args.colour:
                    continue
                bitmap_x = (x - args.bitmap_x_origin) // args.bitmap_x_scale
                bitmap_y = y - args.bitmap_y_origin
                if not (0 <= bitmap_x < 320 and 0 <= bitmap_y < 200):
                    raise ValueError("matching pixel is outside supplied bitmap mapping")
                offset = bitmap_y * BYTES_PER_ROW + (bitmap_x >> 3)
                mask = 0x80 >> (bitmap_x & 7)
                indices.append(sum(((chip[pointer + offset] & mask) != 0) << plane
                                   for plane, pointer in enumerate(pointers)))
        if not indices:
            raise ValueError("chip frame has no matching landmark pixels")
        counts = {str(index): indices.count(index) for index in sorted(set(indices))}
        if len(counts) != 1:
            raise ValueError(f"matching pixels have multiple bitplane indices: {counts}")
        index = int(next(iter(counts)))
        report["bitplane_encoding"] = {
            "chip": str(args.chip), "frame": args.chip_frame,
            "screen_to_bitmap": {"x": f"(screen_x - {args.bitmap_x_origin}) / {args.bitmap_x_scale}",
                                 "y": f"screen_y - {args.bitmap_y_origin}"},
            "matching_screen_pixels": len(indices), "bitplane_index_counts": counts,
            "colour_register": f"COLOR{index:02d}",
            "rgb4_word": f"${registers[0x180 + index * 2]:03X}",
        }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"frames": len(rows), "matching_frames": sum(row["pixels"] > 0 for row in rows)}))


if __name__ == "__main__":
    main()
