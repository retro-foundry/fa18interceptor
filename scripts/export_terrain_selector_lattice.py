"""Export the proven 32x32 terrain-template selector lattice.

The output is a static directory decode of the inputs observed in the
controlled origin-bin sweep.  It is deliberately a selector/page diagnostic,
not a world-coordinate terrain mesh export.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE = 0xC00000


def address(value: int) -> str:
    return f"${value:06X}"


def decode_stream(slow: bytes, group: int, row_key: int) -> int | None:
    group_offset = group - SLOW_BASE
    header = int.from_bytes(slow[group_offset:group_offset + 2], "big", signed=True)
    if header < 0 or header & 1:
        return None
    count = header // 2
    keys = [int.from_bytes(slow[group_offset + 2 + index * 2:group_offset + 4 + index * 2], "big")
            for index in range(count)]
    try:
        index = keys.index(row_key)
    except ValueError:
        return None
    pointer_offset = group_offset + header + 2 + index * 4
    return int.from_bytes(slow[pointer_offset:pointer_offset + 4], "big")


def lattice(sweep: dict, slow: bytes) -> list[dict]:
    groups = sweep["group_axis"]
    rows = sweep["row_axis"]
    result = []
    for group_sample, row_sample in zip(groups, rows):
        group_records = [int(value[1:], 16) for value in group_sample["group_records"]]
        for row_bin, row_values in enumerate(sample["values"] for sample in rows):
            selections = [decode_stream(slow, record, row_key)
                          for record, row_key in zip(group_records, row_values)]
            streams = sorted({value for value in selections if value is not None})
            result.append({"group_bin": group_sample["bin"], "row_bin": row_bin,
                           "stream_selection_count": sum(value is not None for value in selections),
                           "unique_streams": [address(value) for value in streams]})
    return result


def validate_control(sweep: dict, slow: bytes) -> dict:
    groups = sweep["group_axis"]
    rows = sweep["row_axis"]
    group = next(item for item in groups if item["bin"] == 16)["values"]
    row = next(item for item in rows if item["bin"] == 16)["values"]
    records = [int(value[1:], 16) for value in next(item for item in groups if item["bin"] == 16)["group_records"]]
    decoded = [address(value) if value is not None else None
               for value in (decode_stream(slow, a, b) for a, b in zip(records, row))]
    actual = json.loads((ROOT / "analysis/data/static_template_selector_groups_origin_control.json").read_text(encoding="utf-8"))
    observed = [entry["first_template_stream_byte"] for entry in actual["groups"]]
    if decoded != observed:
        raise AssertionError("static directory decode does not reproduce controlled bin-16 stream choices")
    return {"group_bin": 16, "row_bin": 16, "call_count": len(decoded),
            "exact_stream_choice_match": True,
            "observed_inventory": "analysis/data/static_template_selector_groups_origin_control.json"}


def svg(cells: list[dict]) -> str:
    size, margin = 20, 58
    maximum = max(cell["stream_selection_count"] for cell in cells)
    lookup = {(cell["group_bin"], cell["row_bin"]): cell for cell in cells}
    lines = [
        '<svg xmlns="http://www.w3.org/2000/svg" width="800" height="770" viewBox="0 0 800 770">',
        '<rect width="100%" height="100%" fill="#10151b"/>',
        '<style>text{font-family:monospace;fill:#dbe7f3}.dim{fill:#9fb2c4}</style>',
        '<text x="58" y="28" font-size="18">Static terrain-template selector lattice — selected stream count</text>',
        '<text class="dim" x="58" y="47" font-size="12">group-bin →; row-bin ↓. Directory coverage diagnostic, not a geographical terrain map.</text>',
    ]
    for group_bin in range(32):
        for row_bin in range(32):
            cell = lookup[(group_bin, row_bin)]
            count = cell["stream_selection_count"]
            shade = int(24 + 190 * count / maximum) if maximum else 24
            color = f"rgb({shade // 3},{shade},{min(255, shade + 35)})"
            x, y = margin + group_bin * size, margin + row_bin * size
            tooltip = f"group={group_bin}, row={row_bin}: {count} selections, {len(cell['unique_streams'])} unique streams"
            lines.append(f'<rect x="{x}" y="{y}" width="{size - 1}" height="{size - 1}" fill="{color}"><title>{tooltip}</title></rect>')
    for value in range(0, 32, 4):
        lines.append(f'<text class="dim" x="{margin + value * size}" y="{margin - 5}" font-size="10">{value}</text>')
        lines.append(f'<text class="dim" x="20" y="{margin + value * size + 10}" font-size="10">{value}</text>')
    lines += [
        f'<text class="dim" x="58" y="730" font-size="12">Maximum selected streams in a cell: {maximum}; each cell combines 41 traced selector-call positions.</text>',
        '<text class="dim" x="58" y="750" font-size="12">A cell identifies static template streams admitted by this update packet, not mesh ownership, elevation, or LOD.</text>',
        '</svg>', '',
    ]
    return "\n".join(lines)


def png(cells: list[dict]) -> Image.Image:
    size, margin = 20, 58
    maximum = max(cell["stream_selection_count"] for cell in cells)
    lookup = {(cell["group_bin"], cell["row_bin"]): cell for cell in cells}
    image = Image.new("RGB", (800, 770), "#10151b")
    draw = ImageDraw.Draw(image)
    font = ImageFont.load_default()
    draw.text((58, 18), "Static terrain-template selector lattice - selected stream count", fill="#dbe7f3", font=font)
    draw.text((58, 38), "group-bin ->; row-bin down. Directory diagnostic, not a geographical terrain map.", fill="#9fb2c4", font=font)
    for group_bin in range(32):
        for row_bin in range(32):
            count = lookup[(group_bin, row_bin)]["stream_selection_count"]
            shade = int(24 + 190 * count / maximum) if maximum else 24
            color = (shade // 3, shade, min(255, shade + 35))
            x, y = margin + group_bin * size, margin + row_bin * size
            draw.rectangle((x, y, x + size - 2, y + size - 2), fill=color)
    for value in range(0, 32, 4):
        draw.text((margin + value * size, margin - 14), str(value), fill="#9fb2c4", font=font)
        draw.text((20, margin + value * size + 4), str(value), fill="#9fb2c4", font=font)
    draw.text((58, 725), f"Maximum selected streams in a cell: {maximum}; each cell combines 41 traced selector-call positions.", fill="#9fb2c4", font=font)
    draw.text((58, 744), "A cell identifies static template streams, not mesh ownership, elevation, or LOD.", fill="#9fb2c4", font=font)
    return image


def markdown(cells: list[dict], validation: dict) -> str:
    counts = [cell["stream_selection_count"] for cell in cells]
    return "\n".join([
        "# Static terrain-template selector lattice",
        "",
        "Classification: **controlled static directory decode**. This exports the static",
        "template streams selected by every combination of the two already-proven 5-bit",
        "origin inputs for one 41-call update packet. It is a selector/page map, not a",
        "global-coordinate terrain mesh, heightmap, or LOD table.",
        "",
        "The decoder combines the group-axis group-record pointer resolved at `$C1D406` with",
        "the row-axis live-key sequence, rejects negative group headers, and accepts only",
        "exact static row-key matches. Its bin-16",
        "combination exactly reproduces all 41 observed stream/no-stream choices in the",
        "independent runtime inventory.",
        "",
        f"All 1,024 selector-bin cells have been decoded. Their selected-stream counts range from {min(counts)} to {max(counts)}; ",
        "the companion SVG/PNG visualizes these counts and the JSON retains every selected static stream address.",
        "",
        "The lattice's group/row labels are directory-input axes only. It does not establish",
        "their cardinal orientation, physical spacing, full map extent, or whether every",
        "static template stream represents terrain rather than another scene element.",
        "",
    ])


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-json", type=Path,
                        default=ROOT / "analysis/data/terrain_template_selector_lattice.json")
    parser.add_argument("--output-markdown", type=Path,
                        default=ROOT / "analysis/data/terrain_template_selector_lattice.md")
    parser.add_argument("--output-svg", type=Path,
                        default=ROOT / "analysis/plots/terrain_template_selector_lattice.svg")
    parser.add_argument("--output-png", type=Path,
                        default=ROOT / "analysis/plots/terrain_template_selector_lattice.png")
    args = parser.parse_args()
    sweep = json.loads((ROOT / "analysis/data/origin_selector_axis_sweep_00_1f.json").read_text(encoding="utf-8"))
    slow = (ROOT / "build/run033_origin_control_trace/slow.bin").read_bytes()
    validation = validate_control(sweep, slow)
    cells = lattice(sweep, slow)
    payload = {"classification": "controlled_static_template_selector_lattice_not_world_mesh_or_lod",
               "dimensions": {"group_bins": 32, "row_bins": 32, "calls_per_cell": sweep["call_count"]},
               "validation": validation, "cells": cells}
    args.output_json.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    args.output_markdown.write_text(markdown(cells, validation), encoding="utf-8")
    args.output_svg.write_text(svg(cells), encoding="utf-8")
    png(cells).save(args.output_png)
    print(f"wrote {args.output_json.relative_to(ROOT)}, {args.output_markdown.relative_to(ROOT)}, {args.output_svg.relative_to(ROOT)}, and {args.output_png.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
