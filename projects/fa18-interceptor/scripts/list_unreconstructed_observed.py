"""List executed P-code instruction ranges not yet covered by byte-exact source.

This is a source-reconstruction work queue, not a new execution-coverage
metric: inputs are the existing imported P-code packets and the assembled
``source_amiga/observed`` slices.  A row may cross routine boundaries; the
packet list is retained so a follow-up can select a bounded authority packet.
"""
from __future__ import annotations

import argparse
import json
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import coverage as cov

ROOT = Path(__file__).resolve().parents[1]


def observed_instructions(pcode_root: Path):
    """Return canonical executed instructions keyed by runtime start address."""
    rows: dict[int, dict] = {}
    packets: defaultdict[int, set[str]] = defaultdict(set)
    for export in sorted(pcode_root.glob("*/instructions.pcode.jsonl")):
        for line in export.read_text(encoding="utf-8").splitlines():
            row = json.loads(line)
            address = row["address"]
            raw = bytes.fromhex(row["bytes"])
            prior = rows.get(address)
            if prior is not None and prior["bytes"] != raw:
                raise ValueError(f"conflicting observed bytes at ${address:06X}")
            rows[address] = {"address": address, "bytes": raw}
            packets[address].add(export.parent.name)
    return rows, packets


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--pcode-root", type=Path, default=ROOT / "pcode" / "raw")
    parser.add_argument("--source-dir", type=Path, default=ROOT / "source_amiga" / "observed")
    parser.add_argument("--limit", type=int, default=50)
    parser.add_argument("--json", type=Path)
    args = parser.parse_args()

    _, covered = cov.assemble_slices(args.source_dir)
    instructions, packets = observed_instructions(args.pcode_root)
    missing = [row for row in instructions.values()
               if any(byte not in covered
                      for byte in range(row["address"], row["address"] + len(row["bytes"])))]
    missing.sort(key=lambda row: row["address"])

    ranges = []
    for row in missing:
        start = row["address"]
        end = start + len(row["bytes"])
        packet_names = set(packets[start])
        if ranges and start == ranges[-1]["end"]:
            ranges[-1]["end"] = end
            ranges[-1]["instructions"] += 1
            packet_names.update(ranges[-1]["packets"])
            ranges[-1]["packets"] = sorted(packet_names)
        else:
            ranges.append({"start": start, "end": end, "instructions": 1,
                           "packets": sorted(packet_names)})

    for row in ranges:
        row["bytes"] = row["end"] - row["start"]
    ranked = sorted(ranges, key=lambda row: (-row["bytes"], row["start"]))
    result = {"observed_instruction_bytes": sum(len(row["bytes"]) for row in instructions.values()),
              "covered_observed_bytes": len(covered & cov.observed_bytes(args.pcode_root)[0]),
              "missing_observed_bytes": sum(row["bytes"] for row in ranges),
              "ranges": ranked}

    print(f"missing observed bytes: {result['missing_observed_bytes']:,} in {len(ranges):,} contiguous ranges")
    for row in ranked[:args.limit]:
        packets_text = ", ".join(row["packets"][:3])
        if len(row["packets"]) > 3:
            packets_text += ", ..."
        print(f"${row['start']:06X}-${row['end'] - 1:06X}  {row['bytes']:4,} bytes  "
              f"{row['instructions']:3,} starts  {packets_text}")
    if args.json:
        args.json.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
