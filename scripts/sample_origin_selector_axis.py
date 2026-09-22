"""Sample one controlled origin axis at every `$C1D3F4` terrain-selector call.

This deliberately records selector inputs only.  It is a cheaper companion to
the full instruction traces used to establish the static stream/copy path, and
does not reclassify selector indices as global coordinates.
"""
from __future__ import annotations

import argparse
import ctypes as C
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT, sha
from profile_window import read_events


STAGE = 0xC1C860
SELECTOR = 0xC1D3F4
RETURN = 0xC0F048
ORIGIN_X = 0xC45C3E
ORIGIN_Z = 0xC45C46
ORIGIN_MODE = 0xC45785


def parse_bin(value: str) -> int:
    result = int(value, 0)
    if not 0 <= result <= 0xFF:
        raise argparse.ArgumentTypeError("bin must be in 0..255")
    return result


def write(engine: Engine, address: int, value: int, size: int) -> None:
    writer = engine.bind("e9k_debug_write_memory", C.c_int, C.c_uint32,
                         C.c_uint32, C.c_size_t)
    if not writer(address, value, size):
        raise RuntimeError(f"debug write failed at ${address:06X}")


def reach_stage(engine: Engine, events: dict[int, list], arm_frame: int, frames: int) -> int:
    for frame in range(1, frames + 1):
        if frame == arm_frame:
            engine.core.e9k_debug_add_breakpoint(STAGE)
        for kind, values in events.get(frame, []):
            engine.event(kind, values)
        engine.core.retro_run()
        while engine.core.e9k_debug_is_paused():
            # Engine9000 retains debugger breakpoints across core lifetimes.
            # A previous sample's selector/return breakpoint can therefore be
            # encountered while replaying this fresh restored state.
            if engine.regs()["pc"] == STAGE:
                return frame
            engine.core.e9k_debug_step_instr()
            engine.core.e9k_debug_resume()
            engine.core.retro_run()
    raise RuntimeError("stage breakpoint not reached")


def sample_one(config: Path, restore: Path, events: dict[int, list], args: argparse.Namespace,
               bin_value: int, output: Path) -> dict:
    engine = Engine(config, output / "saves")
    try:
        engine.core.retro_run()
        state = restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        hit_frame = reach_stage(engine, events, args.arm_frame, args.frames)
        # The known normal alternate-origin path requires this mode byte.
        write(engine, ORIGIN_MODE, 1, 1)
        write(engine, ORIGIN_X if args.axis == "row" else ORIGIN_Z, bin_value << 24, 4)
        engine.core.e9k_debug_add_breakpoint(SELECTOR)
        engine.core.e9k_debug_add_breakpoint(RETURN)
        calls = []
        for _ in range(args.max_resumes):
            engine.core.e9k_debug_resume()
            engine.core.retro_run()
            if not engine.core.e9k_debug_is_paused():
                continue
            registers = engine.regs()
            pc = registers["pc"]
            if pc == RETURN:
                return {"bin": bin_value, "hit_frame": hit_frame, "calls": calls,
                        "termination": "return"}
            if pc != SELECTOR:
                raise RuntimeError(f"unexpected breakpoint ${pc:06X}")
            calls.append({"selector_index": registers["d0"] & 0xFFFF,
                          "live_row_key": registers["d1"] & 0xFFFF,
                          "workspace_band": registers["a3"] & 0xFFFFFF})
            engine.core.e9k_debug_step_instr()
        raise RuntimeError("resume cap before stage return")
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local/fa18.uae")
    parser.add_argument("--axis", choices=("row", "group"), required=True)
    parser.add_argument("--bin", type=parse_bin, required=True)
    parser.add_argument("--arm-frame", type=int, default=18)
    parser.add_argument("--frames", type=int, default=24)
    parser.add_argument("--max-resumes", type=int, default=256)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    args.output.mkdir(parents=True)
    events = read_events(args.playback)
    config = args.config.resolve()
    results = [sample_one(config, args.restore.resolve(), events, args, args.bin, args.output)]
    report = {
        "classification": "controlled_selector_input_sampling_not_global_coordinate_decode",
        "axis": args.axis,
        "writes": [{"address": f"${ORIGIN_MODE:06X}", "value": "$01", "size": 1},
                   {"address": f"${ORIGIN_X if args.axis == 'row' else ORIGIN_Z:06X}",
                    "value": "bin << 24", "size": 4}],
        "authority": {"restore": str(args.restore), "playback": str(args.playback),
                      "restore_sha256": sha(args.restore.read_bytes()),
                      "playback_sha256": sha(args.playback.read_bytes()),
                      "stage": f"${STAGE:06X}", "selector": f"${SELECTOR:06X}",
                      "return": f"${RETURN:06X}"},
        "samples": results,
    }
    (args.output / "selector_axis_samples.json").write_text(json.dumps(report, indent=2) + "\n",
                                                               encoding="utf-8")
    print(json.dumps({"axis": args.axis, "bins": len(results),
                      "calls": sum(len(row["calls"]) for row in results),
                      "output": str(args.output)}))


if __name__ == "__main__":
    main()
