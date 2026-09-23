"""Render the union of exact M-map packet paths observed across sealed runs.

The input paths are immutable segment-68 pair reads.  Their order is retained
inside each traced batch, while duplicate batch paths are rendered once.  This
is an inspection aid for the planar source layer, not a stitched terrain mesh.
"""
from __future__ import annotations

import argparse
import json
import math
from collections import Counter
from pathlib import Path

from PIL import Image, ImageDraw


ROOT = Path(__file__).resolve().parents[1]
SIZE = 1000
MARGIN = 68
BACKGROUND = "#10151c"
GRID = "#2b3542"
COLOURS = ("#55d6be", "#ffb454", "#c88cff", "#f36f9e")


def address(value: int) -> str:
    return f"${value:06X}"


def round_down(value: int, step: int) -> int:
    return math.floor(value / step) * step


def round_up(value: int, step: int) -> int:
    return math.ceil(value / step) * step


def collect(paths: list[Path]) -> tuple[list[dict], dict]:
    unique: dict[tuple[str, ...], dict] = {}
    all_addresses: set[str] = set()
    source_rows = []
    for path in paths:
        path = path.resolve()
        report = json.loads(path.read_text(encoding="utf-8"))
        local_addresses = {pair["address"] for batch in report["batches"]
                           for pair in batch["consumed_pairs"]}
        source_rows.append({"inventory": str(path.relative_to(ROOT)),
                            "batches": len(report["batches"]),
                            "unique_pairs": len(local_addresses)})
        all_addresses.update(local_addresses)
        for batch in report["batches"]:
            pairs = batch["consumed_pairs"]
            key = tuple(pair["address"] for pair in pairs)
            entry = unique.setdefault(key, {"addresses": list(key), "pairs": pairs,
                                            "observations": []})
            entry["observations"].append({"inventory": str(path.relative_to(ROOT)),
                                          "frame": batch["frame"],
                                          "trace_index": batch["trace_index"]})
    batches = list(unique.values())
    for batch in batches:
        batch["observations"].sort(key=lambda row: (row["inventory"], row["trace_index"]))
    batches.sort(key=lambda row: (row["addresses"][0], row["addresses"], row["observations"][0]["inventory"]))
    return batches, {"inventories": source_rows, "unique_pair_addresses": sorted(all_addresses)}


def render(batches: list[dict], output: Path) -> dict:
    points = [pair["xy"] for batch in batches for pair in batch["pairs"]]
    if not points:
        raise ValueError("no source pairs to render")
    minimum_x = round_down(min(point[0] for point in points), 512)
    maximum_x = round_up(max(point[0] for point in points), 512)
    minimum_y = round_down(min(point[1] for point in points), 512)
    maximum_y = round_up(max(point[1] for point in points), 512)
    span = max(maximum_x - minimum_x, maximum_y - minimum_y, 1)

    def project(pair: list[int]) -> tuple[float, float]:
        return (MARGIN + (pair[0] - minimum_x) * (SIZE - 2 * MARGIN) / span,
                SIZE - MARGIN - (pair[1] - minimum_y) * (SIZE - 2 * MARGIN) / span)

    image = Image.new("RGB", (SIZE, SIZE), BACKGROUND)
    draw = ImageDraw.Draw(image)
    for value in range(minimum_x, maximum_x + 1, 512):
        x, _ = project([value, minimum_y])
        draw.line((x, MARGIN, x, SIZE - MARGIN), fill=GRID)
        draw.text((x + 2, SIZE - MARGIN + 6), str(value), fill="#8fa4b8")
    for value in range(minimum_y, maximum_y + 1, 512):
        _, y = project([minimum_x, value])
        draw.line((MARGIN, y, SIZE - MARGIN, y), fill=GRID)
        draw.text((6, y - 6), str(value), fill="#8fa4b8")
    for index, batch in enumerate(batches):
        colour = COLOURS[index % len(COLOURS)]
        projected = [project(pair["xy"]) for pair in batch["pairs"]]
        if len(projected) > 1:
            draw.line(projected, fill=colour, width=1)
        for x, y in projected:
            draw.ellipse((x - 1, y - 1, x + 1, y + 1), fill=colour)
    draw.rectangle((MARGIN, MARGIN, SIZE - MARGIN, SIZE - MARGIN), outline="#d5e0ea")
    draw.text((MARGIN, 16), "M-map packet source-pair coverage (all sealed map traces)", fill="#f1f5f9")
    draw.text((MARGIN, 34), "Each colour is an observed ordered batch; duplicates removed. No joins, closure, placement, or faces inferred.", fill="#cbd5e1")
    output.parent.mkdir(parents=True, exist_ok=True)
    image.save(output)
    return {"x": [minimum_x, maximum_x], "y": [minimum_y, maximum_y], "axis_step": 512}


