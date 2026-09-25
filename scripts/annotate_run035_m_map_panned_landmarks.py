"""Add fixed run003 landmark anchors to the run035 source-vector map.

The transfer is an explicit panning join, not a new source/control identity:
the existing oracle comparison proves run035(x+142, y+36) corresponds to
run003(x, y) for the shared coastline.  Flight-object markers are deliberately
excluded because their ownership/motion has not been proven.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PAN = (142, 36)
RUN003_ANCHORS = (
    ("GOLDEN GATE", (194, 59.5), "#e53935", "trace-backed run003 bridge contexts"),
    ("MOUNTAIN ?", (218, 94), "#f59e0b", "run003 terrain candidate"),
)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path,
                        default=ROOT / "analysis/visuals/run035_m_map_projected_polygon_vectors.svg")
    parser.add_argument("--svg", type=Path,
                        default=ROOT / "analysis/visuals/run035_m_map_projected_polygon_vectors_landmarks.svg")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/data/run035_m_map_panned_landmarks.json")
    args = parser.parse_args()
    if args.svg.exists() or args.output.exists():
        raise FileExistsError("refusing to overwrite panned-landmark artifact")
    svg = args.input.read_text(encoding="utf-8")
    if not svg.rstrip().endswith("</svg>"):
        raise RuntimeError("input is not a complete SVG")
    labels = []
    fragments = ['<g shape-rendering="geometricPrecision" font-family="monospace">']
    for name, (x, y), colour, evidence in RUN003_ANCHORS:
        px, py = x - PAN[0], y - PAN[1]
        labels.append({"name": name, "run003_anchor": [x, y], "run035_anchor": [px, py], "evidence": evidence})
        fragments.extend([
            f'<circle cx="{px:g}" cy="{py:g}" r="3.5" fill="{colour}" stroke="#fff" stroke-width=".8"/>',
            f'<path d="M {px + 3:g} {py - 2:g} L {px + 41:g} {py - 15:g}" stroke="{colour}" stroke-width="1.2"/>',
            f'<rect x="{px + 42:g}" y="{py - 22:g}" width="82" height="13" fill="#000" fill-opacity=".78" stroke="{colour}" stroke-width=".5"/>',
            f'<text x="{px + 45:g}" y="{py - 13:g}" fill="#fff" font-size="8">{name}</text>',
        ])
    fragments.append('</g>')
    args.svg.parent.mkdir(parents=True, exist_ok=True)
    args.svg.write_text(svg.rsplit("</svg>", 1)[0] + "\n" + "\n".join(fragments) + "\n</svg>\n", encoding="utf-8")
    report = {
        "classification": "panning_identity_transferred_fixed_landmark_anchors",
        "source_svg": str(args.input), "panning_relation": "run035 = run003 - (142, 36) host pixels",
        "labels": labels,
        "excluded": "flight-object marker: movement/ownership not proven",
        "qualification": "This transfers fixed world-feature anchors using the measured coastline translation; it is not a new run035 source/control trace for either landmark.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"labels": len(labels), "pan": PAN}))


if __name__ == "__main__":
    main()
