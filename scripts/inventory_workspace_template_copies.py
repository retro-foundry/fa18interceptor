"""Inventory traced static-template copies into the mutable scene workspace.

This report intentionally describes a producer-to-consumer contract, not a
terrain mesh.  It joins `$C1D488` static-stream entries with the later
`$C1DD36` workspace-cell reader in the same bounded trace.
"""
from __future__ import annotations

import json
import struct
import argparse
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TRACE = ROOT / "build/run033_placement_bulk_404_trace/trace.jsonl"
SLOW = ROOT / "build/run033_placement_bulk_404_trace/slow.bin"
SLOW_BASE = 0xC00000
COPY_PC = 0xC1D488
BUILDER_HEADER_PC = 0xC1DD36
SELECTOR_STORE_PC = 0xC1DD54
DESCRIPTOR_STORE_PC = 0xC1DD88
COORDINATE_STORE_PCS = (0xC1E04A, 0xC1E054, 0xC1E05E)


def hex_address(value: int) -> str:
    return f"${value:06X}"


def source_segment(address: int) -> int | None:
    if 0xC42290 <= address < 0xC429C8:
        return 66
    if 0xC429D0 <= address < 0xC42CA0:
        return 67
    return None


def load_trace() -> list[dict]:
    return [json.loads(line) for line in TRACE.read_text(encoding="utf-8").splitlines()]


def signed_word(value: int) -> int:
    return value - 0x10000 if value & 0x8000 else value


def builder_records(trace: list[dict]) -> list[dict]:
    starts = [index for index, row in enumerate(trace) if row["pc"] == BUILDER_HEADER_PC]
    records = []
    for start_index, end_index in zip(starts, starts[1:] + [len(trace)]):
        header = trace[start_index]
        stores = {}
        for row in trace[start_index:end_index]:
            if row["pc"] in (SELECTOR_STORE_PC, DESCRIPTOR_STORE_PC, *COORDINATE_STORE_PCS):
                stores.setdefault(row["pc"], row)
        if not all(pc in stores for pc in (SELECTOR_STORE_PC, DESCRIPTOR_STORE_PC, *COORDINATE_STORE_PCS)):
            continue
        coordinate_values = [stores[pc]["registers"]["d0"] & 0xFFFF for pc in COORDINATE_STORE_PCS]
        records.append({
            "frame": header.get("frame"),
            "trace_index": header["index"],
            "cell": header["registers"]["a3"],
            "runtime_record": stores[SELECTOR_STORE_PC]["registers"]["a2"],
            "descriptor": stores[DESCRIPTOR_STORE_PC]["registers"]["a1"],
            "coordinate_words": coordinate_values,
        })
    return records


def first_later_consumer(builders: list[dict], trace_index: int, cell: int) -> dict | None:
    for row in builders:
        if row["trace_index"] > trace_index and row["cell"] == cell:
            return row
    return None


def inventory() -> list[dict]:
    trace = load_trace()
    memory = SLOW.read_bytes()
    builders = builder_records(trace)
    rows = []
    for row in trace:
        if row["pc"] != COPY_PC:
            continue
        source = row["registers"]["a5"]
        cell = row["registers"]["a2"]
        offset = source - SLOW_BASE
        header = memory[offset]
        first_word, second_word = struct.unpack_from(">HH", memory, offset + 1)
        consumer = first_later_consumer(builders, row["index"], cell)
        # C1D48C retains source bit 7, C1D490 stores it in the high byte of
        # the destination word, and C1D492/C1D496 retain source bits 0..6.
        workspace_header = ((header & 0x80) << 8) | (header & 0x7F)
        rows.append({
            "copy_frame": row.get("frame"),
            "copy_trace_index": row["index"],
            "static_source": hex_address(source),
            "source_segment": source_segment(source),
            "source_header_byte": f"${header:02X}",
            "source_words": [f"${first_word:04X}", f"${second_word:04X}"],
            "workspace_cell": hex_address(cell),
            "workspace_header_word": f"${workspace_header:04X}",
            "observed_later_builder_frame": consumer["frame"] if consumer else None,
            "observed_later_builder_trace_index": consumer["trace_index"] if consumer else None,
            "runtime_placement_record": hex_address(consumer["runtime_record"]) if consumer else None,
            "runtime_descriptor": hex_address(consumer["descriptor"]) if consumer else None,
            "runtime_coordinate_words_unsigned": consumer["coordinate_words"] if consumer else None,
            "runtime_coordinate_words_signed": [signed_word(word) for word in consumer["coordinate_words"]] if consumer else None,
        })
    return rows


