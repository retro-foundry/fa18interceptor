"""Render a static-triple plus renderer-observed topology JSON file as a PNG sheet."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[1]
MEMORY = ROOT / "captures" / "baseline_menu" / "slow.bin"


def font(size: int):
    return ImageFont.truetype("C:/Windows/Fonts/consola.ttf", size)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--topology", type=Path, required=True)
    parser.add_argument("--title", required=True)
    parser.add_argument("--subtitle", required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--overwrite", action="store_true",
                        help="replace an existing explicitly named output")
    parser.add_argument("--traced-last", type=int,
                        help="last fully transform-traced vertex index; later points draw amber")
    args = parser.parse_args()
    args.topology = args.topology.resolve()
    args.output = args.output.resolve()
    if args.output.exists() and not args.overwrite:
        raise FileExistsError(args.output)
    topology = json.loads(args.topology.read_text(encoding="utf-8"))
    source = topology["vertex_source"]
    start, count = int(source["start"][1:], 16), source["count"]
    memory = MEMORY.read_bytes()
    points = [tuple(int.from_bytes(memory[start - 0xC00000 + index * 6 + axis * 2:start - 0xC00000 + index * 6 + axis * 2 + 2], "big", signed=True)
                    for axis in range(3)) for index in range(count)]
    views = (("X-Y", lambda point: (point[0], point[1])), ("X-Z", lambda point: (point[0], point[2])),
             ("Y-Z", lambda point: (point[1], point[2])), ("isometric", lambda point: (point[0] - point[1], point[2] - (point[0] + point[1]) / 2)))
    image = Image.new("RGB", (2390, 760), "#101419")
    draw = ImageDraw.Draw(image)
    draw.text((28, 20), args.title, font=font(26), fill="#f1f5f9")
    draw.text((28, 58), args.subtitle, font=font(16), fill="#aab7c4")
    colours = ("#65d5ff", "#ffcc66", "#8ee28e", "#ff8fab", "#c9a7ff", "#f7e36b", "#70e0c4", "#ffad70", "#a4c2f4", "#f4a4d7")
    panel, margin = 520, 60
    for panel_index, (label, project) in enumerate(views):
        left, top = 40 + panel_index * 590, 150
        projected = [project(point) for point in points]
        min_x, max_x = min(point[0] for point in projected), max(point[0] for point in projected)
        min_y, max_y = min(point[1] for point in projected), max(point[1] for point in projected)
        scale = (panel - margin * 2) / max(1, max(max_x - min_x, max_y - min_y))
        cx, cy = left + panel / 2, top + panel / 2
        mx, my = (min_x + max_x) / 2, (min_y + max_y) / 2
        def position(point): return cx + (point[0] - mx) * scale, cy - (point[1] - my) * scale
        draw.rectangle((left, top, left + panel, top + panel), fill="#151c24", outline="#506070", width=2)
        draw.text((left + 12, top + 12), label, font=font(24), fill="#f1f5f9")
        for face_index, face in enumerate(topology["faces"]):
            path = [position(project(points[index])) for index in face["indices"]]
            path.append(path[0]); draw.line(path, fill=colours[face_index % len(colours)], width=3, joint="curve")
        for index, point in enumerate(projected):
            x, y = position(point)
            colour = "#65d5ff" if args.traced_last is None or index <= args.traced_last else "#f7e36b"
            draw.ellipse((x - 3, y - 3, x + 3, y + 3), fill=colour)
    draw.text((28, 710), "Coloured outlines are renderer-observed faces mapped to static source-order triples; no unseen faces or links are inferred.", font=font(16), fill="#aab7c4")
    args.output.parent.mkdir(exist_ok=True)
    image.save(args.output)
    print(f"wrote {args.output.relative_to(ROOT)} ({count} vertices, {len(topology['faces'])} faces)")


if __name__ == "__main__":
    main()
