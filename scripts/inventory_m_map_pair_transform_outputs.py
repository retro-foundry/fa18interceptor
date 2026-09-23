"""Calculate M-map pair-transform workspace triples from trace state.

This is a narrow arithmetic oracle for the byte-exact $C2AF9C transform loop.
It retains immutable pair inputs separately from the calculated mutable
three-word renderer workspace results. The matrix words come from the paired
trace snapshot; this script does not read the transient workspace itself.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE = 0xC00000
PAIR_X, PAIR_Y, DISPLAY_STAGE = 0xC2AF9C, 0xC2AF9E, 0xC2AFE2


def signed_word(value: int) -> int:
    value &= 0xFFFF
    return value - 0x10000 if value & 0x8000 else value


def word(memory: bytes, address: int) -> int:
    return int.from_bytes(memory[address - SLOW_BASE:address - SLOW_BASE + 2], "big", signed=True)


def address(value: int) -> str:
    return f"${value & 0xFFFFFF:06X}"


def asl_word(value: int, count: int) -> int:
    return signed_word((value & 0xFFFF) << (count & 63))


def transform(row: dict, memory: bytes) -> dict:
    registers = row["registers"]
    pair_address = registers["a3"] & 0xFFFFFF
    source_pair = [word(memory, pair_address), word(memory, pair_address + 2)]
    d0 = registers["d0"]
    coordinates = [signed_word((d0 >> 16) + source_pair[0]),
                   signed_word(d0 + source_pair[1])]
    matrix_base = registers["a0"] & 0xFFFFFF
    matrix = [word(memory, matrix_base + offset) for offset in (0, 4, 6, 10)]
    shift = registers["d3"] & 0xFFFF
    first = asl_word(((coordinates[0] * matrix[0] + coordinates[1] * matrix[1]) >> 8)
                     + signed_word(registers["d6"]), shift)
    second = asl_word(((coordinates[0] * matrix[2] + coordinates[1] * matrix[3]) >> 8)
                      + signed_word(registers["a2"]), shift)
    # The final pair uses the next two words after the earlier post-incremented reads.
    third_matrix = [word(memory, matrix_base + offset) for offset in (12, 16)]
    third = asl_word(((coordinates[0] * third_matrix[0] + coordinates[1] * third_matrix[1]) >> 8)
                     + signed_word(registers["a4"]), shift)
    return {
        "trace_index": row["index"], "frame": row["frame"],
        "source_address": address(pair_address), "source_pair": source_pair,
        "base_pair_from_d0": [signed_word(d0 >> 16), signed_word(d0)],
        "translated_pair": coordinates, "matrix_base": address(matrix_base),
        "matrix_words": matrix + third_matrix, "detail_shift": shift,
        "workspace_address": address(registers["a5"]),
        "workspace_triple": [first, second, third],
    }


def markdown(report: dict) -> str:
    return "\n".join([
        "# M-map pair-transform workspace outputs",
        "",
        "Classification: **trace-state arithmetic calculation from a byte-exact renderer transform**.",
        "",
        "The immutable segment-68 inputs remain signed two-word pairs. For each observed",
        "`$C2AF9C` read, this report reproduces the three signed words that the exact",
        "`$C2AF9C-$C2AFF7` loop calculates for writes through `A5`, using the instruction",
        "register state and the paired snapshot's matrix words. It demonstrates renderer-space triple",
        "formation, not stored elevation or global map coordinates.",
        "",
        f"Observed pair transforms: {report['pair_transforms']}; display-stage entries: {report['display_stage_entries']}.",
        f"All {report['a5_stride_matches']} adjacent same-batch output addresses advance by six bytes,",
        "as required by three 16-bit workspace writes per pair.",
        "",
        "| property | observed range |",
        "| --- | --- |",
        f"| source X | {report['ranges']['source_pair'][0]}..{report['ranges']['source_pair'][1]} |",
        f"| source Y | {report['ranges']['source_pair'][2]}..{report['ranges']['source_pair'][3]} |",
        f"| output word 0 | {report['ranges']['workspace_triple'][0]}..{report['ranges']['workspace_triple'][1]} |",
        f"| output word 1 | {report['ranges']['workspace_triple'][2]}..{report['ranges']['workspace_triple'][3]} |",
        f"| output word 2 (computed) | {report['ranges']['workspace_triple'][4]}..{report['ranges']['workspace_triple'][5]} |",
        "",
        "The JSON companion retains every input/calculated-output row, including the live pair translation,",
        "snapshot matrix words, detail shift, and target workspace address. The workspace itself is",
        "mutable and not sampled at each write, so these calculated triples must not be exported as",
        "immutable terrain vertices.",
        "",
    ])


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, required=True)
    parser.add_argument("--slow", type=Path, required=True)
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/data/run003_m_map_pair_transform_outputs.json")
    parser.add_argument("--replace", action="store_true")
    args = parser.parse_args()
    markdown_path = args.output.with_suffix(".md")
    if not args.replace and (args.output.exists() or markdown_path.exists()):
        raise FileExistsError(args.output)
    memory = args.slow.read_bytes()
    trace = [json.loads(line) for line in args.trace.read_text(encoding="utf-8").splitlines()]
    rows = [transform(row, memory) for row in trace if row["pc"] == PAIR_X]
    if not rows:
        raise ValueError("trace contains no C2AF9C pair-transform entries")
    display_indices = [row["index"] for row in trace if row["pc"] == DISPLAY_STAGE]
    adjacent_pairs = [(previous, current) for previous, current in zip(rows, rows[1:])
                      if not any(previous["trace_index"] < stage < current["trace_index"]
                                 for stage in display_indices)]
    stride_matches = sum(
        int(current["workspace_address"][1:], 16) == int(previous["workspace_address"][1:], 16) + 6
        for previous, current in adjacent_pairs
    )
    if stride_matches != len(adjacent_pairs):
        raise ValueError(f"{len(adjacent_pairs) - stride_matches} in-batch A5 strides are not six bytes")
    source = [value for row in rows for value in row["source_pair"]]
    output = [value for row in rows for value in row["workspace_triple"]]
    report = {
        "classification": "trace_state_calculation_of_m_map_pair_to_mutable_renderer_triple",
        "trace": str(args.trace.resolve().relative_to(ROOT)),
        "slow": str(args.slow.resolve().relative_to(ROOT)),
        "pair_transforms": len(rows),
        "display_stage_entries": sum(row["pc"] == DISPLAY_STAGE for row in trace),
        "a5_stride_matches": stride_matches,
        "a5_stride_candidates": len(adjacent_pairs),
        "ranges": {"source_pair": [min(source[::2]), max(source[::2]), min(source[1::2]), max(source[1::2])],
                   "workspace_triple": [min(output[::3]), max(output[::3]), min(output[1::3]), max(output[1::3]), min(output[2::3]), max(output[2::3])]},
        "rows": rows,
        "qualification": "Rows calculate expected mutable renderer workspace outputs from immutable pairs using instruction registers and paired snapshot matrix words. The transient workspace was not read at each write, so this is not a direct memory-output sample; the rows are neither stored terrain triples nor global map coordinates.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    markdown_path.write_text(markdown(report) + "\n", encoding="utf-8")
    print(json.dumps({"pairs": len(rows), "stride_matches": stride_matches}))


if __name__ == "__main__":
    main()
