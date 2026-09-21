"""Collect repeated C2FF48 polygon submissions from a sealed runtime checkpoint.

The collector never writes emulated memory.  It records finalized C4B990
triples and their C4B390 projected counterpart each time the renderer wrapper
is entered, then single-steps past that entry to await the next submission.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT


SUBMIT = 0xC2FF48
TRIPLES = 0xC4B990
PAIRS = 0xC4B390


def words(engine: Engine, address: int, count: int) -> list[int]:
    raw = engine.memory(address, count * 2)
    return [int.from_bytes(raw[index:index + 2], "big", signed=True)
            for index in range(0, len(raw), 2)]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    parser.add_argument("--frames", type=int, default=2)
    parser.add_argument("--max-submissions", type=int, default=64)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    if args.frames < 1 or args.max_submissions < 1:
        raise ValueError("frames and max-submissions must be positive")
    args.output.mkdir(parents=True)

    engine = Engine(args.config.resolve(), args.output / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        engine.core.e9k_debug_add_breakpoint(SUBMIT)
        submissions = []
        for _ in range(args.frames):
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                if engine.regs()["pc"] != SUBMIT:
                    raise RuntimeError(f"unexpected breakpoint PC {engine.regs()['pc']:06X}")
                count = int.from_bytes(engine.memory(PAIRS, 2), "big")
                # The existing Golden Gate sample has 3--4 vertices, while
                # the external-camera pass reaches a valid ten-vertex list.
                # The tuple workspace holds up to sixteen triples.
                if not 3 <= count <= 16:
                    raise RuntimeError(f"invalid polygon count {count} at C2FF48")
                triple_words = words(engine, TRIPLES, count * 3)
                pair_words = words(engine, PAIRS + 2, count * 2)
                registers = engine.regs()
                submissions.append({
                    "submission": len(submissions), "host_frame": engine.frame,
                    "count": count,
                    "triples": [triple_words[index:index + 3] for index in range(0, len(triple_words), 3)],
                    "stored_screen_pairs": [pair_words[index:index + 2] for index in range(0, len(pair_words), 2)],
                    "context": {name: f"${registers[name]:06X}" for name in ("a5", "a2", "a3", "a4")},
                })
                if len(submissions) >= args.max_submissions:
                    break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if len(submissions) >= args.max_submissions:
                break
        report = {"scope": "finalized polygon tuples at C2FF48", "restore": str(args.restore),
                  "frames": args.frames, "submission_count": len(submissions), "submissions": submissions,
                  "qualification": "C4B990/C4B390 are mutable finalized workspaces; polygon owner requires upstream control-stream correlation."}
        (args.output / "polygon_submissions.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"submissions": len(submissions), "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
