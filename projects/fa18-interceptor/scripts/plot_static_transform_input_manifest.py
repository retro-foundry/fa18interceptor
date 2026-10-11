"""Render an orthographic point-only plot of immutable transform inputs.

No input order, edge, face, or renderer-workspace relationship is inferred.
"""
from __future__ import annotations

import argparse
import html
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
COLOURS = ("#65d5ff", "#ffcc66", "#ff8fb1", "#8ee887", "#b79cff", "#ff9f68", "#f1f5f9")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--title", required=True)
    args = parser.parse_args()
    payload = json.loads(args.input.read_text(encoding="utf-8"))
    points = [(packet["source"], index, tuple(point))
              for packet in payload["packets"]
              for index, point in enumerate(packet["triples"])]
    extent = max(1, max(abs(component) for _, _, point in points for component in point))
    width, height, panel, margin = 1800, 700, 540, 52
    scale = (panel - 2 * margin) / (2 * extent)
    views = (("X-Y", 0, 1), ("X-Z", 0, 2), ("Y-Z", 1, 2))
    parts = [f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">',
             '<rect width="100%" height="100%" fill="#101419"/>',
             f'<text x="28" y="38" fill="#f1f5f9" font-family="sans-serif" font-size="25">{html.escape(args.title)}</text>',
             '<text x="28" y="67" fill="#aab7c4" font-family="monospace" font-size="16">immutable direct input triples only · colours identify packets · no edges/faces inferred</text>']
    colours = {packet["source"]: COLOURS[index % len(COLOURS)] for index, packet in enumerate(payload["packets"])}
    for panel_index, (label, horizontal, vertical) in enumerate(views):
        ox, oy = 35 + panel_index * 590, 115
        cx, cy = ox + panel / 2, oy + panel / 2
        parts += [f'<rect x="{ox}" y="{oy}" width="{panel}" height="{panel}" fill="#151c24" stroke="#506070"/>',
                  f'<line x1="{ox + margin}" y1="{cy}" x2="{ox + panel - margin}" y2="{cy}" stroke="#536579"/>',
                  f'<line x1="{cx}" y1="{oy + margin}" x2="{cx}" y2="{oy + panel - margin}" stroke="#536579"/>',
                  f'<text x="{ox + 10}" y="{oy + 28}" fill="#f1f5f9" font-family="sans-serif" font-size="24">{label}</text>']
        for source, index, point in points:
            x, y = cx + point[horizontal] * scale, cy - point[vertical] * scale
            colour = colours[source]
            parts += [f'<circle cx="{x:.2f}" cy="{y:.2f}" r="5" fill="{colour}"/>',
                      f'<text x="{x + 7:.2f}" y="{y - 7:.2f}" fill="{colour}" font-family="monospace" font-size="12">{source[1:]}:{index}</text>']
    legend_y = 680
    for index, packet in enumerate(payload["packets"]):
        x = 28 + index * 250
        parts += [f'<circle cx="{x}" cy="{legend_y - 5}" r="6" fill="{colours[packet["source"]]}"/>',
                  f'<text x="{x + 11}" y="{legend_y}" fill="#d9e2ec" font-family="monospace" font-size="14">{packet["source"]} ({len(packet["triples"])})</text>']
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("\n".join(parts + ["</svg>", ""]), encoding="utf-8")
    print(json.dumps({"points": len(points), "output": str(args.output)}))


if __name__ == "__main__":
    main()
