"""Find one C1F100 transform source and retain its following no-input trace."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import capstone

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


DEFAULT_TRANSFORM_ENTRY = 0xC1F100


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    parser.add_argument("--source", type=lambda value: int(value, 0), required=True)
    parser.add_argument("--occurrence", type=int, default=1,
                        help="select this 1-based occurrence of --source (default: 1)")
    parser.add_argument("--entry", type=lambda value: int(value, 0), default=DEFAULT_TRANSFORM_ENTRY,
                        help="matrix-transform entry to search (default: C1F100)")
    parser.add_argument("--frames", type=int, default=12)
    parser.add_argument("--instructions", type=int, default=12000)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.occurrence < 1:
        parser.error("--occurrence must be positive")
    if args.output.exists():
        raise FileExistsError(args.output)
    args.output.mkdir(parents=True)
    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), args.output / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        engine.core.e9k_debug_add_breakpoint(args.entry)
        found = None
        matching_occurrences = 0
        for _ in range(args.frames):
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                if registers["pc"] != args.entry:
                    engine.core.e9k_debug_step_instr()
                    engine.core.e9k_debug_resume()
                    engine.core.retro_run()
                    continue
                if (registers["a1"] & 0xFFFFFF) == args.source:
                    matching_occurrences += 1
                    if matching_occurrences == args.occurrence:
                        found = {"host_frame": engine.frame, "registers": registers,
                                 "occurrence": matching_occurrences}
                        break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if found:
                break
        if found is None:
            raise RuntimeError(f"selected matrix source was not reached {args.occurrence} times")
        if any(frame > found["host_frame"] for frame in events):
            raise ValueError("future input after selected source")

        decoder = capstone.Cs(capstone.CS_ARCH_M68K,
                               capstone.CS_MODE_BIG_ENDIAN | capstone.CS_MODE_M68K_000)
        rows = []
        for index in range(args.instructions):
            registers = engine.regs()
            pc = registers["pc"]
            instruction = next(decoder.disasm(engine.memory(pc, 10), pc, 1), None)
            if instruction is None:
                raise RuntimeError(f"cannot decode {pc:06X}")
            row = {"index": index, "pc": pc,
                   "bytes": engine.memory(pc, instruction.size).hex(),
                   "asm": f"{instruction.mnemonic} {instruction.op_str}".strip(),
                   "registers": registers}
            engine.core.e9k_debug_step_instr()
            engine.core.retro_run()
            row["next_pc"] = engine.regs()["pc"]
            rows.append(row)
        (args.output / "trace.jsonl").write_text(
            "".join(json.dumps(row, separators=(",", ":")) + "\n" for row in rows), encoding="utf-8")
        (args.output / "trace_summary.json").write_text(json.dumps({
            "scope": "no-future-input trace beginning at selected C1F100 source",
            "source": f"${args.source:06X}", "found": found,
            "requested_occurrence": args.occurrence,
            "instructions": len(rows), "termination": "instruction_cap"}, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"source": f"${args.source:06X}", "instructions": len(rows), "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
