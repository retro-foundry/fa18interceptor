"""Count `$C1718E` callback entries per ordinary run075 replay frame.

The callback is stepped over only to leave its entry breakpoint. Replay input
is delivered once at each outer presentation frame, before any entries from
that frame are collected.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


ENTRY = 0xC1718E
MODE_STATE = 0xC458A0


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path, required=True)
    parser.add_argument("--start-frame", type=int, required=True,
                        help="replay frame represented by the restored state")
    parser.add_argument("--frames", type=int, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    args = parser.parse_args()
    if args.frames < 1:
        parser.error("--frames must be positive")
    if args.output.exists():
        raise FileExistsError(args.output)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), args.output.parent / "saves")
    try:
        engine.core.retro_run()
        payload = args.restore.read_bytes()
        if not engine.core.retro_unserialize(payload, len(payload)):
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
                registers = engine.regs()
                if registers["pc"] != ENTRY:
                    raise RuntimeError(f"unexpected breakpoint ${registers['pc']:06X}")
                mode = engine.memory(MODE_STATE, 5)
                entries.append({
                    "mode_current": mode[0],
                    "mode_target": mode[1],
                    "mode_countdown": mode[3],
                    "mode_state": mode[4],
                    "cycle_count": engine.core.e9k_debug_read_cycle_count(),
                })
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            frames.append({"frame": frame, "entry_count": len(entries), "entries": entries})
        report = {
            "authority": {
                "restore": str(args.restore),
                "playback": str(args.playback),
                "entry": f"${ENTRY:06X}",
                "mode_state_address": f"${MODE_STATE:06X}",
                "start_frame": args.start_frame,
                "frames": args.frames,
            },
            "frames": frames,
            "qualification": (
                "Each entry is a live `$C1718E` breakpoint. The one instruction "
                "step only escapes that breakpoint; it is not an instruction-faithful "
                "trace of the rest of the callback."
            ),
        }
        args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"frames": len(frames),
                          "entries": sum(row["entry_count"] for row in frames),
                          "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
