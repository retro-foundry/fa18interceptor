"""Join two directly captured M-map polygon passes using their measured pan.

This is a source-vector mosaic: each filled shape is a `$C4B390` polygon
captured immediately before `$C2FF48`.  It deliberately excludes screen-fixed
grid lines and the state-dependent flight-object marker.
"""
from __future__ import annotations

import json
from pathlib import Path

from PIL import Image, ImageDraw


ROOT = Path(__file__).resolve().parents[1]
LAND, SEA = "#115511", "#003366"
RUNS = (
    # Normalise at run042's upper-left. run042 = run035 + (130,61), and
    # run035 = run003 + (142,36), so add (130,61) to run035 and (272,97) to run003.
    ("run042", ROOT / "build/run042_m_map_projected_polygons/projected_polygons.json", 0, 0, 40),
    ("run035", ROOT / "build/run035_m_map_projected_polygons/projected_polygons.json", 130, 61, 49),
    ("run003", ROOT / "build/run003_m_map_projected_polygons/projected_polygons.json", 272, 97, 42),
)
WIDTH, HEIGHT = 912, 297
ANCHORS = (("GOLDEN GATE", 466, 156.5, "#e53935"), ("MOUNTAIN ?", 490, 191, "#f59e0b"))
GOLDEN_GATE_LINES = (((466, 153), (466, 157)), ((466, 160), (466, 156)))


def canonical_pass(polygons: list[dict]) -> list[dict]:
    first = polygons[0]["context_a5"], polygons[0]["projected_pairs"]
    for index, polygon in enumerate(polygons[1:], 1):
        if (polygon["context_a5"], polygon["projected_pairs"]) == first:
            return polygons[:index]
    raise RuntimeError("missing repeated opening polygon")


def main() -> None:
    svg_path = ROOT / "analysis/visuals/m_map_panned_projected_polygon_mosaic.svg"
    png_path = ROOT / "analysis/visuals/m_map_panned_projected_polygon_mosaic.png"
    report_path = ROOT / "analysis/data/m_map_panned_projected_polygon_mosaic.json"
    if any(path.exists() for path in (svg_path, png_path, report_path)):
        raise FileExistsError("refusing to overwrite mosaic evidence output")
    svg = [
        '<?xml version="1.0" encoding="UTF-8"?>',
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{WIDTH}" height="{HEIGHT}" viewBox="0 0 {WIDTH} {HEIGHT}" shape-rendering="crispEdges">',
        '<title>M-map source-vector panned mosaic</title>',
        '<desc>Two C4B390-before-C2FF48 polygon captures joined by the measured run035-to-run003 coastline translation.</desc>',
        f'<rect width="{WIDTH}" height="{HEIGHT}" fill="{LAND}"/>', '<g fill="#003366" stroke="none">',
    ]
    image = Image.new("RGB", (WIDTH, HEIGHT), LAND)
    draw = ImageDraw.Draw(image)
    report_runs = []
    for name, path, tx, ty, expected in RUNS:
        polygons = canonical_pass(json.loads(path.read_text(encoding="utf-8"))["polygons"])
        if len(polygons) != expected:
            raise RuntimeError(f"{name}: expected {expected} polygons, got {len(polygons)}")
        for polygon in polygons:
            pairs = polygon["projected_pairs"]
            if len(pairs) < 3:
                continue
            points = [(x * 2 + tx, y + ty) for x, y in pairs]
            svg.append('<polygon points="' + " ".join(f"{x},{y}" for x, y in points) + f'"><title>{name}: {polygon["context_a5"]}, submission {polygon["submission"]}</title></polygon>')
            draw.polygon(points, fill=SEA)
        report_runs.append({"name": name, "polygon_capture": str(path), "translation": [tx, ty], "canonical_submissions": len(polygons)})
    svg.append('</g><g shape-rendering="geometricPrecision" font-family="monospace">')
    for (x1, y1), (x2, y2) in GOLDEN_GATE_LINES:
        svg.append(f'<line x1="{x1}" y1="{y1}" x2="{x2}" y2="{y2}" stroke="#880000" stroke-width="2"/>')
        draw.line(((x1, y1), (x2, y2)), fill="#880000", width=2)
    for name, x, y, colour in ANCHORS:
        svg.extend([f'<circle cx="{x}" cy="{y}" r="4" fill="{colour}" stroke="#fff" stroke-width="1"/>',
                    f'<path d="M {x + 4} {y - 3} L {x + 51} {y - 18}" stroke="{colour}" stroke-width="1.5"/>',
                    f'<rect x="{x + 52}" y="{y - 25}" width="92" height="13" fill="#000" fill-opacity=".78" stroke="{colour}" stroke-width=".5"/>',
                    f'<text x="{x + 55}" y="{y - 16}" fill="#fff" font-size="8">{name}</text>'])
        draw.ellipse((x - 4, y - 4, x + 4, y + 4), fill=colour, outline="white", width=1)
        draw.line(((x + 4, y - 3), (x + 51, y - 18)), fill=colour, width=1)
        draw.rectangle((x + 52, y - 25, x + 144, y - 12), fill="black", outline=colour, width=1)
        draw.text((x + 55, y - 24), name, fill="white")
    svg.append('</g></svg>')
    svg_path.write_text("\n".join(svg) + "\n", encoding="utf-8")
    image.save(png_path)
    report = {
        "classification": "two_capture_source_vector_coastline_mosaic",
        "runs": report_runs,
        "landmarks": [{"name": name, "anchor": [x, y]} for name, x, y, _ in ANCHORS],
        "golden_gate_segments": [[list(start), list(end)] for start, end in GOLDEN_GATE_LINES],
        "qualification": "The normalized mosaic places run042 at (0,0), run035 at +130,+61, and run003 at +272,+97 host pixels. The first translation is the 98.4816%-agreement run035/run042 water-pixel join; the latter composes it with run035 = run003 + (142,36). Its red Golden Gate vectors are C3559A/C355D2 lines transferred through the red-pixel-validated relation. This is a joined observed coverage view, not absolute global coordinates, a complete world map, or a full terrain-model extraction. Screen-relative grid and state-dependent flight-object symbols are excluded.",
    }
    report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"runs": len(report_runs), "svg": str(svg_path), "png": str(png_path)}))


if __name__ == "__main__":
    main()
