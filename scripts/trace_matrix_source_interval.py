"""Trace one `$C1F4AC` source-bounded matrix/renderer interval without input."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import capstone

from engine9000_bridge import Engine, ROOT


ENTRY = 0xC1F4AC


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--source", type=lambda value: int(value, 0), required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    parser.add_argument("--frames", type=int, default=16,
                        help="normal no-input frames in which to find the source")
    parser.add_argument("--max-instructions", type=int, default=120000)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    args.output.mkdir(parents=True)
    decoder = capstone.Cs(capstone.CS_ARCH_M68K,
                          capstone.CS_MODE_BIG_ENDIAN | capstone.CS_MODE_M68K_000)
    engine = Engine(args.config.resolve(), args.output / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        engine.core.e9k_debug_add_breakpoint(ENTRY)
        found = None
        for _ in range(args.frames):
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                if registers["pc"] != ENTRY:
                    raise RuntimeError(f"unexpected breakpoint {registers['pc']:06X}")
                if (registers["a1"] & 0xFFFFFF) == args.source:
                    found = registers
                    break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if found:
                break
        if found is None:
            raise RuntimeError(f"source ${args.source:06X} not reached")
        rows = []
        termination = "max_instructions"
        for index in range(args.max_instructions):
            registers = engine.regs()
            pc = registers["pc"]
            if index and pc == ENTRY:
                termination = "next_matrix_source"
                break
            raw = engine.memory(pc, 10)
            instruction = next(decoder.disasm(raw, pc, 1), None)
            if instruction is None:
                raise RuntimeError(f"68000 decode failed at ${pc:06X}")
            rows.append({"index": index, "pc": f"${pc:06X}",
                         "bytes": raw[:instruction.size].hex(),
                         "asm": f"{instruction.mnemonic} {instruction.op_str}".strip(),
                         "registers": registers})
            engine.core.e9k_debug_step_instr()
            engine.core.retro_run()
        (args.output / "trace.jsonl").write_text(
            "".join(json.dumps(row, separators=(",", ":")) + "\n" for row in rows),
            encoding="utf-8")
        report = {"source": f"${args.source:06X}", "entry": f"${ENTRY:06X}",
                  "hit_frame": engine.frame, "initial_registers": found,
                  "instructions": len(rows), "termination": termination,
                  "limitation": "No replay input is delivered after the restored checkpoint."}
        (args.output / "summary.json").write_text(json.dumps(report, indent=2) + "\n")
        if termination != "next_matrix_source":
            raise RuntimeError("instruction cap before next matrix source")
        print(json.dumps(report))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
