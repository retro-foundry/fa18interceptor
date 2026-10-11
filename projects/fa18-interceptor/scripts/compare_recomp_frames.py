"""Compare drawing and RGB444, excluding only source-table Copper fade.

The selected index must match even during a black fade. For indices 0..15,
different RGB values are excluded only when both belong to that index's
original 16-stage table at C08510 (advance_input_device_callback, C1718E).
Indices 16..31 and colours outside that table still require exact RGB equality.
"""
from __future__ import annotations

import argparse
import json
import mmap
from pathlib import Path
import struct
import sys
import numpy as np

PIXELS = 320 * 256
RGB_BYTES = PIXELS * 2
FADE_OFFSET = 0x80000 + 0x8510  # --ram-out: Chip RAM followed by Slow RAM.


def fade_palette(ram: Path) -> list[set[int]]:
    with ram.open("rb") as stream:
        stream.seek(FADE_OFFSET)
        data = stream.read(16 * 16 * 2)
    if len(data) != 512:
        raise ValueError("reference RAM does not contain the source fade table")
    words = struct.unpack(">256H", data)
    if any(value > 0xfff for value in words):
        raise ValueError("reference fade table contains non-RGB4 data")
    return [set(words[index::16]) for index in range(16)]


from amiga_decomp.raster import compare_frames as compare_raster


def compare_frames(reference, actual, reference_indices, actual_indices, palette):
    return compare_raster(reference, actual, reference_indices, actual_indices, palette, PIXELS)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("reference", type=Path)
    parser.add_argument("actual", type=Path)
    parser.add_argument("reference_indices", type=Path)
    parser.add_argument("actual_indices", type=Path)
    parser.add_argument("--reference-ram", type=Path, required=True)
    args = parser.parse_args()
    result = compare_frames(args.reference, args.actual, args.reference_indices,
                            args.actual_indices, fade_palette(args.reference_ram))
    print(json.dumps(result))
    return 0 if result["passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
