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
    # Normalise at run042's upper-left. run042 = run035 + (130,61),
    # run035 = run037 + (116,16), and run035 = run003 + (142,36).
    ("run042", ROOT / "build/run042_m_map_projected_polygons/projected_polygons.json", 0, 0, 40, None, None),
    # run035 = run002 + (-114,70), measured from the exact blue-water join.
    ("run002", ROOT / "build/run002_m_map_projected_polygons/projected_polygons.json", 16, 131, 43, 0, 43),
    # run035 = later run002 + (-120,61), measured from the exact blue-water join.
    ("run002_late", ROOT / "build/run002_late_m_map_projected_polygons/projected_polygons.json", 10, 122, 43, 0, 43),
    ("run037", ROOT / "build/run037_m_map_projected_polygons/projected_polygons.json", 14, 45, 47, None, None),
    ("run035", ROOT / "build/run035_m_map_projected_polygons/projected_polygons.json", 130, 61, 49, None, None),
    # run035 = run038 + (24,10), measured from the exact blue-water join.
    ("run038", ROOT / "build/run038_m_map_projected_polygons/projected_polygons.json", 154, 71, 58, 8, 58),
    ("run041", ROOT / "build/run041_m_map_projected_polygons/projected_polygons.json", 172, 72, 50, 10, 50),
    # run033 frame 5,250 = run035 + (80,18), measured from exact blue-water overlap.
    # Its C3559A/C355D2 lines independently place the Golden Gate at this join.
    ("run033_5250", ROOT / "build/run033_frame05250_m_map_projected_polygons/projected_polygons.json", 210, 79, 49, None, None),
    # run035 = run001 + (64,60), measured from the exact blue-water join.
    ("run001", ROOT / "build/run001_m_map_projected_polygons/projected_polygons.json", 194, 121, 36, 0, 36),
    # run035 = run024 + (140,35), measured from the exact blue-water join.
    ("run024", ROOT / "build/run024_m_map_projected_polygons/projected_polygons.json", 270, 96, 42, 0, 42),
    ("run003", ROOT / "build/run003_m_map_projected_polygons/projected_polygons.json", 272, 97, 42, None, None),
    # run035 = run004 + (226,109), measured from the exact blue-water join.
    ("run004", ROOT / "build/run004_m_map_projected_polygons/projected_polygons.json", 356, 170, 22, 2, 22),
)
WIDTH, HEIGHT = 1000, 370
VIEWPORT_WIDTH, VIEWPORT_HEIGHT = 640, 180
ANCHORS = (("GOLDEN GATE", 466, 156.5, "#e53935"),)
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
        '<desc>Direct C4B390-before-C2FF48 polygon captures joined by measured coastline translations. Black is uncaptured space.</desc>',
        f'<rect width="{WIDTH}" height="{HEIGHT}" fill="#000"/>',
    ]
    image = Image.new("RGB", (WIDTH, HEIGHT), "black")
    draw = ImageDraw.Draw(image)
    report_runs = []
    coverage_svg = []
    polygon_svg = []
    raster_polygons = []
    for name, path, tx, ty, expected, start, count in RUNS:
        captured = json.loads(path.read_text(encoding="utf-8"))["polygons"]
        polygons = canonical_pass(captured) if count is None else captured[start:start + count]
        if len(polygons) != expected:
            raise RuntimeError(f"{name}: expected {expected} polygons, got {len(polygons)}")
        coverage_svg.append(f'<rect x="{tx}" y="{ty}" width="{VIEWPORT_WIDTH}" height="{VIEWPORT_HEIGHT}" fill="{LAND}"><title>{name}: observed map viewport</title></rect>')
        for polygon in polygons:
            pairs = polygon["projected_pairs"]
            if len(pairs) < 3:
                continue
            points = [(x * 2 + tx, y + ty) for x, y in pairs]
            polygon_svg.append('<polygon points="' + " ".join(f"{x},{y}" for x, y in points) + f'"><title>{name}: {polygon["context_a5"]}, submission {polygon["submission"]}</title></polygon>')
            raster_polygons.append(points)
        report_runs.append({"name": name, "polygon_capture": str(path), "translation": [tx, ty], "canonical_submissions": len(polygons),
                            "start_submission": start, "bounded_submissions": count})
    svg.extend(coverage_svg)
    svg.extend(['<g fill="#003366" stroke="none">', *polygon_svg, '</g>'])
    for _, _, tx, ty, _, _, _ in RUNS:
        draw.rectangle((tx, ty, tx + VIEWPORT_WIDTH - 1, ty + VIEWPORT_HEIGHT - 1), fill=LAND)
    for points in raster_polygons:
        draw.polygon(points, fill=SEA)
    svg.append('<g shape-rendering="geometricPrecision" font-family="monospace">')
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
        "classification": "multi_capture_source_vector_coastline_mosaic",
        "runs": report_runs,
        "landmarks": [{"name": name, "anchor": [x, y]} for name, x, y, _ in ANCHORS],
        "golden_gate_segments": [[list(start), list(end)] for start, end in GOLDEN_GATE_LINES],
        "qualification": "Each green rectangle is one directly observed 640×180 M-map viewport; blue polygons are direct renderer vectors inside that observed coverage, while black is uncaptured space. The normalized mosaic places run042 at (0,0), run002 at +16,+131, later run002 at +10,+122, run037 at +14,+45, run035 at +130,+61, run038 at +154,+71, run041 at +172,+72, run033 frame 5,250 at +210,+79, run001 at +194,+121, run024 at +270,+96, run003 at +272,+97, and run004 at +356,+170 host pixels. Its red Golden Gate vectors are C3559A/C355D2 lines directly captured both in run033's user-identified bridge state and in the prior map state, agreeing through the red-pixel-validated join. Reusable components are deliberately excluded as unproven landmarks. This is a joined observed coverage view, not absolute global coordinates, a complete world map, or a full terrain-model extraction. Screen-relative grid and state-dependent object symbols are excluded.",
    }
    report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"runs": len(report_runs), "svg": str(svg_path), "png": str(png_path)}))


if __name__ == "__main__":
    main()
