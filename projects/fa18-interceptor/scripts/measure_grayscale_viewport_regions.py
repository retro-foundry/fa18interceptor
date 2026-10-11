"""Measure connected grayscale regions in a screenshot viewport.

This deliberately reports raster facts only.  It does not assign a region to a
city, terrain, or renderer primitive.
"""
from __future__ import annotations

import argparse
import json
from collections import Counter
from pathlib import Path

from PIL import Image


def is_gray(pixel: tuple[int, int, int], minimum: int, maximum: int) -> bool:
    return pixel[0] == pixel[1] == pixel[2] and minimum <= pixel[0] <= maximum


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--image", type=Path, required=True)
    parser.add_argument("--viewport", nargs=4, type=int, required=True,
                        metavar=("LEFT", "TOP", "RIGHT", "BOTTOM"))
    parser.add_argument("--minimum", type=int, default=1)
    parser.add_argument("--maximum", type=int, default=254)
    parser.add_argument("--minimum-area", type=int, default=1)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    left, top, right, bottom = args.viewport
    if not (0 <= args.minimum <= args.maximum <= 255 and left < right and top < bottom):
        raise ValueError("invalid grayscale range or viewport")

    image = Image.open(args.image).convert("RGB")
    if not (0 <= left < right <= image.width and 0 <= top < bottom <= image.height):
        raise ValueError("viewport lies outside image")
    pixels = image.load()
    palette = Counter(pixels[x, y] for y in range(top, bottom) for x in range(left, right))
    candidates = {(x, y) for y in range(top, bottom) for x in range(left, right)
                  if is_gray(pixels[x, y], args.minimum, args.maximum)}
    regions = []
    while candidates:
        pending = [candidates.pop()]
        region = []
        while pending:
            x, y = pending.pop()
            region.append((x, y))
            for neighbor in ((x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1)):
                if neighbor in candidates:
                    candidates.remove(neighbor)
                    pending.append(neighbor)
        if len(region) >= args.minimum_area:
            xs, ys = zip(*region)
            regions.append({"pixels": len(region),
                            "bounds": [min(xs), min(ys), max(xs), max(ys)]})
    regions.sort(key=lambda item: item["pixels"], reverse=True)
    report = {"scope": "connected grayscale screenshot regions",
              "image": str(args.image), "viewport": [left, top, right, bottom],
              "gray_range": [args.minimum, args.maximum],
              "palette": [{"rgb": list(rgb), "pixels": count}
                          for rgb, count in palette.most_common()],
              "regions": regions,
              "qualification": "Raster measurement only; no renderer, object, or map ownership is implied."}
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"regions": len(regions), "output": str(args.output)}))


if __name__ == "__main__":
    main()
