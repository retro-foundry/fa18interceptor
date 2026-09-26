"""Replay to a breakpoint and identify stepped CPU writes to one RAM window."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
from collections import deque

import capstone

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


def parse_int(value: str) -> int:
    return int(value, 0)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path,
                        help="replay input used while seeking a breakpoint")
    parser.add_argument("--breakpoint", type=parse_int,
                        help="PC at which to pause before stepping")
    parser.add_argument("--arm-frame", type=int,
                        help="replay frame on which to arm --breakpoint")
    parser.add_argument("--start-immediately", action="store_true",
                        help="step directly from --restore instead of replaying to a breakpoint")
    parser.add_argument("--frames", type=int, required=True)
    parser.add_argument("--watch-address", type=parse_int, required=True)
    parser.add_argument("--watch-size", type=parse_int, required=True)
    parser.add_argument("--max-instructions", type=int, required=True)
    parser.add_argument("--context-instructions", type=int, default=0,
                        help="attach this many preceding stepped instructions to each write")
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=False)
    if args.start_immediately:
        if args.breakpoint is not None or args.arm_frame is not None or args.playback is not None:
            parser.error("--start-immediately cannot be combined with replay-seek arguments")
    elif args.playback is None or args.breakpoint is None or args.arm_frame is None:
        parser.error("supply --start-immediately, or all of --playback, --breakpoint, and --arm-frame")
    events = read_events(args.playback) if args.playback else {}
    decoder = capstone.Cs(capstone.CS_ARCH_M68K,
                          capstone.CS_MODE_BIG_ENDIAN | capstone.CS_MODE_M68K_000)
    engine = Engine(args.config.resolve(), args.output / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        hit_frame = None
        if args.start_immediately:
            hit_frame = 0
        else:
            for frame in range(1, args.frames + 1):
                if frame == args.arm_frame:
                    engine.core.e9k_debug_add_breakpoint(args.breakpoint)
                for kind, values in events.get(frame, []):
                    engine.event(kind, values)
                engine.core.retro_run()
                if engine.core.e9k_debug_is_paused():
                    if engine.regs()["pc"] != args.breakpoint:
                        raise RuntimeError(f"unexpected breakpoint {engine.regs()['pc']:06x}")
                    hit_frame = frame
                    break
            if hit_frame is None:
                raise RuntimeError("breakpoint not reached")
        before = engine.memory(args.watch_address, args.watch_size)
        writes = []
        context = deque(maxlen=args.context_instructions)
        for index in range(args.max_instructions):
            registers = engine.regs()
            pc = registers["pc"]
            raw = engine.memory(pc, 10)
            instruction = next(decoder.disasm(raw, pc, 1), None)
            if instruction is None:
                raise RuntimeError(f"68000 decode failed at {pc:06x}")
            row = {"index": index, "pc": f"${pc:06X}",
                   "bytes": raw[:instruction.size].hex(),
                   "asm": f"{instruction.mnemonic} {instruction.op_str}".strip()}
            engine.core.e9k_debug_step_instr()
            engine.core.retro_run()
            after = engine.memory(args.watch_address, args.watch_size)
            if after != before:
                writes.append({**row,
                               "before": before.hex(), "after": after.hex(),
                               "registers": registers,
                               "preceding_instructions": list(context)})
                before = after
            context.append(row)
        report = {"breakpoint": f"${args.breakpoint:06X}" if args.breakpoint is not None else None,
                  "start_immediately": args.start_immediately, "hit_frame": hit_frame,
                  "watch_address": f"${args.watch_address:06X}", "watch_size": args.watch_size,
                  "instructions": args.max_instructions, "writes": writes,
                  "limitation": "Future replay input is not delivered while instruction stepping."}
        (args.output / "memory_writes.json").write_text(json.dumps(report, indent=2) + "\n")
        print(json.dumps({"hit_frame": hit_frame, "writes": len(writes),
                          "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
