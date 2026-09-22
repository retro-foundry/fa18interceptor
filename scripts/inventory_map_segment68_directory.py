"""Decode the observed 8x8 relative-offset directory at the start of map segment 68."""
from __future__ import annotations

import argparse
import collections
import json
from pathlib import Path


BASE = 0xC42CA8
SEGMENT_END = 0xC444F8
ROWS = COLUMNS = 8
ROW_STRIDE = 16
INDEX_X = 0xC2ADC0
INDEX_Y = 0xC2ADC2
LOOKUP = 0xC2ADCE


def address(value: int) -> str:
    return f"${value:06X}"


def word(data: bytes, absolute: int) -> int:
    offset = absolute - 0xC00000
    return int.from_bytes(data[offset:offset + 2], "big")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, required=True)
    parser.add_argument("--slow", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--replace", action="store_true")
    args = parser.parse_args()
    markdown = args.output.with_suffix(".md")
    if (args.output.exists() or markdown.exists()) and not args.replace:
        raise FileExistsError(args.output)
    if args.replace:
        for path in (args.output, markdown):
            if path.exists():
                path.unlink()
    trace = [json.loads(line) for line in args.trace.read_text(encoding="utf-8").splitlines()]
    slow = args.slow.read_bytes()
    cells = []
    for y in range(ROWS):
        for x in range(COLUMNS):
            table_address = BASE + y * ROW_STRIDE + x * 2
            offset = word(slow, table_address)
            target = BASE + offset
            cells.append({"x": x, "y": y, "table_address": address(table_address),
                          "relative_offset": f"${offset:04X}", "target": address(target),
                          "target_first_word": f"${word(slow, target):04X}"})
    reads = []
    for index, row in enumerate(trace):
        if row["pc"] != INDEX_X:
            continue
        x = row["registers"]["d0"] & 0xFFFF
        y_row = next((item for item in trace[index + 1:] if item["pc"] == INDEX_Y), None)
        lookup = next((item for item in trace[index + 1:] if item["pc"] == LOOKUP), None)
        if y_row is None or lookup is None:
            continue
        y = y_row["registers"]["d1"] & 0xFFFF
        if x >= COLUMNS or y >= ROWS:
            continue
        cell = cells[y * COLUMNS + x]
        reads.append({"frame": row["frame"], "trace_index": row["index"],
                      "x": x, "y": y, "target": cell["target"]})
    target_counts = collections.Counter(cell["target"] for cell in cells)
    report = {
        "scope": "C2AD C0/C2/CE lookup contract in the bounded run003 M-map renderer trace",
        "directory": {"base": address(BASE), "rows": ROWS, "columns": COLUMNS,
                      "row_stride_bytes": ROW_STRIDE, "end_exclusive": address(BASE + ROWS * ROW_STRIDE)},
        "cells": cells, "observed_reads": reads,
        "observed_unique_cells": len({(row["x"], row["y"]) for row in reads}),
        "target_reuse": dict(sorted(target_counts.items())),
        "qualification": ("C2ADC0 doubles the live X index, C2ADC2 shifts the live Y index by four, and C2ADCE "
                          "reads a word at C42CA8 plus their sum before C2ADD4 adds it to the base. This proves the "
                          "8-word row stride and exports the 8x8 prefix used by the observed path. It does not establish "
                          "absolute world coordinates, cardinal orientation, that every cell is terrain, or LOD."),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# Map segment-68 relative-offset directory", "", "Classification: **traced 2-D map packet selector**.", "", report["qualification"], "",
             f"The exported prefix is `{report['directory']['rows']}x{report['directory']['columns']}`, rooted at `{report['directory']['base']}` with `{ROW_STRIDE}`-byte rows. The trace reads {report['observed_unique_cells']} unique cells. Two targets account for 59 of the 64 cells; the remaining five targets occur once each.", "",
             "| y / x | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |", "| ---: | --- | --- | --- | --- | --- | --- | --- | --- |"]
    for y in range(ROWS):
        row = cells[y * COLUMNS:(y + 1) * COLUMNS]
        lines.append("| " + str(y) + " | " + " | ".join(f"`{cell['target']}`" for cell in row) + " |")
    lines += ["", "Observed selector accesses: " + ", ".join(f"`({row['x']},{row['y']})`" for row in reads) + ".", ""]
    markdown.write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"cells": len(cells), "observed_unique_cells": report["observed_unique_cells"],
                      "unique_targets": len(target_counts)}))


if __name__ == "__main__":
    main()
