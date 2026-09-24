"""Record the run041 selector-origin invariance across its detail transition.

This is a negative physical-range probe: matching origin inputs cannot prove a
physical range or LOD rule, but they exclude an origin-state change as the
observed source-family switch's explanation at these four checkpoints.
"""
import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE = 0xC00000
LONG_FIELDS = (0xC45C3E, 0xC45C42, 0xC45C46)
BYTE_FIELDS = (0xC457B6, 0xC458AE, 0xC458B2, 0xC45785)
CHECKPOINTS = (
    (5000, ROOT / "build/run041_frame05000_12f_trace/slow.bin",
     ROOT / "build/run041_frame05000_12f_trace/trace.jsonl"),
    (5750, ROOT / "build/run041_frame05750_checkpoint/slow.bin", None),
    (6000, ROOT / "build/run041_frame06000_checkpoint/slow.bin", None),
    (6250, ROOT / "build/run041_frame06250_12f_trace_retry/slow.bin",
     ROOT / "build/run041_frame06250_12f_trace_retry/trace.jsonl"),
)


def read_value(slow, address, size):
    offset = address - SLOW_BASE
    return int.from_bytes(slow[offset:offset + size], "big")


def count_pcs(trace, start, end):
    if trace is None:
        return None
    count = 0
    with trace.open(encoding="utf-8") as handle:
        for line in handle:
            pc = json.loads(line)["pc"]
            if start <= pc <= end:
                count += 1
    return count


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--json-output", type=Path,
                        default=ROOT / "analysis/data/run041_origin_invariance.json")
    parser.add_argument("--report-output", type=Path,
                        default=ROOT / "analysis/data/run041_origin_invariance.md")
    args = parser.parse_args()

    rows = []
    for frame, slow_path, trace_path in CHECKPOINTS:
        slow = slow_path.read_bytes()
        values = {f"${address:06X}": read_value(slow, address, 4)
                  for address in LONG_FIELDS}
        values.update({f"${address:06X}": read_value(slow, address, 1)
                       for address in BYTE_FIELDS})
        rows.append({"frame": frame, "slow": str(slow_path.relative_to(ROOT)),
                     "trace": str(trace_path.relative_to(ROOT)) if trace_path else None,
                     "values": values,
                     "origin_producer_instruction_count": count_pcs(trace_path, 0xC29042, 0xC295D0),
                     "template_selection_instruction_count": count_pcs(trace_path, 0xC1D10C, 0xC1DC08)})

    invariant = all(row["values"] == rows[0]["values"] for row in rows[1:])
    report = {
        "classification": "scenario_backed_origin_invariance_not_physical_distance_or_lod",
        "invariant": invariant,
        "fields": [f"${address:06X}" for address in LONG_FIELDS + BYTE_FIELDS],
        "rows": rows,
        "conclusion": ("The observed run041 source-family transition occurs with identical "
                       "sampled selector-origin state. This excludes an origin-state change "
                       "as its explanation, but does not measure physical range or prove LOD."),
    }
    args.json_output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")

    fields = report["fields"]
    lines = ["# Run041 selector-origin invariance", "",
             "Classification: **scenario-backed selector-origin invariance; not physical-distance LOD proof**.",
             "", "The four saved run041 checkpoints span the measured red-Golden-Gate growth and the documented renderer source-family transition. Their sampled selector-origin state is identical. The two bounded trace windows nevertheless execute the downstream `$C1D10C-$C1DC08` template-selection region.",
             "", "| Frame | " + " | ".join(fields) + " | `$C29042-$C295D0` trace instructions | `$C1D10C-$C1DC08` trace instructions |",
             "| ---: | " + " | ".join(["---:"] * len(fields)) + " | ---: | ---: |"]
    for row in rows:
        values = [str(row["values"][field]) for field in fields]
        producer = "--" if row["origin_producer_instruction_count"] is None else str(row["origin_producer_instruction_count"])
        selector = "--" if row["template_selection_instruction_count"] is None else str(row["template_selection_instruction_count"])
        lines.append("| " + str(row["frame"]) + " | " + " | ".join(values) + f" | {producer} | {selector} |")
    lines += ["", "## Result", "",
              "The sampled `$C45C3E/$C45C42/$C45C46` selector-origin triple, `$C457B6` adjustment mode, `$C458AE` detail mode, `$C458B2` detail index, and `$C45785` enable byte are identical at frames 5,000, 5,750, 6,000, and 6,250. The producer range does not execute in either bounded trace, while the downstream template-selection region executes in both.",
              "", "This rules out a change in these sampled selector-origin inputs as the cause of the run041 source-family change. It does **not** measure aircraft-to-landmark range, establish a renderer selector threshold, isolate all camera state, or prove a physical-distance LOD rule.",
              "", "Authority: sealed `captures/run041` replay checkpoints and the listed ignored build artifacts. Reproduce with `python scripts/summarize_run041_origin_invariance.py`.", ""]
    args.report_output.write_text("\n".join(lines), encoding="utf-8")
    print(f"wrote {args.json_output.relative_to(ROOT)}")
    print(f"wrote {args.report_output.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
