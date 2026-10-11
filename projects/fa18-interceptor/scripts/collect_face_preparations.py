"""Collect C2005C variable-length faces before orientation, clipping, or culling.

C2005C reads consecutive C48390 offsets until a signed terminal offset.  The
entry capture retains faces that never reach C2FF48. The selected triples are
transformed workspace coordinates, not immutable source-model vertices.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


PREPARE = 0xC2005C
VERTICES = 0xC48390


def signed_word(engine: Engine, address: int) -> int:
    return int.from_bytes(engine.memory(address, 2), "big", signed=True)


def triple(engine: Engine, offset: int) -> list[int]:
    return [signed_word(engine, VERTICES + offset + axis * 2) for axis in range(3)]


def longword(engine: Engine, address: int) -> int:
    return int.from_bytes(engine.memory(address, 4), "big", signed=False)


def face(engine: Engine, address: int) -> tuple[list[int], list[list[int]]]:
    offsets = []
    for index in range(16):
        offset = signed_word(engine, address + index * 2)
        terminal = offset < 0
        offsets.append(offset & 0x7FFF)
        if terminal:
            if len(offsets) < 3:
                raise RuntimeError(f"C2005C face at ${address:06X} has fewer than three vertices")
            return offsets, [triple(engine, value) for value in offsets]
    raise RuntimeError(f"C2005C face at ${address:06X} exceeds 16 vertices")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path,
                        help="optional input recording delivered before collection")
    parser.add_argument("--arm-frame", type=int, default=1,
                        help="install the face breakpoint immediately before this replay frame")
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    parser.add_argument("--frames", type=int, default=2)
    parser.add_argument("--max-faces", type=int, default=256)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    args.output.mkdir(parents=True)
    events = read_events(args.playback) if args.playback else {}
    engine = Engine(args.config.resolve(), args.output / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        submissions = []
        for frame in range(1, args.frames + 1):
            if frame == args.arm_frame:
                engine.core.e9k_debug_add_breakpoint(PREPARE)
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                if registers["pc"] != PREPARE:
                    raise RuntimeError(f"unexpected breakpoint PC {registers['pc']:06X}")
                a2 = registers["a2"] & 0xFFFFFF
                offsets, triples = face(engine, a2)
                stack_pointer = registers["a7"] & 0xFFFFFF
                submissions.append({
                    "submission": len(submissions), "host_frame": engine.frame,
                    "count": len(triples), "offsets": offsets, "triples": triples,
                    "context": {name: f"${registers[name] & 0xFFFFFF:06X}" for name in ("a5", "a2", "a3", "a4")},
                    "return_pc": f"${longword(engine, stack_pointer) & 0xFFFFFF:06X}",
                })
                if len(submissions) >= args.max_faces:
                    break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if len(submissions) >= args.max_faces:
                break
        report = {
            "scope": "C2005C selected faces before orientation/clipping", "restore": str(args.restore),
            "playback": str(args.playback) if args.playback else None, "arm_frame": args.arm_frame,
            "frames": args.frames, "submission_count": len(submissions), "submissions": submissions,
            "qualification": "These are face records observed before back-face and clip rejection. Triples come from mutable C48390 and establish renderer topology, not immutable source coordinates.",
        }
        (args.output / "face_preparations.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"faces": len(submissions), "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
