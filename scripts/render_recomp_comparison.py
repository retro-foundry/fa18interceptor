#!/usr/bin/env python3
"""Render a bounded, fresh source OFF / registered C ON frame comparison.

Keeps small PNG/JSON artifacts; capped RGB scratch streams are always removed.
The brightened crop is labelled separately from the unmodified-colour frames.
"""
from array import array
from collections import Counter, deque
from concurrent.futures import ThreadPoolExecutor
import argparse
import hashlib
import json
from pathlib import Path
import sys

from PIL import Image, ImageDraw, ImageFont
from probe_recomp_timing import (ROOT, WIDTH, HEIGHT, BYTES_PER_FRAME,
                                replay, first_difference)
from compare_recomp_frames import compare_frames, fade_palette


def values(data):
    result = array("H", data)
    if sys.byteorder == "big":
        result.byteswap()
    return result


def rgb_image(pixels, gain=1):
    result = Image.new("RGB", (WIDTH, HEIGHT))
    result.putdata([(min(255, ((p >> 8) & 15) * 17 * gain),
                     min(255, ((p >> 4) & 15) * 17 * gain),
                     min(255, (p & 15) * 17 * gain)) for p in pixels])
    return result


def components(changed):
    remaining = set(changed)
    result = []
    while remaining:
        seed = remaining.pop()
        queue, group = deque([seed]), [seed]
        while queue:
            point = queue.popleft()
            x, y = point % WIDTH, point // WIDTH
            for dx in (-1, 0, 1):
                for dy in (-1, 0, 1):
                    if 0 <= x + dx < WIDTH and 0 <= y + dy < HEIGHT:
                        other = (y + dy) * WIDTH + x + dx
                        if other in remaining:
                            remaining.remove(other); queue.append(other); group.append(other)
        result.append({"pixels": len(group), "bbox": [min(p % WIDTH for p in group),
                       min(p // WIDTH for p in group), max(p % WIDTH for p in group),
                       max(p // WIDTH for p in group)]})
    return sorted(result, key=lambda c: (-c["pixels"], c["bbox"]))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--frame", type=int, default=416)
    parser.add_argument("--context-frames", type=int, default=4,
                        help="extra frames to check whether the difference persists")
    parser.add_argument("--recording", type=Path, default=ROOT / "captures/native/demo01")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/figures/native_frame_416_comparison.png")
    parser.add_argument("--only", default=None, help="optional isolated ports selector")
    args = parser.parse_args()
    if not 1 <= args.frame <= 1000:
        parser.error("frame must be 1..1000 for a bounded comparison")
    if not 0 <= args.context_frames <= 20:
        parser.error("context-frames must be 0..20")
    replay_frames = args.frame + args.context_frames
    scratch = ROOT / "build/recomp"
    scratch.mkdir(parents=True, exist_ok=True)
    off, on = scratch / "comparison_off.rgb444", scratch / "comparison_on.rgb444"
    off_indices, on_indices = off.with_suffix(".index8"), on.with_suffix(".index8")
    off_ram = off.with_suffix(".ram")
    try:
        executable = ROOT / "build/recomp/fa18_recomp.exe"
        rom = ROOT / "local/system/kick13.rom"
        with ThreadPoolExecutor(max_workers=2) as pool:
            jobs = [pool.submit(replay, executable, args.recording, rom, replay_frames,
                                mode, path, args.only if mode == "on" else None,
                                indices, off_ram if mode == "off" else None)
                    for mode, path, indices in (("off", off, off_indices), ("on", on, on_indices))]
            for job in jobs:
                job.result()
        first = first_difference(off, on, replay_frames)
        comparison = compare_frames(off, on, off_indices, on_indices, fade_palette(off_ram))
        raw = []
        for path in (off, on):
            with path.open("rb") as stream:
                stream.seek((args.frame - 1) * BYTES_PER_FRAME)
                raw.append(stream.read(BYTES_PER_FRAME))
        source, actual = map(values, raw)
        changed = [i for i, pair in enumerate(zip(source, actual)) if pair[0] != pair[1]]
        colors = Counter(f"{source[i]:03X}->{actual[i]:03X}" for i in changed)
        context = []
        with off.open("rb") as a, on.open("rb") as b:
            start = max(1, args.frame - 2)
            a.seek((start - 1) * BYTES_PER_FRAME)
            b.seek((start - 1) * BYTES_PER_FRAME)
            for frame in range(start, replay_frames + 1):
                av, bv = values(a.read(BYTES_PER_FRAME)), values(b.read(BYTES_PER_FRAME))
                context.append({"frame": frame, "source_nonblack": sum(p != 0 for p in av),
                                "on_nonblack": sum(p != 0 for p in bv),
                                "on_matches_source_checkpoint": bv == source,
                                "changed_pixels": sum(x != y for x, y in zip(av, bv))})
        mask = Image.new("RGB", (WIDTH, HEIGHT))
        for i in changed:
            mask.putpixel((i % WIDTH, i // WIDTH), (255, 80, 200))
        summary = {"recording": str(args.recording.resolve().relative_to(ROOT)),
                   "frame": args.frame, "source": "fresh --ports off",
                   "candidate": "fresh --ports on " + (args.only or "ALL"),
                   "first_difference": first, "changed_pixels": len(changed),
                   "frame_comparison_ignoring_copper_fade": comparison,
                   "total_pixels": WIDTH * HEIGHT, "color_changes": dict(colors),
                   "nearby_frames": context,
                   "source_frame_sha256": hashlib.sha256(raw[0]).hexdigest(),
                   "on_frame_sha256": hashlib.sha256(raw[1]).hexdigest(),
                   "components_8_connected": components(changed)}
        # Full frames: RGB444 nibble * 17, nearest-neighbour enlargement.
        # Crop gain is explicitly labelled; the mask contains changed pixels only.
        canvas = Image.new("RGB", (1992, 916), (24, 28, 35))
        draw = ImageDraw.Draw(canvas)
        font_path = Path("C:/Windows/Fonts/consola.ttf")
        font = ImageFont.truetype(str(font_path), 20) if font_path.is_file() else ImageFont.load_default()
        small = ImageFont.truetype(str(font_path), 17) if font_path.is_file() else font
        draw.text((24, 14), f"{args.recording.name} | FRAME {args.frame} | {len(changed)} changed pixels",
                  font=font, fill="white")
        prefix = f"Frames 1-{first[0] - 1} identical" if first else f"Frames 1-{args.frame} identical"
        draw.text((24, 43), prefix + " | Same executable, machine, sealed state and input", font=small,
                  fill=(200, 210, 220))
        panels = [rgb_image(source), rgb_image(actual), mask]
        captions = ["SOURCE OFF: original instructions", "CURRENT ALL ON: C replacements",
                    "STRICT RGB MASK: includes fade diagnostics"]
        if args.only:
            captions[1] = "CURRENT ON: " + args.only
        crop = (0, 96, 320, 208)
        lower = [rgb_image(source, 8).crop(crop), rgb_image(actual, 8).crop(crop), mask.crop(crop)]
        for col, (panel, caption, detail) in enumerate(zip(panels, captions, lower)):
            x = 24 + col * 656
            draw.text((x, 83), caption, font=small, fill="white")
            canvas.paste(panel.resize((640, 512), Image.Resampling.NEAREST), (x, 110))
            draw.text((x, 647), "y=96..207 crop | " + ("brightness x8" if col < 2 else "same mask"),
                      font=small, fill=(200, 210, 220))
            canvas.paste(detail.resize((640, 224), Image.Resampling.NEAREST), (x, 678))
        args.output.parent.mkdir(parents=True, exist_ok=True)
        canvas.save(args.output)
        args.output.with_suffix(".json").write_text(json.dumps(summary, indent=2) + "\n")
        print(json.dumps({"image": str(args.output), **summary}, indent=2))
    finally:
        off.unlink(missing_ok=True)
        on.unlink(missing_ok=True)
        off_indices.unlink(missing_ok=True)
        on_indices.unlink(missing_ok=True)
        off_ram.unlink(missing_ok=True)


if __name__ == "__main__":
    main()
