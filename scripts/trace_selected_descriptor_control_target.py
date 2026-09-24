"""Trace from a selected descriptor +8 store at C1CC70 without later replay input."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import capstone

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


ENTRY = 0xC1CC70


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path,
                        help="optional replay input before the selected target; omit for a sealed checkpoint")
    parser.add_argument("--config", type=Path, default=ROOT / "local/fa18.uae")
    parser.add_argument("--target", type=lambda value: int(value, 0), required=True,
                        help="descriptor +8 field address read by C1CC70")
    parser.add_argument("--arm-frame", type=int, required=True)
    parser.add_argument("--frames", type=int, required=True)
    parser.add_argument("--instructions", type=int, default=12000)
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
        engine.core.e9k_debug_add_breakpoint(ENTRY)
        found = None
        for frame in range(1, args.frames + 1):
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                regs = engine.regs()
                if regs["pc"] != ENTRY:
                    raise RuntimeError(f"unexpected breakpoint ${regs['pc']:06X}")
                field_address = regs["a1"] & 0xFFFFFF
                field_value = int.from_bytes(engine.memory(field_address, 4), "big") & 0xFFFFFF
                if field_value == args.target:
                    found = {"host_frame": engine.frame, "registers": regs}
                    break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if found:
                break
        if found is None:
            raise RuntimeError(f"target ${args.target:06X} not reached")
        decoder = capstone.Cs(capstone.CS_ARCH_M68K, capstone.CS_MODE_BIG_ENDIAN | capstone.CS_MODE_M68K_000)
        rows = []
        for index in range(args.instructions):
            regs = engine.regs()
            pc = regs["pc"]
            instruction = next(decoder.disasm(engine.memory(pc, 10), pc, 1), None)
            if instruction is None:
                raise RuntimeError(f"cannot decode ${pc:06X}")
            row = {"index": index, "pc": pc, "bytes": engine.memory(pc, instruction.size).hex(),
                   "asm": f"{instruction.mnemonic} {instruction.op_str}".strip(), "registers": regs}
            engine.core.e9k_debug_step_instr()
            engine.core.retro_run()
            row["next_pc"] = engine.regs()["pc"]
            rows.append(row)
        (args.output / "trace.jsonl").write_text(
            "".join(json.dumps(row, separators=(",", ":")) + "\n" for row in rows), encoding="utf-8")
        summary = {"scope": "no-future-input trace from selected descriptor +8 C1CC70 store",
                   "target": f"${args.target:06X}", "entry": f"${ENTRY:06X}", "found": found,
                   "instructions": len(rows), "termination": "instruction_cap",
                   "future_input_omitted": any(frame > found["host_frame"] for frame in events)}
        (args.output / "trace_summary.json").write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"target": summary["target"], "frame": found["host_frame"], "instructions": len(rows)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
