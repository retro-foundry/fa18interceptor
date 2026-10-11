"""Collect live C1F6F8 control-stream entries from a sealed checkpoint."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import capstone

from engine9000_bridge import Engine, ROOT


ENTRY = 0xC1F6F8
CURRENT_STREAM = 0xC45A36


def read_words(engine: Engine, address: int, count: int) -> list[str] | None:
    try:
        raw = engine.memory(address, count * 2)
    except Exception:
        return None
    if len(raw) != count * 2:
        return None
    return [f"${int.from_bytes(raw[offset:offset + 2], 'big'):04X}"
            for offset in range(0, len(raw), 2)]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    parser.add_argument("--frames", type=int, default=12)
    parser.add_argument("--max-entries", type=int, default=64)
    parser.add_argument("--trace-stream", type=lambda value: int(value, 0),
                        help="when this C45A36 value is entered, single-step and save its trace")
    parser.add_argument("--trace-instructions", type=int, default=600)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    if args.frames < 1 or args.max_entries < 1:
        raise ValueError("frames and max-entries must be positive")
    args.output.mkdir(parents=True)
    engine = Engine(args.config.resolve(), args.output / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        engine.core.e9k_debug_add_breakpoint(ENTRY)
        entries = []
        inherited_breakpoints = []
        for _ in range(args.frames):
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                if registers["pc"] != ENTRY:
                    # A checkpoint can retain an unrelated debugger breakpoint.
                    # It is not evidence about this control-stream capture; step
                    # past it and continue to the explicitly installed entry.
                    inherited_breakpoints.append({"pc": f"${registers['pc']:06X}",
                                                  "host_frame": engine.frame})
                    engine.core.e9k_debug_step_instr()
                    engine.core.e9k_debug_resume()
                    engine.core.retro_run()
                    continue
                stream = int.from_bytes(engine.memory(CURRENT_STREAM, 4), "big")
                entries.append({"entry": len(entries), "host_frame": engine.frame,
                                "stream": f"${stream:06X}", "first_words": read_words(engine, stream, 12),
                                "caller_a5": f"${registers['a5']:06X}"})
                if args.trace_stream == stream:
                    decoder = capstone.Cs(capstone.CS_ARCH_M68K,
                                           capstone.CS_MODE_BIG_ENDIAN | capstone.CS_MODE_M68K_000)
                    trace = []
                    for index in range(args.trace_instructions):
                        regs = engine.regs()
                        pc = regs["pc"]
                        instruction = next(decoder.disasm(engine.memory(pc, 10), pc, 1), None)
                        if instruction is None:
                            raise RuntimeError(f"cannot decode {pc:06X}")
                        row = {"index": index, "pc": pc, "bytes": engine.memory(pc, instruction.size).hex(),
                               "asm": f"{instruction.mnemonic} {instruction.op_str}".strip(), "registers": regs}
                        engine.core.e9k_debug_step_instr()
                        engine.core.retro_run()
                        row["next_pc"] = engine.regs()["pc"]
                        trace.append(row)
                    (args.output / "selected_stream_trace.jsonl").write_text(
                        "".join(json.dumps(row, separators=(",", ":")) + "\n" for row in trace), encoding="utf-8")
                    break
                if len(entries) >= args.max_entries:
                    break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if len(entries) >= args.max_entries:
                break
        report = {"scope": "live C1F6F8 control-stream entries", "restore": str(args.restore),
                  "frames": args.frames, "entry_count": len(entries), "entries": entries,
                  "inherited_breakpoints_skipped": inherited_breakpoints,
                  "qualification": "A stream pointer is a renderer-control boundary, not a named model or immutable vertex source."}
        (args.output / "control_stream_entries.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"entries": len(entries), "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
