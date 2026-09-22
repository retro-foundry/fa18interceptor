"""Generate evidence-labelled orthographic SVG plots from traced runtime geometry.

The plotted points come from mutable C48390 projection workspace snapshots.
They are useful for human identification, but are not asserted to be immutable
source-model coordinates.
"""
from __future__ import annotations

import html
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "analysis" / "plots"
MEMORY_BASE = 0xC00000
VERTEX_BASE = 0xC48390


def word(memory: bytes, address: int) -> int:
    value = int.from_bytes(memory[address - MEMORY_BASE:address - MEMORY_BASE + 2], "big", signed=False)
    return value - 0x10000 if value & 0x8000 else value


def vertex(memory: bytes, offset: int) -> tuple[int, int, int]:
    return tuple(word(memory, VERTEX_BASE + offset + axis * 2) for axis in range(3))


def edge_list(memory: bytes, address: int) -> list[tuple[tuple[int, int, int], tuple[int, int, int]]]:
    cursor = address + 2  # selector word
    edges = []
    while True:
        first, second = word(memory, cursor), word(memory, cursor + 2)
        cursor += 4
        terminal = second < 0
        edges.append((vertex(memory, first), vertex(memory, second & 0x7FFF)))
        if terminal:
            return edges


def consecutive_pairs(memory: bytes, address: int, count: int) -> list[tuple[tuple[int, int, int], tuple[int, int, int]]]:
    return [(vertex(memory, address - VERTEX_BASE + index * 12),
             vertex(memory, address - VERTEX_BASE + index * 12 + 6))
            for index in range(count)]


def closed_polygon(memory: bytes, address: int, count: int) -> list[tuple[tuple[int, int, int], tuple[int, int, int]]]:
    points = [vertex(memory, address - VERTEX_BASE + index * 6) for index in range(count)]
    return list(zip(points, points[1:] + points[:1]))


def submission_polygons(path: Path, context: str) -> list[tuple[tuple[int, int, int], tuple[int, int, int]]]:
    report = json.loads(path.read_text(encoding="utf-8"))
    edges = []
    for submission in report["submissions"]:
        if submission["context"]["a5"] != context:
            continue
        points = [tuple(point) for point in submission["triples"]]
        edges.extend(zip(points, points[1:] + points[:1]))
    if not edges:
        raise ValueError(f"no polygon submissions with A5={context} in {path}")
    return edges


def submission_polygon_contexts(path: Path, contexts: set[str]) -> list[tuple[tuple[int, int, int], tuple[int, int, int]]]:
    """Return one trace-proven controller family's finalized polygon edges."""
    report = json.loads(path.read_text(encoding="utf-8"))
    edges = []
    for submission in report["submissions"]:
        if submission["context"]["a5"] not in contexts:
            continue
        points = [tuple(point) for point in submission["triples"]]
        edges.extend(zip(points, points[1:] + points[:1]))
    if not edges:
        raise ValueError(f"no polygon submissions with contexts {sorted(contexts)} in {path}")
    return edges


def plot_svg(title: str, source: str, edges: list[tuple[tuple[int, int, int], tuple[int, int, int]]]) -> str:
    # Each panel uses the same coordinate extent, preserving proportions among
    # XY, XZ, and YZ.  Inversion of SVG Y is presentation-only.
    points = [point for edge in edges for point in edge]
    extent = max(1, max(abs(component) for point in points for component in point))
    width, height, panel, margin = 1200, 460, 360, 45
    pairs = (("X-Y", 0, 1), ("X-Z", 0, 2), ("Y-Z", 1, 2))
    scale = (panel - 2 * margin) / (2 * extent)
    items = [f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">',
             '<rect width="100%" height="100%" fill="#101419"/>',
             f'<text x="24" y="30" fill="#f1f5f9" font-family="sans-serif" font-size="20">{html.escape(title)}</text>',
             f'<text x="24" y="54" fill="#aab7c4" font-family="sans-serif" font-size="13">{html.escape(source)} · {len(edges)} traced segment(s) · workspace coordinates, not asserted source mesh</text>']
    for panel_index, (label, horizontal, vertical) in enumerate(pairs):
        origin_x = 30 + panel_index * 390
        origin_y = 100
        centre_x, centre_y = origin_x + panel / 2, origin_y + panel / 2
        items += [f'<rect x="{origin_x}" y="{origin_y}" width="{panel}" height="{panel}" fill="#151c24" stroke="#506070"/>',
                  f'<line x1="{origin_x + margin}" y1="{centre_y}" x2="{origin_x + panel - margin}" y2="{centre_y}" stroke="#536579"/>',
                  f'<line x1="{centre_x}" y1="{origin_y + margin}" x2="{centre_x}" y2="{origin_y + panel - margin}" stroke="#536579"/>',
                  f'<text x="{origin_x + 8}" y="{origin_y + 22}" fill="#f1f5f9" font-family="sans-serif" font-size="16">{label}</text>',
                  f'<text x="{origin_x + 8}" y="{origin_y + panel - 10}" fill="#aab7c4" font-family="sans-serif" font-size="11">range ±{extent}</text>']
        for index, (left, right) in enumerate(edges):
            x1, y1 = centre_x + left[horizontal] * scale, centre_y - left[vertical] * scale
            x2, y2 = centre_x + right[horizontal] * scale, centre_y - right[vertical] * scale
            colour = "#65d5ff" if index % 2 == 0 else "#ffcc66"
            items.append(f'<line x1="{x1:.2f}" y1="{y1:.2f}" x2="{x2:.2f}" y2="{y2:.2f}" stroke="{colour}" stroke-width="2"/>')
            for x, y in ((x1, y1), (x2, y2)):
                items.append(f'<circle cx="{x:.2f}" cy="{y:.2f}" r="2.5" fill="#ffffff"/>')
            midpoint_x, midpoint_y = (x1 + x2) / 2, (y1 + y2) / 2
            items.append(f'<text x="{midpoint_x + 4:.2f}" y="{midpoint_y - 4:.2f}" fill="{colour}" font-family="monospace" font-size="11">{index}</text>')
    return "\n".join(items + ["</svg>", ""])


