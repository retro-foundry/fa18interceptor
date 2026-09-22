"""Export immutable template records referenced by the selector lattice."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE = 0xC00000


def address(value: int) -> str:
    return f"${value:06X}"


def parse_stream(slow: bytes, start: int) -> list[dict]:
    # `$C1D442` consumes the stream's leading selector/control byte before
    # `$C1D488` reaches its first six-byte template record.  The observed
    # `$C42646 -> $C42647` and `$C42BD4 -> $C42BD5` pairs establish this
    # one-byte preamble for the active copy path.
    cursor = start + 1
    records = []
    while True:
        offset = cursor - SLOW_BASE
        header = slow[offset]
        if header == 0xFF:
            return records
        if offset + 6 > len(slow):
            raise ValueError(f"unterminated stream at {address(start)}")
        records.append({"source": address(cursor), "header": f"${header:02X}",
                        "word_1": f"${int.from_bytes(slow[offset + 1:offset + 3], 'big'):04X}",
                        "word_2": f"${int.from_bytes(slow[offset + 3:offset + 5], 'big'):04X}"})
        cursor += 6


def markdown(streams: list[dict], cells: list[dict]) -> str:
    total = sum(len(stream["records"]) for stream in streams)
    lines = [
        "# Immutable templates referenced by the selector lattice",
        "",
        "Classification: **static template-payload export for a bounded selector lattice**.",
        "These are exact header/two-word records consumed after `$C1D442` advances past",
        "each stream's leading control byte, and before",
        "their mutable workspace expansion. They are not global placement coordinates,",
        "terrain vertices, elevation values, or a complete world mesh.",
        "",
        f"The 32×32 lattice reaches {len(streams)} distinct static streams and {total} non-terminator",
        "static records. The JSON carries every record plus the selector-bin cells that",
        "reference its stream; a zero-record stream is an observed immediate `$FF` terminator.",
        "",
        "| Static stream | records | selector-bin cells |",
        "| --- | ---: | ---: |",
    ]
    for stream in streams:
        lines.append(f"| {stream['stream']} | {len(stream['records'])} | {len(stream['selector_cells'])} |")
    lines += [
        "",
        "The exact record format and one-byte stream preamble are scenario-backed by the",
        "direct copy trace. A stream's",
        "presence in a selector bin is page-content evidence only: downstream code combines",
        "these reusable records with mutable placement context, so its words must not be",
        "drawn as absolute map points.",
        "",
    ]
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-json", type=Path,
                        default=ROOT / "analysis/data/terrain_lattice_stream_templates.json")
    parser.add_argument("--output-markdown", type=Path,
                        default=ROOT / "analysis/data/terrain_lattice_stream_templates.md")
    args = parser.parse_args()
    lattice = json.loads((ROOT / "analysis/data/terrain_template_selector_lattice.json").read_text(encoding="utf-8"))
    slow = (ROOT / "build/run033_origin_control_trace/slow.bin").read_bytes()
    refs: dict[str, list[list[int]]] = {}
    for cell in lattice["cells"]:
        for stream in cell["unique_streams"]:
            refs.setdefault(stream, []).append([cell["group_bin"], cell["row_bin"]])
    streams = [{"stream": stream, "records": parse_stream(slow, int(stream[1:], 16)),
                "selector_cells": refs[stream]}
               for stream in sorted(refs)]
    payload = {"classification": "bounded_selector_lattice_static_templates_not_world_mesh",
               "lattice": "analysis/data/terrain_template_selector_lattice.json",
               "streams": streams}
    args.output_json.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    args.output_markdown.write_text(markdown(streams, lattice["cells"]), encoding="utf-8")
    print(f"wrote {args.output_json.relative_to(ROOT)} and {args.output_markdown.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
