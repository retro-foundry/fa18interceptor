"""Reconstruct bounded caller ancestry at selected PCs from Engine9000 traces.

Only observed JSR/BSR transitions with the 68000 return-address stack push
become frames. A conditional or unconditional branch keeps its current frame.
The trace may start inside a call or cross an interrupt; such ancestry is
necessarily incomplete. This tool reports execution evidence, not function
ownership or semantic names.
"""
from __future__ import annotations

import argparse
import json
from collections import Counter, defaultdict
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def address(value: int | str) -> int:
    return int(value[1:], 16) if isinstance(value, str) and value.startswith("$") else int(value)


def label(value: int) -> str:
    return f"${value:06X}"


def relative(path: Path) -> str:
    resolved = path.resolve()
    return resolved.relative_to(ROOT).as_posix() if resolved.is_relative_to(ROOT) else str(resolved)


def analyze(path: Path, anchors: set[int], focus_targets: set[int], max_chains: int) -> dict:
    frames: list[dict] = []
    focus_calls = []
    chains: dict[int, Counter] = defaultdict(Counter)
    examples: dict[int, dict] = defaultdict(dict)
    counters = Counter()
    previous = None

    with path.open(encoding="utf-8") as stream:
        for index, line in enumerate(stream):
            row = json.loads(line)
            pc = address(row["pc"])
            if previous is not None:
                prior = previous["row"]
                prior_pc = previous["pc"]
                operation = prior["asm"].split(" ", 1)[0].lower()
                prior_sp = prior["registers"]["a7"]
                next_sp = row["registers"]["a7"]
                if operation.startswith(("jsr", "bsr")):
                    if (address(prior.get("next_pc", pc)) == pc
                            and next_sp == ((prior_sp - 4) & 0xFFFFFFFF)):
                        frames.append({"caller": prior_pc, "callee": pc,
                                       "return": prior_pc + len(prior["bytes"]) // 2,
                                       "callee_sp": next_sp, "caller_sp": prior_sp,
                                       "entry_index": index, "entry_frame": row.get("frame"),
                                       "anchor_counts": Counter()})
                        counters["verified_call_pushes"] += 1
                    else:
                        counters["unverified_call_transitions"] += 1
                elif operation == "rts":
                    match = next((i for i in range(len(frames) - 1, -1, -1)
                                  if frames[i]["return"] == pc
                                  and frames[i]["callee_sp"] == prior_sp
                                  and frames[i]["caller_sp"] == next_sp), None)
                    if match is None:
                        counters["returns_without_in_window_call"] += 1
                    else:
                        counters["frames_unwound_above_return"] += len(frames) - match - 1
                        for frame in frames[match:]:
                            if frame["callee"] in focus_targets:
                                focus_calls.append({
                                    "site": label(frame["caller"]), "target": label(frame["callee"]),
                                    "entry_index": frame["entry_index"],
                                    "entry_frame": frame["entry_frame"],
                                    "return_index": index if frame is frames[match] else None,
                                    "return_pc": label(pc) if frame is frames[match] else None,
                                    "termination": "verified_return" if frame is frames[match] else "unwound",
                                    "anchor_counts": {label(key): count for key, count in frame["anchor_counts"].items()},
                                })
                        del frames[match:]
                        counters["verified_returns"] += 1

            if pc in anchors:
                counters["anchor_hits"] += 1
                # Keep the current 64 KiB stack region, so interrupt-stack
                # calls cannot be mistaken for the paused user-stack chain.
                stack_region = row["registers"]["a7"] & 0xFF0000
                for frame in frames:
                    if (frame["callee"] in focus_targets
                            and (frame["callee_sp"] & 0xFF0000) == stack_region):
                        frame["anchor_counts"][pc] += 1
                game_chain = tuple((frame["caller"], frame["callee"])
                                   for frame in frames
                                   if 0xC00000 <= frame["callee"] < 0xC60000
                                   and (frame["callee_sp"] & 0xFF0000) == stack_region)
                chains[pc][game_chain] += 1
                if game_chain not in examples[pc]:
                    examples[pc][game_chain] = {
                        "trace_index": index, "frame": row.get("frame"),
                        "predecessor_pc": label(previous["pc"]) if previous else None,
                        "predecessor_asm": previous["row"]["asm"] if previous else None,
                        "a7": label(row["registers"]["a7"]),
                    }
            previous = {"pc": pc, "row": row}

    for frame in frames:
        if frame["callee"] in focus_targets:
            focus_calls.append({"site": label(frame["caller"]), "target": label(frame["callee"]),
                                "entry_index": frame["entry_index"],
                                "entry_frame": frame["entry_frame"],
                                "return_index": None, "return_pc": None,
                                "termination": "trace_end",
                                "anchor_counts": {label(key): count for key, count in frame["anchor_counts"].items()}})
    result = {"trace": relative(path), "instruction_rows": index + 1,
              "stack_filter": "Slow-RAM calls in the anchor A7 64-KiB region; excludes separate interrupt stack",
              "checks": dict(counters), "focus_calls": focus_calls, "anchors": {}}
    for anchor in sorted(anchors):
        ordered = chains[anchor].most_common()
        result["anchors"][label(anchor)] = {
            "hits": sum(chains[anchor].values()), "distinct_call_chains": len(ordered),
            "chains": [{"hits": count,
                        "calls": [{"site": label(site), "target": label(target)}
                                  for site, target in chain],
                        "example": examples[anchor][chain]}
                       for chain, count in ordered[:max_chains]],
            "omitted_chains": max(0, len(ordered) - max_chains),
        }
    return result


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, action="append", required=True,
                        help="repeat to compare bounded trace windows")
    parser.add_argument("--anchor", type=lambda value: int(value, 0), action="append", required=True,
                        help="runtime PC at which to record the active observed call chain")
    parser.add_argument("--focus-target", type=lambda value: int(value, 0), action="append", default=[],
                        help="report bounded invocations of this callee and anchor hits within each")
    parser.add_argument("--max-chains", type=int, default=12)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.max_chains < 1:
        parser.error("--max-chains must be positive")
    result = {"classification": "observed calls only; branches share their enclosing frame",
              "anchors": [label(value) for value in args.anchor],
              "focus_targets": [label(value) for value in args.focus_target],
              "traces": [analyze(path, set(args.anchor), set(args.focus_target), args.max_chains)
                         for path in args.trace]}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"output": relative(args.output), "traces": len(result["traces"]),
                      "anchor_hits": sum(trace["checks"].get("anchor_hits", 0)
                                         for trace in result["traces"])}))


if __name__ == "__main__":
    main()
