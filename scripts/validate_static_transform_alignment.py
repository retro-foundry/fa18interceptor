"""Check sampled transformed triples against a static source run and a sampled 3x3 matrix."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


def signed_word(value: int) -> int:
    return value - 0x10000 if value >= 0x8000 else value


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--samples", type=Path, required=True)
    parser.add_argument("--memory", type=Path, required=True,
                        help="slow-RAM image whose runtime base is C00000")
    parser.add_argument("--source", type=lambda text: int(text, 0), required=True)
    parser.add_argument("--output", type=lambda text: int(text, 0), required=True)
    parser.add_argument("--matrix", type=lambda text: int(text, 0), required=True)
    parser.add_argument("--count", type=int, required=True)
    parser.add_argument("--tolerance", type=int, default=4)
    parser.add_argument("--report", type=Path, required=True)
    args = parser.parse_args()
    if args.report.exists():
        raise FileExistsError(args.report)

    sample = json.loads(args.samples.read_text(encoding="utf-8"))["records"][-1]["words"]
    words = {int(address, 16): signed_word(value) for address, value in sample.items()}
    memory = args.memory.read_bytes()

    matrix = [words[args.matrix + index * 2] for index in range(9)]
    rows = []
    for index in range(args.count):
        address = args.source + index * 6 - 0xC00000
        source = tuple(int.from_bytes(memory[address + axis * 2:address + axis * 2 + 2], "big", signed=True)
                       for axis in range(3))
        actual = tuple(words[args.output + index * 6 + axis * 2] for axis in range(3))
        expected = tuple(sum(source[column] * matrix[row * 3 + column] for column in range(3)) >> 8
                         for row in range(3))
        residual = tuple(actual[axis] - expected[axis] for axis in range(3))
        rows.append({"index": index, "source": source, "actual": actual, "expected": expected,
                     "residual": residual, "within_tolerance": max(map(abs, residual)) <= args.tolerance})

    report = {
        "scope": "fixed-point 3x3 static-source to sampled-output alignment",
        "samples": str(args.samples), "memory": str(args.memory),
        "source": f"${args.source:06X}", "output": f"${args.output:06X}",
        "matrix": f"${args.matrix:06X}", "matrix_words": matrix,
        "count": args.count, "tolerance": args.tolerance, "rows": rows,
        "matching_rows": sum(row["within_tolerance"] for row in rows),
        "qualification": "This compares snapshots after a replay frame. It corroborates a source-to-output transform relationship but does not identify the CPU instruction that wrote the output or explain rows outside tolerance.",
    }
    args.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"matching_rows": report["matching_rows"], "count": args.count,
                      "report": str(args.report)}))


if __name__ == "__main__":
    main()
