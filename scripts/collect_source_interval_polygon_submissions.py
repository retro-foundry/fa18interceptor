"""Collect polygon workspaces while one matrix-source interval is stepped.

The interval starts at `$C1F4AC` for a requested immutable A1 source and
ends before the next `$C1F4AC` entry.  It does not infer an object identity:
the collected C4B990/C4B390 records are mutable renderer workspaces.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT


ENTRY = 0xC1F4AC
SUBMIT = 0xC2FF48
TRIPLES = 0xC4B990
PAIRS = 0xC4B390


def signed_words(engine: Engine, address: int, count: int) -> list[int]:
    raw = engine.memory(address, count * 2)
    return [int.from_bytes(raw[index:index + 2], "big", signed=True)
            for index in range(0, len(raw), 2)]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--source", type=lambda value: int(value, 0), required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    parser.add_argument("--frames", type=int, default=16)
    parser.add_argument("--max-instructions", type=int, default=120000)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    if args.frames < 1 or args.max_instructions < 1:
        raise ValueError("frames and max-instructions must be positive")
    args.output.mkdir(parents=True)

    engine = Engine(args.config.resolve(), args.output / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        engine.core.e9k_debug_add_breakpoint(ENTRY)
        initial = None
        for _ in range(args.frames):
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                if registers["pc"] != ENTRY:
                    raise RuntimeError(f"unexpected breakpoint ${registers['pc']:06X}")
                if (registers["a1"] & 0xFFFFFF) == args.source:
                    initial = registers
                    break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if initial is not None:
                break
        if initial is None:
            raise RuntimeError(f"source ${args.source:06X} not reached")

        submissions = []
        termination = "max_instructions"
        for index in range(args.max_instructions):
            registers = engine.regs()
            if index and registers["pc"] == ENTRY:
                termination = "next_matrix_source"
                break
            if registers["pc"] == SUBMIT:
                count = int.from_bytes(engine.memory(PAIRS, 2), "big")
                if not 3 <= count <= 16:
                    raise RuntimeError(f"invalid polygon count {count} at ${SUBMIT:06X}")
                triples = signed_words(engine, TRIPLES, count * 3)
                pairs = signed_words(engine, PAIRS + 2, count * 2)
                submissions.append({
                    "instruction": index,
                    "host_frame": engine.frame,
                    "count": count,
                    "triples": [triples[offset:offset + 3]
                                for offset in range(0, len(triples), 3)],
                    "stored_screen_pairs": [pairs[offset:offset + 2]
                                            for offset in range(0, len(pairs), 2)],
                    "context": {name: f"${registers[name]:06X}"
                                for name in ("a5", "a2", "a3", "a4")},
                })
            engine.core.e9k_debug_step_instr()
            engine.core.retro_run()
        if termination != "next_matrix_source":
            raise RuntimeError("instruction cap before next matrix source")
        report = {
            "scope": "C2FF48 workspaces within one C1F4AC source interval",
            "source": f"${args.source:06X}",
            "initial_registers": initial,
            "instructions": index,
            "termination": termination,
            "submissions": submissions,
            "qualification": "C4B990/C4B390 are mutable finalized workspaces; source-interval membership does not by itself prove semantic object identity.",
        }
        (args.output / "polygon_submissions.json").write_text(
            json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"source": report["source"], "submissions": len(submissions),
                          "instructions": report["instructions"]}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
