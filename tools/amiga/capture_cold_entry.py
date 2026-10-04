"""Capture a true power-on Engine9000 launch at an evidence-backed entry PC.

This oracle-only tool never restores a state. Output belongs in ignored build/
or local/ storage and is not runtime initialization material.
"""
import argparse
import hashlib
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
from engine9000_bridge import Engine, ENGINE


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", type=Path, required=True)
    parser.add_argument("--entry", type=lambda s: int(s, 0), required=True)
    parser.add_argument("--frames", type=int, default=12000)
    parser.add_argument("--playback", type=Path)
    parser.add_argument("--input-offset", type=int, default=0,
                        help="power-on frames before applying a desktop input recording")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if not 0 <= args.entry <= 0xFFFFFF or args.frames < 1:
        parser.error("invalid entry/frame bound")
    output = args.output.resolve()
    if not any(output.is_relative_to(ROOT / name) for name in ("build", "local")):
        parser.error("binary oracle evidence must remain in build/ or local/")
    output.mkdir(parents=True, exist_ok=True)
    events = {}
    if args.playback:
        lines = args.playback.read_text().splitlines()
        if not lines or lines[0] != "E9K_INPUT_V1":
            parser.error("expected E9K_INPUT_V1 playback")
        for line in lines[1:]:
            parts = line.split()
            if parts and parts[0] == "F":
                events.setdefault(int(parts[1]) + args.input_offset, []).append(
                    (parts[2], list(map(int, parts[3:]))))
    engine = Engine(args.config.resolve(), output)
    try:
        engine.core.e9k_debug_add_breakpoint(args.entry)
        for frame in range(args.frames):
            for kind, values in events.get(frame + 1, []):
                engine.event(kind, values)
            engine.core.retro_run()
            if engine.core.e9k_debug_is_paused():
                regs = engine.regs()
                if regs["pc"] != args.entry:
                    raise RuntimeError(f"unexpected paused PC {regs['pc']:06X}")
                state = engine.state()
                (output / "state.bin").write_bytes(state)
                (output / "chip.bin").write_bytes(engine.memory(0, 0x80000))
                (output / "slow.bin").write_bytes(engine.memory(0xC00000, 0x80000))
                manifest = {
                    "schema": "amiga.cold_entry.v1", "power_on": True, "restored_state": False,
                    "host_runs": frame + 1, "engine_frames": engine.frame,
                    "cycle": engine.core.e9k_debug_read_cycle_count(),
                    "clock_unit": "OCS colour clocks (Engine9000 get_cycles / CYCLE_UNIT)",
                    "registers": regs,
                    "config_sha256": sha(args.config),
                    "engine_sha256": sha(ENGINE / "system/ami9000.dll"),
                    "playback_sha256": sha(args.playback) if args.playback else None,
                    "input_offset": args.input_offset,
                    "files": {p: sha(output / p) for p in ("state.bin", "chip.bin", "slow.bin")},
                }
                (output / "entry.json").write_text(json.dumps(manifest, indent=2) + "\n")
                print(json.dumps(manifest), flush=True)
                return
            if frame % 1000 == 0:
                print(f"cold launch: {frame + 1} host frames; PC={engine.regs()['pc']:06X}", flush=True)
        engine.screenshot(output / "unreached.png")
        (output / "unreached_state.bin").write_bytes(engine.state())
        raise RuntimeError(f"entry {args.entry:06X} not reached within {args.frames} frames; terminal evidence saved")
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
