"""Collect renderer-observed C212B0 line lists from a sealed checkpoint.

Each captured list is decoded exactly as C212B0 consumes it: a selector word,
then (first_offset, second_offset) pairs whose final second offset is signed.
The endpoint triples are copied from the current C48390 workspace.  They are
therefore renderer-observed geometry, not asserted immutable source vertices.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT


SUBMIT = 0xC212B0
VERTICES = 0xC48390


def signed_word(engine: Engine, address: int) -> int:
    return int.from_bytes(engine.memory(address, 2), "big", signed=True)


def triple(engine: Engine, offset: int) -> list[int]:
    address = VERTICES + offset
    return [signed_word(engine, address + axis * 2) for axis in range(3)]


def decode_list(engine: Engine, address: int) -> tuple[int, list[dict[str, object]]]:
    selector = signed_word(engine, address)
    cursor = address + 2
    segments = []
    for _ in range(128):
        first = signed_word(engine, cursor)
        second = signed_word(engine, cursor + 2)
        cursor += 4
        terminal = second < 0
        second &= 0x7FFF
        segments.append({"offsets": [first, second],
                         "triples": [triple(engine, first), triple(engine, second)]})
        if terminal:
            return selector, segments
    raise RuntimeError(f"unterminated C212B0 list at ${address:06X}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    parser.add_argument("--frames", type=int, default=2)
    parser.add_argument("--max-submissions", type=int, default=128)
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
                registers = engine.regs()
                if registers["pc"] != SUBMIT:
                    raise RuntimeError(f"unexpected breakpoint PC {registers['pc']:06X}")
                a2 = registers["a2"] & 0xFFFFFF
                selector, segments = decode_list(engine, a2)
                submissions.append({
                    "submission": len(submissions), "host_frame": engine.frame,
                    "selector": selector, "segments": segments,
                    "context": {name: f"${registers[name] & 0xFFFFFF:06X}" for name in ("a5", "a2", "a3", "a4")},
                })
                if len(submissions) >= args.max_submissions:
                    break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if len(submissions) >= args.max_submissions:
                break
        report = {
            "scope": "C212B0 offset-pair line submissions", "restore": str(args.restore),
            "frames": args.frames, "submission_count": len(submissions), "submissions": submissions,
            "qualification": "Endpoints come from mutable C48390 at each observed C212B0 entry; source-model ownership requires upstream correlation.",
        }
        (args.output / "line_submissions.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"submissions": len(submissions), "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
