"""Sample the `$C1718E` viewport-mode state during ordinary replay frames."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events

MODE_STATE = 0xC458A0
OUTER_INDEX = 0xC4566C
COPPER_COLOURS = 0x0577B6


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
    args.output.parent.mkdir(parents=True, exist_ok=True)
    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), ROOT / "local" / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        samples = []
        for frame in range(args.start_frame + 1, args.start_frame + args.frames + 1):
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            mode = engine.memory(MODE_STATE, 5)
            colours = engine.memory(COPPER_COLOURS, 32)
            samples.append({
                "frame": frame,
                "mode_current": mode[0],
                "mode_target": mode[1],
                "mode_countdown": mode[3],
                "mode_state": mode[4],
                "outer_selected_index": int.from_bytes(engine.memory(OUTER_INDEX, 2), "big"),
                "copper_colour_00_to_07": [
                    int.from_bytes(colours[offset:offset + 2], "big")
                    for offset in range(0, len(colours), 4)
                ],
            })
        changed = []
        prior = None
        for row in samples:
            state = {key: value for key, value in row.items() if key != "frame"}
            if state != prior:
                changed.append(row)
                prior = state
        report = {
            "authority": {
                "restore": str(args.restore),
                "playback": str(args.playback),
                "start_frame": args.start_frame,
                "frames": args.frames,
                "mode_state_address": f"${MODE_STATE:06X}",
                "outer_index_address": f"${OUTER_INDEX:06X}",
                "copper_colour_value_address": f"${COPPER_COLOURS:06X}",
            },
            "samples": samples,
            "state_changes": changed,
        }
        args.output.write_text(json.dumps(report, indent=2) + "\n")
        print(json.dumps({"samples": len(samples), "state_changes": len(changed),
                          "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
