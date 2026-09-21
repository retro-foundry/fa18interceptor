"""Collect polygon tuples at C2469E before clipping and final submission.

The active face workspace has a status word at C4BF90, vertex count at
C4BF92, and triples from C4BF94.  This catches faces accepted by their record
handler even when clipping later prevents C2FF48 submission.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT


PRECLIP = 0xC2469E
WORKSPACE = 0xC4BF90


def signed_word(engine: Engine, address: int) -> int:
    return int.from_bytes(engine.memory(address, 2), "big", signed=True)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    parser.add_argument("--frames", type=int, default=2)
    parser.add_argument("--max-faces", type=int, default=256)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    args.output.mkdir(parents=True)
    engine = Engine(args.config.resolve(), args.output / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        engine.core.e9k_debug_add_breakpoint(PRECLIP)
        submissions = []
        for _ in range(args.frames):
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                if registers["pc"] != PRECLIP:
                    raise RuntimeError(f"unexpected breakpoint PC {registers['pc']:06X}")
                count = int.from_bytes(engine.memory(WORKSPACE + 2, 2), "big")
                if not 3 <= count <= 16:
                    raise RuntimeError(f"invalid C2469E workspace count {count}")
                raw = engine.memory(WORKSPACE + 4, count * 6)
                triples = [[int.from_bytes(raw[index + axis * 2:index + axis * 2 + 2], "big", signed=True)
                            for axis in range(3)] for index in range(0, len(raw), 6)]
                submissions.append({
                    "submission": len(submissions), "host_frame": engine.frame,
                    "count": count, "triples": triples,
                    "context": {name: f"${registers[name] & 0xFFFFFF:06X}" for name in ("a5", "a2", "a3", "a4")},
                })
                if len(submissions) >= args.max_faces:
                    break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if len(submissions) >= args.max_faces:
                break
        report = {
            "scope": "C2469E face tuples before clipping", "restore": str(args.restore),
            "frames": args.frames, "submission_count": len(submissions), "submissions": submissions,
            "qualification": "Faces have passed their handler's orientation gate but may be clipped/rejected before C2FF48. Coordinates are mutable C4BF workspace values.",
        }
        (args.output / "preclip_faces.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"faces": len(submissions), "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
