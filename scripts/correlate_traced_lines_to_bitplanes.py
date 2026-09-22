"""Report final bitplane indices at C2FA7E call endpoints from a saved trace."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from analyze_cockpit_bitplanes import BYTES_PER_ROW, copper_registers, plane_pointers


LINE_CALL = 0xC2FA7E


def index_at(chip: bytes, pointers: list[int], x: int, y: int) -> int | None:
    if not 0 <= x < 320 or not 0 <= y < 200:
        return None
    offset = y * BYTES_PER_ROW + (x >> 3)
    mask = 0x80 >> (x & 7)
    return sum(((chip[pointer + offset] & mask) != 0) << plane
               for plane, pointer in enumerate(pointers))


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, required=True)
    parser.add_argument("--chip", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--markdown", type=Path, required=True)
    args = parser.parse_args()
    chip = args.chip.read_bytes()
    pointers = plane_pointers(copper_registers(chip))
    rows = [json.loads(line) for line in args.trace.read_text(encoding="utf-8").splitlines()]
    lines = []
    for row in rows:
        if row["pc"] != LINE_CALL:
            continue
        registers = row["registers"]
        endpoints = [[registers["d0"] & 0xFFFF, registers["d1"] & 0xFFFF],
                     [registers["d2"] & 0xFFFF, registers["d3"] & 0xFFFF]]
        lines.append({"trace_index": row["index"], "endpoints": endpoints,
                      "final_bitplane_indices": [index_at(chip, pointers, *point) for point in endpoints]})
    report = {
        "authority": {"trace": str(args.trace), "chip": str(args.chip),
                      "copper_plane_pointers": [f"${pointer:06X}" for pointer in pointers]},
        "qualification": "An endpoint's final bitplane index establishes display-space overlap only. It does not prove this individual line call wrote that pixel or identify the source mesh.",
        "lines": lines,
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    text = ["# Traced line endpoint bitplane correlation", "", report["qualification"], "",
            "| trace index | endpoints | final bitplane indices |", "| ---: | --- | --- |"]
    for row in lines:
        text.append(f"| {row['trace_index']} | {row['endpoints'][0]} -> {row['endpoints'][1]} | {row['final_bitplane_indices']} |")
    args.markdown.parent.mkdir(parents=True, exist_ok=True)
    args.markdown.write_text("\n".join(text) + "\n", encoding="utf-8")
    print(json.dumps({"lines": len(lines)}))


if __name__ == "__main__":
    main()
