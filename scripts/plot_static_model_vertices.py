"""Plot trace-proven immutable input vertices without inventing face connectivity."""
from __future__ import annotations

import html
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MEMORY = ROOT / "captures" / "baseline_menu" / "slow.bin"
OUTPUT = ROOT / "analysis" / "plots" / "external_aircraft_c3515e_static_vertices.svg"
START, END = 0xC3515E, 0xC35234


def main() -> None:
    data = MEMORY.read_bytes()
    vertices = []
    for address in range(START, END + 1, 6):
        offset = address - 0xC00000
        vertices.append((address, tuple(int.from_bytes(data[offset + axis * 2:offset + axis * 2 + 2], "big", signed=True)
                                        for axis in range(3))))
    if len(vertices) < 3:
        raise ValueError("expected at least three static vertices")
    extent = max(abs(value) for _, point in vertices for value in point)
    width, height, panel, margin = 1200, 500, 360, 48
    scale = (panel - 2 * margin) / (2 * extent)
    views = (("X-Y", 0, 1), ("X-Z", 0, 2), ("Y-Z", 1, 2))
    parts = [f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">',
             '<rect width="100%" height="100%" fill="#101419"/>',
             '<text x="24" y="30" fill="#f1f5f9" font-family="sans-serif" font-size="20">Static local-vertex run $C3515E-$C35234</text>',
             '<text x="24" y="54" fill="#aab7c4" font-family="sans-serif" font-size="13">36 immutable Hunk-41 triples; indices 0-21 transform-traced, 22-35 contiguous continuation candidate; no edges inferred</text>']
    for panel_index, (label, horizontal, vertical) in enumerate(views):
        origin_x, origin_y = 30 + panel_index * 390, 105
        centre_x, centre_y = origin_x + panel / 2, origin_y + panel / 2
        parts += [f'<rect x="{origin_x}" y="{origin_y}" width="{panel}" height="{panel}" fill="#151c24" stroke="#506070"/>',
                  f'<line x1="{origin_x + margin}" y1="{centre_y}" x2="{origin_x + panel - margin}" y2="{centre_y}" stroke="#536579"/>',
                  f'<line x1="{centre_x}" y1="{origin_y + margin}" x2="{centre_x}" y2="{origin_y + panel - margin}" stroke="#536579"/>',
                  f'<text x="{origin_x + 8}" y="{origin_y + 22}" fill="#f1f5f9" font-family="sans-serif" font-size="16">{label}</text>']
        for index, (_, point) in enumerate(vertices):
            x = centre_x + point[horizontal] * scale
            y = centre_y - point[vertical] * scale
            parts += [f'<circle cx="{x:.2f}" cy="{y:.2f}" r="4" fill="#65d5ff"/>',
                      f'<text x="{x + 6:.2f}" y="{y - 6:.2f}" fill="#f1f5f9" font-family="monospace" font-size="12">{index}</text>']
    parts.append('</svg>')
    OUTPUT.parent.mkdir(exist_ok=True)
    OUTPUT.write_text("\n".join(parts) + "\n", encoding="utf-8")
    print(f"wrote {OUTPUT.relative_to(ROOT)} ({len(vertices)} vertices)")


if __name__ == "__main__":
    main()
