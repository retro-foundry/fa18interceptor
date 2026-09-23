"""Find stable runtime longword references to candidate M-map packet headers."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE = 0xC00000
SEGMENT68_START, SEGMENT68_END = 0xC42CA8, 0xC444F8


def address(value: int) -> str:
    return f"${value:06X}"


def owners(resolved: dict) -> list[tuple[int, int, int, str]]:
    result = []
    for raw_segment, entry in resolved["resolved"].items():
        if entry.get("bank") != "slow" or "runtime_payload_base" not in entry or "size_bytes" not in entry:
            continue
        start = entry["runtime_payload_base"]
        result.append((start, start + entry["size_bytes"], int(raw_segment), entry["status"]))
    return result


def owner(reference: int, ranges: list[tuple[int, int, int, str]]) -> str:
    for start, end, segment, status in ranges:
        if start <= reference < end:
            return f"segment {segment} ({status}) +${reference - start:04X}"
    return "unmapped slow RAM"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--candidates", type=Path,
                        default=ROOT / "analysis/data/static_m_map_packet_streams.json")
    parser.add_argument("--baseline", type=Path,
                        default=ROOT / "captures/baseline_menu/slow.bin")
    parser.add_argument("--comparison", type=Path,
                        default=ROOT / "build/run037_m_map_stable_13f_trace/slow.bin")
    parser.add_argument("--resolved", type=Path,
                        default=ROOT / "analysis/hunk_runtime_resolved.json")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/data/m_map_packet_header_references.json")
    parser.add_argument("--replace", action="store_true")
    args = parser.parse_args()
    markdown = args.output.with_suffix(".md")
    if not args.replace and (args.output.exists() or markdown.exists()):
        raise FileExistsError(args.output)
    baseline, comparison = args.baseline.read_bytes(), args.comparison.read_bytes()
    if len(baseline) != 0x80000 or len(comparison) != 0x80000:
        raise ValueError("both slow-RAM snapshots must be 512 KiB")
    candidate_rows = json.loads(args.candidates.read_text(encoding="utf-8"))["header_candidates"]
    candidate_values = {int(row["header"][1:], 16): row for row in candidate_rows}
    ranges = owners(json.loads(args.resolved.read_text(encoding="utf-8")))
    references: dict[int, list[dict]] = {value: [] for value in candidate_values}
    for offset in range(0, len(baseline) - 3, 2):
        value = int.from_bytes(baseline[offset:offset + 4], "big") & 0xFFFFFF
        if value not in references:
            continue
        if comparison[offset:offset + 4] != baseline[offset:offset + 4]:
            continue
        reference = SLOW_BASE + offset
        if SEGMENT68_START <= reference < SEGMENT68_END:
            continue
        references[value].append({"reference": address(reference), "owner": owner(reference, ranges)})
    rows = []
    for value, candidate in sorted(candidate_values.items()):
        if references[value]:
            rows.append({"header": address(value), "observed_direct_entry": candidate["observed_direct_entry"],
                         "observed_selector_sample": candidate["observed_selector_sample"],
                         "stable_external_references": references[value]})
    report = {
        "classification": "stable_runtime_longword_reference_scan_for_m_map_packet_header_candidates",
        "snapshots": {"baseline": str(args.baseline), "comparison": str(args.comparison)},
        "scan": "all even-aligned slow-RAM longwords identical in both snapshots; segment-68 self-references excluded",
        "candidate_count": len(candidate_values), "referenced_candidate_count": len(rows), "references": rows,
        "qualification": ("A match proves only a stable longword equal to a candidate header. It does not prove the "
                          "referencer executes, is a map selector, or that an unreferenced candidate is invalid; "
                          "relative-offset tables and computed pointers are outside this scan.")}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# Stable references to M-map packet-header candidates", "", "Classification: **static longword-reference scan**.", "", report["qualification"], "",
             f"The scan checks {report['candidate_count']} candidate headers in two snapshots. It finds stable external longword references for {report['referenced_candidate_count']} candidates.", "",
             "| Header | Live direct entry | Live selector sample | Stable external references |", "| --- | --- | --- | --- |"]
    for row in rows:
        refs = "; ".join(f"`{ref['reference']}` ({ref['owner']})" for ref in row["stable_external_references"])
        lines.append(f"| `{row['header']}` | {row['observed_direct_entry']} | {row['observed_selector_sample']} | {refs} |")
    if not rows:
        lines.append("| none | n/a | n/a | none |")
    lines.append("")
    markdown.write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"candidates": len(candidate_values), "referenced_candidates": len(rows),
                      "references": sum(len(row["stable_external_references"]) for row in rows)}))


if __name__ == "__main__":
    main()
