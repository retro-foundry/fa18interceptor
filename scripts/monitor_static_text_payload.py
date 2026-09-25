"""Replay normally until the static glyph reader enters a specified payload range."""

from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT, sha
from profile_window import read_events


GLYPH_READER = 0xC32FCE


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path, required=True)
    parser.add_argument("--start", type=lambda value: int(value, 0), required=True)
    parser.add_argument("--end", type=lambda value: int(value, 0), required=True)
    parser.add_argument("--frames", type=int, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    args = parser.parse_args()
    if args.output.exists() or args.start > args.end or args.frames < 1:
        raise ValueError("output must not exist; payload range and frame count must be valid")

    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), args.output / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        engine.core.e9k_debug_add_breakpoint(GLYPH_READER)
        total_hits = 0
        selected = None
        for frame in range(1, args.frames + 1):
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                if registers["pc"] != GLYPH_READER:
                    raise RuntimeError(f"unexpected breakpoint ${registers['pc']:06X}")
                total_hits += 1
                cursor = registers["a2"] & 0xFFFFFF
                if args.start <= cursor <= args.end:
                    selected = {"relative_frame": frame, "cursor": f"${cursor:06X}",
                                "byte": engine.memory(cursor, 1).hex(),
                                "registers": registers,
                                "return_pc": f"${int.from_bytes(engine.memory(registers['a7'], 4), 'big'):06X}"}
                    break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if selected is not None:
                break

        args.output.mkdir(parents=True)
        report = {
            "scope": "normal replay with C32FCE glyph-reader breakpoint; no post-hit stepping",
            "restore": str(args.restore), "playback": str(args.playback),
            "payload_range": [f"${args.start:06X}", f"${args.end:06X}"],
            "selected": selected, "glyph_reader_hits": total_hits,
            "qualification": "A selected cursor is direct evidence that C32FCE reads a byte from the requested static payload. It does not identify the producer of the cursor or persistent state.",
        }
        if selected is not None:
            for name, address, size in (("chip", 0, 0x80000), ("slow", 0xC00000, 0x80000)):
                (args.output / f"{name}.bin").write_bytes(engine.memory(address, size))
            payload = engine.state()
            (args.output / "state.bin").write_bytes(payload)
            engine.screenshot(args.output / "screen.png")
            report["state_sha256"] = sha(payload)
        (args.output / "report.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"glyph_reader_hits": total_hits, "selected": selected is not None}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
