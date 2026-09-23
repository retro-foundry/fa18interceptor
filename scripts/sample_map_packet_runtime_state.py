"""Sample live map packet stream-selector state from a sealed emulator state."""
from __future__ import annotations

import argparse
import json
import tempfile
from pathlib import Path

from engine9000_bridge import Engine, ROOT


STREAM_GATE = 0xC2AF40


def address(value: int) -> str:
    return f"${value & 0xFFFFFF:06X}"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--state", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--max-instructions", type=int, default=180000)
    parser.add_argument("--config", type=Path, default=ROOT / "local/fa18.uae")
    parser.add_argument("--replace", action="store_true")
    args = parser.parse_args()
    markdown = args.output.with_suffix(".md")
    if (args.output.exists() or markdown.exists()) and not args.replace:
        raise FileExistsError(args.output)
    if args.max_instructions < 1:
        raise ValueError("max-instructions must be positive")
    state = args.state.read_bytes()
    with tempfile.TemporaryDirectory(prefix="fa18-map-packet-") as temporary:
        engine = Engine(args.config.resolve(), Path(temporary))
        try:
            engine.core.retro_run()
            if not engine.core.retro_unserialize(state, len(state)):
                raise RuntimeError("core rejected state")
            samples = []
            for instruction in range(args.max_instructions):
                registers = engine.regs()
                if registers["pc"] != STREAM_GATE:
                    engine.core.e9k_debug_step_instr()
                    engine.core.retro_run()
                    continue
                a6, a3 = registers["a6"], registers["a3"]
                header = a3 - 4
                alternate = int.from_bytes(engine.memory(header, 4), "big")
                metric = int.from_bytes(engine.memory(a6 - 0x28, 4), "big", signed=True)
                mode = int.from_bytes(engine.memory(a6 - 0x3E, 2), "big", signed=True)
                d7 = registers["d7"] & 0xFFFF
                samples.append({"instruction": instruction, "header": address(header),
                                "inline_stream": address(a3), "alternate_stream": address(alternate),
                                "selected_stream": address(alternate if d7 else a3), "route": "alternate" if d7 else "inline",
                                "d7": d7, "metric": metric, "alternate_mode": mode})
                engine.core.e9k_debug_step_instr()
                engine.core.retro_run()
        finally:
            engine.core.retro_unload_game()
            engine.core.retro_deinit()
    report = {"scope": "live C2AF40 map packet selector samples from a sealed state without replay input",
              "state": str(args.state), "max_instructions": args.max_instructions, "samples": samples,
              "unique_headers": sorted({row["header"] for row in samples}),
              "route_counts": {"inline": sum(row["route"] == "inline" for row in samples),
                               "alternate": sum(row["route"] == "alternate" for row in samples)},
              "qualification": ("Metric and alternate-mode values are live frame values read at C2AF40. They establish "
                                "the immediate selector state only; their higher-level physical meaning requires separate "
                                "dataflow evidence.")}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# Live map packet selector samples", "", report["qualification"], "",
             f"- Inline route samples: {report['route_counts']['inline']}",
             f"- Alternate route samples: {report['route_counts']['alternate']}", "",
             f"- Instruction budget: {report['max_instructions']}",
             f"- Distinct headers: {len(report['unique_headers'])}", "",
             "| Instruction | Header | Metric | Alternate mode | `D7` | Route | Selected stream |",
             "| ---: | --- | ---: | ---: | ---: | --- | --- |"]
    for row in samples:
        lines.append(f"| {row['instruction']} | `{row['header']}` | {row['metric']} | {row['alternate_mode']} | "
                     f"{row['d7']} | {row['route']} | `{row['selected_stream']}` |")
    lines.append("")
    markdown.write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"samples": len(samples), **report["route_counts"]}))


if __name__ == "__main__":
    main()
