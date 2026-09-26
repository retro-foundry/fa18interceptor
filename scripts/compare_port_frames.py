"""Byte-compare every decoded native chunky frame to the Engine9000 PNG oracle."""

from __future__ import annotations

import argparse
from pathlib import Path
import struct
import subprocess

from pack_port_frames import HEIGHT, MAGIC, WIDTH, game_pixels


ROOT = Path(__file__).resolve().parents[1]


def read_exact(pipe, count: int) -> bytes:
    result = bytearray()
    while len(result) < count:
        chunk = pipe.read(count - len(result))
        if not chunk:
            raise EOFError(f"native stream ended after {len(result)} of {count} bytes")
        result.extend(chunk)
    return bytes(result)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--oracle", type=Path, required=True)
    parser.add_argument("--bundle", type=Path, required=True)
    parser.add_argument("--executable", type=Path,
                        default=ROOT / "build/port-native/fa18_port.exe")
    args = parser.parse_args()
    with args.bundle.open("rb") as packed:
        header = packed.read(28)
    if len(header) != 28 or header[:8] != MAGIC:
        raise ValueError("invalid frame bundle header")
    version, width, height, first, last = struct.unpack("<5I", header[8:])
    if version != 1 or (width, height) != (WIDTH, HEIGHT):
        raise ValueError("unsupported frame bundle")
    process = subprocess.Popen([str(args.executable.resolve()), "--frames",
                                str(args.bundle.resolve()), "--stream-rgb444"],
                               stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    try:
        assert process.stdout is not None
        for frame in range(first, last + 1):
            actual = read_exact(process.stdout, WIDTH * HEIGHT * 2)
            expected = game_pixels(args.oracle / f"{frame}.png")
            if actual != expected:
                pixel = next(i for i in range(WIDTH * HEIGHT)
                             if actual[i * 2:i * 2 + 2] != expected[i * 2:i * 2 + 2])
                raise AssertionError(f"frame {frame} differs at pixel ({pixel % WIDTH}, {pixel // WIDTH})")
            if frame % 1000 == 0:
                print(f"exact through frame {frame}", flush=True)
        if process.stdout.read(1):
            raise ValueError("native stream has trailing frame bytes")
        exit_code = process.wait()
        if exit_code:
            assert process.stderr is not None
            raise RuntimeError(f"native reader exited {exit_code}: {process.stderr.read().decode(errors='replace')}")
    finally:
        if process.poll() is None:
            process.kill()
            process.wait()
    print(f"Exact chunky RGB444 match for {last - first + 1} frames, {first}..{last}")


if __name__ == "__main__":
    main()
