"""Write one labelled RGB444 image from the normal native replay path.

This is a visual-progress gate: it runs `fa18_port` with only the original ADF,
recorded controls, and optional replay-owned timing stream.  It never supplies
an emulator frame, Chip-RAM capture, or Oracle pixels to the native executable.
"""
from __future__ import annotations

import argparse
import array
import json
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WIDTH, HEIGHT, FIRST_FRAME = 320, 200, 200
PIXELS, FRAME_BYTES = WIDTH * HEIGHT, WIDTH * HEIGHT * 2


def executable(build: Path, config: str) -> Path:
    for path in (build / config / "fa18_port.exe", build / "fa18_port.exe",
                 build / config / "fa18_port", build / "fa18_port"):
        if path.is_file():
            return path
    raise SystemExit(f"fa18_port is not present under {build}")


def ppm(path: Path, pixels: array.array) -> None:
    rgb = bytearray()
    for value in pixels:
        rgb.extend((((value >> 8) & 15) * 17, ((value >> 4) & 15) * 17,
                    (value & 15) * 17))
    path.write_bytes(f"P6\n{WIDTH} {HEIGHT}\n255\n".encode() + rgb)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--frame", type=int, default=402)
    parser.add_argument("--config", default="Release")
    parser.add_argument("--build-dir", type=Path, default=ROOT / "build/port-native")
    parser.add_argument("--adf", type=Path)
    parser.add_argument("--replay", type=Path, default=ROOT / "captures/run075/playback.e9k")
    parser.add_argument("--timing", type=Path, default=ROOT / "captures/run075/timing.e9t")
    parser.add_argument("--render-active-scene", action="store_true",
                        help="run the existing capture-free native diagnostic at the final frame")
    parser.add_argument("--output", type=Path, required=True, help="PPM path")
    args = parser.parse_args()
    if args.frame < FIRST_FRAME:
        parser.error(f"--frame must be at least {FIRST_FRAME}")
    adfs = [args.adf] if args.adf else sorted(ROOT.glob("*.adf"))
    if len(adfs) != 1 or not adfs[0].is_file():
        parser.error("pass --adf: expected exactly one repository-root ADF")
    exe = executable(args.build_dir, args.config)
    command = [str(exe), "--adf", str(adfs[0]), "--replay", str(args.replay)]
    if args.timing.is_file():
        command += ["--timing", str(args.timing)]
    command += ["--headless", "--to", str(args.frame), "--dump-rgb444", "-"]
    if args.render_active_scene:
        command.append("--render-active-scene")
    completed = subprocess.run(command, cwd=ROOT, stdin=subprocess.DEVNULL,
                               stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    expected = (args.frame - FIRST_FRAME + 1) * FRAME_BYTES
    if completed.returncode or len(completed.stdout) != expected:
        raise SystemExit(f"native replay failed: exit {completed.returncode}, wrote "
                         f"{len(completed.stdout)} of {expected} bytes; "
                         f"{completed.stderr.decode('utf8', 'replace').strip()}")
    values = array.array("H")
    values.frombytes(completed.stdout[-FRAME_BYTES:])
    if values.itemsize != 2 or len(values) != PIXELS:
        raise SystemExit("native frame has invalid RGB444 payload")
    if __import__("sys").byteorder == "big":
        values.byteswap()
    nonblack = [index for index, value in enumerate(values) if value]
    args.output.parent.mkdir(parents=True, exist_ok=True)
    ppm(args.output, values)
    summary = {
        "kind": "capture_free_native_diagnostic" if args.render_active_scene else "normal_native_replay",
        "frame": args.frame, "adf": str(adfs[0].relative_to(ROOT)),
        "replay": str(args.replay.relative_to(ROOT)),
        "timing": str(args.timing.relative_to(ROOT)) if args.timing.is_file() else None,
        "nonblack_pixels": len(nonblack),
        "bounds": None if not nonblack else {"x_min": min(i % WIDTH for i in nonblack),
                                                "x_max": max(i % WIDTH for i in nonblack),
                                                "y_min": min(i // WIDTH for i in nonblack),
                                                "y_max": max(i // WIDTH for i in nonblack)},
    }
    summary_path = args.output.with_suffix(".json")
    summary_path.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf8")
    print(json.dumps({"image": str(args.output), "summary": str(summary_path), **summary}))


if __name__ == "__main__":
    main()
