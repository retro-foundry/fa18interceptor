"""Extract the immutable C351 flight-object inputs and static C34A face data.

This deliberately exports neither C48390 nor C45BEA: both are mutable runtime
state.  The companion manifest records the required transform and tail-code
contract for reconstructing the rendered geometry.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from engine9000_bridge import ROOT


SLOW_BASE = 0xC00000
COORDINATES = 0xC3515E
TRIPLE_COUNT = 22


def parse_address(value: str) -> int:
    return int(value.removeprefix("$").replace("-", ""), 16)


def word(memory: bytes, address: int, *, signed: bool = True) -> int:
    offset = address - SLOW_BASE
    return int.from_bytes(memory[offset:offset + 2], "big", signed=signed)


def face(memory: bytes, address: int) -> dict[str, object]:
    offsets: list[int] = []
    for index in range(16):
        value = word(memory, address + index * 2, signed=False)
        offsets.append(value & 0x7FFF)
        if value & 0x8000:
            if len(offsets) < 3:
                raise ValueError(f"face ${address:06X} has fewer than three offsets")
            return {
                "address": f"${address:06X}",
                "offsets": offsets,
                "vertex_slots": [offset // 6 for offset in offsets],
                "word_count": len(offsets),
            }
    raise ValueError(f"face ${address:06X} has no signed terminal offset")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--slow", type=Path, required=True,
                        help="slow-RAM snapshot containing the loaded Hunk-41 payload")
    parser.add_argument("--manifest", type=Path,
                        default=ROOT / "analysis/data/c351_flight_object_export_manifest.json")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    memory = args.slow.read_bytes()
    if len(memory) != 0x80000:
        raise ValueError(f"expected 512 KiB slow-RAM snapshot, got {len(memory)} bytes")
    manifest = json.loads(args.manifest.read_text(encoding="utf-8"))
    records = manifest["immutable_data"]["face_records"]["records"]
    triples = []
    for index in range(TRIPLE_COUNT):
        address = COORDINATES + index * 6
        triples.append({"index": index, "address": f"${address:06X}",
                        "xyz": [word(memory, address + axis * 2) for axis in range(3)]})
    faces = [face(memory, parse_address(address)) for address in records]
    output = {
        "schema": "fa18interceptor.static-flight-object-payload/v1",
        "source_snapshot": str(args.slow),
        "manifest": str(args.manifest),
        "immutable_coordinate_triples": triples,
        "immutable_face_records": faces,
        "required_procedural_contract": manifest["required_code"],
        "excluded_runtime_data": manifest["workspace_contract"]["explicitly_not_source_data"],
        "qualification": (
            "This is static input/face data only. Reconstruct C34A slots 22-39 with the "
            "manifested derivation routine; do not consume C48390 or C45BEA as source data."
        ),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(output, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"triples": len(triples), "faces": len(faces), "output": str(args.output)}))


if __name__ == "__main__":
    main()