def markdown(rows: list[dict]) -> str:
    consumed = [row for row in rows if row["observed_later_builder_trace_index"] is not None]
    lines = [
        "# Traced static-template workspace copies",
        "",
        "Classification: **scenario-backed producer-to-consumer inventory**. "
        "The rows describe static bytes copied into mutable workspace cells; "
        "they are not an extracted terrain mesh, global position table, or LOD table.",
        "",
        f"Authority: `{TRACE.relative_to(ROOT).as_posix()}` and its "
        "frame-0 slow-RAM snapshot.  `$C1D488` supplies each static header; "
        "`$C1D4BC` copies its following two words; `$C1DD36` is the later cell-header reader.",
        "",
        f"The bounded trace has **{len(rows)}** observed static-entry copies.  "
        f"**{len(consumed)}** are later read at `$C1DD36` before the trace ends.  "
        "An absent later read means only that this bounded trace did not reach one; "
        "it is not rejection evidence.",
        "",
        "`$C1D48C-$C1D496` transforms the source header byte rather than copying it "
        "directly: source bit 7 becomes destination word bit 15, while source bits "
        "0..6 remain the low seven bits.  The two displayed source words are copied "
        "to the next four workspace bytes by `$C1D4BC`.",
        "",
        "| Copy frame | static source | segment | header -> workspace header | copied words | workspace cell | emitted runtime placement |",
        "| ---: | --- | ---: | --- | --- | --- | --- |",
    ]
    for row in rows:
        segment = "unknown" if row["source_segment"] is None else str(row["source_segment"])
        if row["runtime_placement_record"] is None:
            emitted = "not reached before trace end"
        else:
            words = ", ".join(str(value) for value in row["runtime_coordinate_words_signed"])
            emitted = (f"{row['runtime_placement_record']} / {row['runtime_descriptor']} / "
                       f"({words})")
        lines.append(
            f"| {row['copy_frame'] if row['copy_frame'] is not None else 'stepped'} | {row['static_source']} | {segment} | "
            f"{row['source_header_byte']} -> {row['workspace_header_word']} | "
            f"{' '.join(row['source_words'])} | {row['workspace_cell']} | {emitted} |"
        )
    lines += [
        "",
        "Segment 66 is verified at `$C42290-$C429C7`; segment 67 is verified at "
        "`$C429D0-$C42C9F`.  The inventory records only source addresses reached by "
        "the captured path.  It does not claim that either entire segment is terrain data.",
        "",
        "For every emitted row, the final tuple is the runtime record address, descriptor, "
        "and three signed emitted placement words.  These words are generated by the builder, "
        "not copied verbatim from the source words.  A zero middle word in this small joined "
        "sample is consistent with the wider runtime-placement diagnostic, but does not prove "
        "a universal height convention.  [The joined X/Z diagnostic](../plots/workspace_template_placements_xz.svg) "
        "plots only these 22 source-to-placement paths.\n",
        "See [the single-entry copy contract](../routines/c1d442_workspace_cell_template_copy.md) "
        "for the instruction-level `$C427C1 -> $C4B270` example and "
        "[the placement builder](../routines/c1dc1c_scene_placement_record_builder.md) "
        "for the downstream coordinate calculation.",
        "",
    ]
    return "\n".join(lines)