def markdown(report: dict) -> str:
    return "\n".join([
        "# Cumulative M-map packet source visual",
        "",
        "Classification: **bounded renderer-input inspection visual**. It overlays",
        "deduplicated ordered pair paths consumed by the static segment-68 M-map",
        "packet loop across the listed sealed captures. It is not a complete terrain",
        "mesh, a world-coordinate export, or an LOD visualization.",
        "",
        f"The image contains {report['unique_batches']} distinct observed paths and",
        f"{report['unique_pair_count']} exact immutable pair addresses. This is",
        f"{report['segment_pair_byte_fraction']:.2%} of the {report['segment_bytes']}",
        "bytes in segment 68 when counted as four-byte pair payloads. Consecutive",
        "points are joined only within the same traced transform batch; no separate",
        "batches are stitched together.",
        "",
        "![Cumulative observed M-map packet paths](../plots/m_map_packet_source_coverage.png)",
        "",
        "| Inventory | traced batches | unique pair addresses |",
        "| --- | ---: | ---: |",
        *[f"| `{row['inventory']}` | {row['batches']} | {row['unique_pairs']} |" for row in report["inventories"]],
        "",
        "Viewer convention: the image plots each signed source pair directly as `(x, y)`.",
        "The renderer computes its depth component later; this display does not assign",
        "game axes, absolute map position, paths, coast ownership, or unobserved detail.",
        "",
        "Authority: the exact pair inventories named above, generated by",
        "`scripts/inventory_map_polygon_static_packets.py`, and the byte-exact",
        "`$C2AF9C/$C2AF9E` pair reader.",
        "",
    ])


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--inventory", type=Path, action="append", required=True)
    parser.add_argument("--output-png", type=Path,
                        default=ROOT / "analysis/plots/m_map_packet_source_coverage.png")
    parser.add_argument("--output-json", type=Path,
                        default=ROOT / "analysis/data/m_map_packet_source_coverage.json")
    parser.add_argument("--output-markdown", type=Path,
                        default=ROOT / "analysis/data/m_map_packet_source_coverage.md")
    parser.add_argument("--replace", action="store_true",
                        help="replace existing generated outputs")
    args = parser.parse_args()
    if not args.replace:
        for path in (args.output_png, args.output_json, args.output_markdown):
            if path.exists():
                raise FileExistsError(path)
    batches, coverage = collect(args.inventory)
    bounds = render(batches, args.output_png)
    pair_bytes = len(coverage["unique_pair_addresses"]) * 4
    report = {"classification": "bounded_segment68_m_map_packet_pair_visual_not_world_mesh_or_lod",
              "inventories": coverage["inventories"], "unique_batches": len(batches),
              "unique_pair_count": len(coverage["unique_pair_addresses"]),
              "unique_pair_addresses": coverage["unique_pair_addresses"],
              "segment_bytes": 0xC444F8 - 0xC42CA8,
              "segment_pair_byte_fraction": pair_bytes / (0xC444F8 - 0xC42CA8),
              "plot_bounds": bounds, "batches": batches}
    args.output_json.parent.mkdir(parents=True, exist_ok=True)
    args.output_json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    args.output_markdown.write_text(markdown(report), encoding="utf-8")
    print(json.dumps({"batches": len(batches), "unique_pairs": len(coverage["unique_pair_addresses"]),
                      "output": str(args.output_png)}))


if __name__ == "__main__":
    main()
