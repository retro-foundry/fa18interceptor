"""Capture `$C4B390` projected polygon pairs at each run003 M-map submission.

The capture stops at `$C2FF48`, before the renderer consumes the list.  These
are its own projected vector pairs, after clipping/projection and before the
area-blit implementation; they are not decoded bitplane pixels.
"""
from __future__ import annotations

import argparse
import ctypes as C
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


ENTRY = 0xC2FF48
LIST = 0xC4B390
MAX_PAIRS = 64
ACTIVE_FILL_PLANE_MASK = 0xC456E7


def signed_word(data: bytes, offset: int) -> int:
    return int.from_bytes(data[offset:offset + 2], "big", signed=True)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, default=ROOT / "build/run003_pre_m_2183/state.bin")
    parser.add_argument("--playback", type=Path,
                        default=ROOT / "build/run003_m_press_only.e9k",
                        help="optional events delivered after restore")
    parser.add_argument("--no-playback", action="store_true",
                        help="do not deliver the default run003 M-key event stream")
    parser.add_argument("--diagnostic-input", action="store_true",
                        help="mark supplied replay input as diagnostic rather than sealed-scenario evidence")
    parser.add_argument("--config", type=Path, default=ROOT / "local/fa18.uae")
    parser.add_argument("--frames", type=int, default=100)
    parser.add_argument("--max-polygons", type=int, default=128)
    parser.add_argument("--write-memory", action="append", nargs=3,
                        metavar=("ADDRESS", "VALUE", "SIZE"), default=[],
                        help="diagnostic pre-replay write (size 1, 2, or 4); does not modify the checkpoint")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    args.output.mkdir(parents=True)
    events = {} if args.no_playback else read_events(args.playback)
    engine = Engine(args.config.resolve(), args.output / "saves")
    polygons = []
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        writes = []
        if args.write_memory:
            write = engine.bind("e9k_debug_write_memory", C.c_int, C.c_uint32,
                                C.c_uint32, C.c_size_t)
            for address_text, value_text, size_text in args.write_memory:
                address, value, size = int(address_text, 0), int(value_text, 0), int(size_text, 0)
                if size not in (1, 2, 4):
                    raise ValueError("--write-memory size must be 1, 2, or 4")
                if not write(address, value, size):
                    raise RuntimeError(f"debug write failed at ${address:06X}")
                writes.append({"address": f"${address:06X}", "value": value, "size": size})
        engine.core.e9k_debug_add_breakpoint(ENTRY)
        for frame in range(1, args.frames + 1):
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                if registers["pc"] != ENTRY:
                    raise RuntimeError(f"unexpected breakpoint PC ${registers['pc']:06X}")
                payload = engine.memory(LIST, 2 + MAX_PAIRS * 4)
                count = signed_word(payload, 0)
                if not 0 <= count <= MAX_PAIRS:
                    raise RuntimeError(f"invalid projected-pair count {count}")
                pairs = [[signed_word(payload, 2 + pair * 4), signed_word(payload, 4 + pair * 4)]
                         for pair in range(count)]
                polygons.append({
                    "submission": len(polygons), "host_frame": engine.frame,
                    "context_a5": f"${registers['a5'] & 0xFFFFFF:06X}",
                    "active_fill_plane_mask": engine.memory(ACTIVE_FILL_PLANE_MASK, 1)[0],
                    "projected_pairs": pairs,
                })
                if len(polygons) >= args.max_polygons:
                    break
                engine.core.e9k_debug_step_instr()
                engine.core.e9k_debug_resume()
                engine.core.retro_run()
            if len(polygons) >= args.max_polygons:
                break
        classification = ("diagnostic_projected_renderer_vectors_before_area_blit"
                          if writes or args.diagnostic_input else
                          "scenario_backed_projected_renderer_vectors_before_area_blit")
        report = {
            "scope": "C4B390 projected pairs immediately before C2FF48 in supplied M-map replay",
            "classification": classification,
            "authority": {"restore": str(args.restore), "playback": None if args.no_playback else str(args.playback),
                          "config": str(args.config)},
            "polygon_count": len(polygons), "polygons": polygons,
            "debug_writes": writes,
            "qualification": (
                "Pairs are renderer-produced projected vectors. Context is a live renderer cursor; it is not by itself a static source-model identity."
                if not writes and not args.diagnostic_input else
                "Pairs are renderer-produced projected vectors from a diagnostic replay; the sealed checkpoint is unchanged. "
                "The diagnostic input or debugger mutation establishes display-state causality only, not a normal gameplay map position or static source-model identity."),
        }
        (args.output / "projected_polygons.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"polygons": len(polygons), "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
