"""Join run041 descriptor-stage sources and outputs within observed call frames."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
STAGE_TARGETS = {0xC1EE14, 0xC1ED3C, 0xC1ED4C}
OUTPUTS = {0xC2FF48: "polygons", 0xC2FA7E: "lines"}


def addr(value: int | str) -> int:
    return int(value[1:], 16) if isinstance(value, str) and value.startswith("$") else int(value)


def hexaddr(value: int | None) -> str | None:
    return f"${value:06X}" if value is not None else None


def relative(path: Path) -> str:
    resolved = path.resolve()
    return resolved.relative_to(ROOT).as_posix() if resolved.is_relative_to(ROOT) else str(resolved)


def collect(path: Path) -> dict:
    stack = []
    calls = []
    previous = None
    latest_descriptor = None
    counts = {"verified_call_pushes": 0, "verified_returns": 0,
              "unmatched_returns": 0, "unwound_frames": 0}

    with path.open(encoding="utf-8") as source:
        for index, line in enumerate(source):
            row = json.loads(line)
            pc = addr(row["pc"])
            sp = row["registers"]["a7"]
            if previous:
                prior = previous["row"]
                prior_pc = previous["pc"]
                old_sp = prior["registers"]["a7"]
                op = prior["asm"].split(" ", 1)[0].lower()
                if op.startswith(("jsr", "bsr")):
                    if addr(prior.get("next_pc", pc)) == pc and sp == ((old_sp - 4) & 0xFFFFFFFF):
                        frame = {"site": prior_pc, "target": pc,
                                 "return": prior_pc + len(prior["bytes"]) // 2,
                                 "callee_sp": sp, "caller_sp": old_sp}
                        if pc in STAGE_TARGETS:
                            descriptor = latest_descriptor if (prior_pc == 0xC1CC86
                                and latest_descriptor is not None
                                and index - latest_descriptor["index"] <= 10
                                and row.get("frame") == latest_descriptor["frame"]) else None
                            frame["record"] = {
                                "entry_index": index, "entry_frame": row.get("frame"),
                                "call_site": hexaddr(prior_pc), "target": hexaddr(pc),
                                "descriptor": hexaddr(descriptor["address"]) if descriptor else None,
                                "descriptor_store_index": descriptor["index"] if descriptor else None,
                                "control_pointer": None, "control_read_index": None,
                                "transform_sources": [], "walker_streams": [],
                                "transform_entries": 0, "walker_entries": 0,
                                "polygons": 0, "lines": 0,
                                "threshold": None, "shift": None,
                                "entered_c1ee14": False,
                            }
                        stack.append(frame)
                        counts["verified_call_pushes"] += 1
                elif op == "rts":
                    match = next((i for i in range(len(stack) - 1, -1, -1)
                                  if stack[i]["return"] == pc
                                  and stack[i]["callee_sp"] == old_sp
                                  and stack[i]["caller_sp"] == sp), None)
                    if match is None:
                        counts["unmatched_returns"] += 1
                    else:
                        counts["unwound_frames"] += len(stack) - match - 1
                        for i, frame in enumerate(stack[match:]):
                            if "record" in frame:
                                record = frame["record"]
                                record["termination"] = "verified_return" if i == 0 else "unwound"
                                record["return_index"] = index if i == 0 else None
                                record["return_pc"] = hexaddr(pc) if i == 0 else None
                                calls.append(record)
                        del stack[match:]
                        counts["verified_returns"] += 1

            if pc == 0xC1CC70:
                latest_descriptor = {"index": index, "frame": row.get("frame"),
                                     "address": row["registers"]["a1"] - 8}
            stack_region = sp & 0xFF0000
            for frame in stack:
                if ("record" not in frame
                        or (frame["callee_sp"] & 0xFF0000) != stack_region):
                    continue
                record = frame["record"]
                if pc == 0xC1EE14:
                    record["entered_c1ee14"] = True
                elif pc == 0xC1EE22:
                    record["threshold"] = row["registers"]["d1"] & 0xFFFF
                elif pc == 0xC1EE54 and record["shift"] is None:
                    record["shift"] = row["registers"]["d3"] & 0xFFFF
                elif pc == 0xC1EE4A:
                    record["control_pointer"] = hexaddr(row["registers"]["a2"] & 0xFFFFFF)
                    record["control_read_index"] = index
                elif pc == 0xC1F4AC:
                    record["transform_entries"] += 1
                    pointer = hexaddr(row["registers"]["a1"] & 0xFFFFFF)
                    if pointer not in record["transform_sources"]:
                        record["transform_sources"].append(pointer)
                elif pc == 0xC1F6F8:
                    record["walker_entries"] += 1
                elif pc == 0xC1F70E:
                    pointer = hexaddr(row["registers"]["a5"] & 0xFFFFFF)
                    if pointer not in record["walker_streams"]:
                        record["walker_streams"].append(pointer)
                elif pc in OUTPUTS:
                    record[OUTPUTS[pc]] += 1
            previous = {"pc": pc, "row": row}

    for frame in stack:
        if "record" in frame:
            record = frame["record"]
            record["termination"] = "trace_end"
            record["return_index"] = None
            record["return_pc"] = None
            calls.append(record)
    calls.sort(key=lambda item: item["entry_index"])
    return {"trace": relative(path), "instruction_rows": index + 1,
            "checks": counts, "calls": calls}


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, action="append", required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    result = {"classification": "same-invocation register/PC joins; no physical-coordinate or object identity claim",
              "traces": [collect(path) for path in args.trace]}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"output": relative(args.output),
                      "calls": [len(trace["calls"]) for trace in result["traces"]]}))


if __name__ == "__main__":
    main()
