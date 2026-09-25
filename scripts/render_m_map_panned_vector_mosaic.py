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
    ("run037", ROOT / "build/run037_m_map_projected_polygons/projected_polygons.json", 14, 45, 47, None, None),
    ("run035", ROOT / "build/run035_m_map_projected_polygons/projected_polygons.json", 130, 61, 49, None, None),
    # run035 = run038 + (24,10), measured from the exact blue-water join.
    ("run038", ROOT / "build/run038_m_map_projected_polygons/projected_polygons.json", 154, 71, 58, 8, 58),
    ("run041", ROOT / "build/run041_m_map_projected_polygons/projected_polygons.json", 172, 72, 50, 10, 50),
    # run035 = run001 + (64,60), measured from the exact blue-water join.
    ("run001", ROOT / "build/run001_m_map_projected_polygons/projected_polygons.json", 194, 121, 36, 0, 36),
    # run035 = run024 + (140,35), measured from the exact blue-water join.
    ("run024", ROOT / "build/run024_m_map_projected_polygons/projected_polygons.json", 270, 96, 42, 0, 42),
    ("run003", ROOT / "build/run003_m_map_projected_polygons/projected_polygons.json", 272, 97, 42, None, None),
    # run035 = run004 + (226,109), measured from the exact blue-water join.
    ("run004", ROOT / "build/run004_m_map_projected_polygons/projected_polygons.json", 356, 170, 22, 2, 22),
)
WIDTH, HEIGHT = 1000, 370
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
        '<desc>Two C4B390-before-C2FF48 polygon captures joined by the measured run035-to-run003 coastline translation.</desc>',
        f'<rect width="{WIDTH}" height="{HEIGHT}" fill="{LAND}"/>', '<g fill="#003366" stroke="none">',
    ]
    image = Image.new("RGB", (WIDTH, HEIGHT), LAND)
    draw = ImageDraw.Draw(image)
    report_runs = []
    for name, path, tx, ty, expected, start, count in RUNS:
        captured = json.loads(path.read_text(encoding="utf-8"))["polygons"]
        polygons = canonical_pass(captured) if count is None else captured[start:start + count]
        if len(polygons) != expected:
            raise RuntimeError(f"{name}: expected {expected} polygons, got {len(polygons)}")
        for polygon in polygons:
            pairs = polygon["projected_pairs"]
            if len(pairs) < 3:
                continue
            points = [(x * 2 + tx, y + ty) for x, y in pairs]
            svg.append('<polygon points="' + " ".join(f"{x},{y}" for x, y in points) + f'"><title>{name}: {polygon["context_a5"]}, submission {polygon["submission"]}</title></polygon>')
            draw.polygon(points, fill=SEA)
        report_runs.append({"name": name, "polygon_capture": str(path), "translation": [tx, ty], "canonical_submissions": len(polygons),
                            "start_submission": start, "bounded_submissions": count})
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
        "classification": "multi_capture_source_vector_coastline_mosaic",
        "runs": report_runs,
        "landmarks": [{"name": name, "anchor": [x, y]} for name, x, y, _ in ANCHORS],
        "golden_gate_segments": [[list(start), list(end)] for start, end in GOLDEN_GATE_LINES],
        "qualification": "The normalized mosaic places run042 at (0,0), run002 at +16,+131, run037 at +14,+45, run035 at +130,+61, run038 at +154,+71, run041 at +172,+72, run001 at +194,+121, run024 at +270,+96, run003 at +272,+97, and run004 at +356,+170 host pixels. Run004 is an explicitly bounded 22-submission pass beginning at collector submission 2 from an independent Base-3 free-flight checkpoint; its run004/run035 join is 99.3979% agreement across 28,566 water pixels. Run001 is a 36-submission pass from an independent Base-1 free-flight checkpoint; its run001/run035 join is 97.9020% agreement across 67,968 water pixels. Run002 is a 43-submission pass from an independent Base-4 free-flight checkpoint; its run002/run035 join is 97.9792% agreement across 56,808 water pixels. Run024 is a 42-submission pass from the user-verified San Francisco cockpit checkpoint; its run024/run035 join is 98.1594% agreement across 71,500 water pixels. Run038 uses an explicit stable 58-submission pass beginning at collector submission 8; its run038/run035 join is 98.2684% agreement across 103,488 water pixels. Run041 uses an explicit moving 50-submission bounded pass beginning at collector submission 10. Its run041/run035 join is 98.2697% agreement across 99,866 water pixels and composes with run035/run042. The run037/run035 join is 97.9997% agreement across 84,888 water pixels; run035/run042 is 98.4816%, and run003 follows the established +142,+36 relation from run035. Its red Golden Gate vectors are C3559A/C355D2 lines transferred through the red-pixel-validated relation. The reusable C3B720/C3B6B0 component is deliberately excluded as an unproven landmark. This is a joined observed coverage view, not absolute global coordinates, a complete world map, or a full terrain-model extraction. Screen-relative grid and state-dependent object symbols are excluded.",
    }
    report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"runs": len(report_runs), "svg": str(svg_path), "png": str(png_path)}))


if __name__ == "__main__":
    main()