def main() -> None:
    OUT.mkdir(exist_ok=True)
    datasets = (
        ("attract_packet_c3985a", "Attract packet $C3985A", "attract frame 600, $C3985A → $C212B0", ROOT / "build" / "attract_focus_600" / "slow.bin", lambda memory: edge_list(memory, 0xC3985A)),
        ("attract_packet_c39886", "Attract packet $C39886", "attract frame 600, $C39886 → $C212B0", ROOT / "build" / "attract_focus_600" / "slow.bin", lambda memory: edge_list(memory, 0xC39886)),
        ("attract_packet_c3989a", "Attract packet $C3989A", "attract HUD trace frame 450, $C3989A → $C212B0", ROOT / "build" / "attract_hud_trace_450" / "slow.bin", lambda memory: edge_list(memory, 0xC3989A)),
        ("attract_packet_c393c4", "Attract packet $C393C4", "attract HUD trace frame 450, $C393C4 → $C212B0", ROOT / "build" / "attract_hud_trace_450" / "slow.bin", lambda memory: edge_list(memory, 0xC393C4)),
        ("run001_packet_c3751a", "Run001 packet $C3751A", "run001 record-walker trace, $C3751A → $C212B0", ROOT / "build" / "run001_c1f6f8_record_walk_stage" / "slow.bin", lambda memory: edge_list(memory, 0xC3751A)),
        ("external_view_packet", "External-view edge packet $C38B0A", "run031 frame 7500, $C38B0A → $C212B0", ROOT / "build" / "run031_frame7500_c1f6f8_probe" / "slow.bin", lambda memory: edge_list(memory, 0xC38B0A)),
        ("golden_gate_packet", "Golden Gate consecutive packet $C483BA", "run031 frame 12000, $C35734 → $C211DC", ROOT / "build" / "run031_frame12000_golden_gate_c1f6f8_probe" / "slow.bin", lambda memory: consecutive_pairs(memory, 0xC483BA, 13)),
        ("golden_gate_polygon_c4b990", "Golden Gate polygon tuple $C4B990", "run031 frame 12000, $C24CFE → $C2FF48", ROOT / "build" / "run031_frame12000_c2ff48_noinput_probe" / "slow.bin", lambda memory: closed_polygon(memory, 0xC4B990, 4)),
        ("golden_gate_c355d8_polygons", "Golden Gate stream polygons $C355D8", "run031 frame 12000, two C2FF48 submissions before the stream changes", ROOT / "build" / "run031_frame12000_polygon_submissions_12f" / "polygon_submissions.json", lambda _: submission_polygons(ROOT / "build" / "run031_frame12000_polygon_submissions_12f" / "polygon_submissions.json", "$C355D8")),
        ("golden_gate_external_c35584_polygons", "Golden Gate external stream polygons $C35584", "run031 frame 12600, two C2FF48 submissions in the C355xx scene family", ROOT / "build" / "run031_frame12600_external_polygon_submissions_v2" / "polygon_submissions.json", lambda _: submission_polygons(ROOT / "build" / "run031_frame12600_external_polygon_submissions_v2" / "polygon_submissions.json", "$C35584")),
        ("external_view_c3925c_polygons", "External-view candidate polygons $C3925C", "run031 frame 12600, three C2FF48 submissions", ROOT / "build" / "run031_frame12600_external_polygon_submissions_v2" / "polygon_submissions.json", lambda _: submission_polygons(ROOT / "build" / "run031_frame12600_external_polygon_submissions_v2" / "polygon_submissions.json", "$C3925C")),
        ("external_view_c3925e_polygons", "External-view candidate polygons $C3925E", "run031 frame 12600, four C2FF48 submissions", ROOT / "build" / "run031_frame12600_external_polygon_submissions_v2" / "polygon_submissions.json", lambda _: submission_polygons(ROOT / "build" / "run031_frame12600_external_polygon_submissions_v2" / "polygon_submissions.json", "$C3925E")),
        ("external_aircraft_c3925c_frame7500", "External-aircraft candidate polygons $C3925C", "run031 frame 7500, three C2FF48 submissions", ROOT / "build" / "run031_frame7500_external_polygon_submissions" / "polygon_submissions.json", lambda _: submission_polygons(ROOT / "build" / "run031_frame7500_external_polygon_submissions" / "polygon_submissions.json", "$C3925C")),
        ("external_aircraft_c3925e_frame7500", "External-aircraft candidate polygons $C3925E", "run031 frame 7500, three C2FF48 submissions", ROOT / "build" / "run031_frame7500_external_polygon_submissions" / "polygon_submissions.json", lambda _: submission_polygons(ROOT / "build" / "run031_frame7500_external_polygon_submissions" / "polygon_submissions.json", "$C3925E")),
        ("external_aircraft_c3925a_controller_family", "External-aircraft controller family $C3925A", "run031 frame 7500, six C2FF48 polygons reached during the traced $C3925A controller path; external-view entity candidate", ROOT / "build" / "run031_frame7500_external_polygon_submissions" / "polygon_submissions.json", lambda _: submission_polygon_contexts(ROOT / "build" / "run031_frame7500_external_polygon_submissions" / "polygon_submissions.json", {"$C3925C", "$C3925E"})),
        ("external_aircraft_c3515e_c34a9a_polygons", "External-aircraft model candidate $C3515E → $C34A9A", "run031 frame 7500: static local vertices C3515E-C351DC transform, then the traced C34A9A face stream emits these polygons", ROOT / "build" / "run031_frame7500_external_polygon_submissions" / "polygon_submissions.json", lambda _: submission_polygons(ROOT / "build" / "run031_frame7500_external_polygon_submissions" / "polygon_submissions.json", "$C34A9A")),
        ("later_bridge_c36298_polygons", "Later bridge stream polygons $C36298", "run031 frame 14500, three C2FF48 submissions in static scene Hunk 43", ROOT / "build" / "run031_frame14500_bridge_polygon_submissions" / "polygon_submissions.json", lambda _: submission_polygons(ROOT / "build" / "run031_frame14500_bridge_polygon_submissions" / "polygon_submissions.json", "$C36298")),
        ("bridge_silhouette_packet", "Bridge-silhouette edge packet $C37EA0", "run031 frame 14500, $C37EA0 → $C212B0", ROOT / "build" / "run031_frame14500_c1f6f8_probe" / "slow.bin", lambda memory: edge_list(memory, 0xC37EA0)),
    )
    inventory = []
    for filename, title, source, snapshot, extract in datasets:
        edges = extract(snapshot.read_bytes()) if snapshot.suffix == ".bin" else extract(b"")
        (OUT / f"{filename}_orthographic.svg").write_text(plot_svg(title, source, edges), encoding="utf-8")
        inventory.append({"packet": filename, "title": title, "source": source,
                          "snapshot": str(snapshot.relative_to(ROOT)),
                          "plot": str((OUT / f"{filename}_orthographic.svg").relative_to(ROOT)),
                          "edge_count": len(edges),
                          "edges": [{"index": index, "first": list(left), "second": list(right)}
                                    for index, (left, right) in enumerate(edges)]})
        print(f"wrote analysis/plots/{filename}_orthographic.svg ({len(edges)} edges)")
    (ROOT / "analysis" / "runtime_geometry_orthographic_packets.json").write_text(
        json.dumps({"scope": "traced mutable C48390 workspace geometry", "packets": inventory}, indent=2) + "\n",
        encoding="utf-8")
    print("wrote analysis/runtime_geometry_orthographic_packets.json")


if __name__ == "__main__":
    main()
