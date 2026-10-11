"""Capture exact pre-submit state for each run003 M-map C304F4 area blit.

Unlike the Custom-write inventory, this probe pauses immediately before the
BLTSIZE write.  It records the live CPU registers, Custom blitter register
image, and the complete Chip-RAM snapshot.  The latter is required because the
A/C inputs are mutable renderer scratch, not static map assets.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


ENTRY = 0xC304F4
CUSTOM = 0xDFF000
CHIP_SIZE = 512 * 1024


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, default=ROOT / "build/run003_pre_m_2183/state.bin")
    parser.add_argument("--playback", type=Path, default=ROOT / "build/run003_m_press_only.e9k")
    parser.add_argument("--config", type=Path, default=ROOT / "local/fa18.uae")
    parser.add_argument("--frames", type=int, default=13)
    parser.add_argument("--max-jobs", type=int, default=128)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    args.output.mkdir(parents=True)
    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), args.output / "saves")
    jobs = []
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        engine.core.e9k_debug_add_breakpoint(ENTRY)
        for frame in range(1, args.frames + 1):
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                if registers["pc"] != ENTRY:
                    raise RuntimeError(f"unexpected breakpoint PC ${registers['pc']:06X}")
                sequence = len(jobs)
                (args.output / f"chip_before_{sequence:03d}.bin").write_bytes(engine.memory(0, CHIP_SIZE))
                jobs.append({
                    "submission": sequence, "host_frame": engine.frame,
                    "registers": {name: f"${registers[name] & 0xFFFFFFFF:08X}"
                                  for name in ("d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "a0", "a1", "a2")},
                    "custom_40_67": engine.memory(CUSTOM + 0x40, 0x28).hex(),
                })
                if len(jobs) >= args.max_jobs:
                    break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if len(jobs) >= args.max_jobs:
                break
        (args.output / "span_inputs.json").write_text(json.dumps({
            "scope": "pre-BLTSIZE C304F4 state in run003 M-map replay",
            "submissions": jobs,
        }, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"jobs": len(jobs), "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
