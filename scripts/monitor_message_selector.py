"""Replay normally until a selected C32D24 message-record invocation occurs."""

from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT, sha
from profile_window import read_events


SELECTOR = 0xC32D24


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path, required=True)
    parser.add_argument("--selector", type=lambda value: int(value, 0), default=74)
    parser.add_argument("--frames", type=int, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    args = parser.parse_args()
    if args.output.exists() or not 0 <= args.selector <= 0xFFFF:
        raise ValueError("output must not exist and selector must be a word")

    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), args.output / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        engine.core.e9k_debug_add_breakpoint(SELECTOR)
        hits = []
        selected = None
        for frame in range(1, args.frames + 1):
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                pc = registers["pc"]
                if pc != SELECTOR:
                    raise RuntimeError(f"unexpected breakpoint ${pc:06X}")
                value = registers["d0"] & 0xFFFF
                row = {"relative_frame": frame, "selector": value,
                       "registers": registers,
                       "return_pc": f"${int.from_bytes(engine.memory(registers['a7'], 4), 'big'):06X}"}
                hits.append(row)
                if value == args.selector:
                    selected = row
                    break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if selected is not None:
                break

        args.output.mkdir(parents=True)
        report = {
            "scope": "normal replay with C32D24 selector breakpoint; no post-hit stepping",
            "restore": str(args.restore), "playback": str(args.playback),
            "target_selector": args.selector, "selected": selected, "hits": hits,
            "qualification": "The selector value is a direct register observation. Its caller, queue writer, and persistent-state meaning require separate trace evidence.",
        }
        if selected is not None:
            for name, address, size in (("chip", 0, 0x80000), ("slow", 0xC00000, 0x80000)):
                (args.output / f"{name}.bin").write_bytes(engine.memory(address, size))
            payload = engine.state()
            (args.output / "state.bin").write_bytes(payload)
            engine.screenshot(args.output / "screen.png")
            report["state_sha256"] = sha(payload)
        (args.output / "report.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"hits": len(hits), "selected": selected is not None}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
