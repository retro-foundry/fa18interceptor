"""Decode the captured run003 M-map bitplanes and compare them to its screen.

This produces an exact display-page rendering, not a source-geometry map
reconstruction. It establishes the colour/fill oracle that a future native
renderer must match before it can claim M-map visual parity.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

from PIL import Image


ROOT = Path(__file__).resolve().parents[1]
PLANE_STARTS = (0x04DB30, 0x04FA70, 0x0519B0, 0x0538F0)
PLANE_BYTES = 0x1F40
WIDTH, HEIGHT = 320, 200
BYTES_PER_ROW = 40
SCREEN_LEFT, SCREEN_TOP = 40, 16


def rgb4(word: int) -> tuple[int, int, int]:
    return ((word >> 8 & 0xF) * 17, (word >> 4 & 0xF) * 17, (word & 0xF) * 17)


def map_palette() -> list[tuple[int, int, int]]:
    assets = json.loads((ROOT / "analysis/disk_graphics_assets.json").read_text(encoding="utf-8"))["assets"]
    asset = next(row for row in assets if row["adf_path"] == "pix/frnt5")
    return [rgb4(int(word[1:], 16)) for word in asset["palette_rgb4_words"][:16]]


def decode(chip: bytes, palette: list[tuple[int, int, int]]) -> Image.Image:
    planes = [chip[start:start + PLANE_BYTES] for start in PLANE_STARTS]
    if any(len(plane) != PLANE_BYTES for plane in planes):
        raise ValueError("one or more M-map bitplanes are outside the supplied Chip-RAM snapshot")
    image = Image.new("RGB", (WIDTH, HEIGHT))
    pixels = []
    for y in range(HEIGHT):
        for x in range(WIDTH):
            byte = y * BYTES_PER_ROW + (x >> 3)
            mask = 0x80 >> (x & 7)
            index = sum(((plane[byte] & mask) != 0) << number for number, plane in enumerate(planes))
            pixels.append(palette[index])
    image.putdata(pixels)
    return image


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--chip", type=Path, default=ROOT / "build/run003_m_visual_30/chip.bin")
    parser.add_argument("--screen", type=Path, default=ROOT / "analysis/visuals/run003_m_map_display.png")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/visuals/run003_m_map_bitplane_decode.png")
    parser.add_argument("--report", type=Path,
                        default=ROOT / "analysis/data/run003_m_map_bitplane_decode.json")
    args = parser.parse_args()

    decoded = decode(args.chip.read_bytes(), map_palette())
    displayed = decoded.resize((WIDTH * 2, HEIGHT), Image.Resampling.NEAREST)
    screen = Image.open(args.screen).convert("RGB")
    reference = screen.crop((SCREEN_LEFT, SCREEN_TOP, SCREEN_LEFT + WIDTH * 2, SCREEN_TOP + HEIGHT))
    mismatches = sum(left != right for left, right in zip(displayed.getdata(), reference.getdata()))
    args.output.parent.mkdir(parents=True, exist_ok=True)
    displayed.save(args.output)
    report = {
        "classification": "scenario_backed_exact_m_map_display_page_decode_not_source_geometry",
        "chip": str(args.chip.relative_to(ROOT)),
        "screen": str(args.screen.relative_to(ROOT)),
        "planes": [f"${address:06X}" for address in PLANE_STARTS],
        "logical_dimensions": [WIDTH, HEIGHT],
        "host_screen_rectangle": [SCREEN_LEFT, SCREEN_TOP, SCREEN_LEFT + WIDTH * 2, SCREEN_TOP + HEIGHT],
        "horizontal_scale": 2,
        "palette_source": "analysis/disk_graphics_assets.json pix/frnt5 RGB4 entries 0-15",
        "decoded_sha256": hashlib.sha256(args.output.read_bytes() if args.output.exists() else b"").hexdigest(),
        "screen_pixel_mismatches": mismatches,
        "qualification": ("The decoded bitplane page supplies the filled land/water/grid display state. "
                          f"It differs from the final captured host rectangle at {mismatches} pixels, so exact visual "
                          "comparison must use the screen-derived SVG oracle. Neither artifact is a static coastline asset "
                          "or a source-geometry reconstruction."),
    }
    # The PNG digest must describe the newly saved artifact, not an earlier file.
    displayed.save(args.output)
    report["decoded_sha256"] = hashlib.sha256(args.output.read_bytes()).hexdigest()
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"output": str(args.output), "mismatches": mismatches,
                      "sha256": report["decoded_sha256"]}, indent=2))


if __name__ == "__main__":
    main()
