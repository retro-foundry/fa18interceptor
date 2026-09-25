"""Report compact changed-byte ranges between two 512 KiB Slow-RAM snapshots."""

from __future__ import annotations

import argparse
import itertools
import json
from pathlib import Path


SLOW_BASE = 0xC00000
SLOW_SIZE = 0x80000


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("before", type=Path)
    parser.add_argument("after", type=Path)
    args = parser.parse_args()
    before = args.before.read_bytes()
    after = args.after.read_bytes()
    if len(before) != SLOW_SIZE or len(after) != SLOW_SIZE:
        raise ValueError("each input must be exactly one 512 KiB Slow-RAM dump")
    changed = [index for index, pair in enumerate(zip(before, after)) if pair[0] != pair[1]]
    ranges: list[dict[str, object]] = []
    for _, group in itertools.groupby(enumerate(changed), key=lambda row: row[1] - row[0]):
        offsets = [row[1] for row in group]
        start, end = offsets[0], offsets[-1]
        ranges.append({
            "start": f"${SLOW_BASE + start:06X}",
            "end": f"${SLOW_BASE + end:06X}",
            "length": len(offsets),
            "before_hex": before[start:end + 1].hex(),
            "after_hex": after[start:end + 1].hex(),
        })
    print(json.dumps({"changed_bytes": len(changed), "changed_ranges": ranges}, indent=2))


if __name__ == "__main__":
    main()
