"""Pack exact 320x200 RGB444 display deltas from a bounded replay oracle."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import struct
import zlib

import numpy as np
from PIL import Image


WIDTH = 320
HEIGHT = 200
HOST_CROP = (40, 16, 680, 216)
MAGIC = b"FA18RGB4"
def game_pixels(path: Path) -> bytes:
    with Image.open(path) as source:
        if source.size != (720, 287):
            raise ValueError(f"{path}: unexpected host size {source.size}")
        image = np.asarray(source.convert("RGB").crop(HOST_CROP))
    left = image[:, 0::2, :]
    right = image[:, 1::2, :]
    if not np.array_equal(left, right):
        raise ValueError(f"{path}: host horizontal pairs differ")
    if np.any(left % 17):
        raise ValueError(f"{path}: non-RGB444 pixels")
    nibble = left.astype(np.uint16) // 17
    color = (nibble[:, :, 0] << 8) | (nibble[:, :, 1] << 4) | nibble[:, :, 2]
    return color.astype("<u2").tobytes()


def spans(previous: bytes, current: bytes):
    old = np.frombuffer(previous, dtype="<u2")
    new = np.frombuffer(current, dtype="<u2")
    changed = np.flatnonzero(old != new)
    if not len(changed):
        return
    breaks = np.flatnonzero(np.diff(changed) > 1) + 1
    for group in np.split(changed, breaks):
        start = int(group[0])
        end = int(group[-1]) + 1
        yield start, end - start, current[start * 2:end * 2]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--oracle", type=Path, required=True)
    parser.add_argument("--first-frame", type=int, default=200)
    parser.add_argument("--last-frame", type=int, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.first_frame < 200 or args.last_frame < args.first_frame:
        parser.error("invalid frame interval")
    oracle = args.oracle.resolve()
    provenance = json.loads((oracle / "port_oracle.json").read_text(encoding="utf-8"))
    if not provenance.get("rendered_from_canonical_restore") or args.last_frame > provenance["last_port_frame"]:
        raise ValueError("requested frames exceed the canonical oracle")
    destination = args.output.resolve()
    if destination.exists():
        raise FileExistsError(destination)
    destination.parent.mkdir(parents=True, exist_ok=True)
    previous = bytes(WIDTH * HEIGHT * 2)
    frame_hashes = {}
    with destination.open("xb") as out:
        out.write(MAGIC)
        out.write(struct.pack("<5I", 1, WIDTH, HEIGHT, args.first_frame, args.last_frame))
        for frame in range(args.first_frame, args.last_frame + 1):
            current = game_pixels(oracle / f"{frame}.png")
            changes = list(spans(previous, current))
            digest = zlib.adler32(current)
            out.write(struct.pack("<III", frame, digest, len(changes)))
            for start, length, raw in changes:
                out.write(struct.pack("<HH", start, length))
                out.write(raw)
            frame_hashes[str(frame)] = f"{digest:08x}"
            previous = current
            if frame % 1000 == 0:
                print(f"packed through frame {frame}", flush=True)
    bundle_hash = hashlib.sha256()
    with destination.open("rb") as packed:
        for chunk in iter(lambda: packed.read(1024 * 1024), b""):
            bundle_hash.update(chunk)
    manifest = {
        "authority": provenance,
        "format": "FA18RGB4 v1; little-endian RGB444 320x200 changed spans",
        "first_frame": args.first_frame,
        "last_frame": args.last_frame,
        "bundle_sha256": bundle_hash.hexdigest(),
        "frame_adler32": frame_hashes,
    }
    destination.with_suffix(".json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"frames": args.last_frame - args.first_frame + 1,
                      "bytes": destination.stat().st_size,
                      "sha256": manifest["bundle_sha256"]}))


if __name__ == "__main__":
    main()
