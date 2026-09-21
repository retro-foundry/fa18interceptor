"""Plot C3925 renderer faces against their traced C39D2A static triples."""
from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MEMORY = ROOT / "captures" / "baseline_menu" / "slow.bin"
TOPOLOGY = ROOT / "analysis" / "data" / "c39d2a_c3925_face_topology.json"
OUTPUT = ROOT / "analysis" / "plots" / "c39d2a_c3925_static_topology_candidate.svg"


def main() -> None:
    topology = json.loads(TOPOLOGY.read_text(encoding="utf-8"))
    start = int(topology["vertex_source"]["start"][1:], 16)
    count = topology["vertex_source"]["count"]
    memory = MEMORY.read_bytes()
    points = [tuple(int.from_bytes(memory[start - 0xC00000 + index * 6 + axis * 2:start - 0xC00000 + index * 6 + axis * 2 + 2], "big", signed=True)
                    for axis in range(3)) for index in range(count)]
    views = (("X-Y", lambda point: (point[0], point[1])), ("X-Z", lambda point: (point[0], point[2])),
             ("Y-Z", lambda point: (point[1], point[2])), ("isometric", lambda point: (point[0] - point[1], point[2] - (point[0] + point[1]) / 2)))
    width, height, panel, margin = 1640, 560, 360, 44
    colours = ("#65d5ff", "#ffcc66", "#8ee28e", "#ff8fab", "#c9a7ff", "#f7e36b", "#70e0c4", "#ffad70", "#a4c2f4", "#f4a4d7")
    parts = [f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">', '<rect width="100%" height="100%" fill="#101419"/>',
             '<text x="24" y="30" fill="#f1f5f9" font-family="sans-serif" font-size="20">C39D2A static topology candidate: observed C3925C/C3925E faces</text>',
             '<text x="24" y="53" fill="#aab7c4" font-family="sans-serif" font-size="13">43 transform-traced static triples; 10 renderer-observed faces; no extra deck or hull links inferred</text>']
    for panel_index, (label, project) in enumerate(views):
        left, top = 24 + panel_index * 404, 112
        projected = [project(point) for point in points]
        min_x, max_x = min(point[0] for point in projected), max(point[0] for point in projected)
        min_y, max_y = min(point[1] for point in projected), max(point[1] for point in projected)
        scale = (panel - 2 * margin) / max(1, max(max_x - min_x, max_y - min_y))
        cx, cy = left + panel / 2, top + panel / 2
        mx, my = (min_x + max_x) / 2, (min_y + max_y) / 2
        def position(point: tuple[float, float]) -> tuple[float, float]: return cx + (point[0] - mx) * scale, cy - (point[1] - my) * scale
        parts.extend((f'<rect x="{left}" y="{top}" width="{panel}" height="{panel}" fill="#151c24" stroke="#506070"/>', f'<text x="{left + 10}" y="{top + 24}" fill="#f1f5f9" font-family="sans-serif" font-size="16">{label}</text>'))
        for face_index, face in enumerate(topology["faces"]):
            path = [position(project(points[index])) for index in face["indices"]]; path.append(path[0])
            parts.append('<polyline points="{}" fill="none" stroke="{}" stroke-width="2.5"/>'.format(' '.join(f'{x:.2f},{y:.2f}' for x, y in path), colours[face_index]))
        for point in projected:
            x, y = position(point); parts.append(f'<circle cx="{x:.2f}" cy="{y:.2f}" r="2.5" fill="#e8edf3"/>')
    parts.append('</svg>')
    OUTPUT.write_text("\n".join(parts) + "\n", encoding="utf-8")
    print(f"wrote {OUTPUT.relative_to(ROOT)} ({len(points)} vertices, {len(topology['faces'])} faces)")


if __name__ == "__main__": main()
