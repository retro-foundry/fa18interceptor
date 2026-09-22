"""Collect descriptor-resolved C1F99A transform lanes during normal replay."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


ENTRY = 0xC1F99A
DESCRIPTOR_PTR = 0xC45A32


def read_long(engine: Engine, address: int) -> int:
    return int.from_bytes(engine.memory(address, 4), "big") & 0xFFFFFF


def signed_word(value: int) -> int:
    value &= 0xFFFF
    return value - 0x10000 if value >= 0x8000 else value


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path, required=True)
    parser.add_argument("--arm-frame", type=int, required=True)
    parser.add_argument("--frames", type=int, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)

    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), args.output.parent / "c1f99a_entry_saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        records = []
        for frame in range(1, args.frames + 1):
            if frame == args.arm_frame:
                engine.core.e9k_debug_add_breakpoint(ENTRY)
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                if registers["pc"] != ENTRY:
                    raise RuntimeError(f"unexpected breakpoint ${registers['pc']:06X}")
                offset = signed_word(registers["d7"])
                descriptor = read_long(engine, DESCRIPTOR_PTR)
                source = (descriptor + 10 + offset) & 0xFFFFFF
                destination = (registers["a3"] + offset) & 0xFFFFFF
                records.append({
                    "host_frame": engine.frame,
                    "descriptor": f"${descriptor:06X}",
                    "descriptor_source": f"${source:06X}",
                    "source_offset": offset,
                    "workspace_base": f"${registers['a3'] & 0xFFFFFF:06X}",
                    "predicted_destination": f"${destination:06X}",
                    "seed_a1": f"${registers['a1'] & 0xFFFFFF:06X}",
                    "matrix": f"${registers['a4'] & 0xFFFFFF:06X}",
                })
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
        report = {"scope": "C1F99A descriptor-resolved transform entries",
                  "entry": f"${ENTRY:06X}", "arm_frame": args.arm_frame,
                  "records": records,
                  "qualification": "The source and destination follow the setup arithmetic at C1F99A-C1F9ED. A row proves a transform lane, not its later face ownership."}
        args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"records": len(records), "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
