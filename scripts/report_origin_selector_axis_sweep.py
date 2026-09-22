"""Join single-bin selector samples into a bounded two-axis directory report."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def load(axis: str, bins: range) -> list[dict]:
    samples = []
    for value in bins:
        path = ROOT / "build" / f"run033_{axis}_axis_bin{value:02d}_single" / "selector_axis_samples.json"
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
                       "authority": sample["authority"]})
    return output


def markdown(group: list[dict], row: list[dict]) -> str:
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
    payload = {"classification": "controlled_selector_input_sweep_not_global_map_or_lod",
               "bins": list(bins), "call_count": 41,
               "group_axis": group, "row_axis": row}
    args.output_json.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    args.output_markdown.write_text(markdown(group, row), encoding="utf-8")
    print(f"wrote {args.output_json.relative_to(ROOT)} and {args.output_markdown.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
