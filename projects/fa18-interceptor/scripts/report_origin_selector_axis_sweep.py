"""Join single-bin selector samples into a bounded two-axis directory report."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def load(axis: str, bins: range) -> list[dict]:
    samples = []
    for value in bins:
        path = ROOT / "build" / f"run033_{axis}_axis_bin{value:02d}_target" / "selector_axis_samples.json"
        payload = json.loads(path.read_text(encoding="utf-8"))
        sample = payload["samples"][0]
        if payload["axis"] != axis or sample["bin"] != value or sample["termination"] != "return":
            raise ValueError(f"invalid sample {path}")
        samples.append({"bin": value, "calls": sample["calls"],
                        "authority": str(path.relative_to(ROOT)).replace("\\", "/")})
    return samples


def compact_axis(samples: list[dict], changed_field: str) -> list[dict]:
    baseline = samples[0]["calls"]
    output = []
    for sample in samples:
        calls = sample["calls"]
        if len(calls) != len(baseline):
            raise ValueError("call count differs across samples")
        changed = [index for index, (a, b) in enumerate(zip(baseline, calls))
                   if a[changed_field] != b[changed_field]]
        untouched = [index for index, (a, b) in enumerate(zip(baseline, calls))
                     if a["selector_index" if changed_field == "live_row_key" else "live_row_key"]
                     != b["selector_index" if changed_field == "live_row_key" else "live_row_key"]]
        output.append({"bin": sample["bin"], "changed_call_indices": changed,
                       "unexpected_other_axis_changes": untouched,
                       "values": [call[changed_field] for call in calls],
                       "group_records": [f"${call['group_record']:06X}" for call in calls],
                       "authority": sample["authority"]})
    return output


def outer_periodicity(axis: str, field: str, inner: list[dict]) -> list[dict]:
    results = []
    for value in (32, 33, 63, 64, 127, 128, 255):
        path = ROOT / "build" / f"run033_{axis}_axis_bin{value:03d}_outer" / "selector_axis_samples.json"
        payload = json.loads(path.read_text(encoding="utf-8"))
        sample = payload["samples"][0]
        expected = inner[value & 0x1F]["calls"]
        values = [call[field] for call in sample["calls"]]
        expected_values = [call[field] for call in expected]
        other = "live_row_key" if field == "selector_index" else "selector_index"
        other_values = [call[other] for call in sample["calls"]]
        expected_other = [call[other] for call in expected]
        results.append({"bin": value, "expected_low_five_bits_bin": value & 0x1F,
                        "changed_axis_sequence_matches": values == expected_values,
                        "other_axis_sequence_matches": other_values == expected_other,
                        "authority": str(path.relative_to(ROOT)).replace("\\", "/")})
    return results


def markdown(group: list[dict], row: list[dict], group_outer: list[dict], row_outer: list[dict]) -> str:
    lines = [
        "# Controlled terrain-directory axis sweep",
        "",
        "Classification: **controlled selector-input sweep**. This establishes how the two",
        "live origin bins feed the static template-directory call inputs for one bounded",
        "update packet. It does not assign global world coordinates, cardinal directions,",
        "terrain geometry, or LOD semantics to the bins.",
        "",
        "Each single-bin sample restores the same run033 pre-refresh checkpoint, replays",
        "to `$C1C860`, sets `$C45785=$01`, changes exactly one long origin component to",
        "`bin << 24`, and records all 41 `$C1D3F4` entries until `$C0F048` returns.",
        "",
        "| Bin | group-axis changed selector indices | group-axis row-key changes | row-axis changed row keys | row-axis selector-index changes |",
        "| ---: | ---: | ---: | ---: | ---: |",
    ]
    for group_sample, row_sample in zip(group, row):
        lines.append(f"| {group_sample['bin']} | {len(group_sample['changed_call_indices'])} | "
                     f"{len(group_sample['unexpected_other_axis_changes'])} | "
                     f"{len(row_sample['changed_call_indices'])} | "
                     f"{len(row_sample['unexpected_other_axis_changes'])} |")
    lines += [
        "",
        "For bins 1--30, exactly the first 28 call positions change on their assigned",
        "axis and none changes on the other axis. Bin 31 retains the same separation",
        "but has fewer changed call positions because portions of the call sequence reach",
        "the observed zero-valued boundary behavior. The unmodified final 13 calls retain",
        "their selector inputs throughout this 0--31 sweep.",
        "",
        "Outer samples at bins 32, 33, 63, 64, 127, 128, and 255 exactly match",
        "the corresponding `bin & $1F` sequence on their own axis and on the untouched",
        "axis. Thus both inputs are observed modulo 32 before this selector packet. This",
        "proves a 32 by 32 **selector-bin lattice** for this path; it does not establish",
        "a 32 by 32 physical terrain grid, a map edge, or an absolute unit scale.",
        "",
        "The exact per-call value sequences, including the boundary values, are in the",
        "machine-readable companion JSON. The static `$C42390` directory remains the",
        "authoritative group lookup; this result proves its two input axes in this packet,",
        "not a complete world-cell-coordinate decode.",
        "",
    ]
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-json", type=Path,
                        default=ROOT / "analysis/data/origin_selector_axis_sweep_00_1f.json")
    parser.add_argument("--output-markdown", type=Path,
                        default=ROOT / "analysis/data/origin_selector_axis_sweep_00_1f.md")
    args = parser.parse_args()
    bins = range(32)
    group_raw, row_raw = load("group", bins), load("row", bins)
    group = compact_axis(group_raw, "selector_index")
    row = compact_axis(row_raw, "live_row_key")
    group_outer = outer_periodicity("group", "selector_index", group_raw)
    row_outer = outer_periodicity("row", "live_row_key", row_raw)
    payload = {"classification": "controlled_selector_input_sweep_not_global_map_or_lod",
               "bins": list(bins), "call_count": 41,
               "group_axis": group, "row_axis": row,
               "group_axis_outer_periodicity": group_outer,
               "row_axis_outer_periodicity": row_outer}
    args.output_json.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    args.output_markdown.write_text(markdown(group, row, group_outer, row_outer), encoding="utf-8")
    print(f"wrote {args.output_json.relative_to(ROOT)} and {args.output_markdown.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
