"""Inventory C29F00 signed selector byte pairs observed at the M-map packet selector."""
from __future__ import annotations

import argparse
import json
from collections import defaultdict
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE = 0xC00000
ENTRY = 0xC2AD80
TABLE = 0xC29F00


def address(value: int) -> str:
    return f"${value:06X}"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, action="append", required=True)
    parser.add_argument("--slow", type=Path, required=True,
                        help="verified segment-36 slow-RAM snapshot")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/data/m_map_selector_byte_pairs.json")
    parser.add_argument("--replace", action="store_true")
    args = parser.parse_args()
    markdown = args.output.with_suffix(".md")
    if not args.replace and (args.output.exists() or markdown.exists()):
        raise FileExistsError(args.output)
    slow = args.slow.read_bytes()
    if len(slow) != 0x80000:
        raise ValueError("expected 512 KiB slow RAM")
    hits: dict[int, list[dict]] = defaultdict(list)
    for trace_path in args.trace:
        for row in (json.loads(line) for line in trace_path.read_text(encoding="utf-8").splitlines()):
            if row["pc"] != ENTRY:
                continue
            selector = row["registers"]["d2"] & 0xFFFF
            table_address = TABLE + selector * 2
            if not SLOW_BASE <= table_address <= SLOW_BASE + len(slow) - 2:
                raise ValueError(f"selector {selector} reaches outside slow RAM")
            offset = table_address - SLOW_BASE
            pair = [int.from_bytes(slow[offset:offset + 1], "big", signed=True),
                    int.from_bytes(slow[offset + 1:offset + 2], "big", signed=True)]
            hits[selector].append({"trace": str(trace_path.resolve().relative_to(ROOT)),
                                   "frame": row["frame"], "trace_index": row["index"],
                                   "pair": pair})
    rows = []
    for selector, observations in sorted(hits.items()):
        pairs = {tuple(item["pair"]) for item in observations}
        if len(pairs) != 1:
            raise AssertionError(f"selector {selector} has inconsistent snapshot pair values")
        rows.append({"selector": selector, "table_address": address(TABLE + selector * 2),
                     "signed_pair": list(pairs.pop()), "observations": observations})
    observed_pairs = {tuple(row["signed_pair"]) for row in rows}
    expected_local_lattice = {(x, y) for y in range(-2, 3) for x in range(-2, 3)}
    full_local_lattice = observed_pairs == expected_local_lattice
    report = {"classification": "scenario_observed_c29f00_signed_selector_byte_pairs",
              "entry": address(ENTRY), "table": address(TABLE),
              "traces": [str(path.resolve().relative_to(ROOT)) for path in args.trace],
              "entries": rows, "full_local_lattice_minus2_to_2": full_local_lattice,
              "qualification": ("Each pair is read by the byte-exact C2AD80 selector and added to live row/column "
                                "minima before bounded relative-offset lookup. This is an observed subtable only; "
                                "it does not establish the complete C29F00 table, global coordinates, or terrain identity.")}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# Observed M-map selector byte pairs", "", "Classification: **scenario-backed selector-input subtable**.", "", report["qualification"], "",
             f"`$C2AD80` observes {len(rows)} selector values at `$C29F00`; the JSON retains every hit.",
             f"Their distinct pairs are the complete `{{-2..2}} × {{-2..2}}` lattice: {full_local_lattice}.", "",
             "| Selector | table bytes | signed pair | observations |", "| ---: | --- | --- | ---: |"]
    for row in rows:
        lines.append(f"| {row['selector']} | `{row['table_address']}` | {tuple(row['signed_pair'])} | {len(row['observations'])} |")
    lines.append("")
    markdown.write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"selectors": len(rows), "hits": sum(len(row["observations"]) for row in rows)}))


if __name__ == "__main__":
    main()
