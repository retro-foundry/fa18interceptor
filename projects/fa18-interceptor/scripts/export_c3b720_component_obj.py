"""Export the trace-proven C3B720 local component to a minimal OBJ mesh."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


FACES = ((0, 1, 4), (1, 2, 4), (2, 3, 4), (3, 0, 4))


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    payload = json.loads(args.input.read_text(encoding="utf-8"))
    vertices = payload["triples"]
    if len(vertices) != 5:
        raise ValueError(f"expected five source triples, got {len(vertices)}")
    lines = [
        "# Trace-proven partial local component: $C3B720 -> $C3B6B0",
        "# Four triangular sides only; no base face or missing topology is inferred.",
        "o C3B720_trace_proven_partial_component",
    ]
    lines.extend(f"v {x} {y} {z}" for x, y, z in vertices)
    lines.extend("f " + " ".join(str(index + 1) for index in face) for face in FACES)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(json.dumps({"vertices": len(vertices), "faces": len(FACES), "output": str(args.output)}))


if __name__ == "__main__":
    main()