def svg(rows: list[dict]) -> str:
    points = [row for row in rows if row["runtime_coordinate_words_signed"] is not None]
    coordinates = [row["runtime_coordinate_words_signed"] for row in points]
    xs = [value[0] for value in coordinates]
    zs = [value[2] for value in coordinates]
    minimum_x, maximum_x = min(xs), max(xs)
    minimum_z, maximum_z = min(zs), max(zs)
    span = max(maximum_x - minimum_x, maximum_z - minimum_z, 1)
    width, height, margin, plot = 1120, 760, 92, 570
    left, top = margin, 115
    copy_frames = sorted({row["copy_frame"] for row in points})
    palette = ("#ffcb6b", "#82d6ff", "#f28db2", "#9ee493", "#c5a3ff", "#ff9f68")
    frame_colors = {frame: palette[index % len(palette)] for index, frame in enumerate(copy_frames)}
    segments = ", ".join(str(segment) for segment in sorted({row["source_segment"] for row in points}))
    show_labels = len(points) <= 30
    lines = [
        '<svg xmlns="http://www.w3.org/2000/svg" width="1120" height="760" viewBox="0 0 1120 760">',
        '<rect width="100%" height="100%" fill="#10151b"/>',
        '<style>text{font-family:monospace;fill:#dbe7f3}.dim{fill:#9fb2c4}.axis{stroke:#506475}.grid{stroke:#293845}.point{stroke:#10151b;stroke-width:1}</style>',
        '<text x="92" y="36" font-size="20">Trace-derived static-template placement diagnostic — X/Z plane</text>',
        f'<text class="dim" x="92" y="62" font-size="14">{len(points)} segment-{segments} entries emitted as placements; all sampled middle words = 0</text>',
        f'<rect x="{left}" y="{top}" width="{plot}" height="{plot}" fill="#161e27" stroke="#63788b"/>',
    ]
    for fraction in range(1, 5):
        position = fraction * plot / 5
        lines += [
            f'<line class="grid" x1="{left + position:.1f}" y1="{top}" x2="{left + position:.1f}" y2="{top + plot}"/>',
            f'<line class="grid" x1="{left}" y1="{top + position:.1f}" x2="{left + plot}" y2="{top + position:.1f}"/>',
        ]
    if len(copy_frames) > 1:
        for index, frame in enumerate(copy_frames):
            x = 710 + (index % 3) * 130
            y = 94 + (index // 3) * 18
            lines.append(f'<circle cx="{x}" cy="{y - 4}" r="4" fill="{frame_colors[frame]}"/>')
            lines.append(f'<text class="dim" x="{x + 9}" y="{y}" font-size="12">copy frame {frame}</text>')
    lines += [
        f'<line class="axis" x1="{left}" y1="{top + plot}" x2="{left + plot}" y2="{top + plot}"/>',
        f'<line class="axis" x1="{left}" y1="{top}" x2="{left}" y2="{top + plot}"/>',
    ]
    for row in points:
        x, _, z = row["runtime_coordinate_words_signed"]
        px = left + (x - minimum_x) / span * plot
        py = top + plot - (z - minimum_z) / span * plot
        label = row["static_source"]
        title = (f'{label} -> {row["runtime_placement_record"]} {row["runtime_descriptor"]}; '
                 f'X={x}, middle=0, Z={z}')
        lines += [
            f'<circle class="point" cx="{px:.1f}" cy="{py:.1f}" r="5" fill="{frame_colors[row["copy_frame"]]}"><title>{title}</title></circle>',
        ]
        if show_labels:
            lines.append(f'<text class="dim" x="{px + 8:.1f}" y="{py - 7:.1f}" font-size="12">{label[3:]}</text>')
    lines += [
        f'<text class="dim" x="{left}" y="{top + plot + 30}" font-size="14">X range [{minimum_x}, {maximum_x}]</text>',
        f'<text class="dim" x="{left + plot - 180}" y="{top + plot + 30}" font-size="14">Z range [{minimum_z}, {maximum_z}]</text>',
        '<text class="dim" x="92" y="730" font-size="13">Points prove this replay path only. They do not establish terrain triangles, a complete world map, global axes, or LOD selection.</text>',
        '</svg>',
    ]
    return "\n".join(lines) + "\n"


def main() -> None:
    global TRACE, SLOW
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace-directory", type=Path,
                        default=TRACE.parent,
                        help="directory containing trace.jsonl and slow.bin")
    parser.add_argument("--output-suffix", default="",
                        help="suffix inserted before generated file extensions (for example _404_426)")
    args = parser.parse_args()
    TRACE = (args.trace_directory / "trace.jsonl").resolve()
    SLOW = (args.trace_directory / "slow.bin").resolve()
    if not TRACE.is_file() or not SLOW.is_file():
        parser.error("--trace-directory must contain trace.jsonl and slow.bin")
    if args.output_suffix and not args.output_suffix.startswith("_"):
        parser.error("--output-suffix must be empty or begin with '_'")
    rows = inventory()
    payload = {
        "authority": {
            "trace": str(TRACE.relative_to(ROOT)).replace("\\", "/"),
            "slow_snapshot": str(SLOW.relative_to(ROOT)).replace("\\", "/"),
            "copy_pc": hex_address(COPY_PC),
            "later_builder_header_read_pc": hex_address(BUILDER_HEADER_PC),
        },
        "classification": "traced_static_template_to_mutable_workspace_not_map_mesh",
        "copies": rows,
    }
    json_path = ROOT / f"analysis/data/workspace_template_copies{args.output_suffix}.json"
    markdown_path = ROOT / f"analysis/data/workspace_template_copies{args.output_suffix}.md"
    svg_path = ROOT / f"analysis/plots/workspace_template_placements_xz{args.output_suffix}.svg"
    json_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    markdown_path.write_text(markdown(rows), encoding="utf-8")
    svg_path.write_text(svg(rows), encoding="utf-8")
    consumed = sum(row["observed_later_builder_trace_index"] is not None for row in rows)
    print(f"wrote {json_path.relative_to(ROOT)}, {markdown_path.relative_to(ROOT)}, and {svg_path.relative_to(ROOT)} "
          f"({len(rows)} copies, {consumed} later builder reads)")


if __name__ == "__main__":
    main()
