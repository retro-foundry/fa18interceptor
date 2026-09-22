"""Export the two trace-proven map local polylines as grouped OBJ line primitives."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


COMPONENTS = (("$C35BDE", "C35BDE_trace_proven_polyline"),
              ("$C36220", "C36220_trace_proven_polyline"))


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    entries = {row["transform_input"]: row
               for row in json.loads(args.input.read_text(encoding="utf-8"))["components"]}
    lines = [
        "# Trace-proven local map polylines; no faces, placement, or stitching inferred.",
    ]
    base = 1
    for address, name in COMPONENTS:
        vertices = entries[address]["raw_local_triples"]
        if len(vertices) != 3:
            raise ValueError(f"expected three vertices for {address}, got {len(vertices)}")
        lines.append(f"o {name}")
        lines.extend(f"v {x} {y} {z}" for x, y, z in vertices)
        lines.append(f"l {base} {base + 1}")
        lines.append(f"l {base + 1} {base + 2}")
        base += len(vertices)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(json.dumps({"objects": len(COMPONENTS), "vertices": base - 1,
                      "lines": len(COMPONENTS) * 2, "output": str(args.output)}))


if __name__ == "__main__":
    main()
