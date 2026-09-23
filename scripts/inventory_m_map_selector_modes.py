"""Inventory observed C2ADCE selector modes from paired trace/snapshot inputs."""
from __future__ import annotations

import argparse
import json
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE, LOOKUP, MODE_OFFSET = 0xC00000, 0xC2ADCE, 0x3E


def address(value: int) -> str:
    return f"${value & 0xFFFFFF:06X}"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--pair", action="append", required=True,
                        help="trace.jsonl::slow.bin (one paired capture)")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/data/m_map_selector_modes.json")
    parser.add_argument("--replace", action="store_true")
    args = parser.parse_args()
    markdown = args.output.with_suffix(".md")
    if not args.replace and (args.output.exists() or markdown.exists()):
        raise FileExistsError(args.output)
    rows = []
    total = 0
    for pair in args.pair:
        trace_text, slow_text = pair.split("::", 1)
        trace_path, slow_path = Path(trace_text), Path(slow_text)
        memory = slow_path.read_bytes()
        counts = Counter()
        for line in trace_path.read_text(encoding="utf-8").splitlines():
            item = json.loads(line)
            if item["pc"] != LOOKUP:
                continue
            a6 = item["registers"]["a6"] & 0xFFFFFF
            mode = int.from_bytes(memory[a6 - MODE_OFFSET - SLOW_BASE:a6 - MODE_OFFSET - SLOW_BASE + 2], "big", signed=True)
            counts[(item["registers"]["a1"] & 0xFFFFFF, mode)] += 1
            total += 1
        for (base, mode), visits in sorted(counts.items()):
            rows.append({"trace": str(trace_path.resolve().relative_to(ROOT)),
                         "slow": str(slow_path.resolve().relative_to(ROOT)),
                         "base": address(base), "mode": mode,
                         "row_stride_bytes": 16 if mode == 0 else 64,
                         "visits": visits})
    report = {"classification": "scenario_backed_C2AD_selector_mode_inventory",
              "visits": total, "rows": rows,
              "qualification": "Mode is the signed word at -$3E(A6) read by the byte-exact selector. Zero chooses the 16-byte row stride and non-zero chooses the 64-byte stride. The paired snapshot supplies the frame-local word; this does not map either layout to global terrain coordinates or LOD."}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# M-map selector stride modes", "", "Classification: **scenario-backed frame-local selector mode inventory**.", "", report["qualification"], "",
             f"{total} `$C2ADCE` visits were decoded from paired trace/snapshot inputs.", "",
             "| trace | base | mode | row stride | visits |", "| --- | --- | ---: | ---: | ---: |"]
    for row in rows:
        lines.append(f"| `{row['trace']}` | `{row['base']}` | {row['mode']} | {row['row_stride_bytes']} | {row['visits']} |")
    lines.append("")
    markdown.write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"visits": total, "rows": len(rows)}))


if __name__ == "__main__":
    main()
