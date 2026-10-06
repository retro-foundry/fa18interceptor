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


def compare_frames(reference: Path, actual: Path, reference_indices: Path,
                   actual_indices: Path, palette: list[set[int]]) -> dict:
    sizes = [path.stat().st_size for path in
             (reference, actual, reference_indices, actual_indices)]
    if (sizes[0] % RGB_BYTES or sizes[1] % RGB_BYTES or
            sizes[2:] != [sizes[0] // 2, sizes[1] // 2]):
        raise ValueError(f"inconsistent RGB/index streams: {sizes}")
    if sys.byteorder != "little":
        raise ValueError("RGB444 stream comparison requires the runner's little-endian host format")
    counts = [size // RGB_BYTES for size in sizes[:2]]
    result = dict(frames=min(counts), reference_frames=counts[0], actual_frames=counts[1],
                  frame_counts_equal=counts[0] == counts[1], first_rgb_difference=None,
                  first_index_difference=None, first_nonfade_difference=None,
                  ignored_fade_pixels=0, nonfade_pixels=0, index_difference_pixels=0,
                  passed=counts[0] == counts[1])
    if not result["frames"]:
        return result
    allowed = np.zeros((32, 4096), dtype=np.bool_)
    for index, colors in enumerate(palette):
        allowed[index, list(colors)] = True
    from contextlib import ExitStack
    with ExitStack() as stack:
        maps = [stack.enter_context(mmap.mmap(stack.enter_context(path.open("rb")).fileno(),
                                             0, access=mmap.ACCESS_READ))
                for path in (reference, actual, reference_indices, actual_indices)]
        rgb_a, rgb_b, idx_a, idx_b = maps
        for frame in range(result["frames"]):
            lo, hi = frame * PIXELS, (frame + 1) * PIXELS
            rgb_same = rgb_a[lo * 2:hi * 2] == rgb_b[lo * 2:hi * 2]
            indices_same = idx_a[lo:hi] == idx_b[lo:hi]
            if rgb_same and indices_same:
                continue
            if not rgb_same and result["first_rgb_difference"] is None:
                result["first_rgb_difference"] = frame + 1
            if not indices_same and result["first_index_difference"] is None:
                result["first_index_difference"] = frame + 1
            # Limit views to this frame; release them before closing mmap.
            av = np.frombuffer(rgb_a, dtype="<u2", count=PIXELS, offset=lo * 2)
            bv = np.frombuffer(rgb_b, dtype="<u2", count=PIXELS, offset=lo * 2)
            ia = np.frombuffer(idx_a, dtype=np.uint8, count=PIXELS, offset=lo)
            ib = np.frombuffer(idx_b, dtype=np.uint8, count=PIXELS, offset=lo)
            try:
                if max(int(ia.max()), int(ib.max())) > 31 or max(int(av.max()), int(bv.max())) > 0xfff:
                    raise ValueError("invalid palette index or RGB444 pixel")
                geometry = ia != ib
                colors_changed = av != bv
                fade = ~geometry & colors_changed & allowed[ia, av] & allowed[ia, bv]
                indices_changed = int(np.count_nonzero(geometry))
                excluded = int(np.count_nonzero(fade))
                changed = int(np.count_nonzero(geometry | (colors_changed & ~fade)))
            finally:
                del av, bv, ia, ib
            result["ignored_fade_pixels"] += excluded
            result["nonfade_pixels"] += changed
            result["index_difference_pixels"] += indices_changed
            if changed and result["first_nonfade_difference"] is None:
                result["first_nonfade_difference"] = dict(frame=frame + 1, pixels=changed)
                result["passed"] = False
    return result


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
