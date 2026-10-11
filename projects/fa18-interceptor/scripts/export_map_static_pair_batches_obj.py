"""Export traced M-map static pair batches as OBJ line groups for inspection."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--inventory", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    report = json.loads(args.inventory.read_text(encoding="utf-8"))
    lines = [
        "# Traced M-map source-pair batches from original segment 68.",
        "# OBJ uses (source_x, 0, source_y) solely as a viewer convention.",
        "# No faces, path closure, global placement, or game-axis meaning is inferred.",
    ]
    base = 1
    total_lines = 0
    for index, batch in enumerate(report["batches"], start=1):
        pairs = batch["consumed_pairs"]
        lines.append(f"o map_static_pair_batch_{index:02d}_{batch['first_pair'][1:]}")
        lines.extend(f"v {pair['xy'][0]} 0 {pair['xy'][1]}" for pair in pairs)
        if len(pairs) > 1:
            lines.append("l " + " ".join(str(base + offset) for offset in range(len(pairs))))
            total_lines += 1
        base += len(pairs)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(json.dumps({"objects": len(report["batches"]), "vertices": base - 1,
                      "lines": total_lines, "output": str(args.output)}))


if __name__ == "__main__":
    main()
