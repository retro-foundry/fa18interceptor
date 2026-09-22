"""Associate C2FF48 polygons and C212B0 lines with one matrix source pass.

An instance begins at a selected transform entry and ends immediately before
the next same transform entry. Geometry is captured at renderer entries while single
stepping that bounded interval, so separate transforms sharing C48390 cannot
be accidentally merged.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT


TRANSFORM = 0xC1F4AC
POLYGON = 0xC2FF48
LINE = 0xC212B0
TRIPLES = 0xC4B990
PAIRS = 0xC4B390
VERTICES = 0xC48390


def words(engine: Engine, address: int, count: int) -> list[int]:
    raw = engine.memory(address, count * 2)
    return [int.from_bytes(raw[index:index + 2], "big", signed=True)
            for index in range(0, len(raw), 2)]


def polygon(engine: Engine, registers: dict[str, int]) -> dict[str, object]:
    count = int.from_bytes(engine.memory(PAIRS, 2), "big")
    if not 3 <= count <= 16:
        raise RuntimeError(f"invalid polygon count {count}")
    values = words(engine, TRIPLES, count * 3)
    return {"count": count, "triples": [values[index:index + 3] for index in range(0, len(values), 3)],
            "context_a5": f"${registers['a5'] & 0xFFFFFF:06X}"}


def line(engine: Engine, registers: dict[str, int]) -> dict[str, object]:
    cursor = registers["a2"] & 0xFFFFFF
    selector = int.from_bytes(engine.memory(cursor, 2), "big", signed=True)
    cursor += 2
    segments = []
    for _ in range(128):
        first, second = words(engine, cursor, 2)
        cursor += 4
        terminal = second < 0
        second &= 0x7FFF
        segments.append({"offsets": [first, second],
                         "triples": [words(engine, VERTICES + first, 3), words(engine, VERTICES + second, 3)]})
        if terminal:
            return {"selector": selector, "segments": segments,
                    "context_a5": f"${registers['a5'] & 0xFFFFFF:06X}"}
    raise RuntimeError("unterminated C212B0 line list")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    parser.add_argument("--source", action="append", required=True,
                        type=lambda value: int(value, 0))
    parser.add_argument("--entry", type=lambda value: int(value, 0), default=TRANSFORM,
                        help="matrix entry delimiting instances (default: C1F4AC)")
    parser.add_argument("--frames", type=int, default=64)
    parser.add_argument("--occurrences", type=int, default=1,
                        help="capture this many batches for each requested source")
    parser.add_argument("--max-instructions", type=int, default=160000)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    args.output.mkdir(parents=True)
    wanted = set(args.source)
    engine = Engine(args.config.resolve(), args.output / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        # Renderer entries must be breakpoints too.  Without them, retro_run()
        # can execute an entire line/polygon submission between two matrix
        # breakpoints and the source-bounded loop never observes it.
        for address in {args.entry, POLYGON, LINE}:
            engine.core.e9k_debug_add_breakpoint(address)
        instances: list[dict[str, object]] = []
        for _ in range(args.frames):
            engine.core.retro_run()
            while engine.core.e9k_debug_is_paused():
                registers = engine.regs()
                if registers["pc"] != args.entry:
                    # A renderer submission outside a wanted source interval
                    # is not evidence for that source.  Step past it and keep
                    # looking for the next matrix entry.
                    engine.core.e9k_debug_step_instr()
                    engine.core.e9k_debug_resume()
                    engine.core.retro_run()
                    continue
                source = registers["a1"] & 0xFFFFFF
                occurrence_count = sum(item["source"] == f"${source:06X}" for item in instances)
                if source not in wanted or occurrence_count >= args.occurrences:
                    engine.core.e9k_debug_step_instr()
                    engine.core.e9k_debug_resume()
                    engine.core.retro_run()
                    continue
                item: dict[str, object] = {"source": f"${source:06X}", "occurrence": occurrence_count + 1,
                                           "host_frame": engine.frame,
                                           "matrix": f"${registers['a4'] & 0xFFFFFF:06X}",
                                           "polygons": [], "lines": []}
                for instruction in range(args.max_instructions):
                    registers = engine.regs()
                    pc = registers["pc"]
                    if instruction and pc == args.entry:
                        item["instructions"] = instruction
                        item["end_source"] = f"${registers['a1'] & 0xFFFFFF:06X}"
                        break
                    if pc == POLYGON:
                        item["polygons"].append(polygon(engine, registers))
                    elif pc == LINE:
                        item["lines"].append(line(engine, registers))
                    engine.core.e9k_debug_step_instr()
                    engine.core.retro_run()
                else:
                    raise RuntimeError(f"instruction cap following ${source:06X}")
                instances.append(item)
                if all(sum(item["source"] == f"${source:06X}" for item in instances) >= args.occurrences
                       for source in wanted):
                    break
            if all(sum(item["source"] == f"${source:06X}" for item in instances) >= args.occurrences
                   for source in wanted):
                break
        report = {"scope": "matrix-source bounded renderer geometry", "restore": str(args.restore),
                  "transform_entry": f"${args.entry:06X}",
                  "instances": instances,
                  "qualification": "Each entry is bounded by consecutive C1F4AC transforms. Polygon triples and line endpoints are mutable renderer workspaces."}
        (args.output / "instance_geometry.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps({"instances": len(instances), "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
