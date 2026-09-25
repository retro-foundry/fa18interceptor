"""Census renderer-facing component addresses in a bounded demo-flight trace.

This intentionally produces a census rather than placement or mesh ownership:
the trace was captured in flight without a simultaneous placement builder trace.
"""
from __future__ import annotations

import argparse
import collections
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE = 0xC00000
EVENTS = {
    0xC1CC70: "descriptor_field_store",
    0xC1EF10: "stream_cursor_publish",
    # C1F4AC is the routine entry; the first source-triple read is at C1F4B0.
    # A long trace may enter through the preceding instruction without emitting
    # a row at the symbol boundary, so census the actual immutable-source read.
    0xC1F4B0: "transform_source",
    0xC1F6F8: "control_walker_entry",
    0xC1F70E: "control_stream_load",
    0xC2FA7E: "line_submit",
    0xC2FF48: "polygon_submit",
}


def address(value: int) -> str:
    return f"${value & 0xFFFFFF:06X}"


def counts(rows: list[dict], key: str) -> list[dict]:
    return [{key: value, "count": count} for value, count in
            sorted(collections.Counter(row[key] for row in rows).items())]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, required=True)
    parser.add_argument("--slow", type=Path, required=True,
                        help="slow-RAM snapshot valid for descriptor fields")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    trace = [json.loads(line) for line in args.trace.read_text(encoding="utf-8").splitlines()]
    slow = args.slow.read_bytes()
    rows: dict[str, list[dict]] = {name: [] for name in EVENTS.values()}
    for row in trace:
        name = EVENTS.get(row["pc"])
        if name is None:
            continue
        registers = row["registers"]
        item = {"frame": row["frame"], "trace_index": row["index"]}
        if name == "descriptor_field_store":
            field = registers["a1"] & 0xFFFFFF
            offset = field - SLOW_BASE
            if not 0 <= offset <= len(slow) - 4:
                raise ValueError(f"descriptor field {address(field)} outside supplied slow RAM")
            item.update(descriptor=address(field - 8), field=address(int.from_bytes(slow[offset:offset + 4], "big")))
        elif name == "transform_source":
            item["source"] = address(registers["a1"])
        elif name == "control_walker_entry":
            item["entry_a1"] = address(registers["a1"])
        elif name in {"stream_cursor_publish", "control_stream_load", "line_submit", "polygon_submit"}:
            item["a5"] = address(registers["a2"] if name == "stream_cursor_publish" else registers["a5"])
        rows[name].append(item)
    payload = {
        "classification": "demo_flight_renderer_address_census_not_placement_or_mesh_ownership",
        "authority": {
            "trace": str(args.trace.resolve().relative_to(ROOT)).replace("\\", "/"),
            "slow": str(args.slow.resolve().relative_to(ROOT)).replace("\\", "/"),
        },
        "events": rows,
        "summary": {
            name: {"events": len(values), "distinct": counts(values, "field" if name == "descriptor_field_store" else "source" if name == "transform_source" else "entry_a1" if name == "control_walker_entry" else "a5")}
            for name, values in rows.items()
        },
        "qualification": "This is a renderer/control census of an ordinary demo-flight trace. It does not join a flight placement record to these addresses and therefore does not prove named buildings, landmarks, complete models, or per-instance primitive ownership.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    lines = ["# Demonstration-flight renderer component census", "",
             "Classification: **renderer/control address census, not placement or mesh ownership**.", "",
             "The bounded flight trace reaches the renderer repeatedly. This report retains every distinct descriptor field, immutable transform source, control pointer, and submission context without assigning feature names or joining them to flight placement records.", ""]
    for name, summary in payload["summary"].items():
        lines += [f"## `{name}`", "", f"Events: **{summary['events']}**", "",
                  "| address | count |", "| --- | ---: |"]
        field_name = "field" if name == "descriptor_field_store" else "source" if name == "transform_source" else "entry_a1" if name == "control_walker_entry" else "a5"
        lines += [f"| {row[field_name]} | {row['count']} |" for row in summary["distinct"]]
        lines.append("")
    lines += [payload["qualification"], ""]
    args.output.with_suffix(".md").write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({name: value["events"] for name, value in payload["summary"].items()}))


if __name__ == "__main__":
    main()
