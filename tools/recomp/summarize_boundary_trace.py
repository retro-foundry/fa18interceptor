"""Summarize a routine's live instruction/event CSV into timing evidence.

Example:
  python tools/recomp/summarize_boundary_trace.py TRACE.csv --entry C2B05A \
      --returns C2B3AA C2B3B2 --out build/recomp/region_boundaries_demo01.json
The input comes from FA18_BOUNDARY_TRACE/FA18_BOUNDARY_RANGE. Sandbox rows
are excluded. This reports observed costs, not proposed fixed cycle charges.
"""
import argparse
from collections import Counter
import csv
import json
from pathlib import Path


def summarize(path, entry, returns):
    calls = []
    active = None
    events = Counter()
    with path.open(newline="") as source:
        for row in csv.DictReader(source):
            if row["log_mode"] == "1":
                continue
            pc = int(row["source_pc"], 16)
            kind = row["kind"]
            cycle = int(row["cycle"])
            if kind == "instruction" and pc == entry:
                if active is not None:
                    raise ValueError(f"new entry before prior return at cycle {cycle}")
                active = {
                    "start_cycle": cycle, "start_frame": int(row["frame"]),
                    "start_blits": int(row["blits"]), "instructions": 0,
                    "events": [],
                }
            if active is None:
                continue
            if kind == "instruction":
                active["instructions"] += 1
            elif kind in ("event_before", "event_after", "frame_end"):
                events[(kind, row["source_pc"])] += 1
                active["events"].append({
                    "kind": kind, "source_pc": row["source_pc"], "pc": row["pc"],
                    "elapsed": cycle - active["start_cycle"],
                    "next_event": int(row["next_event"]), "frame": int(row["frame"]),
                    "vpos": int(row["vpos"]), "blits": int(row["blits"]),
                    "sp": row["a7"], "sr": row["sr"],
                    "interrupt": kind == "event_after" and row["pc"] != row["source_pc"],
                })
            elif kind == "flow_target" and pc in returns:
                active["cycles"] = cycle - active["start_cycle"]
                active["end_frame"] = int(row["frame"])
                active["end_blits"] = int(row["blits"])
                calls.append(active)
                active = None
    costs = [call["cycles"] for call in calls]
    return {
        "trace": path.as_posix(), "entry": f"{entry:06X}",
        "completed_calls": len(calls), "incomplete_call": active,
        "min_cycles": min(costs) if costs else None,
        "max_cycles": max(costs) if costs else None,
        "mean_cycles": sum(costs) // len(costs) if costs else None,
        "calls_with_frame_end": sum(any(e["kind"] == "frame_end" for e in c["events"]) for c in calls),
        "calls_spanning_frames": sum(c["start_frame"] != c["end_frame"] for c in calls),
        "calls_with_interrupt": sum(any(e["interrupt"] for e in c["events"]) for c in calls),
        "event_sites": [{"kind": k, "pc": p, "count": n}
                        for (k, p), n in sorted(events.items())],
        "calls": calls,
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("trace", type=Path)
    parser.add_argument("--entry", required=True, type=lambda v: int(v, 16))
    parser.add_argument("--returns", required=True, nargs="+", type=lambda v: int(v, 16))
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args()
    result = summarize(args.trace, args.entry, set(args.returns))
    args.out.write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps({k: v for k, v in result.items()
                      if k not in ("calls", "event_sites", "incomplete_call")}))
    if result["incomplete_call"] is not None:
        print("trace ended inside a call; see incomplete_call in output")


if __name__ == "__main__":
    main()
