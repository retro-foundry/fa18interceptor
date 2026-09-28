"""Count live `$C0EFD4` parent-update entries per ordinary replay frame."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


ENTRY = 0xC0EFD4
UPDATE_GATE = 0xC45795
NOTIFICATION_COUNTDOWN = 0xC45890


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path, required=True)
    parser.add_argument("--start-frame", type=int, required=True)
    parser.add_argument("--frames", type=int, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    args = parser.parse_args()
    if args.frames < 1 or args.output.exists():
        raise ValueError("frames must be positive and output must not exist")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), args.output.parent / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        engine.core.e9k_debug_remove_breakpoint(ENTRY)
        engine.core.e9k_debug_add_breakpoint(ENTRY)
        frames = []
        for frame in range(args.start_frame + 1, args.start_frame + args.frames + 1):
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            entries = []
            while engine.core.e9k_debug_is_paused():
                if engine.regs()["pc"] != ENTRY:
                    raise RuntimeError("unexpected breakpoint")
                entries.append({
                    "update_gate": engine.memory(UPDATE_GATE, 1)[0],
                    "notification_countdown": engine.memory(NOTIFICATION_COUNTDOWN, 1)[0],
                    "cycle_count": engine.core.e9k_debug_read_cycle_count(),
                })
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            frames.append({"frame": frame, "entry_count": len(entries), "entries": entries})
        args.output.write_text(json.dumps({
            "authority": {"restore": str(args.restore), "playback": str(args.playback),
                          "entry": "$C0EFD4", "start_frame": args.start_frame,
                          "frames": args.frames},
            "frames": frames,
            "qualification": "Entries are live breakpoints; one instruction is stepped to escape each entry."
        }, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"frames": len(frames),
                          "entries": sum(row["entry_count"] for row in frames),
                          "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
