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


def copper_registers(chip: bytes, copper_list: int = COPPER_LIST) -> dict[int, int]:
    """Return final move values before the copper-list terminator."""
    registers: dict[int, int] = {}
    for offset in range(copper_list, min(len(chip), copper_list + 0x1000), 4):
        address = int.from_bytes(chip[offset : offset + 2], "big")
        value = int.from_bytes(chip[offset + 2 : offset + 4], "big")
        if address == 0xFFFF and value == 0xFFFE:
            break
        if not address & 1:
            registers[address] = value
    return registers


def plane_pointers(registers: dict[int, int], plane_count: int = len(PLANE_POINTER_REGISTERS)) -> list[int]:
    return [
        (registers[0x00E0 + plane * 4] << 16) | registers[0x00E2 + plane * 4]
        for plane in range(plane_count)
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


def changed_composite_components(before: bytes, after: bytes, pointers: list[int]) -> list[dict[str, int]]:
    """Return four-connected changed-pixel components across every active plane."""
    changed: set[tuple[int, int]] = set()
    for y in range(ROWS):
        for x in range(WIDTH):
            offset = y * BYTES_PER_ROW + (x >> 3)
            mask = 0x80 >> (x & 7)
            before_value = sum(((before[pointer + offset] & mask) != 0) << plane
                               for plane, pointer in enumerate(pointers))
            after_value = sum(((after[pointer + offset] & mask) != 0) << plane
                              for plane, pointer in enumerate(pointers))
            if before_value != after_value:
                changed.add((x, y))
    components: list[dict[str, int]] = []
    while changed:
        seed = changed.pop()
        pending = [seed]
        pixels = [seed]
        while pending:
            x, y = pending.pop()
            for neighbor in ((x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1)):
                if neighbor in changed:
                    changed.remove(neighbor)
                    pending.append(neighbor)
                    pixels.append(neighbor)
        xs = [x for x, _ in pixels]
        ys = [y for _, y in pixels]
        components.append({'pixels': len(pixels), 'x_min': min(xs), 'y_min': min(ys),
                           'x_max': max(xs), 'y_max': max(ys)})
    return sorted(components, key=lambda component: (-component['pixels'], component['y_min'], component['x_min']))


def render(labelled_captures: list[tuple[str, bytes, list[int]]], output: Path) -> None:
    panel_height = ROWS + 20
    panel_count = len(labelled_captures[0][2]) + 1
    image = Image.new("RGB", (WIDTH * panel_count, panel_height * len(labelled_captures)), "#202020")
    draw = ImageDraw.Draw(image)
    for capture_index, (label, chip, pointers) in enumerate(labelled_captures):
        planes = [chip[pointer : pointer + PLANE_BYTES] for pointer in pointers]
        values: list[int] = []
        for y in range(ROWS):
            for x in range(WIDTH):
                offset = y * BYTES_PER_ROW + (x >> 3)
                mask = 0x80 >> (x & 7)
                values.append(sum(((plane[offset] & mask) != 0) << number for number, plane in enumerate(planes)))
        for panel in range(panel_count):
            x0, y0 = panel * WIDTH, capture_index * panel_height
            draw.text((x0 + 4, y0 + 3), f"{label} " +
                      ("composite" if panel == len(planes) else f"plane {panel + 1}"), fill="white")
            bitmap = Image.new("L", (WIDTH, ROWS))
            bitmap.putdata([value * 17 if panel == len(planes) else (255 if value & (1 << panel) else 0)
                            for value in values])
            image.paste(bitmap.convert("RGB"), (x0, y0 + 20))
    image.save(output)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--before", type=Path, default=Path("build/attract_focus_600/chip.bin"))
    parser.add_argument("--after", type=Path, default=Path("build/attract_focus_1800/chip.bin"))
    parser.add_argument("--copper-list", type=lambda value: int(value, 0), default=COPPER_LIST,
                        help="Chip-RAM address of the active Copper list (default: $00A400)")
    parser.add_argument("--plane-count", type=int, default=len(PLANE_POINTER_REGISTERS),
                        help="Number of consecutive BPL pointer pairs to extract (default: 4)")
    parser.add_argument("--before-label", default="frame 600",
                        help="Label for the first snapshot in the inspection image")
    parser.add_argument("--after-label", default="frame 1800",
                        help="Label for the second snapshot in the inspection image")
    parser.add_argument("--json", type=Path, default=Path("analysis/cockpit_bitplane_map.json"))
    parser.add_argument("--image", type=Path, default=Path("analysis/visuals/attract_cockpit_bitplanes_600_1800.png"))
    args = parser.parse_args()
    if not 1 <= args.plane_count <= 8:
        raise ValueError("plane-count must be in 1..8")

    captures = []
    for label, path in ((args.before_label, args.before), (args.after_label, args.after)):
        chip = path.read_bytes()
        pointers = plane_pointers(copper_registers(chip, args.copper_list), args.plane_count)
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
            "sha256_before": hashlib.sha256(before).hexdigest(),
            "sha256_after": hashlib.sha256(after).hexdigest(),
            "changed_bytes": changed_bytes,
            "changed_row_ranges": changed_rows,
        })
    data = {
        "copper_list": f"${args.copper_list:06X}",
        "plane_bytes": PLANE_BYTES,
        "rows": ROWS,
        "bytes_per_row": BYTES_PER_ROW,
        "width": WIDTH,
        "planes": planes,
        "changed_composite_components": changed_composite_components(captures[0][1], captures[1][1], pointers),
    }
    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.image.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(json.dumps(data, indent=2) + "\n")
    render(captures, args.image)
    print(f"wrote {args.json}")
    print(f"wrote {args.image}")


if __name__ == "__main__":
    main()
