"""Inventory live matrix-transform inputs without treating output workspaces as sources."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


def pointer(value: int) -> str:
    return f"${value & 0xFFFFFF:06X}"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path,
                        help="optional replay input; omit for sealed no-input checkpoints")
    parser.add_argument("--entry", type=lambda value: int(value, 0), default=0xC1F100)
    parser.add_argument("--frames", type=int, required=True)
    parser.add_argument("--max-entries", type=int, default=8192)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    args = parser.parse_args()
    if args.frames < 1 or args.max_entries < 1:
        parser.error("--frames and --max-entries must be positive")
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
        engine.core.e9k_debug_add_breakpoint(args.entry)
        entries = []
        inherited_breakpoints = []
        for frame in range(1, args.frames + 1):
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                if registers["pc"] != args.entry:
                    inherited_breakpoints.append({"pc": pointer(registers["pc"]),
                                                  "host_frame": engine.frame})
                else:
                    entries.append({
                        "entry": len(entries),
                        "host_frame": engine.frame,
                        "a1": pointer(registers["a1"]),
                        "a3": pointer(registers["a3"]),
                        "a5": pointer(registers["a5"]),
                        "d0": f"${registers['d0'] & 0xFFFFFFFF:08X}",
                    })
                    if len(entries) >= args.max_entries:
                        break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if len(entries) >= args.max_entries:
                break
        summary = {
            "scope": "live matrix-transform entry registers",
            "restore": str(args.restore),
            "playback": str(args.playback) if args.playback else None,
            "entry": pointer(args.entry),
            "frames_requested": args.frames,
            "entry_count": len(entries),
            "entries": entries,
            "inherited_breakpoints_skipped": inherited_breakpoints,
            "qualification": (
                "A1 is an input pointer only at the recorded matrix-entry instruction. "
                "A3 is a destination/workspace pointer and must not be promoted to source data."
            ),
        }
        (args.output / "matrix_transform_entries.json").write_text(
            json.dumps(summary, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"entries": len(entries), "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
