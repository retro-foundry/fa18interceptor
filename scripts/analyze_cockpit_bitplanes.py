"""Extract and compare the Amiga display bitplanes used by attract captures."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

from PIL import Image, ImageDraw

COPPER_LIST = 0xA400
PLANE_POINTER_REGISTERS = (0x00E0, 0x00E4, 0x00E8, 0x00EC)
PLANE_BYTES = 0x1F40
ROWS = 200
BYTES_PER_ROW = PLANE_BYTES // ROWS
WIDTH = BYTES_PER_ROW * 8


def copper_registers(chip: bytes) -> dict[int, int]:
    """Return final move values before the copper-list terminator."""
    registers: dict[int, int] = {}
    for offset in range(COPPER_LIST, min(len(chip), COPPER_LIST + 0x1000), 4):
        address = int.from_bytes(chip[offset : offset + 2], "big")
        value = int.from_bytes(chip[offset + 2 : offset + 4], "big")
        if address == 0xFFFF and value == 0xFFFE:
            break
        if not address & 1:
            registers[address] = value
    return registers


def plane_pointers(registers: dict[int, int]) -> list[int]:
    return [
        (registers[reg] << 16) | registers[reg + 2]
        for reg in PLANE_POINTER_REGISTERS
    ]


def changed_row_ranges(before: bytes, after: bytes) -> tuple[int, list[list[int]]]:
    changed_rows = sorted({
        index // BYTES_PER_ROW
        for index, (a, b) in enumerate(zip(before, after)) if a != b
    })
    ranges: list[list[int]] = []
    for row in changed_rows:
        if not ranges or row != ranges[-1][1] + 1:
            ranges.append([row, row])
        else:
            ranges[-1][1] = row
    return sum(a != b for a, b in zip(before, after)), ranges


def render(labelled_captures: list[tuple[str, bytes, list[int]]], output: Path) -> None:
    panel_height = ROWS + 20
    image = Image.new("RGB", (WIDTH * 5, panel_height * len(labelled_captures)), "#202020")
    draw = ImageDraw.Draw(image)
    for capture_index, (label, chip, pointers) in enumerate(labelled_captures):
        planes = [chip[pointer : pointer + PLANE_BYTES] for pointer in pointers]
        values: list[int] = []
        for y in range(ROWS):
            for x in range(WIDTH):
                offset = y * BYTES_PER_ROW + (x >> 3)
                mask = 0x80 >> (x & 7)
                values.append(sum(((plane[offset] & mask) != 0) << number for number, plane in enumerate(planes)))
        for panel in range(5):
            x0, y0 = panel * WIDTH, capture_index * panel_height
            draw.text((x0 + 4, y0 + 3), f"{label} " + ("composite" if panel == 4 else f"plane {panel + 1}"), fill="white")
            bitmap = Image.new("L", (WIDTH, ROWS))
            bitmap.putdata([value * 17 if panel == 4 else (255 if value & (1 << panel) else 0) for value in values])
            image.paste(bitmap.convert("RGB"), (x0, y0 + 20))
    image.save(output)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--before", type=Path, default=Path("build/attract_focus_600/chip.bin"))
    parser.add_argument("--after", type=Path, default=Path("build/attract_focus_1800/chip.bin"))
    parser.add_argument("--json", type=Path, default=Path("analysis/cockpit_bitplane_map.json"))
    parser.add_argument("--image", type=Path, default=Path("analysis/visuals/attract_cockpit_bitplanes_600_1800.png"))
    args = parser.parse_args()

    captures = []
    for label, path in (("frame 600", args.before), ("frame 1800", args.after)):
        chip = path.read_bytes()
        pointers = plane_pointers(copper_registers(chip))
        captures.append((label, chip, pointers))
    if captures[0][2] != captures[1][2]:
        raise SystemExit("captures use different bitplane pointers; compare them separately")

    pointers = captures[0][2]
    planes = []
    for number, pointer in enumerate(pointers, 1):
        before = captures[0][1][pointer : pointer + PLANE_BYTES]
        after = captures[1][1][pointer : pointer + PLANE_BYTES]
        changed_bytes, changed_rows = changed_row_ranges(before, after)
        planes.append({
            "plane": number,
            "start": f"${pointer:06X}",
            "end_exclusive": f"${pointer + PLANE_BYTES:06X}",
            "sha256_frame_600": hashlib.sha256(before).hexdigest(),
            "sha256_frame_1800": hashlib.sha256(after).hexdigest(),
            "changed_bytes": changed_bytes,
            "changed_row_ranges": changed_rows,
        })
    data = {
        "copper_list": f"${COPPER_LIST:06X}",
        "plane_bytes": PLANE_BYTES,
        "rows": ROWS,
        "bytes_per_row": BYTES_PER_ROW,
        "width": WIDTH,
        "planes": planes,
    }
    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.image.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(json.dumps(data, indent=2) + "\n")
    render(captures, args.image)
    print(f"wrote {args.json}")
    print(f"wrote {args.image}")


if __name__ == "__main__":
    main()
