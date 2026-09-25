"""Add landmark anchors and a directly traced marker to the run035 vector map.

The transfer is an explicit panning join, not a new source/control identity:
the existing oracle comparison proves run035(x+142, y+36) corresponds to
run003(x, y) for the shared coastline.  The flight-object marker is separate:
its run035 coordinates come directly from the `$C4C598` line trace, while its
game ownership deliberately remains unassigned.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from PIL import Image, ImageDraw


ROOT = Path(__file__).resolve().parents[1]
PAN = (142, 36)
RUN003_ANCHORS = (
    ("GOLDEN GATE", (194, 59.5), "#e53935", "trace-backed run003 bridge contexts"),
    ("MOUNTAIN ?", (218, 94), "#f59e0b", "run003 terrain candidate"),
)
RUN035_FLIGHT_OBJECT = {
    "name": "FLIGHT OBJECT ?",
    "segments": (((103, 166), (103, 166)), ((97, 167), (107, 167)), ((98, 168), (106, 168))),
    "evidence": "run035_m_map_stable_20f_trace: C2FA7E with A5=$C4C598",
}
# Direct run003 C3559A/C355D2 map vectors, transferred by the verified pan.
RUN035_GOLDEN_GATE_LINES = (((336, 92), (336, 96)), ((336, 99), (336, 95)))
RUN035_GRID = (
    ((283, 0), (283, 179)), ((160, 0), (160, 179)), ((39, 0), (39, 179)),
    ((0, 176), (319, 176)), ((0, 47), (319, 47)),
)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path,
                        default=ROOT / "analysis/visuals/run035_m_map_projected_polygon_vectors.svg")
    parser.add_argument("--svg", type=Path,
                        default=ROOT / "analysis/visuals/run035_m_map_projected_polygon_vectors_landmarks_traced_marker.svg")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/data/run035_m_map_panned_landmarks_traced_marker.json")
    parser.add_argument("--png", type=Path,
                        default=ROOT / "analysis/visuals/run035_m_map_projected_polygon_vectors_landmarks_traced_marker.png")
    args = parser.parse_args()
    if any(path.exists() for path in (args.svg, args.output, args.png)):
        raise FileExistsError("refusing to overwrite panned-landmark artifact")
    svg = args.input.read_text(encoding="utf-8")
    if not svg.rstrip().endswith("</svg>"):
        raise RuntimeError("input is not a complete SVG")
    labels = []
    fragments = ['<g shape-rendering="geometricPrecision" font-family="monospace">']
    for (x1, y1), (x2, y2) in RUN035_GRID:
        fragments.append(f'<line x1="{x1 * 2:g}" y1="{y1:g}" x2="{x2 * 2:g}" y2="{y2:g}" stroke="#555" stroke-width="1"/>')
    for (x1, y1), (x2, y2) in RUN035_GOLDEN_GATE_LINES:
        fragments.append(f'<line x1="{x1:g}" y1="{y1:g}" x2="{x2:g}" y2="{y2:g}" stroke="#880000" stroke-width="2"/>')
    for name, (x, y), colour, evidence in RUN003_ANCHORS:
        px, py = x + PAN[0], y + PAN[1]
        labels.append({"name": name, "run003_anchor": [x, y], "run035_anchor": [px, py], "evidence": evidence})
        fragments.extend([
            f'<circle cx="{px:g}" cy="{py:g}" r="3.5" fill="{colour}" stroke="#fff" stroke-width=".8"/>',
            f'<path d="M {px + 3:g} {py - 2:g} L {px + 41:g} {py - 15:g}" stroke="{colour}" stroke-width="1.2"/>',
            f'<rect x="{px + 42:g}" y="{py - 22:g}" width="82" height="13" fill="#000" fill-opacity=".78" stroke="{colour}" stroke-width=".5"/>',
            f'<text x="{px + 45:g}" y="{py - 13:g}" fill="#fff" font-size="8">{name}</text>',
        ])
    segments = RUN035_FLIGHT_OBJECT["segments"]
    for (x1, y1), (x2, y2) in segments:
        fragments.append(f'<line x1="{x1 * 2:g}" y1="{y1:g}" x2="{x2 * 2:g}" y2="{y2:g}" stroke="#000" stroke-width="1"/>')
    marker_x, marker_y = segments[0][0]
    marker_x *= 2
    labels.append({"name": RUN035_FLIGHT_OBJECT["name"], "anchor": [marker_x, marker_y],
                   "segments": [[list(start), list(end)] for start, end in segments],
                   "evidence": RUN035_FLIGHT_OBJECT["evidence"]})
    fragments.extend([
        f'<path d="M {marker_x - 3:g} {marker_y - 3:g} L {marker_x + 43:g} {marker_y - 20:g}" stroke="#111" stroke-width="1.2"/>',
        f'<rect x="{marker_x + 44:g}" y="{marker_y - 27:g}" width="104" height="13" fill="#000" fill-opacity=".78" stroke="#111" stroke-width=".5"/>',
        f'<text x="{marker_x + 47:g}" y="{marker_y - 18:g}" fill="#fff" font-size="8">{RUN035_FLIGHT_OBJECT["name"]}</text>',
    ])
    # Keep the source bridge strokes above the callout circle at map scale.
    for (x1, y1), (x2, y2) in RUN035_GOLDEN_GATE_LINES:
        fragments.append(f'<line x1="{x1:g}" y1="{y1:g}" x2="{x2:g}" y2="{y2:g}" stroke="#880000" stroke-width="2"/>')
    fragments.append('</g>')
    args.svg.parent.mkdir(parents=True, exist_ok=True)
    args.svg.write_text(svg.rsplit("</svg>", 1)[0] + "\n" + "\n".join(fragments) + "\n</svg>\n", encoding="utf-8")
    image = Image.open(ROOT / "analysis/visuals/run035_m_map_projected_polygon_vectors.png").convert("RGB")
    draw = ImageDraw.Draw(image)
    for (x1, y1), (x2, y2) in RUN035_GRID:
        draw.line(((x1 * 2, y1), (x2 * 2, y2)), fill="#555555", width=1)
    for (x1, y1), (x2, y2) in RUN035_GOLDEN_GATE_LINES:
        draw.line(((x1, y1), (x2, y2)), fill="#880000", width=2)
    for name, (x, y), colour, _ in RUN003_ANCHORS:
        px, py = x + PAN[0], y + PAN[1]
        draw.ellipse((px - 3.5, py - 3.5, px + 3.5, py + 3.5), fill=colour, outline="white", width=1)
        draw.line(((px + 3, py - 2), (px + 41, py - 15)), fill=colour, width=1)
        draw.rectangle((px + 42, py - 22, px + 124, py - 9), fill="black", outline=colour, width=1)
        draw.text((px + 45, py - 21), name, fill="white")
    for (x1, y1), (x2, y2) in segments:
        draw.line(((x1 * 2, y1), (x2 * 2, y2)), fill="black", width=1)
    draw.line(((marker_x - 3, marker_y - 3), (marker_x + 43, marker_y - 20)), fill="#111111", width=1)
    draw.rectangle((marker_x + 44, marker_y - 27, marker_x + 148, marker_y - 14), fill="black", outline="#111111", width=1)
    draw.text((marker_x + 47, marker_y - 26), RUN035_FLIGHT_OBJECT["name"], fill="white")
    for (x1, y1), (x2, y2) in RUN035_GOLDEN_GATE_LINES:
        draw.line(((x1, y1), (x2, y2)), fill="#880000", width=2)
    image.save(args.png)
    report = {
        "classification": "panning_transferred_fixed_landmarks_plus_directly_traced_flight_object_marker",
        "source_svg": str(args.input), "panning_relation": "run035 = run003 + (142, 36) host pixels",
        "grid_segments": [[list(start), list(end)] for start, end in RUN035_GRID],
        "golden_gate_segments": [[list(start), list(end)] for start, end in RUN035_GOLDEN_GATE_LINES],
        "labels": labels,
        "qualification": "Grid and FLIGHT OBJECT ? are direct run035 renderer traces. The two red Golden Gate segments are direct run003 C3559A/C355D2 map vectors transferred by the red-pixel-validated panning relation. Mountain ? is a transferred fixed-world anchor. The marker trace does not establish ownership or semantic game identity.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"labels": len(labels), "pan": PAN, "png": str(args.png)}))


if __name__ == "__main__":
    main()
