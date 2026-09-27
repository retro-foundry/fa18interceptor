"""Compare the published run075 five-plane Copper page with RGB444 frame 392.

This is an oracle probe only. It decodes captured mutable Chip-RAM planes and
the Copper-written RGB4 palette; it does not provide native runtime input.
"""
from __future__ import annotations

import argparse
import collections
import json
import struct
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WIDTH = 320
HEIGHT = 200
ROW_BYTES = WIDTH // 8
PLANE_BYTES = ROW_BYTES * HEIGHT
PLANE_POINTERS = (0x04DB30, 0x04FA70, 0x0519B0, 0x0538F0, 0x055830)
COPPER_START = 0x0577B0
TARGET = 392


def oracle_rgb444(path: Path, target: int) -> bytes:
    data = path.read_bytes()
    if data[:8] != b"FA18RGB4":
        raise ValueError(f"{path}: expected FA18RGB4 bundle")
    version, width, height, first, last = struct.unpack_from("<5I", data, 8)
    if (version, width, height) != (1, WIDTH, HEIGHT) or not first <= target <= last:
        raise ValueError(f"{path}: frame {target} is unavailable")
    chunky = bytearray(WIDTH * HEIGHT * 2)
    offset = 28
    for number in range(first, last + 1):
        frame, _checksum, spans = struct.unpack_from("<3I", data, offset)
        offset += 12
        if frame != number:
            raise ValueError(f"{path}: expected frame {number}, found {frame}")
        for _ in range(spans):
            start, length = struct.unpack_from("<2H", data, offset)
            offset += 4
            chunky[start * 2:(start + length) * 2] = data[offset:offset + length * 2]
            offset += length * 2
        if number == target:
            return bytes(chunky)
    raise AssertionError("checked target range did not yield a frame")


def copper_palette(chip: bytes) -> list[int]:
    if len(chip) < COPPER_START + 4 * 64:
        raise ValueError("Chip-RAM snapshot does not contain the published Copper prefix")
    palette: list[int | None] = [None] * 32
    for offset in range(COPPER_START, COPPER_START + 4 * 64, 4):
        register, value = struct.unpack_from(">HH", chip, offset)
        if register & 1:
            continue
        if 0x0180 <= register <= 0x01BE:
            palette[(register - 0x0180) // 2] = value & 0x0FFF
    if any(value is None for value in palette):
        raise ValueError("published Copper prefix did not write all 32 palette registers")
    return [int(value) for value in palette]


def decode_rgb444(chip: bytes) -> bytes:
    planes = [chip[start:start + PLANE_BYTES] for start in PLANE_POINTERS]
    if len(chip) < PLANE_POINTERS[-1] + PLANE_BYTES or any(len(plane) != PLANE_BYTES
                                                            for plane in planes):
        raise ValueError("Chip-RAM snapshot does not contain the five display planes")
    palette = copper_palette(chip)
    output = bytearray(WIDTH * HEIGHT * 2)
    for y in range(HEIGHT):
        for x in range(WIDTH):
            byte_offset = y * ROW_BYTES + (x >> 3)
            bit = 0x80 >> (x & 7)
            colour = sum(((plane[byte_offset] & bit) != 0) << index
                         for index, plane in enumerate(planes))
            struct.pack_into("<H", output, (y * WIDTH + x) * 2, palette[colour])
    return bytes(output)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--chip", type=Path,
                        default=ROOT / "build/run075_frame389_c1612c_outer_child/final_chip.bin")
    parser.add_argument("--oracle", type=Path, default=ROOT / "build/port_run075_demo.fa18")
    parser.add_argument("--report", type=Path,
                        default=ROOT / "build/run075_frame392_copper_page_decode.json")
    args = parser.parse_args()
    args.chip = args.chip.resolve()
    args.oracle = args.oracle.resolve()
    args.report = args.report.resolve()

    chip = args.chip.read_bytes()
    actual = decode_rgb444(chip)
    expected = oracle_rgb444(args.oracle, TARGET)
    differing = [index for index in range(WIDTH * HEIGHT)
                 if actual[index * 2:index * 2 + 2] != expected[index * 2:index * 2 + 2]]
    actual_colours = collections.Counter(struct.unpack_from("<H", actual, index * 2)[0]
                                         for index in range(WIDTH * HEIGHT))
    expected_colours = collections.Counter(struct.unpack_from("<H", expected, index * 2)[0]
                                           for index in range(WIDTH * HEIGHT))
    report = {
        "classification": "captured_mutable_five_plane_page_decode_not_native_runtime_input",
        "chip": str(args.chip.relative_to(ROOT)),
        "oracle": str(args.oracle.relative_to(ROOT)),
        "target_frame": TARGET,
        "plane_pointers": [f"${pointer:06X}" for pointer in PLANE_POINTERS],
        "copper_palette_start": f"${COPPER_START:06X}",
        "copper_palette_rgb4": [f"{value:03X}" for value in copper_palette(chip)],
        "pixel_mismatches": len(differing),
        "first_mismatch": ([differing[0] % WIDTH, differing[0] // WIDTH]
                           if differing else None),
        "decoded_rgb444_counts": {f"{colour:03X}": count
                                  for colour, count in sorted(actual_colours.items())},
        "oracle_rgb444_counts": {f"{colour:03X}": count
                                 for colour, count in sorted(expected_colours.items())},
    }
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
