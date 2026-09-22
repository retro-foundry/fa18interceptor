"""Inventory coordinate-bearing runtime scene placements without promoting them to map meshes.

The $C1CB74 selector loop uses $C4E9AA as its actual 24-byte-record base. A
record begins with a selector word, then holds a descriptor pointer followed
by four word fields and ten bytes of per-frame state. This utility preserves
that distinction and emits a
top-down diagnostic of the word-1/word-3 coordinate plane.
"""
from __future__ import annotations

import json
import struct
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
CAPTURES = (
    ("run033_frame05250", ROOT / "build/run033_frame05250_checkpoint/slow.bin"),
    ("run033_frame05500", ROOT / "build/run033_frame05500_checkpoint/slow.bin"),
    ("run033_frame06250", ROOT / "build/run033_frame06250_checkpoint/slow.bin"),
)
SLOW_BASE = 0xC00000
ANCHOR = 0xC4E9AA
RECORD_START = ANCHOR
RECORD_SIZE = 24
MAX_RECORDS = 300


def signed(word: int) -> int:
    return word - 0x10000 if word & 0x8000 else word


def records(path: Path) -> list[dict]:
    raw = path.read_bytes()
    rows: list[dict] = []
    for index in range(MAX_RECORDS):
        address = RECORD_START + index * RECORD_SIZE
        offset = address - SLOW_BASE
        selector = struct.unpack_from(">H", raw, offset)[0]
        descriptor = struct.unpack_from(">I", raw, offset + 2)[0]
        words = struct.unpack_from(">8H", raw, offset + 6)
        # The bounded descriptor range is deliberately conservative: it is the
        # relocated scene table family visible in this capture, not a universal
        # definition of scene data.
        if not 0xC22000 <= descriptor < 0xC23000:
            continue
        rows.append({
            "index": index,
            "address": f"${address:06X}",
            "selector": f"${selector:04X}",
            "descriptor": f"${descriptor:06X}",
            "coordinate_words_unsigned": list(words[:3]),
            "coordinate_words_signed": [signed(word) for word in words[:3]],
            "field_3": f"${words[3]:04X}",
            "per_frame_words": [f"${word:04X}" for word in words[4:]],
        })
    return rows


def contiguous_blocks(rows: list[dict]) -> list[dict]:
    blocks: list[list[dict]] = []
    for row in rows:
        if not blocks or row["index"] != blocks[-1][-1]["index"] + 1:
            blocks.append([row])
        else:
            blocks[-1].append(row)
    return [{"first_index": block[0]["index"], "last_index": block[-1]["index"],
             "start": block[0]["address"], "end": f"${int(block[-1]['address'][1:], 16) + RECORD_SIZE - 1:06X}",
             "records": len(block)} for block in blocks]


def svg(scenes: list[dict]) -> str:
    width, height, margin = 1500, 540, 42
    all_points = [row["coordinate_words_signed"] for scene in scenes for row in scene["records"]]
    xs, zs = [point[0] for point in all_points], [point[2] for point in all_points]
    min_x, max_x, min_z, max_z = min(xs), max(xs), min(zs), max(zs)
    span = max(max_x - min_x, max_z - min_z, 1)
    panels = len(scenes)
    panel_w = (width - margin * 2) / panels
    out = [f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">',
           '<rect width="100%" height="100%" fill="#10151b"/>',
           '<style>text{font-family:monospace;fill:#dbe7f3}.dim{fill:#9fb2c4}.axis{stroke:#506475}.point{fill:#ffcb6b}</style>',
           '<text x="42" y="28" font-size="18">Runtime scene-placement diagnostic — X/Z plane; Y word is zero for every plotted record</text>']
    for panel, scene in enumerate(scenes):
        left, top, size = margin + panel * panel_w, 68, min(panel_w - 28, 410)
        out += [f'<rect x="{left}" y="{top}" width="{size}" height="{size}" fill="#161e27" stroke="#63788b"/>',
                f'<text x="{left + 10}" y="{top + 24}" font-size="15">{scene["capture"]}: {len(scene["records"])} placements</text>',
                f'<line class="axis" x1="{left + 12}" y1="{top + size - 12}" x2="{left + size - 12}" y2="{top + size - 12}"/>',
                f'<line class="axis" x1="{left + 12}" y1="{top + 12}" x2="{left + 12}" y2="{top + size - 12}"/>']
        for row in scene["records"]:
            x, _, z = row["coordinate_words_signed"]
            px = left + 12 + (x - min_x) / span * (size - 24)
            py = top + size - 12 - (z - min_z) / span * (size - 24)
            out.append(f'<circle class="point" cx="{px:.1f}" cy="{py:.1f}" r="2.4"><title>{row["address"]} {row["descriptor"]} X={x} Z={z}</title></circle>')
        out.append(f'<text class="dim" x="{left + 10}" y="{top + size + 28}" font-size="13">X [{min_x}, {max_x}], Z [{min_z}, {max_z}]</text>')
    out.append('</svg>')
    return "\n".join(out) + "\n"


