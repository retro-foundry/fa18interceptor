"""Inventory unexecuted, relocation-linked scene-table candidate Hunks."""
from __future__ import annotations

import json
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TARGETS = tuple(range(41, 51))


def main() -> None:
    inventory = json.loads((ROOT / "analysis" / "hunk_inventory.json").read_text())
    resolved = json.loads((ROOT / "analysis" / "hunk_runtime_resolved.json").read_text())["resolved"]
    rows = []
    for target in TARGETS:
        segment = inventory["segments"][target]
        incoming = []
        for source in inventory["segments"]:
            for relocation in source.get("reloc32", []):
                if relocation["target_segment"] == target:
                    source_runtime = resolved.get(str(source["index"]), {})
                    incoming.extend({
                        "source_segment": source["index"],
                        "payload_offset": f"${offset:X}",
                        "runtime_extension_address": (
                            f"${source_runtime['runtime_payload_base'] + offset:06X}"
                            if "runtime_payload_base" in source_runtime else None),
                    } for offset in relocation["offsets"])
        source_counts = Counter(item["source_segment"] for item in incoming)
        segment16_table_sites = [item for item in incoming if item["source_segment"] == 16
                                 and 0x360 <= int(item["payload_offset"][1:], 16) <= 0x7A3]
        # The bounded segment-16 table uses three adjacent relocated longs per
        # record, but a record may target more than one Hunk. Keep a site count
        # per target instead of falsely treating it as a record count.
        cross_scene_sources = {str(source): count for source, count in source_counts.items()
                               if source in TARGETS and source != target}
        runtime = resolved.get(str(target), {})
        rows.append({"segment": target, "original_hunk_kind": segment["kind"],
                     "size_bytes": segment["size_bytes"], "runtime_status": runtime.get("status", "unresolved"),
                     "runtime_start": f"${runtime['runtime_payload_base']:06X}" if "runtime_payload_base" in runtime else None,
                     "static_incoming_relocation_count": len(incoming),
                     "incoming_relocation_sites": incoming,
                     "incoming_source_counts": dict(sorted(source_counts.items())),
                     "segment16_pointer_relocation_site_count": len(segment16_table_sites),
                     "cross_scene_incoming_relocation_counts": dict(sorted(cross_scene_sources.items())),
                     "classification": "candidate_scene_table_hunk",
                     "qualification": "Original CODE kind retained: relocation linkage without execution or decoded consumer semantics does not prove data."})
    output = {"scope": "segments 41-50 adjacent to verified scene-table placement",
              "policy": "Candidate table evidence is not a code-to-data reclassification.", "rows": rows}
    (ROOT / "analysis" / "scene_table_candidate_hunks.json").write_text(json.dumps(output, indent=2) + "\n")
    lines = ["# Candidate scene-table Hunks", "", output["policy"], "", "| Segment | Original kind | Runtime | Bytes | Segment-16 pointer sites | Cross-scene incoming relocations |", "| ---: | --- | --- | ---: | ---: | --- |"]
    for row in rows:
        cross = ", ".join(f"{source}:{count}" for source, count in row["cross_scene_incoming_relocation_counts"].items()) or "none"
        lines.append(f"| {row['segment']} | {row['original_hunk_kind']} | {row['runtime_start'] or row['runtime_status']} | {row['size_bytes']:,} | {row['segment16_pointer_relocation_site_count']} | {cross} |")
    lines += ["", "The segment-16 counts are relocation-backed pointer sites in the exact `$C223A8-$C227EB` inline-data table. Adjacent three-pointer records can target several Hunks, so per-target site counts must not be interpreted as record counts. Cross-scene links expose static storage families, but do not establish whether a target is instructions, tables, mixed content, or a named model. Consumer tracing is required before extraction or reclassification.", ""]
    (ROOT / "analysis" / "scene_table_candidate_hunks.md").write_text("\n".join(lines))
    print(f"wrote {len(rows)} candidate rows")


if __name__ == "__main__":
    main()
