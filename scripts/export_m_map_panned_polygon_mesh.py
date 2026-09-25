"""Export the joined M-map source polygons as direct WebGL triangle buffers.

The inputs are the same `$C4B390`-before-`$C2FF48` captures used for the v4
coastline mosaic.  This exporter deliberately emits raw joined map coordinates
instead of sampling its PNG/SVG rasterisation.
"""
from __future__ import annotations

import hashlib
import json
from pathlib import Path

from render_m_map_panned_vector_mosaic import (
    FILL_MASK_COLOURS,
    HEIGHT,
    RUNS,
    VIEWPORT_HEIGHT,
    VIEWPORT_WIDTH,
    WIDTH,
    canonical_pass,
)


ROOT = Path(__file__).resolve().parents[1]
OUT_JS = ROOT / "analysis/visuals/m_map_panned_projected_polygon_mosaic_v4_direct_mesh.js"
OUT_JSON = ROOT / "analysis/data/m_map_panned_projected_polygon_mosaic_v4_direct_mesh.json"


def twice_area(points: list[tuple[float, float]]) -> float:
    return sum(x1 * y2 - y1 * x2
               for (x1, y1), (x2, y2) in zip(points, points[1:] + points[:1]))


def cross(a: tuple[float, float], b: tuple[float, float], c: tuple[float, float]) -> float:
    return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])


def in_or_on_triangle(point: tuple[float, float], a: tuple[float, float],
                      b: tuple[float, float], c: tuple[float, float], ccw: bool) -> bool:
    values = (cross(a, b, point), cross(b, c, point), cross(c, a, point))
    # Boundary points block an ear: clipping through them would create an
    # overlapping triangle. A later zero-area remainder is discarded only
    # after area conservation has been checked.
    return all(value >= 0 for value in values) if ccw else all(value <= 0 for value in values)


def clean_polygon(points: list[tuple[float, float]]) -> list[tuple[float, float]]:
    cleaned: list[tuple[float, float]] = []
    for point in points:
        if not cleaned or point != cleaned[-1]:
            cleaned.append(point)
    if len(cleaned) > 1 and cleaned[0] == cleaned[-1]:
        cleaned.pop()
    # The original span renderer accepts collinear intermediate/backtracking
    # vertices. They carry no signed area and make an otherwise exact ear
    # decomposition ambiguous. Removing any such middle point retains the
    # polygon's signed area; `triangulate` checks that conservation below.
    changed = True
    while changed and len(cleaned) > 3:
        changed = False
        for index, middle in enumerate(cleaned):
            before, after = cleaned[(index - 1) % len(cleaned)], cleaned[(index + 1) % len(cleaned)]
            if cross(before, middle, after) == 0:
                del cleaned[index]
                changed = True
                break
    return cleaned


def triangulate(points: list[tuple[float, float]], label: str) -> list[tuple[tuple[float, float], ...]]:
    """Ear-clip one simple source polygon without changing its vertices."""
    points = clean_polygon(points)
    if len(points) < 3:
        return []
    area = twice_area(points)
    if area == 0:
        return []
    ccw = area > 0
    indices = list(range(len(points)))
    triangles: list[tuple[tuple[float, float], ...]] = []
    while len(indices) > 3:
        found = False
        for offset, middle in enumerate(indices):
            before = indices[(offset - 1) % len(indices)]
            after = indices[(offset + 1) % len(indices)]
            a, b, c = points[before], points[middle], points[after]
            turn = cross(a, b, c)
            if (ccw and turn <= 0) or (not ccw and turn >= 0):
                continue
            if any(in_or_on_triangle(points[index], a, b, c, ccw)
                   for index in indices if index not in (before, middle, after)):
                continue
            triangles.append((a, b, c))
            del indices[offset]
            found = True
            break
        if not found:
            remainder = [points[index] for index in indices]
            if twice_area(remainder) == 0:
                # The remaining source vertices are collinear, so they carry
                # no filled area after the preceding exact ears.
                break
            raise RuntimeError(f"cannot ear-clip direct source polygon {label}")
    if len(indices) == 3:
        triangles.append(tuple(points[index] for index in indices))
    triangulated_area = sum(abs(cross(triangle[0], triangle[1], triangle[2]))
                            for triangle in triangles)
    if triangulated_area != abs(area):
        raise RuntimeError(f"source-area mismatch while triangulating {label}")
    return triangles


