"""Measure the first sampled orange/yellow text pixels in native replay PNGs."""

from __future__ import annotations

import argparse
import json
from pathlib import Path

from PIL import Image


def colored_pixel_count(path: Path) -> int:
    """Count the deliberately narrow orange/yellow range used by the result text."""
    image = Image.open(path).convert("RGB")
    return sum(
        red > 150 and green > 100 and blue < 100
        for red, green, blue in image.getdata()
    )


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--frames", type=Path, required=True,
                        help="native replay PNG directory; file stems must be frame numbers")
    parser.add_argument("--from-frame", type=int, required=True)
    parser.add_argument("--to-frame", type=int, required=True)
    parser.add_argument("--minimum", type=int, default=1,
                        help="minimum matching pixels required to count as present")
    args = parser.parse_args()
    if args.to_frame < args.from_frame or args.minimum < 1:
        raise ValueError("invalid frame interval or minimum")

    samples: list[dict[str, int]] = []
    for path in args.frames.glob("*.png"):
        try:
            frame = int(path.stem)
        except ValueError:
            continue
        if args.from_frame <= frame <= args.to_frame:
            samples.append({"frame": frame, "colored_pixels": colored_pixel_count(path)})
    samples.sort(key=lambda row: row["frame"])
    if not samples:
        raise ValueError("no numbered PNG samples in requested interval")
    first = next((row for row in samples if row["colored_pixels"] >= args.minimum), None)
    print(json.dumps({
        "frames": str(args.frames),
        "from_frame": args.from_frame,
        "to_frame": args.to_frame,
        "minimum": args.minimum,
        "first_matching_sample": first,
        "samples": samples,
    }, indent=2))


if __name__ == "__main__":
    main()
