"""Count how many run075 frames fa18_port draws exactly, starting at frame 200.

This is the port's progress measure. It runs the native build check, builds
fa18_port, runs it headless on the game disk and the recorded control inputs
only, and compares every frame it writes with the emulator oracle bundle.
fa18_port never sees the oracle: it runs in a temporary directory holding
copies of the ADF and the replay file.

fa18_port must support:

    fa18_port --adf FILE --replay FILE --headless --to N --dump-rgb444 -

It starts in the game state of run075 frame 200 and writes frames 200..N to
stdout in order, each 320x200 little-endian uint16 0x0RGB pixels (128,000
bytes, the oracle bundle's pixel format), then exits 0. The comparison stops
at the first frame that differs in any pixel.
"""
from __future__ import annotations

import argparse
import array
import shutil
import struct
import subprocess
import sys
import tempfile
import threading
import zlib
from pathlib import Path
from typing import Iterator

ROOT = Path(__file__).resolve().parents[1]
WIDTH, HEIGHT = 320, 200
FRAME_BYTES = WIDTH * HEIGHT * 2
FIRST_FRAME = 200


def oracle_frames(path: Path) -> tuple[int, int, Iterator[tuple[int, bytes]]]:
    """Open an FA18RGB4 v1 bundle; return its frame range and a frame iterator."""
    data = path.read_bytes()
    if data[:8] != b"FA18RGB4":
        raise SystemExit(f"{path} is not an FA18RGB4 bundle")
    version, width, height, first, last = struct.unpack_from("<5I", data, 8)
    if (version, width, height) != (1, WIDTH, HEIGHT) or first > last:
        raise SystemExit(f"{path}: unsupported bundle header")

    def frames() -> Iterator[tuple[int, bytes]]:
        chunky = bytearray(FRAME_BYTES)
        offset = 28
        for expected_frame in range(first, last + 1):
            frame, checksum, spans = struct.unpack_from("<3I", data, offset)
            offset += 12
            if frame != expected_frame:
                raise SystemExit(f"{path}: expected frame {expected_frame}, found {frame}")
            for _ in range(spans):
                start, length = struct.unpack_from("<2H", data, offset)
                offset += 4
                chunky[start * 2:(start + length) * 2] = data[offset:offset + length * 2]
                offset += length * 2
            if zlib.adler32(chunky) != checksum:
                raise SystemExit(f"{path}: checksum mismatch at frame {frame}")
            yield frame, bytes(chunky)

    return first, last, frames()


def write_ppm(path: Path, pixels: array.array) -> None:
    rgb = bytearray()
    for c in pixels:
        rgb += bytes((((c >> 8) & 15) * 17, ((c >> 4) & 15) * 17, (c & 15) * 17))
    path.write_bytes(f"P6\n{WIDTH} {HEIGHT}\n255\n".encode() + rgb)


