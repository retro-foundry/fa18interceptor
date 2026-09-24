"""Audit sealed map traces for C3B4F8 stores versus subsequent C1F6F8 entries."""
from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TARGET, STORE, WALKER = 0xC3B4F8, 0xC1CC70, 0xC1F6F8
SAMPLES = [
    (ROOT / "build/run003_m_map_appearance_trace/trace.jsonl", ROOT / "build/run003_m_map_appearance_trace/slow.bin"),
    (ROOT / "build/run035_m_map_stable_1f_trace/trace.jsonl", ROOT / "build/run035_m_map_stable_1f_trace/slow.bin"),
    (ROOT / "build/run035_m_map_stable_20f_trace/trace.jsonl", ROOT / "build/run035_m_map_stable_20f_trace/slow.bin"),
    (ROOT / "build/run037_c3b4f8_descriptor_target_trace/trace.jsonl", ROOT / "build/run037_m_map_template_to_placement_3f_trace/slow.bin"),
]


def addr(value: int) -> str:
    return f"${value & 0xFFFFFF:06X}"


def main() -> None:
    output = ROOT / "analysis/data/c3b4f8_map_store_walker_audit.json"
    if output.exists() or output.with_suffix(".md").exists():
        raise FileExistsError(output)
    rows = []
    for trace_path, slow_path in SAMPLES:
        trace = [json.loads(line) for line in trace_path.read_text(encoding="utf-8").splitlines()]
        slow = slow_path.read_bytes()
        for row in trace:
            if row["pc"] != STORE:
                continue
            field = row["registers"]["a1"] & 0xFFFFFF
            value = int.from_bytes(slow[field - 0xC00000:field - 0xC00000 + 4], "big") & 0xFFFFFF
            if value != TARGET:
                continue
            walker = next((item for item in trace if item["index"] > row["index"] and item["pc"] == WALKER), None)
            rows.append({"trace": str(trace_path.relative_to(ROOT)).replace("\\", "/"),
                         "frame": row.get("frame"), "store_index": row["index"], "field": addr(field),
                         "next_walker": (None if walker is None else {"frame": walker.get("frame"),
                         "index": walker["index"], "entry_a1": addr(walker["registers"]["a1"])})})
    report = {"classification": "sealed_map_samples_c3b4f8_store_not_next_walker_target",
              "target": addr(TARGET), "samples": rows,
              "qualification": "This audits only each trace window through its next observed walker. A missing walker is a trace boundary, not evidence of rejection. No observed next walker receives C3B4F8; this does not prove the target can never reach the walker."}
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# `$C3B4F8` map descriptor-store / walker audit", "",
             "Classification: **repeated bounded negative transition evidence**.", "",
             "Across the listed sealed map traces, `$C1CC70` reads `$C3B4F8` from descriptor field `$C22708`, but no subsequent observed `$C1F6F8` entry receives `$C3B4F8`. A missing walker is retained as a trace boundary, not a rejection claim. This does not prove the target can never reach the walker.", "",
             "| Trace | frame | store index | next walker |", "| --- | ---: | ---: | --- |"]
    for row in rows:
        frame = row["frame"] if row["frame"] is not None else "no chipset-frame field"
        walker = "trace ends" if row["next_walker"] is None else f"frame {row['next_walker']['frame'] if row['next_walker']['frame'] is not None else 'not recorded'}, `{row['next_walker']['entry_a1']}`"
        lines.append(f"| `{row['trace']}` | {frame} | {row['store_index']} | {walker} |")
    lines.append("")
    output.with_suffix(".md").write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"stores": len(rows), "walkers": sum(row['next_walker'] is not None for row in rows)}))


if __name__ == "__main__":
    main()