def main() -> None:
    scenes = []
    for capture, path in CAPTURES:
        rows = records(path)
        scenes.append({"capture": capture, "slow_snapshot": str(path.relative_to(ROOT)).replace("\\", "/"),
                       "records": rows, "contiguous_blocks": contiguous_blocks(rows),
                       "all_middle_coordinate_words_zero": all(row["coordinate_words_unsigned"][1] == 0 for row in rows)})
    payload = {"authority": {"selector_loop": "analysis/routines/c1cb14_flight_update_stage.md",
                              "runtime_record_base": "$C4E9AA"},
               "classification": "mutable_runtime_scene_placement_candidates_not_static_map_meshes",
               "selection": "first 300 aligned records whose leading longword is in the relocated $C22000-$C22FFF descriptor range; coordinate values are not selection criteria",
               "record_size_bytes": RECORD_SIZE, "scenes": scenes}
    target = ROOT / "analysis/data/runtime_scene_placements.json"
    target.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    (ROOT / "analysis/plots/runtime_scene_placements_xz.svg").write_text(
        svg(scenes), encoding="utf-8"
    )
    lines = ["# Runtime coordinate-bearing scene placements", "",
             "Classification: **scenario/dataflow evidence**. These are mutable runtime placement records, not an extracted static map mesh or a proven LOD table.  Records are selected solely by their leading relocated descriptor pointer; coordinate values are not a selection criterion.", "",
             "`$C1CB74-$C1CCB6` selects records from `$C4E9AA`. In all three run033 checkpoints, the first populated record starts at that address and has this 24-byte shape:", "",
             "| Bytes | Observed role |", "| --- | --- |", "| `+0..1` | selector word, copied to `$C4585B` and split for downstream control |", "| `+2..5` | relocated descriptor pointer in `$C22000-$C22FFF` |", "| `+6..11` | three signed coordinate-bearing words; the middle word is zero in every exported record |", "| `+12..13` | additional record field (role unknown) |", "| `+14..23` | mutable per-frame words |", "",
             "The selector-loop report proves that the descriptor then selects renderer/control data. The coordinate words therefore locate scene instances before the renderer; they must not be merged with the immutable per-model vertex streams.", "",
             "## Captured placement blocks", "", "| Checkpoint | qualifying records | contiguous index blocks | all middle words zero |", "| --- | ---: | --- | --- |"]
    for scene in scenes:
        blocks = ", ".join(f"{row['first_index']}-{row['last_index']} ({row['records']})" for row in scene["contiguous_blocks"])
        lines.append(f"| {scene['capture']} | {len(scene['records'])} | {blocks} | {scene['all_middle_coordinate_words_zero']} |")
    lines += ["", "[Top-down X/Z diagnostic](../plots/runtime_scene_placements_xz.svg) uses the same coordinate scale in all three panels. It is a placement scatter plot only: points do not imply terrain triangles, roads, or missing links.", "",
              "## Limits", "", "The populated records and their coordinates change between checkpoints, so the captures establish a runtime scene-placement layer but not the original static source table, a terrain mesh, or a distance-selected LOD rule. Proving any of those needs a trace of the writer/refill path into `$C4E9AA` and a source-to-renderer association for individual descriptors.", ""]
    (ROOT / "analysis/data/runtime_scene_placements.md").write_text(
        "\n".join(lines), encoding="utf-8"
    )
    print(f"wrote {target.relative_to(ROOT)} ({sum(len(scene['records']) for scene in scenes)} placements)")


if __name__ == "__main__":
    main()
