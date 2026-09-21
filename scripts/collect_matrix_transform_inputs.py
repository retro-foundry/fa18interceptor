"""Record static input triples consumed by the observed C1F4AC matrix transform."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


UPDATE_ENTRY = 0xC0F090
TRANSFORM_ENTRIES = {
    0xC1F100: ("record_vector", "a0"),
    0xC1F21C: ("next_record_vector", "a0"),
    0xC1F4AC: ("alternate_record", "a3"),
    0xC1F524: ("alternate_loop", "a3"),
}


def signed_words(engine: Engine, address: int, count: int = 3) -> list[int]:
    raw = engine.memory(address, count * 2)
    return [int.from_bytes(raw[index:index + 2], "big", signed=True)
            for index in range(0, count * 2, 2)]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    parser.add_argument("--arm-frame", type=int, default=1)
    parser.add_argument("--frames", type=int, default=8)
    parser.add_argument("--max-instructions", type=int, default=30000)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)

    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), args.output.parent / "matrix_transform_saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        for frame in range(1, args.frames + 1):
            if frame == args.arm_frame:
                engine.core.e9k_debug_add_breakpoint(UPDATE_ENTRY)
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            if engine.core.e9k_debug_is_paused():
                break
        if not engine.core.e9k_debug_is_paused() or engine.regs()["pc"] != UPDATE_ENTRY:
            raise RuntimeError("flight-update entry was not reached")
        if any(frame > engine.frame for frame in events):
            raise ValueError("future input after breakpoint")

        records = []
        for instruction in range(args.max_instructions):
            registers = engine.regs()
            if registers["pc"] in TRANSFORM_ENTRIES:
                source = registers["a1"] & 0xFFFFFF
                entry_kind, destination_register = TRANSFORM_ENTRIES[registers["pc"]]
                destination = registers[destination_register] & 0xFFFFFF
                records.append({"instruction": instruction,
                                "entry": f"${registers['pc']:06X}",
                                "entry_kind": entry_kind,
                                "source": f"${source:06X}",
                                "source_triple": signed_words(engine, source),
                                "destination": f"${destination:06X}",
                                "destination_register": destination_register,
                                "matrix": f"${registers['a4'] & 0xFFFFFF:06X}"})
            engine.core.e9k_debug_step_instr()
            engine.core.retro_run()
        report = {"scope": "C1F4AC static-input matrix transforms during one flight-update entry",
                  "breakpoint": f"${UPDATE_ENTRY:06X}", "hit_frame": engine.frame,
                  "max_instructions": args.max_instructions, "transform_count": len(records),
                  "records": records,
                  "qualification": "Each row proves an input triple and transformed destination. It does not by itself prove a complete model boundary or face topology."}
        args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"transforms": len(records), "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
