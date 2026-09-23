"""Decode the complete statically bounded wide M-map packet directory."""
from __future__ import annotations

import argparse
import json
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE, BASE, ROWS, COLUMNS, STRIDE = 0xC00000, 0xC42E6C, 32, 32, 64


def address(value: int) -> str:
    return f"${value & 0xFFFFFF:06X}"


def word(data: bytes, value: int, signed: bool = False) -> int:
    return int.from_bytes(data[value - SLOW_BASE:value - SLOW_BASE + 2], "big", signed=signed)


def long(data: bytes, value: int) -> int:
    return int.from_bytes(data[value - SLOW_BASE:value - SLOW_BASE + 4], "big")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--slow", type=Path, required=True)
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/data/wide_m_map_packet_directory.json")
    parser.add_argument("--replace", action="store_true")
    args = parser.parse_args()
    markdown = args.output.with_suffix(".md")
    if not args.replace and (args.output.exists() or markdown.exists()):
        raise FileExistsError(args.output)
    data = args.slow.read_bytes()
    cells = []
    for y in range(ROWS):
        for x in range(COLUMNS):
            table = BASE + y * STRIDE + x * 2
            offset = word(data, table, signed=True)
            row = {"x": x, "y": y, "table_address": address(table), "relative_offset": offset}
            if offset <= 0:
                row["classification"] = "nonpositive_relative_offset_rejected_by_C2ADCE"
            else:
                target = BASE + offset
                first_word, first_long = word(data, target), long(data, target)
                row.update({"target": address(target), "target_first_word": f"${first_word:04X}",
                            "target_first_longword": f"${first_long:08X}",
                            "classification": "negative_target_word_guard" if first_word & 0x8000 else "nonnegative_target_word"})
            cells.append(row)
    counts = Counter(row["classification"] for row in cells)
    targets = Counter(row.get("target", "none") for row in cells)
    report = {"classification": "byte_decoded_complete_wide_m_map_relative_offset_directory",
              "directory": {"base": address(BASE), "rows": ROWS, "columns": COLUMNS,
                            "row_stride_bytes": STRIDE, "bytes": ROWS * STRIDE,
                            "end_exclusive": address(BASE + ROWS * STRIDE)},
              "class_counts": dict(sorted(counts.items())), "target_reuse": dict(targets), "cells": cells,
              "qualification": "The dimensions and base are set explicitly by byte-exact C2AB34. A positive relative offset passes C2ADCE's immediate offset test; later target-word and packet-stage branches depend on live state, so this is a complete static directory decode, not a complete rendered terrain map, global coordinate grid, or LOD table."}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# Complete wide M-map packet directory", "", "Classification: **byte-decoded 32×32 static relative-offset directory**.", "", report["qualification"], "",
             f"`{address(BASE)}-{address(BASE + ROWS * STRIDE - 1)}` contains {ROWS * COLUMNS} cells.", "",
             "| cell class | count |", "| --- | ---: |", *[f"| {name} | {count} |" for name, count in sorted(counts.items())], "",
             f"The JSON companion retains all {ROWS * COLUMNS} coordinates, table words, and resolved targets. It is intentionally not rendered as a world map because selector x/y are local directory indices, not established world axes.", ""]
    markdown.write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"cells": len(cells), "classes": dict(counts), "targets": len(targets)}))


if __name__ == "__main__":
    main()
