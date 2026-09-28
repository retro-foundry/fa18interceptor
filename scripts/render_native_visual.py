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


def png(path: Path, pixels: array.array) -> None:
    from PIL import Image
    image = Image.new("RGB", (WIDTH, HEIGHT))
    image.putdata([(((value >> 8) & 15) * 17, ((value >> 4) & 15) * 17,
                   (value & 15) * 17) for value in pixels])
    image.save(path)


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
    parser.add_argument("--bootstrap-render-fixture", type=Path,
                        help="external pre-call Chip page for a labelled display baseline")
    parser.add_argument("--bootstrap-c279-render-fixture", nargs=2, type=Path,
                        metavar=("SLOW", "CHIP"),
                        help="external pre-call state for the labelled native C279 diagnostic")
    parser.add_argument("--delta-against", type=Path,
                        help="write a black-background PNG containing only pixels changed from this PNG")
    parser.add_argument("--output", type=Path, required=True, help="PPM path")
    args = parser.parse_args()
    if args.frame < FIRST_FRAME:
        parser.error(f"--frame must be at least {FIRST_FRAME}")
    if args.render_active_scene and (args.bootstrap_render_fixture or
                                     args.bootstrap_c279_render_fixture):
        parser.error("--render-active-scene cannot be combined with a capture-backed diagnostic")
    if args.bootstrap_render_fixture and args.bootstrap_c279_render_fixture:
        parser.error("select at most one capture-backed diagnostic")
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
    if args.bootstrap_render_fixture:
        command += ["--bootstrap-render-fixture", str(args.bootstrap_render_fixture)]
    if args.bootstrap_c279_render_fixture:
        command += ["--bootstrap-c279-render-fixture",
                    str(args.bootstrap_c279_render_fixture[0]),
                    str(args.bootstrap_c279_render_fixture[1])]
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
    png_path = args.output.with_suffix(".png")
    png(png_path, values)
    delta_path = None
    changed_pixels = None
    if args.delta_against:
        from PIL import Image
        baseline = Image.open(args.delta_against).convert("RGB")
        if baseline.size != (WIDTH, HEIGHT):
            raise SystemExit(f"{args.delta_against}: expected {WIDTH}x{HEIGHT} PNG")
        now = Image.open(png_path).convert("RGB")
        delta = Image.new("RGB", (WIDTH, HEIGHT))
        changed = [current if current != previous else (0, 0, 0)
                   for current, previous in zip(now.getdata(), baseline.getdata())]
        delta.putdata(changed)
        delta_path = args.output.with_name(args.output.stem + "_delta.png")
        delta.save(delta_path)
        changed_pixels = sum(pixel != (0, 0, 0) for pixel in changed)
    summary = {
        "kind": ("capture_free_native_diagnostic" if args.render_active_scene else
                 "capture_backed_native_c279_diagnostic" if args.bootstrap_c279_render_fixture else
                 "capture_backed_display_baseline" if args.bootstrap_render_fixture else
                 "normal_native_replay"),
        "frame": args.frame, "adf": str(adfs[0].relative_to(ROOT)),
        "replay": str(args.replay.relative_to(ROOT)),
        "timing": str(args.timing.relative_to(ROOT)) if args.timing.is_file() else None,
        "nonblack_pixels": len(nonblack),
        "bounds": None if not nonblack else {"x_min": min(i % WIDTH for i in nonblack),
                                                "x_max": max(i % WIDTH for i in nonblack),
                                                "y_min": min(i // WIDTH for i in nonblack),
                                                "y_max": max(i // WIDTH for i in nonblack)},
        "delta_against": str(args.delta_against) if args.delta_against else None,
        "changed_pixels": changed_pixels,
    }
    summary_path = args.output.with_suffix(".json")
    summary_path.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf8")
    print(json.dumps({"image": str(args.output), "png": str(png_path),
                      "delta": str(delta_path) if delta_path else None,
                      "summary": str(summary_path), **summary}))


if __name__ == "__main__":
    main()
