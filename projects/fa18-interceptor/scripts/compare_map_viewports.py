"""Find the best exact-colour translation between two map-display viewports."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np
from PIL import Image


def colour(value: str) -> tuple[int, int, int]:
    value = value.removeprefix("#")
    if len(value) != 6:
        raise argparse.ArgumentTypeError("colour must have six hexadecimal digits")
    try:
        return tuple(int(value[index:index + 2], 16) for index in range(0, 6, 2))
    except ValueError as error:
        raise argparse.ArgumentTypeError(str(error)) from error


def bitmap(path: Path, left: int, top: int, right: int, bottom: int,
           selected: tuple[int, int, int]) -> np.ndarray:
    image = Image.open(path).convert("RGB")
    if image.width < right or image.height < bottom:
        raise ValueError(f"{path} is smaller than viewport")
    pixels = np.asarray(image)
    return np.all(pixels[top:bottom, left:right] == selected, axis=2)


def score(first: np.ndarray, second: np.ndarray, dx: int, dy: int) -> tuple[float, int]:
    height, width = first.shape
    first_x = max(0, -dx)
    second_x = max(0, dx)
    first_y = max(0, -dy)
    second_y = max(0, dy)
    overlap_width = min(width - first_x, width - second_x)
    overlap_height = min(height - first_y, height - second_y)
    if overlap_width <= 0 or overlap_height <= 0:
        return 0.0, 0
    matches = int(np.count_nonzero(
        first[first_y:first_y + overlap_height, first_x:first_x + overlap_width] ==
        second[second_y:second_y + overlap_height, second_x:second_x + overlap_width]
    ))
    area = overlap_width * overlap_height
    return matches / area, area


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--first", type=Path, required=True)
    parser.add_argument("--second", type=Path, required=True)
    parser.add_argument("--colour", type=colour, required=True)
    parser.add_argument("--left", type=int, required=True)
    parser.add_argument("--top", type=int, required=True)
    parser.add_argument("--right", type=int, required=True, help="exclusive")
    parser.add_argument("--bottom", type=int, required=True, help="exclusive")
    parser.add_argument("--max-x", type=int, required=True)
    parser.add_argument("--max-y", type=int, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    first = bitmap(args.first, args.left, args.top, args.right, args.bottom, args.colour)
    second = bitmap(args.second, args.left, args.top, args.right, args.bottom, args.colour)
    best = None
    for dy in range(-args.max_y, args.max_y + 1):
        for dx in range(-args.max_x, args.max_x + 1):
            agreement, pixels = score(first, second, dx, dy)
            candidate = (agreement, pixels, dx, dy)
            if best is None or candidate[:2] > best[:2]:
                best = candidate
    assert best is not None
    agreement, pixels, dx, dy = best
    payload = {
        "scope": "exact-colour viewport translation comparison",
        "first": str(args.first), "second": str(args.second),
        "colour_rgb": list(args.colour),
        "viewport": {"left": args.left, "top": args.top,
                     "right_exclusive": args.right, "bottom_exclusive": args.bottom},
        "correspondence": "second(x + dx, y + dy) == first(x, y)",
        "best_dx": dx, "best_dy": dy,
        "overlap_pixels": pixels, "agreement": agreement,
        "qualification": "A shared translated raster establishes map-view panning, not the static asset producer or the 3-D terrain source.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"dx": dx, "dy": dy, "agreement": agreement, "overlap_pixels": pixels}))


if __name__ == "__main__":
    main()
