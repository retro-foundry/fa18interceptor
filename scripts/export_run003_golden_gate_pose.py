"""Export the trace-observed pre-projection pose of run003 Golden Gate lines."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BASE = 0xC00000
MATRIX = 0xC45BD8
TRANSFORM = 0xC1F4AC
SOURCES = {0xC35BAA: "$C3559A", 0xC35BC2: "$C355D2"}


def signed_word(value: int) -> int:
    value &= 0xFFFF
    return value - 0x10000 if value & 0x8000 else value


def words(memory: bytes, address: int, count: int) -> list[int]:
    offset = address - BASE
    return [int.from_bytes(memory[offset + index:offset + index + 2], "big", signed=True)
            for index in range(0, count * 2, 2)]


def matrix_product(vector: list[int], matrix: list[int]) -> list[int]:
    return [sum(vector[column] * matrix[row * 3 + column] for column in range(3)) >> 8
            for row in range(3)]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, default=ROOT / "build/run003_m_map_appearance_trace/trace.jsonl")
    parser.add_argument("--slow", type=Path, default=ROOT / "build/run003_m_map_appearance_trace/slow.bin")
    parser.add_argument("--output", type=Path, default=ROOT / "analysis/data/run003_golden_gate_view_pose.json")
    args = parser.parse_args()
    if args.output.exists() or args.output.with_suffix(".md").exists():
        raise FileExistsError(args.output)
    memory = args.slow.read_bytes()
    matrix = words(memory, MATRIX, 9)
    trace = [json.loads(line) for line in args.trace.read_text(encoding="utf-8").splitlines()]
    instances = []
    for row in trace:
        if row["pc"] != TRANSFORM:
            continue
        source = row["registers"]["a1"] & 0xFFFFFF
        context = SOURCES.get(source)
        if context is None:
            continue
        shift = signed_word(row["registers"]["d7"])
        if shift < 0 or shift > 15:
            raise ValueError(f"unexpected local shift {shift}")
        live = [signed_word(row["registers"]["d0"]), signed_word(row["registers"]["a5"]),
                signed_word(row["registers"]["d1"])]
        local = [words(memory, source + index * 6, 3) for index in range(2)]
        pre_matrix = [[(component >> shift) + live[axis] for axis, component in enumerate(triple)]
                      for triple in local]
        instances.append({"context": context, "source": f"${source:06X}", "trace_index": row["index"],
                          "local_shift": shift, "live_terms": live, "local_triples": local,
                          "pre_matrix_triples": pre_matrix,
                          "matrix_output_triples": [matrix_product(triple, matrix) for triple in pre_matrix]})
    if {item["context"] for item in instances} != set(SOURCES.values()) or len(instances) != 2:
        raise ValueError("expected exactly one observed pose for each Golden Gate line context")
    payload = {"classification": "run003_golden_gate_view_space_pose_not_static_world_geometry",
               "matrix_address": "$C45BD8", "matrix_words": matrix, "instances": instances,
               "authority": {"trace": str(args.trace.resolve().relative_to(ROOT)).replace("\\", "/"),
                             "slow": str(args.slow.resolve().relative_to(ROOT)).replace("\\", "/")},
               "qualification": "Each pose applies the byte-exact C1F4AC local-shift, live-term addition, and signed matrix product. The output is view-dependent renderer workspace geometry, not static global bridge vertices."}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    lines = ["# Run003 Golden Gate view-space pose", "",
             "Classification: **trace-observed view-space pose, not static world geometry**.", "",
             f"Matrix `$C45BD8`: `{matrix}`.", ""]
    for item in instances:
        lines += [f"## `{item['context']}` from `{item['source']}`", "",
                  f"Trace index `{item['trace_index']}`; local shift `{item['local_shift']}`; live terms `{item['live_terms']}`.", "",
                  "| local triple | pre-matrix triple | matrix output |", "| --- | --- | --- |"]
        lines += [f"| {local} | {pre} | {out} |" for local, pre, out in zip(item["local_triples"], item["pre_matrix_triples"], item["matrix_output_triples"])]
        lines.append("")
    lines += [payload["qualification"], ""]
    args.output.with_suffix(".md").write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"instances": len(instances), "matrix": matrix}))


if __name__ == "__main__":
    main()
