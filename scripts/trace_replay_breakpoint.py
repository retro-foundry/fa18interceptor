"""Trace forward from a breakpoint reached during an event-faithful replay."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import capstone

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path, required=True)
    parser.add_argument("--address", type=lambda value: int(value, 0), required=True)
    parser.add_argument("--frames", type=int, required=True)
    parser.add_argument("--arm-frame", type=int, default=1)
    parser.add_argument("--instructions", type=int, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    args = parser.parse_args()
    if args.output.exists() or args.instructions < 1:
        raise ValueError("output must not exist and instructions must be positive")
    events = read_events(args.playback)
    args.output.mkdir(parents=True)
    engine = Engine(args.config.resolve(), args.output / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        decoder = capstone.Cs(capstone.CS_ARCH_M68K,
                               capstone.CS_MODE_BIG_ENDIAN | capstone.CS_MODE_M68K_000)
        hit = None
        for frame in range(1, args.frames + 1):
            if frame == args.arm_frame:
                engine.core.e9k_debug_add_breakpoint(args.address)
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            if engine.core.e9k_debug_is_paused():
                if engine.regs()["pc"] != args.address:
                    raise RuntimeError(f"unexpected breakpoint ${engine.regs()['pc']:06X}")
                hit = {"frame": frame, "registers": engine.regs()}
                break
        if hit is None:
            raise RuntimeError("breakpoint not reached")
        (args.output / "slow.bin").write_bytes(engine.memory(0xC00000, 0x80000))
        rows = []
        for index in range(args.instructions):
            registers = engine.regs()
            pc = registers["pc"]
            instruction = next(decoder.disasm(engine.memory(pc, 10), pc, 1), None)
            if instruction is None:
                raise RuntimeError(f"cannot decode ${pc:06X}")
            row = {"index": index, "frame": engine.frame, "pc": pc,
                   "bytes": engine.memory(pc, instruction.size).hex(),
                   "asm": f"{instruction.mnemonic} {instruction.op_str}".strip(),
                   "registers": registers}
            engine.core.e9k_debug_step_instr()
            engine.core.retro_run()
            row["next_pc"] = engine.regs()["pc"]
            rows.append(row)
        (args.output / "trace.jsonl").write_text(
            "".join(json.dumps(row, separators=(",", ":")) + "\n" for row in rows), encoding="utf-8")
        (args.output / "report.json").write_text(json.dumps({
            "scope": "event-faithful replay to breakpoint then bounded instruction trace",
            "breakpoint": f"${args.address:06X}", "hit": hit,
            "instructions": len(rows),
            "qualification": "Replay input is applied only during ordinary full-frame execution before the breakpoint. The bounded post-breakpoint trace is valid only when no later input event is required."
        }, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"hit_frame": hit["frame"], "instructions": len(rows)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
