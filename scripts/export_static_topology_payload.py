"""Export a trace-proven static-triple topology JSON to portable data.

Use only with a topology whose `vertex_source` is already source-traced.  This
does not turn a renderer workspace snapshot into an immutable asset.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path


SLOW_BASE = 0xC00000


def signed_word(memory: bytes, address: int) -> int:
    offset = address - SLOW_BASE
    return int.from_bytes(memory[offset:offset + 2], "big", signed=True)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--topology", type=Path, required=True)
    parser.add_argument("--slow", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    topology = json.loads(args.topology.read_text(encoding="utf-8"))
    memory = args.slow.read_bytes()
    if len(memory) != 0x80000:
        raise ValueError("expected a 512 KiB slow-RAM snapshot")
    source = topology["vertex_source"]
    start = int(source["start"].removeprefix("$"), 16)
    count = source["count"]
    triples = [
        {"index": index, "address": f"${start + index * 6:06X}",
         "xyz": [signed_word(memory, start + index * 6 + axis * 2) for axis in range(3)]}
        for index in range(count)
    ]
    output = {
        "schema": "fa18interceptor.static-topology-payload/v1",
        "source_snapshot": str(args.slow),
        "topology_source": str(args.topology),
        "immutable_coordinate_triples": triples,
        "renderer_observed_faces": topology["faces"],
        "qualification": (
            "Triples are copied from the trace-proven static source range. Face edges are only "
            "the renderer-observed topology supplied by the input JSON; no unseen geometry is inferred."
        ),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(output, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"triples": len(triples), "faces": len(topology["faces"]), "output": str(args.output)}))


if __name__ == "__main__":
    main()
