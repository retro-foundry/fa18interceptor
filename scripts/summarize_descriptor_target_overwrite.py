"""Audit whether a selected C1CC70 descriptor target reaches the next C1F6F8 entry."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


SLOW_BASE = 0xC00000
STORE = 0xC1CC70
WALKER = 0xC1F6F8


def address(value: int) -> str:
    return f"${value & 0xFFFFFF:06X}"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, required=True)
    parser.add_argument("--slow", type=Path, required=True)
    parser.add_argument("--target", type=lambda value: int(value, 0), required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists() or args.output.with_suffix(".md").exists():
        raise FileExistsError(args.output)
    trace = [json.loads(line) for line in args.trace.read_text(encoding="utf-8").splitlines()]
    slow = args.slow.read_bytes()
    if len(slow) != 0x80000:
        raise ValueError("expected a 512 KiB slow-RAM snapshot")
    stores = []
    for row in trace:
        if row["pc"] != STORE:
            continue
        field = row["registers"]["a1"] & 0xFFFFFF
        value = int.from_bytes(slow[field - SLOW_BASE:field - SLOW_BASE + 4], "big") & 0xFFFFFF
        stores.append({"trace_index": row["index"], "field": address(field), "value": address(value)})
    selected = next((row for row in stores if row["value"] == address(args.target)), None)
    if selected is None:
        raise ValueError(f"selected target {address(args.target)} was not stored")
    next_walker = next((row for row in trace if row["index"] > selected["trace_index"] and row["pc"] == WALKER), None)
    preceding = [row for row in stores if selected["trace_index"] <= row["trace_index"] < (next_walker["index"] if next_walker else len(trace))]
    walker_value = (address(next_walker["registers"]["a1"]) if next_walker else None)
    report = {"classification": "selected_descriptor_target_store_and_next_walker_boundary",
              "authority": {"trace": str(args.trace), "slow": str(args.slow)},
              "selected_target": address(args.target), "selected_store": selected,
              "stores_before_next_walker": preceding,
              "next_walker": ({"trace_index": next_walker["index"], "entry_a1": walker_value}
                              if next_walker else None),
              "qualification": "C1CC70 store ordering and the next observed C1F6F8 entry are proven. This does not establish that a stored target is executed, parsed, or a model/terrain pointer."}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# `$C3B4F8` descriptor store before control-walker overwrite", "",
             "Classification: **bounded descriptor-store ordering**.", "",
             f"The no-future-input trace starts at `$C1CC70` reading `{selected['field']}`, whose static `+8` field is `{selected['value']}`. Before the next `$C1F6F8` entry, it performs {len(preceding) - 1} later descriptor-field stores. The walker enters with `A1 = {walker_value}`, not `{address(args.target)}`.", "",
             "Therefore this trace proves `$C3B4F8` is live descriptor-control input, but does not prove it reaches the walker, executes `$C3B4FE`, or identifies a mountain instance.", "",
             "| Trace index | descriptor `+8` field | stored control target |", "| ---: | --- | --- |"]
    lines.extend(f"| {row['trace_index']} | `{row['field']}` | `{row['value']}` |" for row in preceding)
    lines.extend(["", report["qualification"], ""])
    args.output.with_suffix(".md").write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"stores_before_walker": len(preceding), "walker": walker_value}))


if __name__ == "__main__":
    main()
