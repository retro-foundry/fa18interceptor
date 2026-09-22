"""Capture complete OCS blitter register state at each observed BLTSIZE write."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


POINTERS = {
    "C": (0x048, 0x04A),
    "B": (0x04C, 0x04E),
    "A": (0x050, 0x052),
    "D": (0x054, 0x056),
}
STATE_REGISTERS = (0x040, 0x042, 0x044, 0x046, 0x060, 0x062, 0x064, 0x066)
BLTSIZE = 0x058
CUSTOM_BASE = 0xDFF000


def parse_range(value: str) -> tuple[int, int]:
    start, end = (int(part, 0) for part in value.split(":", 1))
    if end <= start:
        raise argparse.ArgumentTypeError("range end must exceed start")
    return start, end


def pointer_state(registers: dict[int, int]) -> dict[str, int]:
    return {channel: ((registers.get(high, 0) << 16) | registers.get(low, 0))
            for channel, (high, low) in POINTERS.items()}


def in_ranges(address: int, ranges: list[tuple[int, int]]) -> bool:
    return any(start <= address < end for start, end in ranges)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path, help="Engine9000 custom_writes.jsonl")
    parser.add_argument("--display-range", type=parse_range, action="append", default=[],
                        help="half-open DMA display range, e.g. 0x12bc0:0x14b00")
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--markdown", type=Path, required=True)
    args = parser.parse_args()

    registers: dict[int, int] = {}
    last_writers: dict[int, dict] = {}
    jobs = []
    for sequence, line in enumerate(args.input.read_text().splitlines()):
        row = json.loads(line)
        if row["copper"]:
            continue
        offset = row["address"] - CUSTOM_BASE
        if offset < 0 or offset > 0x1FE:
            continue
        registers[offset] = row["value"]
        last_writers[offset] = {"sequence": sequence, "source_pc": f"${row['source']:06X}",
                                "hardware_frame": row["hardware_frame"]}
        if offset != BLTSIZE:
            continue
        pointers = pointer_state(registers)
        jobs.append({
            "sequence": sequence,
            "hardware_frame": row["hardware_frame"],
            "vpos": row["vpos"],
            "hpos": row["hpos"],
            "trigger_pc": f"${row['source']:06X}",
            "bltsize": f"${row['value']:04X}",
            "bltcon0": f"${registers.get(0x040, 0):04X}",
            "bltcon1": f"${registers.get(0x042, 0):04X}",
            "pointers": {channel: f"${address:06X}" for channel, address in pointers.items()},
            "pointer_last_writers": {
                channel: {"high": last_writers.get(high), "low": last_writers.get(low)}
                for channel, (high, low) in POINTERS.items()
            },
            "active_display_channels": [channel for channel, address in pointers.items()
                                        if in_ranges(address, args.display_range)],
        })

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps({"input": str(args.input), "display_ranges": [
        {"start": f"${start:06X}", "end_exclusive": f"${end:06X}"}
        for start, end in args.display_range], "jobs": jobs}, indent=2) + "\n")
    active = [job for job in jobs if job["active_display_channels"]]
    lines = ["# Observed blitter jobs", "", f"Authority: `{args.input}`.", "",
             f"{len(jobs)} CPU `BLTSIZE` jobs were observed; {len(active)} have a pointer in the supplied active display ranges.",
             "The pointer state is reconstructed from preceding CPU Custom-register writes. It records channel placement, not a graphics-asset interpretation.",
             "", "| Frame | Trigger | BLTSIZE | A | B | C | D | Active display channels |",
             "| ---: | --- | --- | --- | --- | --- | --- | --- |"]
    for job in active:
        p = job["pointers"]
        lines.append(f"| {job['hardware_frame']} | `{job['trigger_pc']}` | `{job['bltsize']}` | `{p['A']}` | `{p['B']}` | `{p['C']}` | `{p['D']}` | {', '.join(job['active_display_channels'])} |")
    args.markdown.parent.mkdir(parents=True, exist_ok=True)
    args.markdown.write_text("\n".join(lines) + "\n")
    print(f"wrote {args.output}"); print(f"wrote {args.markdown}")


if __name__ == "__main__":
    main()
