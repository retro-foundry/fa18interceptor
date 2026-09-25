"""Test whether one documented M-map request-flag byte gates a raw M command.

This is a diagnostic mutation probe.  It never changes a sealed checkpoint:
the value is written only to the deserialized emulator instance and every
result records the write alongside its final screenshot and state.
"""
from __future__ import annotations

import argparse
import ctypes as C
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT, sha
from profile_window import read_events


REQUEST_FLAGS = 0xC4599C


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path, required=True)
    parser.add_argument("--start-frame", type=int, required=True)
    parser.add_argument("--frames", type=int, default=160)
    parser.add_argument("--value", type=lambda text: int(text, 0),
                        help="diagnostic byte to write; omit for an unmodified control")
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    args = parser.parse_args()
    if args.value is not None and not 0 <= args.value <= 0xFF:
        parser.error("--value must fit one byte")
    if args.output.exists():
        raise FileExistsError(args.output)
    events = read_events(args.playback)
    args.output.mkdir(parents=True)
    engine = Engine(args.config.resolve(), args.output / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        before = engine.memory(REQUEST_FLAGS, 1)[0]
        if args.value is not None:
            write = engine.bind("e9k_debug_write_memory", C.c_int, C.c_uint32,
                                C.c_uint32, C.c_size_t)
            if not write(REQUEST_FLAGS, args.value, 1):
                raise RuntimeError("debug write failed")
        after_write = engine.memory(REQUEST_FLAGS, 1)[0]
        engine.frame = args.start_frame
        engine.hardware_frame = args.start_frame
        for frame in range(args.start_frame + 1, args.start_frame + args.frames + 1):
            for kind, values in events.get(frame - args.start_frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
        engine.screenshot(args.output / "screen.png")
        final_state = engine.state()
        (args.output / "state.bin").write_bytes(final_state)
        report = {
            "scope": ("unmodified isolated M-map command control" if args.value is None else
                      "diagnostic mutation of C4599C before isolated M-map command"),
            "restore": str(args.restore), "playback": str(args.playback),
            "start_frame": args.start_frame, "frames": args.frames,
            "address": f"${REQUEST_FLAGS:06X}", "before": before,
            "written": args.value, "after_write": after_write,
            "final": engine.memory(REQUEST_FLAGS, 1)[0],
            "video_sha256": sha(engine.video[0]),
            "state_sha256": sha(final_state),
            "qualification": ("This is an unmodified control." if args.value is None else
                              "This is a debugger-mutated probe, not original replay behavior. "
                              "A visible map can establish the candidate flag's sufficiency only, not its normal producer."),
        }
        (args.output / "report.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps(report))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
