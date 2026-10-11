"""Recover C203 face-local vertex offsets for one transformed source block."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT


VERTEX_READ = 0xC2035A
PRECLIP_CALL = 0xC203BA


def word(engine: Engine, address: int) -> int:
    return int.from_bytes(engine.memory(address, 2), "big", signed=True)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    parser.add_argument("--source-block", type=lambda value: int(value, 0), required=True,
                        help="transformed input block matched in A4, e.g. C46228")
    parser.add_argument("--frames", type=int, default=16)
    parser.add_argument("--max-faces", type=int, default=128)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)

    engine = Engine(args.config.resolve(), args.output.parent / "c203_face_index_saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        engine.core.e9k_debug_add_breakpoint(VERTEX_READ)
        engine.core.e9k_debug_add_breakpoint(PRECLIP_CALL)
        pending: list[int] = []
        faces = []
        for _ in range(args.frames):
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                pc = registers["pc"]
                a4 = registers["a4"] & 0xFFFFFF
                if pc == VERTEX_READ and a4 == args.source_block:
                    pending.append(word(engine, registers["a2"] & 0xFFFFFF))
                elif pc == PRECLIP_CALL and a4 == args.source_block and pending:
                    faces.append({
                        "host_frame": engine.frame,
                        "face_record": f"${registers['a2'] & 0xFFFFFF:06X}",
                        "source_offsets": pending,
                        "source_vertex_indices": [offset // 6 for offset in pending],
                    })
                    pending = []
                    if len(faces) >= args.max_faces:
                        break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if len(faces) >= args.max_faces:
                break
        report = {
            "scope": "C2035A source-offset reads grouped at C203BA pre-clip calls",
            "restore": str(args.restore), "source_block": f"${args.source_block:06X}",
            "frames": args.frames, "face_count": len(faces), "faces": faces,
            "qualification": "Offsets are directly consumed against the selected transformed input block. Dividing by six identifies its three-word records; it does not assert immutable coordinates without a separately traced source transform.",
        }
        args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"faces": len(faces), "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
