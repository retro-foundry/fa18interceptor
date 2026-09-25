"""Render one captured M-map pass from its projected polygon vectors.

The polygons come from `$C4B390` immediately before `$C2FF48`, after the
game's own clipping/projection but before its area-blit fill implementation.
The PNG is only a validation rasterisation of this SVG; it is not its source.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from PIL import Image, ImageDraw


ROOT = Path(__file__).resolve().parents[1]
W, H = 320, 180
HOST_W, HOST_H = 640, 200
LAND, SEA, GRID = "#115511", "#003366", "#555555"
GOLDEN_GATE_CONTEXTS = {"$C3559A", "$C355D2"}


def canonical_pass(polygons: list[dict]) -> list[dict]:
    """Return entries before the first exact repeat of the opening polygon."""
    first = (polygons[0]["context_a5"], polygons[0]["projected_pairs"])
    for index, polygon in enumerate(polygons[1:], 1):
        if (polygon["context_a5"], polygon["projected_pairs"]) == first:
            return polygons[:index]
    raise RuntimeError("capture contains no repeated opening polygon to bound a map pass")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path,
                        default=ROOT / "build/run003_m_map_projected_polygons/projected_polygons.json")
    parser.add_argument("--line-input", type=Path,
                        default=ROOT / "analysis/data/run003_m_map_renderer_vectors.json")
    parser.add_argument("--svg", type=Path,
                        default=ROOT / "analysis/visuals/run003_m_map_projected_polygon_vectors.svg")
    parser.add_argument("--png", type=Path,
                        default=ROOT / "analysis/visuals/run003_m_map_projected_polygon_vectors.png")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/data/run003_m_map_projected_polygon_vectors.json")
    parser.add_argument("--oracle", type=Path,
                        default=ROOT / "analysis/visuals/run003_m_map_display.png",
                        help="captured screen used only to score land/sea agreement")
    args = parser.parse_args()
    if any(path.exists() for path in (args.svg, args.png, args.output)):
        raise FileExistsError("refusing to overwrite vector-map evidence output")
    captured = json.loads(args.input.read_text(encoding="utf-8"))
    polygons = canonical_pass(captured["polygons"])
    lines = json.loads(args.line_input.read_text(encoding="utf-8"))["vectors"]
    if len(polygons) != 42:
        raise RuntimeError(f"expected 42 first-pass submissions, got {len(polygons)}")

    svg = [
        '<?xml version="1.0" encoding="UTF-8"?>',
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{HOST_W}" height="{HOST_H}" viewBox="0 0 {HOST_W} {HOST_H}" shape-rendering="crispEdges">',
        '<title>Run003 M-map projected polygon vectors</title>',
        '<desc>Blue water polygons are captured at C4B390 before C2FF48, over the green land base.</desc>',
        f'<rect width="{HOST_W}" height="{HOST_H}" fill="{LAND}"/>',
        f'<g fill="{SEA}" stroke="none">',
    ]
    for polygon in polygons:
        pairs = polygon["projected_pairs"]
        if len(pairs) < 3:
            continue
        points = " ".join(f"{x * 2},{y}" for x, y in pairs)
        svg.append(f'<polygon points="{points}"><title>{polygon["context_a5"]}, submission {polygon["submission"]}</title></polygon>')
    svg.append('</g>')
    svg.append(f'<g fill="none" stroke="{GRID}" stroke-width="1">')
    for line in lines:
        if line["context"] != "$C4C59E":
            continue
        x1, y1, x2, y2 = line["endpoints"]
        svg.append(f'<line x1="{x1 * 2}" y1="{y1}" x2="{x2 * 2}" y2="{y2}"/>')
    svg.append('</g>')
    svg.append('<g fill="none" stroke="#000000" stroke-width="1">')
    for line in lines:
        if line["context"] != "$C4C598":
            continue
        x1, y1, x2, y2 = line["endpoints"]
        svg.append(f'<line x1="{x1 * 2}" y1="{y1}" x2="{x2 * 2}" y2="{y2}"/>')
    svg.append('</g>')
    # This anchor is independently tied to the two Golden Gate bridge contexts.
    svg.extend([
        '<g shape-rendering="geometricPrecision" font-family="monospace">',
        '<circle cx="194" cy="59.5" r="4" fill="#e53935" stroke="#fff" stroke-width="1"/>',
        '<path d="M 198 56.5 L 246 40.5" stroke="#e53935" stroke-width="1.5"/>',
        '<rect x="247" y="32.5" width="78" height="13" fill="#000" fill-opacity=".78" stroke="#e53935" stroke-width=".5"/>',
        '<text x="250" y="41.5" fill="#fff" font-size="8">GOLDEN GATE</text>',
        '</g>', '</svg>',
    ])
    args.svg.parent.mkdir(parents=True, exist_ok=True)
    args.svg.write_text("\n".join(svg) + "\n", encoding="utf-8")

    # Independent SVG-equivalent rasterisation used for visual checking only.
    image = Image.new("RGB", (HOST_W, HOST_H), LAND)
    draw = ImageDraw.Draw(image)
    for polygon in polygons:
        if len(polygon["projected_pairs"]) >= 3:
            draw.polygon([(x * 2, y) for x, y in polygon["projected_pairs"]], fill=SEA)
    for line in lines:
        if line["context"] == "$C4C59E":
            x1, y1, x2, y2 = line["endpoints"]
            draw.line(((x1 * 2, y1), (x2 * 2, y2)), fill=GRID)
    for line in lines:
        if line["context"] == "$C4C598":
            x1, y1, x2, y2 = line["endpoints"]
            draw.line(((x1 * 2, y1), (x2 * 2, y2)), fill="#000000")
    args.png.parent.mkdir(parents=True, exist_ok=True)
    image.save(args.png)
    oracle = Image.open(args.oracle).convert("RGB").crop((40, 16, 680, 216))
    compared = matches = 0
    for y in range(H):
        for x in range(HOST_W):
            expected = oracle.getpixel((x, y))
            if expected not in ((17, 85, 17), (0, 51, 102)):
                continue
            actual = image.getpixel((x, y))
            if actual not in ((17, 85, 17), (0, 51, 102)):
                continue
            compared += 1
            matches += actual == expected
    report = {
        "classification": "scenario_backed_projected_renderer_vectors_before_area_blit",
        "authority": {"polygon_capture": str(args.input), "line_capture": str(args.line_input)},
        "canonical_polygon_submissions": len(polygons),
        "polygon_context_counts": {context: sum(item["context_a5"] == context for item in polygons)
                                   for context in sorted({item["context_a5"] for item in polygons})},
        "palette": {"land": LAND, "sea": SEA, "grid": GRID},
        "land_sea_oracle_validation": {"oracle": str(args.oracle), "pixels_compared": compared,
                                         "matching_pixels": matches,
                                         "agreement": matches / compared if compared else None},
        "landmark": {"name": "Golden Gate", "anchor": [194, 59.5],
                     "evidence": "$C3559A/$C355D2 bridge contexts in captured line vectors"},
        "traced_map_symbol": {"anchor_bounds": [50, 130, 70, 132],
                              "evidence": "three $C4C598 C2FA7E vector strokes",
                              "semantic_status": "structural; not identified as aircraft, base, or airfield"},
        "qualification": "Polygon vertices are direct renderer vectors, not bitplane runs. The exact hardware area-fill edge rules are still separately retained for pixel-parity work.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"polygons": len(polygons), "svg": str(args.svg), "png": str(args.png)}))


if __name__ == "__main__":
    main()
