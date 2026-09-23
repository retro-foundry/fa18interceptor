"""Decode observed C2ADCE M-map relative-offset directory accesses."""
from __future__ import annotations

import argparse
import json
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE, LOOKUP = 0xC00000, 0xC2ADCE


def address(value: int) -> str:
    return f"${value & 0xFFFFFF:06X}"


def word(data: bytes, value: int) -> int:
    return int.from_bytes(data[value - SLOW_BASE:value - SLOW_BASE + 2], "big")


def long(data: bytes, value: int) -> int:
    return int.from_bytes(data[value - SLOW_BASE:value - SLOW_BASE + 4], "big")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, action="append", required=True)
    parser.add_argument("--slow", type=Path, required=True)
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/data/m_map_observed_directory_accesses.json")
    parser.add_argument("--replace", action="store_true")
    args = parser.parse_args()
    markdown = args.output.with_suffix(".md")
    if not args.replace and (args.output.exists() or markdown.exists()):
        raise FileExistsError(args.output)
    memory = args.slow.read_bytes()
    rows: dict[tuple[int, int], dict] = {}
    total = 0
    for trace_path in args.trace:
        for line in trace_path.read_text(encoding="utf-8").splitlines():
            item = json.loads(line)
            if item["pc"] != LOOKUP:
                continue
            total += 1
            base, index = item["registers"]["a1"] & 0xFFFFFF, item["registers"]["d0"] & 0xFFFF
            table_address = base + index
            offset = word(memory, table_address)
            target = base + offset
            first = long(memory, target)
            key = (base, index)
            row = rows.setdefault(key, {"base": address(base), "index": f"${index:04X}",
                                        "table_address": address(table_address), "relative_offset": f"${offset:04X}",
                                        "target": address(target), "target_first_longword": f"${first:08X}",
                                        "entry_classification": "immediate_C2AEFC_reject" if first & 0x80000000 else "nonnegative_packet_entry_candidate",
                                        "observations": []})
            row["observations"].append({"trace": str(trace_path.resolve().relative_to(ROOT)),
                                        "frame": item["frame"], "trace_index": item["index"]})
    entries = sorted(rows.values(), key=lambda row: (row["base"], row["index"]))
    bases = Counter(row["base"] for row in entries)
    reject = sum(row["entry_classification"] == "immediate_C2AEFC_reject" for row in entries)
    report = {"classification": "observed_C2ADCE_relative_offset_directory_accesses",
              "traces": [str(path.resolve().relative_to(ROOT)) for path in args.trace],
              "lookup_entries": total, "unique_slots": len(entries), "bases": dict(bases),
              "unique_reject_slots": reject, "entries": entries,
              "qualification": "Each row is an observed C2ADCE relative-offset read decoded from the paired slow-RAM snapshot. The alternate 64-byte stride is inferred from the byte-exact selector mode, but the frame-local mode word is not itself recorded here. Targets are packet-entry candidates, not terrain ownership or global world coordinates."}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# Observed M-map relative-offset directory accesses", "", "Classification: **scenario-backed selector access inventory**.", "", report["qualification"], "",
             f"{total} `$C2ADCE` visits resolve to {len(entries)} unique table slots across {len(bases)} static bases; {reject} unique slots immediately reject at `$C2AEFC`.", "",
             "| base | index | table slot | target | entry class | observations |", "| --- | ---: | --- | --- | --- | ---: |"]
    for row in entries:
        lines.append(f"| `{row['base']}` | `{row['index']}` | `{row['table_address']}` | `{row['target']}` | {row['entry_classification']} | {len(row['observations'])} |")
    lines.append("")
    markdown.write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"visits": total, "slots": len(entries), "bases": len(bases), "reject_slots": reject}))


if __name__ == "__main__":
    main()
