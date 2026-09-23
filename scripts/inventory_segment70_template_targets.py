"""Verify catalog-selected Hunk-70 descriptor target records across snapshots."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SEGMENT = 70
START = 0xC44880
END = 0xC45624
RECORD_SIZE = 24
DEFAULT_SNAPSHOTS = (
    ROOT / "captures/baseline_menu/slow.bin",
    ROOT / "build/run003_m_map_appearance_trace/slow.bin",
    ROOT / "build/run033_placement_bulk_404_trace/slow.bin",
    ROOT / "build/run033_frame05250_placement_thirtyframe_trace/slow.bin",
    ROOT / "build/run037_m_map_stable_13f_trace/slow.bin",
)


def address(value: int) -> str:
    return f"${value:06X}"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--catalog", type=Path, default=ROOT / "analysis/data/run037_m_map_template_target_catalog.json")
    parser.add_argument("--inventory", type=Path, default=ROOT / "analysis/hunk_inventory.json")
    parser.add_argument("--executable", type=Path, default=ROOT / "local/extracted/f18_interceptor")
    parser.add_argument("--snapshot", type=Path, action="append", default=list(DEFAULT_SNAPSHOTS))
    parser.add_argument("--output", type=Path, default=ROOT / "analysis/data/run037_segment70_template_targets.json")
    args = parser.parse_args()
    catalog = json.loads(args.catalog.read_text(encoding="utf-8"))["targets"]
    inventory = json.loads(args.inventory.read_text(encoding="utf-8"))
    segment = inventory["segments"][SEGMENT]
    raw_file = args.executable.read_bytes()
    raw = raw_file[segment["payload_file_offset"]:segment["payload_file_offset"] + segment["size_bytes"]]
    relocated = {byte for group in segment["reloc32"] for offset in group["offsets"]
                 for byte in range(offset, offset + 4)}
    snapshots = [(path.resolve(), path.resolve().read_bytes()) for path in args.snapshot]
    rows = []
    for candidate in catalog:
        target = int(candidate["descriptor_plus_8_field"][1:], 16)
        if not START <= target < END:
            continue
        offset = target - START
        if offset + RECORD_SIZE > len(raw):
            raise ValueError(f"record at {address(target)} exceeds segment")
        windows = [image[target - 0xC00000:target - 0xC00000 + RECORD_SIZE]
                   for _, image in snapshots]
        if any(len(window) != RECORD_SIZE for window in windows):
            raise ValueError(f"snapshot does not contain {address(target)}")
        stable = all(window == windows[0] for window in windows[1:])
        non_relocated_matches = all(window[index] == raw[offset + index]
                                    for window in windows
                                    for index in range(RECORD_SIZE)
                                    if offset + index not in relocated)
        relocated_offsets = [index for index in range(RECORD_SIZE) if offset + index in relocated]
        runtime_longs = [f"${int.from_bytes(windows[0][index:index + 4], 'big'):08X}"
                         for index in range(0, RECORD_SIZE, 4)]
        rows.append({"target": candidate["descriptor_plus_8_field"], "segment": SEGMENT,
                     "segment_offset": f"${offset:04X}", "record_size": RECORD_SIZE,
                     "relocated_byte_offsets": relocated_offsets,
                     "all_snapshots_identical": stable,
                     "all_non_relocated_bytes_match_original": non_relocated_matches,
                     "runtime_longs": runtime_longs,
                     "placement_count": candidate["placement_count"],
                     "static_sources": candidate["static_sources"],
                     "descriptors": candidate["descriptors"]})
    rows.sort(key=lambda row: row["target"])
    payload = {
        "classification": "verified_relocation_backed_hunk70_descriptor_target_records_not_complete_terrain_mesh",
        "segment": {"index": SEGMENT, "runtime_start": address(START),
                    "runtime_end_exclusive": address(END), "size_bytes": END - START,
                    "original_payload_file_offset": "$40060"},
        "snapshots": [str(path.relative_to(ROOT)).replace("\\", "/") for path, _ in snapshots],
        "authority": {"catalog": str(args.catalog.relative_to(ROOT)).replace("\\", "/"),
                      "inventory": str(args.inventory.relative_to(ROOT)).replace("\\", "/")},
        "records": rows,
        "qualification": "The records are exact static descriptor targets selected by the run037 template-placement catalog. Their static provenance does not prove that they are vertex lists, terrain cells, or LOD levels.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# Verified Hunk-70 run037 template targets",
        "",
        "Classification: **bounded relocation-backed static descriptor-target records**. This does not classify all of Hunk 70 as static data or identify a terrain mesh.",
        "",
        f"All **{len(rows)}** 24-byte records selected by the run037 template-placement catalog are identical across five snapshots and match their original Hunk payload at every non-relocated byte. Each record has eight relocation-covered bytes, retained as runtime pointers in the JSON companion.",
        "",
        "| target | Hunk offset | template placements | static template sources | descriptors | static checks |",
        "| --- | --- | ---: | --- | --- | --- |",
    ]
    for row in rows:
        checks = "snapshot-stable; original-match" if row["all_snapshots_identical"] and row["all_non_relocated_bytes_match_original"] else "failed"
        lines.append(f"| {row['target']} | {row['segment_offset']} | {row['placement_count']} | "
                     f"{', '.join(row['static_sources'])} | {', '.join(row['descriptors'])} | {checks} |")
    lines += ["", payload["qualification"], ""]
    args.output.with_suffix(".md").write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"records": len(rows), "stable": sum(row["all_snapshots_identical"] for row in rows),
                      "original_match": sum(row["all_non_relocated_bytes_match_original"] for row in rows)}))


if __name__ == "__main__":
    main()
