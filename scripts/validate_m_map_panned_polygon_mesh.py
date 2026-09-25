"""Check the ordered direct-polygon WebGL mesh against the source-vector fill."""
from __future__ import annotations

import json
from pathlib import Path

from PIL import Image, ImageDraw

from render_m_map_panned_vector_mosaic import (
    HEIGHT,
    RUNS,
    VIEWPORT_HEIGHT,
    VIEWPORT_WIDTH,
    WIDTH,
    canonical_pass,
)


ROOT = Path(__file__).resolve().parents[1]
MESH = ROOT / "analysis/visuals/m_map_panned_projected_polygon_mosaic_v4_direct_mesh.js"
MIN_MATCHING_PIXELS = 365_000


def load_mesh() -> dict:
    text = MESH.read_text(encoding="utf-8")
    return json.loads(text.split("=", 1)[1].rstrip(";\n"))


def source_fill() -> Image.Image:
    image = Image.new("RGB", (WIDTH, HEIGHT), "black")
    draw = ImageDraw.Draw(image)
    for name, path, tx, ty, expected, start, count in RUNS:
        rows = json.loads(path.read_text(encoding="utf-8"))["polygons"]
        rows = canonical_pass(rows) if count is None else rows[start:start + count]
        if len(rows) != expected:
            raise RuntimeError(f"{name}: expected {expected} polygons, got {len(rows)}")
        draw.rectangle((tx, ty, tx + VIEWPORT_WIDTH - 1, ty + VIEWPORT_HEIGHT - 1), fill="#115511")
        for row in rows:
            points = [(x * 2 + tx, y + ty) for x, y in row["projected_pairs"]]
            if len(points) >= 3:
                draw.polygon(points, fill="#003300" if row.get("active_fill_plane_mask") == 15 else "#003366")
    return image


def mesh_fill(mesh: dict) -> Image.Image:
    colours = {"land": "#115511", "sea": "#003366", "overlay": "#003300"}
    image = Image.new("RGB", (WIDTH, HEIGHT), "black")
    draw = ImageDraw.Draw(image)
    for item in mesh["draws"]:
        values = item["vertices"]
        points = [(values[index], values[index + 1]) for index in range(0, len(values), 2)]
        if item["primitive"] == "triangles":
            for index in range(0, len(points), 3):
                draw.polygon(points[index:index + 3], fill=colours[item["kind"]])
        elif item["primitive"] == "line_loop":
            draw.line(points + [points[0]], fill=colours[item["kind"]], width=1)
        else:
            raise RuntimeError(f"unexpected direct mesh primitive {item['primitive']}")
    return image


def main() -> None:
    mesh = load_mesh()
    matching = sum(left == right for left, right in zip(source_fill().getdata(), mesh_fill(mesh).getdata()))
    if matching < MIN_MATCHING_PIXELS:
        raise RuntimeError(f"direct mesh has only {matching} matching source-fill pixels; expected at least {MIN_MATCHING_PIXELS}")
    print(json.dumps({"matching_pixels": matching, "total_pixels": WIDTH * HEIGHT,
                      "agreement": matching / (WIDTH * HEIGHT), "draws": len(mesh["draws"])}))


if __name__ == "__main__":
    main()