def flatten(triangles: list[tuple[tuple[float, float], ...]]) -> list[float]:
    return [coordinate for triangle in triangles for point in triangle for coordinate in point]


def main() -> None:
    if OUT_JS.exists() or OUT_JSON.exists():
        raise FileExistsError("refusing to overwrite polygon-mesh evidence output")
    classes: dict[str, list[tuple[tuple[float, float], ...]]] = {"land": [], "sea": [], "overlay": []}
    draws = []
    source_runs = []
    for name, path, tx, ty, expected, start, count in RUNS:
        captured = json.loads(path.read_text(encoding="utf-8"))["polygons"]
        polygons = canonical_pass(captured) if count is None else captured[start:start + count]
        if len(polygons) != expected:
            raise RuntimeError(f"{name}: expected {expected} polygons, got {len(polygons)}")
        land_triangles = (
            ((tx, ty), (tx + VIEWPORT_WIDTH, ty), (tx + VIEWPORT_WIDTH, ty + VIEWPORT_HEIGHT)),
            ((tx, ty), (tx + VIEWPORT_WIDTH, ty + VIEWPORT_HEIGHT), (tx, ty + VIEWPORT_HEIGHT)),
        )
        classes["land"].extend(land_triangles)
        draws.append({"kind": "land", "primitive": "triangles", "vertices": flatten(list(land_triangles))})
        emitted = {"sea": 0, "overlay": 0, "line_loop": 0}
        for polygon in polygons:
            points = [(x * 2 + tx, y + ty) for x, y in polygon["projected_pairs"]]
            kind = "overlay" if polygon.get("active_fill_plane_mask") == 15 else "sea"
            triangles = triangulate(points, f"{name} submission {polygon['submission']}")
            if triangles:
                classes[kind].extend(triangles)
                draws.append({"kind": kind, "primitive": "triangles", "vertices": flatten(triangles)})
                emitted[kind] += len(triangles)
            else:
                boundary = clean_polygon(points)
                if len(boundary) >= 2:
                    draws.append({"kind": kind, "primitive": "line_loop", "vertices": [coordinate for point in boundary for coordinate in point]})
                    emitted["line_loop"] += 1
        source_runs.append({
            "name": name,
            "capture": str(path.relative_to(ROOT)).replace("\\", "/"),
            "translation": [tx, ty],
            "source_polygons": len(polygons),
            "triangles": emitted,
        })
    payload = {"draws": draws}
    digests = {name: hashlib.sha256(json.dumps(flatten(triangles), separators=(",", ":")).encode()).hexdigest()
               for name, triangles in classes.items()}
    metadata = {
        "classification": "direct_c4b390_polygon_mesh_for_webgl",
        "coordinate_system": {"width": WIDTH, "height": HEIGHT, "viewport": [VIEWPORT_WIDTH, VIEWPORT_HEIGHT]},
        "source_runs": source_runs,
        "vertex_counts": {name: len(triangles) * 3 for name, triangles in classes.items()},
        "triangle_counts": {name: len(triangles) for name, triangles in classes.items()},
        "draw_count": len(draws),
        "sha256": digests,
        "golden_gate_anchor": [466, 156.5],
        "qualification": "Each emitted triangle comes from direct C4B390 projected polygon vertices or an observed M-map viewport coverage rectangle. The vertices are in the joined 1000x370 M-map projection and do not encode a global height field, road/building mesh, or real-world-distance scale.",
    }
    OUT_JSON.write_text(json.dumps(metadata, indent=2) + "\n", encoding="utf-8")
    OUT_JS.write_text("// Generated by scripts/export_m_map_panned_polygon_mesh.py; do not hand-edit.\n"
                      "window.FA18_M_MAP_POLYGON_MESH_V4=" + json.dumps({"metadata": metadata, **payload}, separators=(",", ":")) + ";\n",
                      encoding="utf-8")
    print(json.dumps({"triangles": metadata["triangle_counts"], "js": str(OUT_JS), "json": str(OUT_JSON)}))


if __name__ == "__main__":
    main()
