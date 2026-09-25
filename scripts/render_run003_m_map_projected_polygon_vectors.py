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
# These are observed final palette colours for the run003 M-map pass.  Mask 2
# is the water fill; mask 15 is a separate small filled map overlay.
FILL_MASK_COLOURS = {2: SEA, 15: "#003300"}
GOLDEN_GATE_CONTEXTS = {"$C3559A", "$C355D2"}
LINE_COLOURS = {
    "$C4C59E": GRID,
    "$C4C598": "#000000",
    "$C3559A": "#880000",
    "$C355D2": "#880000",
}


def canonical_pass(polygons: list[dict], start: int) -> list[dict]:
    """Return entries before the first exact repeat of the opening polygon."""
    if not 0 <= start < len(polygons):
        raise ValueError("start submission outside supplied capture")
    first = (polygons[start]["context_a5"], polygons[start]["projected_pairs"])
    for index, polygon in enumerate(polygons[start + 1:], start + 1):
        if (polygon["context_a5"], polygon["projected_pairs"]) == first:
            return polygons[start:index]
    raise RuntimeError("capture contains no repeated opening polygon to bound a map pass")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path,
                        default=ROOT / "build/run003_m_map_projected_polygons/projected_polygons.json")
    parser.add_argument("--line-input", type=Path,
                        default=ROOT / "analysis/data/run003_m_map_renderer_vectors.json")
    parser.add_argument("--no-lines", action="store_true",
                        help="omit run003-specific captured grid/symbol vectors")
    parser.add_argument("--start-submission", type=int, default=0,
                        help="skip an observed setup prefix before the repeating map pass")
    parser.add_argument("--submissions", type=int,
                        help="use exactly this many submissions after --start-submission; for a moving pass without an exact repeat")
    parser.add_argument("--expected-submissions", type=int, default=42,
                        help="fail if the bounded repeating pass has a different polygon count")
    parser.add_argument("--no-annotations", action="store_true",
                        help="omit run003-specific landmark annotations")
    parser.add_argument("--svg", type=Path,
                        default=ROOT / "analysis/visuals/run003_m_map_projected_polygon_vectors.svg")
    parser.add_argument("--png", type=Path,
                        default=ROOT / "analysis/visuals/run003_m_map_projected_polygon_vectors.png")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/data/run003_m_map_projected_polygon_vectors.json")
    parser.add_argument("--oracle", type=Path,
                        default=ROOT / "analysis/visuals/run003_m_map_display.png",
                        help="captured screen used only to score land/sea agreement")
    parser.add_argument("--no-oracle-validation", action="store_true",
                        help="render without making an invalid cross-capture pixel comparison")
    args = parser.parse_args()
    if any(path.exists() for path in (args.svg, args.png, args.output)):
        raise FileExistsError("refusing to overwrite vector-map evidence output")
    captured = json.loads(args.input.read_text(encoding="utf-8"))
    if args.submissions is None:
        polygons = canonical_pass(captured["polygons"], args.start_submission)
    else:
        if args.submissions <= 0:
            raise ValueError("--submissions must be positive")
        polygons = captured["polygons"][args.start_submission:args.start_submission + args.submissions]
        if len(polygons) != args.submissions:
            raise RuntimeError("capture ends before requested bounded submission count")
    lines = [] if args.no_lines else json.loads(args.line_input.read_text(encoding="utf-8"))["vectors"]
    if len(polygons) != args.expected_submissions:
        raise RuntimeError(f"expected {args.expected_submissions} pass submissions, got {len(polygons)}")

    svg = [
        '<?xml version="1.0" encoding="UTF-8"?>',
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{HOST_W}" height="{HOST_H}" viewBox="0 0 {HOST_W} {HOST_H}" shape-rendering="crispEdges">',
        '<title>Run003 M-map projected polygon vectors</title>',
        '<desc>Blue water polygons are captured at C4B390 before C2FF48, over the green land base.</desc>',
        f'<rect width="{HOST_W}" height="{HOST_H}" fill="{LAND}"/>',
        '<g stroke="none">',
    ]
    for polygon in polygons:
        pairs = polygon["projected_pairs"]
        if len(pairs) < 3:
            continue
        points = " ".join(f"{x * 2},{y}" for x, y in pairs)
        fill = FILL_MASK_COLOURS.get(polygon.get("active_fill_plane_mask"), SEA)
        svg.append(f'<polygon points="{points}" fill="{fill}"><title>{polygon["context_a5"]}, submission {polygon["submission"]}</title></polygon>')
    svg.append('</g>')
    for line in lines:
        x1, y1, x2, y2 = line["endpoints"]
        colour = LINE_COLOURS.get(line["context"], GRID)
        svg.append(f'<line x1="{x1 * 2}" y1="{y1}" x2="{x2 * 2}" y2="{y2}" '
                   f'stroke="{colour}" stroke-width="1"/>')
    # These anchors are independently tied to run003 contexts and cannot be
    # transferred to another panned map capture without a separate join.
    if not args.no_annotations:
        svg.extend([
        '<g shape-rendering="geometricPrecision" font-family="monospace">',
        '<circle cx="194" cy="59.5" r="4" fill="#e53935" stroke="#fff" stroke-width="1"/>',
        '<path d="M 198 56.5 L 246 40.5" stroke="#e53935" stroke-width="1.5"/>',
        '<rect x="247" y="32.5" width="78" height="13" fill="#000" fill-opacity=".78" stroke="#e53935" stroke-width=".5"/>',
        '<text x="250" y="41.5" fill="#fff" font-size="8">GOLDEN GATE</text>',
        '</g>', '</svg>',
        ])
    else:
        svg.append('</svg>')
    args.svg.parent.mkdir(parents=True, exist_ok=True)
    args.svg.write_text("\n".join(svg) + "\n", encoding="utf-8")

    # Independent SVG-equivalent rasterisation used for visual checking only.
    image = Image.new("RGB", (HOST_W, HOST_H), LAND)
    draw = ImageDraw.Draw(image)
    for polygon in polygons:
        if len(polygon["projected_pairs"]) >= 3:
            draw.polygon([(x * 2, y) for x, y in polygon["projected_pairs"]],
                         fill=FILL_MASK_COLOURS.get(polygon.get("active_fill_plane_mask"), SEA))
    for line in lines:
        x1, y1, x2, y2 = line["endpoints"]
        draw.line(((x1 * 2, y1), (x2 * 2, y2)),
                  fill=LINE_COLOURS.get(line["context"], GRID))
    args.png.parent.mkdir(parents=True, exist_ok=True)
    image.save(args.png)
    compared = matches = 0
    if not args.no_oracle_validation:
        oracle = Image.open(args.oracle).convert("RGB").crop((40, 16, 680, 216))
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
    diagnostic_writes = captured.get("debug_writes", [])
    report = {
        "classification": ("diagnostic_projected_renderer_vectors_before_area_blit"
                           if diagnostic_writes else "scenario_backed_projected_renderer_vectors_before_area_blit"),
        "authority": {"polygon_capture": str(args.input),
                      "line_capture": None if args.no_lines else str(args.line_input)},
        "debug_writes": diagnostic_writes,
        "canonical_polygon_submissions": len(polygons),
        "polygon_context_counts": {context: sum(item["context_a5"] == context for item in polygons)
                                   for context in sorted({item["context_a5"] for item in polygons})},
        "fill_mask_counts": {str(mask): sum(item.get("active_fill_plane_mask") == mask for item in polygons)
                             for mask in sorted({item.get("active_fill_plane_mask") for item in polygons
                                                 if item.get("active_fill_plane_mask") is not None})},
        "line_context_counts": {context: sum(item["context"] == context for item in lines)
                                for context in sorted({item["context"] for item in lines})},
        "palette": {"land": LAND, "sea": SEA, "grid": GRID,
                    "mask_15_overlay": FILL_MASK_COLOURS[15]},
        "land_sea_oracle_validation": (None if args.no_oracle_validation else
            {"oracle": str(args.oracle), "pixels_compared": compared,
             "matching_pixels": matches, "agreement": matches / compared if compared else None}),
        "annotations_included": not args.no_annotations,
        "landmark": ({"name": "Golden Gate", "anchor": [194, 59.5],
                      "evidence": "$C3559A/$C355D2 bridge contexts in captured line vectors"}
                     if not args.no_annotations else None),
        "qualification": (
            "Polygon vertices and line endpoints are direct renderer vectors, not bitplane runs. Where captured, each polygon retains its active fill-plane mask; mask 2 is water and mask 15 is a separately observed dark-green map overlay. Ordinary traced line overlays are drawn grey, the verified C3559A/C355D2 strokes red, and the conditional C4C598 symbol black. Their semantic identities remain unassigned except for Golden Gate. The exact hardware area-fill edge rules are still separately retained for pixel-parity work."
            if not diagnostic_writes else
            "Polygon vertices are direct renderer vectors, not bitplane runs. Their source replay has debugger writes recorded above, so this render proves "
            "the affected display-state dependency only; it is not a normal gameplay map view or a static global-coordinate decode."),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"polygons": len(polygons), "svg": str(args.svg), "png": str(args.png)}))


if __name__ == "__main__":
    main()
