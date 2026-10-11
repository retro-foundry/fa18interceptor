"""Collect C2159E triangle workspaces immediately before their depth gate."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT


GATE = 0xC21642
WORKSPACE = 0xC4BF94


def words(engine: Engine, address: int, count: int) -> list[int]:
    raw = engine.memory(address, count * 2)
    return [int.from_bytes(raw[index:index + 2], "big", signed=True)
            for index in range(0, len(raw), 2)]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    parser.add_argument("--frames", type=int, default=64)
    parser.add_argument("--max-faces", type=int, default=512)
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
        engine.core.e9k_debug_add_breakpoint(GATE)
        submissions = []
        for _ in range(args.frames):
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                if registers["pc"] != GATE:
                    raise RuntimeError(f"unexpected breakpoint PC {registers['pc']:06X}")
                values = words(engine, WORKSPACE, 9)
                submissions.append({
                    "submission": len(submissions), "host_frame": engine.frame, "count": 3,
                    "triples": [values[index:index + 3] for index in range(0, 9, 3)],
                    "context": {name: f"${registers[name] & 0xFFFFFF:06X}" for name in ("a5", "a2", "a3", "a4")},
                })
                if len(submissions) >= args.max_faces:
                    break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if len(submissions) >= args.max_faces:
                break
        report = {"scope": "C2159E face workspaces before its C21642 depth gate", "restore": str(args.restore),
                  "submission_count": len(submissions), "submissions": submissions,
                  "qualification": "Only the C2159E three-point handler is included. Faces may still be rejected after this capture; coordinates are mutable workspaces."}
        (args.output / "pre_gate_faces.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"faces": len(submissions), "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