def describe_mismatch(frame: int, native: bytes, oracle: bytes, report_dir: Path) -> list[str]:
    ours, theirs = array.array("H", native), array.array("H", oracle)
    if sys.byteorder == "big":
        ours.byteswap()
        theirs.byteswap()
    wrong = [i for i in range(WIDTH * HEIGHT) if ours[i] != theirs[i]]
    xs = [i % WIDTH for i in wrong]
    ys = [i // WIDTH for i in wrong]
    report_dir.mkdir(parents=True, exist_ok=True)
    diff = array.array("H", (0xF0F if ours[i] != theirs[i] else (theirs[i] >> 2) & 0x333
                             for i in range(WIDTH * HEIGHT)))
    images = []
    for kind, pixels in (("native", ours), ("oracle", theirs), ("diff", diff)):
        path = report_dir / f"frame{frame}_{kind}.ppm"
        write_ppm(path, pixels)
        images.append(str(path.relative_to(ROOT) if path.is_relative_to(ROOT) else path))
    samples = ", ".join(f"({i % WIDTH},{i // WIDTH}) native {ours[i]:03X} oracle {theirs[i]:03X}"
                        for i in wrong[:3])
    return [f"first mismatch: frame {frame}, {len(wrong):,} of {WIDTH * HEIGHT:,} pixels wrong, "
            f"x {min(xs)}..{max(xs)}, y {min(ys)}..{max(ys)}",
            f"  first pixels: {samples}",
            f"  images: {', '.join(images)}"]


def count_frames(command: list[str], cwd: Path, oracle: Path, last: int | None,
                 report_dir: Path, timeout: float) -> tuple[int, list[str]]:
    """Run fa18_port and return the count of consecutive exact frames plus report lines."""
    first, bundle_last, frames = oracle_frames(oracle)
    if first != FIRST_FRAME:
        raise SystemExit(f"{oracle}: bundle starts at frame {first}, not {FIRST_FRAME}")
    last = bundle_last if last is None else min(last, bundle_last)
    command = command + ["--headless", "--to", str(last), "--dump-rgb444", "-"]
    lines: list[str] = []
    with tempfile.TemporaryFile() as stderr:
        process = subprocess.Popen(command, cwd=cwd, stdin=subprocess.DEVNULL,
                                   stdout=subprocess.PIPE, stderr=stderr)
        timed_out = threading.Event()

        def stop_at_timeout() -> None:
            timed_out.set()
            process.kill()

        watchdog = threading.Timer(timeout, stop_at_timeout)
        watchdog.start()
        matched, mismatch = 0, None
        try:
            for frame, expected in frames:
                if frame > last:
                    break
                native = process.stdout.read(FRAME_BYTES)
                if len(native) < FRAME_BYTES:
                    if native:
                        lines.append(f"fa18_port wrote a partial frame {frame} "
                                     f"({len(native):,} of {FRAME_BYTES:,} bytes)")
                    break
                if native != expected:
                    mismatch = (frame, native, expected)
                    break
                matched += 1
            extra = mismatch is None and matched == last - first + 1 and process.stdout.read(1)
        finally:
            watchdog.cancel()
            if process.poll() is None:
                process.kill()
            process.stdout.close()
            code = process.wait()
        stderr.seek(0)
        errors = stderr.read().decode("utf-8", "replace").strip().splitlines()
    if mismatch:
        lines += describe_mismatch(*mismatch, report_dir)
    elif matched == last - first + 1:
        lines.append(f"all frames {first}..{last} match")
        if extra:
            lines.append(f"fa18_port wrote more output than --to {last} asked for")
        elif code != 0:
            lines.append(f"fa18_port exited with code {code}")
    else:
        stop = "timed out" if timed_out.is_set() else f"exited with code {code}"
        lines.append(f"fa18_port {stop} after {matched:,} frame(s); "
                     f"frame {first + matched} was never written")
    if errors and not mismatch:
        lines += ["  fa18_port stderr:"] + [f"    {line}" for line in errors[-10:]]
    return matched, lines


def find_executable(build: Path, config: str) -> Path:
    for candidate in (build / config / "fa18_port.exe", build / "fa18_port.exe",
                      build / config / "fa18_port", build / "fa18_port"):
        if candidate.is_file():
            return candidate
    raise SystemExit(f"fa18_port was not found in {build}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--to", type=int, help="last frame to compare (default: end of bundle)")
    parser.add_argument("--config", default="Debug", help="CMake build configuration")
    parser.add_argument("--build-dir", type=Path, default=ROOT / "port/build")
    parser.add_argument("--oracle", type=Path, default=ROOT / "build/port_run075_demo.fa18")
    parser.add_argument("--replay", type=Path, default=ROOT / "captures/run075/playback.e9k")
    parser.add_argument("--adf", type=Path, help="game disk (default: the .adf in the repo root)")
    parser.add_argument("--report-dir", type=Path, default=ROOT / "build/native_frame_count")
    parser.add_argument("--timeout", type=float, default=1800, help="seconds for the whole run")
    args = parser.parse_args()

    adfs = [args.adf] if args.adf else sorted(ROOT.glob("*.adf"))
    if len(adfs) != 1 or not adfs[0].is_file():
        raise SystemExit("pass --adf: expected exactly one .adf in the repository root")
    check = subprocess.run([sys.executable, str(ROOT / "scripts/check_native_build.py")],
                           capture_output=True, text=True)
    if check.returncode != 0:
        print("\n".join(check.stdout.splitlines()[:12]))
        print("\nNATIVE_FRAME_COUNT=0 (native build check failed)")
        sys.exit(1)
    cmake = shutil.which("cmake")
    if not cmake:
        raise SystemExit("cmake is not on PATH")
    build = subprocess.run([cmake, "--build", str(args.build_dir), "--config", args.config,
                            "--target", "fa18_port"], capture_output=True, text=True)
    if build.returncode != 0:
        print("\n".join(build.stdout.splitlines()[-15:]))
        print("\nNATIVE_FRAME_COUNT=0 (fa18_port build failed)")
        sys.exit(1)
    executable = find_executable(args.build_dir, args.config)

    with tempfile.TemporaryDirectory(prefix="fa18_native_") as sandbox:
        sandbox = Path(sandbox)
        shutil.copy2(executable, sandbox / executable.name)
        shutil.copy2(adfs[0], sandbox / "game.adf")
        shutil.copy2(args.replay, sandbox / "playback.e9k")
        command = [str(sandbox / executable.name), "--adf", str(sandbox / "game.adf"),
                   "--replay", str(sandbox / "playback.e9k")]
        matched, lines = count_frames(command, sandbox, args.oracle, args.to,
                                      args.report_dir, args.timeout)
    end = f" (frames {FIRST_FRAME}..{FIRST_FRAME + matched - 1} exact)" if matched else ""
    print(f"fa18_port native frames: {matched:,}{end}")
    for line in lines:
        print(line)
    print(f"NATIVE_FRAME_COUNT={matched}")


if __name__ == "__main__":
    main()
