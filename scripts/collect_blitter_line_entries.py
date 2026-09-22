"""Collect C2FA7E screen-line inputs with their selected bitplane mask.

The optional replay is delivered before the breakpoint is armed.  Captured
endpoints are the four words C2FA7E receives, before it turns them into OCS
blitter line parameters; a matching line is still not proof of object owner.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


EMIT = 0xC2FA7E
PLANE_MASK = 0xC456E7


def signed_register_word(value: int) -> int:
    value &= 0xFFFF
    return value - 0x10000 if value & 0x8000 else value


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path,
                        help="optional input recording delivered before collection")
    parser.add_argument("--arm-frame", type=int, default=1,
                        help="install the C2FA7E breakpoint immediately before this replay frame")
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    parser.add_argument("--frames", type=int, required=True)
    parser.add_argument("--max-lines", type=int, default=512)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    if args.arm_frame < 1 or args.frames < args.arm_frame or args.max_lines < 1:
        raise ValueError("invalid replay frame or line limit")
    args.output.mkdir(parents=True)
    events = read_events(args.playback) if args.playback else {}
    engine = Engine(args.config.resolve(), args.output / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        lines = []
        for frame in range(1, args.frames + 1):
            if frame == args.arm_frame:
                engine.core.e9k_debug_add_breakpoint(EMIT)
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                if registers["pc"] != EMIT:
                    raise RuntimeError(f"unexpected breakpoint PC {registers['pc']:06X}")
                lines.append({
                    "submission": len(lines), "host_frame": engine.frame,
                    "endpoints": [signed_register_word(registers[f"d{number}"])
                                  for number in range(4)],
                    "active_line_plane_mask": engine.memory(PLANE_MASK, 1)[0],
                    "context": {name: f"${registers[name] & 0xFFFFFF:06X}"
                                for name in ("a2", "a3", "a4", "a5")},
                    "return_pc": f"${int.from_bytes(engine.memory(registers['a7'] & 0xFFFFFF, 4), 'big') & 0xFFFFFF:06X}",
                })
                if len(lines) >= args.max_lines:
                    break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if len(lines) >= args.max_lines:
                break
        report = {
            "scope": "C2FA7E line-emitter input endpoints and active plane mask",
            "restore": str(args.restore), "playback": str(args.playback) if args.playback else None,
            "arm_frame": args.arm_frame, "frames": args.frames,
            "submission_count": len(lines), "submissions": lines,
            "qualification": "The endpoints and plane mask describe a renderer emission. They do not alone prove a displayed pixel, source face, terrain record, or bridge ownership.",
        }
        (args.output / "blitter_line_entries.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"lines": len(lines), "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
