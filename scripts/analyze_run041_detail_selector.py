"""Extract C1EE14 threshold decisions for the two Golden Gate control streams."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
CONTROL_STREAMS = {"$C35568", "$C355A0"}
SLOW_BASE = 0xC00000


def word(slow: bytes, address: int) -> int:
    offset = address - SLOW_BASE
    if not 0 <= offset <= len(slow) - 2:
        raise ValueError(f"${address:06X} is outside canonical Slow RAM")
    return int.from_bytes(slow[offset:offset + 2], "big")


def signed(value: int) -> int:
    value &= 0xFFFF
    return value - 0x10000 if value & 0x8000 else value


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--invocations", type=Path,
                        default=ROOT / "analysis/data/run041_descriptor_stage_invocations.json")
    parser.add_argument("--slow", type=Path, default=ROOT / "captures/baseline_menu/slow.bin")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/data/run041_detail_selector_decisions.json")
    args = parser.parse_args()
    source = json.loads(args.invocations.read_text(encoding="utf-8"))
    slow = args.slow.read_bytes()
    result = {"classification": "same-invocation scaled-word threshold and source selection; no physical-range claim",
              "authority": args.invocations.relative_to(ROOT).as_posix(), "traces": []}
    for trace in source["traces"]:
        rows = [json.loads(line) for line in (ROOT / trace["trace"]).open(encoding="utf-8")]
        observed = []
        for call in trace["calls"]:
            if call["control_pointer"] not in CONTROL_STREAMS:
                continue
            stop = call["return_index"] if call["return_index"] is not None else len(rows)
            checks = []
            selected = None
            for index in range(call["entry_index"], stop):
                row = rows[index]
                pc = row["pc"]
                if pc == 0xC1EE54:
                    pointer = row["registers"]["a2"] & 0xFFFFFF
                    raw = rows[index + 1]["registers"]["d0"] & 0xFFFF
                    if raw != word(slow, pointer):
                        raise ValueError(f"${pointer:06X}: runtime ${raw:04X} differs from snapshot")
                    checks.append({"address": f"${pointer:06X}", "raw": f"${raw:04X}",
                                   "trace_index": index})
                elif pc == 0xC1EE60:
                    if not checks:
                        raise ValueError("comparison without a fetched header")
                    checks[-1]["scaled"] = signed(row["registers"]["d0"])
                    checks[-1]["limit"] = signed(row["registers"]["d1"])
                    checks[-1]["shift"] = row["registers"]["d3"] & 0xFFFF
                elif pc == 0xC1EE62:
                    check = checks[-1]
                    taken = row["next_pc"] == 0xC1EE7A
                    if taken != (check["scaled"] > check["limit"]):
                        raise ValueError(f"branch mismatch at trace row {index}")
                    check["above_limit"] = taken
                elif pc == 0xC1EE84 and selected is None:
                    pointer = (row["registers"]["a2"] - 2) & 0xFFFFFF
                    raw = row["registers"]["d0"] & 0xFFFF
                    if raw != word(slow, pointer):
                        raise ValueError(f"selected ${pointer:06X}: runtime ${raw:04X} differs")
                    selected = {"address": f"${pointer:06X}", "word": f"${raw:04X}",
                                "trace_index": index}
            if checks and "scaled" not in checks[-1]:
                if selected is None or checks[-1]["address"] != selected["address"]:
                    raise ValueError(f"unaccounted terminal record at call {call['entry_index']}")
                checks.pop()
            if any("above_limit" not in check for check in checks):
                raise ValueError(f"incomplete comparison at call {call['entry_index']}")
            observed.append({"entry_frame": call["entry_frame"],
                             "entry_index": call["entry_index"],
                             "call_site": call["call_site"],
                             "descriptor": call["descriptor"],
                             "control_pointer": call["control_pointer"],
                             "threshold": call["threshold"], "shift": call["shift"],
                             "comparisons": checks, "selected_record": selected,
                             "transform_sources": call["transform_sources"],
                             "walker_streams": call["walker_streams"],
                             "polygons": call["polygons"], "lines": call["lines"],
                             "termination": call["termination"]})
        result["traces"].append({"trace": trace["trace"], "calls": observed})
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"output": args.output.relative_to(ROOT).as_posix(),
                      "calls": [len(row["calls"]) for row in result["traces"]]}))


if __name__ == "__main__":
    main()
