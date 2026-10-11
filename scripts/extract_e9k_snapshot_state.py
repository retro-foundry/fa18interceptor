"""Extract the current serialized core state from an Engine9000 .e9k-save file.

This keeps user-created GUI saves useful as deterministic analysis checkpoints
without treating the wrapper format as a game-data format.
"""
from __future__ import annotations

import argparse
import struct
from pathlib import Path


OUTER_SIZE = 40 + 16 + 36
INNER_HEADER_SIZE = 40


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("snapshot", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    raw = args.snapshot.read_bytes()
    if len(raw) < OUTER_SIZE + INNER_HEADER_SIZE:
        raise ValueError("snapshot is too short")
    magic, version = struct.unpack_from("<8sI", raw)
    if magic != b"E9KSNAP\0" or version != 8:
        raise ValueError("unsupported Engine9000 snapshot wrapper")
    inner_magic, inner_version, inner_size, state_size = struct.unpack_from(
        "<8sIII", raw, OUTER_SIZE
    )
    if inner_magic != b"E9KSTATE" or inner_version != 2 or inner_size != INNER_HEADER_SIZE:
        raise ValueError("unsupported Engine9000 serialized-state record")
    start = OUTER_SIZE + INNER_HEADER_SIZE
    end = start + state_size
    if end > len(raw):
        raise ValueError("truncated serialized state")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(raw[start:end])
    print({"state_bytes": state_size, "output": str(args.output)})


if __name__ == "__main__":
    main()
