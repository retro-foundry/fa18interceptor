"""Report immutable model-input candidates separately from mutable geometry workspaces.

This is deliberately a boundary report, not a model-name generator.  A table is
only called immutable when it matches its original Hunk payload after relocation
sites are excluded.  The projection consumer table at C48390 is tested against
several snapshots specifically to prevent transformed geometry from being
mistaken for a source model.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SAMPLES = (
    ("baseline menu", ROOT / "captures" / "baseline_menu" / "slow.bin"),
    ("attract frame 600", ROOT / "build" / "attract_focus_600" / "slow.bin"),
    ("attract frame 1800", ROOT / "build" / "attract_focus_1800" / "slow.bin"),
    ("Golden Gate frame 12000", ROOT / "build" / "run031_frame12000_golden_gate_c1f6f8_probe" / "slow.bin"),
    ("bridge silhouette frame 14500", ROOT / "build" / "run031_frame14500_c1f6f8_probe" / "slow.bin"),
)
SUBDIVISION_TRACE = ROOT / "build" / "run031_frame7500_c1f6f8_probe" / "trace.jsonl"


def address(value: str | int) -> int:
    if isinstance(value, int):
        return value
    return int(value.removeprefix("$"), 16)


def original_payload(executable: bytes, segment: dict) -> bytes:
    start = segment["payload_file_offset"]
    return executable[start:start + segment["size_bytes"]]


def relocation_offsets(segment: dict) -> set[int]:
    # A relocation replaces a complete big-endian longword, not only its first
    # byte. Exclude the entire patched operand when comparing source payloads.
    return {offset + byte for group in segment["reloc32"] for offset in group["offsets"]
            for byte in range(4)}


def snapshot_comparisons(payload: bytes, base: int, relocations: set[int]) -> list[dict]:
    rows = []
    for label, path in SAMPLES:
        if not path.is_file():
            continue
        memory = path.read_bytes()
        captured = memory[base - 0xC00000:base - 0xC00000 + len(payload)]
        changed = [offset for offset, (left, right) in enumerate(zip(payload, captured))
                   if left != right and offset not in relocations]
        rows.append({"capture": label, "path": str(path.relative_to(ROOT)),
                     "non_relocation_difference_bytes": len(changed),
                     "captured_sha256": hashlib.sha256(captured).hexdigest()})
    return rows


def signed_word(value: int) -> int:
    value &= 0xffff
    return value - 0x10000 if value & 0x8000 else value


def observed_subdivision() -> dict:
    rows = [json.loads(line) for line in SUBDIVISION_TRACE.read_text().splitlines()]
    # C21C4C loads D0-D5 from the selected record.  The following C21C52
    # rows therefore carry the actual pre-subdivision six-word values.
    first = next(row for row in rows if row["pc"] == 0xC21C52)
    second = next(row for row in rows if row["pc"] == 0xC21C52 and row["index"] > first["index"])
    first_registers = first["registers"]
    second_registers = second["registers"]
    return {
        "trace": str(SUBDIVISION_TRACE.relative_to(ROOT)),
        "control_cursor_before_second_offset": f"${second_registers['a2'] - 2:06X}",
        "first_record": {"address": f"${first_registers['a3']:06X}",
                         "words": [signed_word(first_registers[f"d{index}"]) for index in range(6)]},
        "second_record": {"address": f"${second_registers['a3']:06X}",
                          "words": [signed_word(second_registers[f"d{index}"]) for index in range(6)]},
    }


def exact_static_record_matches(executable: bytes, inventory: dict) -> list[dict]:
    """Find byte-for-byte source copies of selected transformed records.

    A miss only rules out an unmodified static copy: it cannot rule out a
    scaled, rotated, packed, or generated upstream coordinate source.
    """
    traced = (
        ("external-frame subdivision endpoint", SAMPLES[2][1], 0xC48390),
        ("Golden Gate consecutive projection record 0", SAMPLES[3][1], 0xC483BA),
    )
    rows = []
    for label, path, runtime_address in traced:
        memory = path.read_bytes()
        record = memory[runtime_address - 0xC00000:runtime_address - 0xC00000 + 12]
        matches = []
        cursor = executable.find(record)
        while cursor >= 0:
            owner = next((segment["index"] for segment in inventory["segments"]
                          if segment["payload_file_offset"] <= cursor < segment["payload_file_offset"] + segment["size_bytes"]), None)
            matches.append({"file_offset": f"${cursor:X}", "segment": owner})
            cursor = executable.find(record, cursor + 1)
        rows.append({"label": label, "snapshot": str(path.relative_to(ROOT)),
                     "runtime_address": f"${runtime_address:06X}",
                     "six_words_hex": record.hex().upper(), "matches": matches,
                     "qualification": "An empty exact-byte match list does not exclude transformed, packed, or generated upstream coordinates."})
    return rows


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis" / "model_geometry_boundaries.json")
    parser.add_argument("--markdown", type=Path,
                        default=ROOT / "analysis" / "model_geometry_boundaries.md")
    args = parser.parse_args()

    inventory = json.loads((ROOT / "analysis" / "hunk_inventory.json").read_text())
    resolved = json.loads((ROOT / "analysis" / "hunk_runtime_resolved.json").read_text())["resolved"]
    executable = (ROOT / "local" / "extracted" / "f18_interceptor").read_bytes()

    geometry_segment = inventory["segments"][72]
    geometry_base = address(resolved["72"]["runtime_payload_base"])
    geometry_payload = original_payload(executable, geometry_segment)
    comparisons = snapshot_comparisons(geometry_payload, geometry_base,
                                       relocation_offsets(geometry_segment))
    assert comparisons and all(row["non_relocation_difference_bytes"] > 0 for row in comparisons)

    staging_segment = inventory["segments"][71]
    staging_base = address(resolved["71"]["runtime_payload_base"])
    staging_comparisons = snapshot_comparisons(original_payload(executable, staging_segment), staging_base,
                                               relocation_offsets(staging_segment))
    assert staging_comparisons and all(row["non_relocation_difference_bytes"] > 0 for row in staging_comparisons)

    scene_segment = inventory["segments"][42]
    scene_base = address(resolved["42"]["runtime_payload_base"])
    scene_comparisons = snapshot_comparisons(original_payload(executable, scene_segment), scene_base,
                                             relocation_offsets(scene_segment))
    immutable_scene_hunks = []
    for index in (41, 42, 43, 44, 45, 46, 47, 49, 50):
        segment = inventory["segments"][index]
        base = address(resolved[str(index)]["runtime_payload_base"])
        stable = snapshot_comparisons(original_payload(executable, segment), base,
                                      relocation_offsets(segment))
        assert stable and all(row["non_relocation_difference_bytes"] == 0 for row in stable)
        immutable_scene_hunks.append({"segment": index,
                                      "range": f"${base:06X}-${base + segment['size_bytes'] - 1:06X}",
                                      "size_bytes": segment["size_bytes"],
                                      "comparisons": stable})
    subdivision = observed_subdivision()
    static_record_matches = exact_static_record_matches(executable, inventory)

    report = {
        "scope": "model-input versus transformed-geometry boundary",
        "projection_consumer_workspace": {
            "range": f"${geometry_base:06X}-${geometry_base + geometry_segment['size_bytes'] - 1:06X}",
            "segment": 72,
            "classification": "mutable_geometry_working_table_not_immutable_model",
            "consumer": "$C212B0 copies selected triples to $C4C592, then $C2EE4A projects them",
            "writer": "$C21C2E writes midpoint-derived triples at selected C48390-record offsets +$1E and +$24",
            "snapshot_comparisons": comparisons,
        },
        "preprojection_record_workspace": {
            "range": f"${staging_base:06X}-${staging_base + staging_segment['size_bytes'] - 1:06X}",
            "segment": 71,
            "classification": "mutable_geometry_record_workspace_not_immutable_model",
            "observed_member": "$C47628 is selected by C21C2E in the external-frame trace",
            "snapshot_comparisons": staging_comparisons,
            "ordinary_replay_cpu_write_watch": {
                "address": "$C47628",
                "scenario": "sealed run031, frames 1-7500",
                "result": "miss",
                "qualification": "No CPU write occurs during ordinary replay from the saved initial state. The earlier no-input stepping trace writes derived midpoint fields, so population precedes this sealed replay or follows another unobserved path.",
            },
        },
        "immutable_upstream_candidates": {
            "verified_byte_stable_scene_hunks": immutable_scene_hunks,
            "verified_scene_control_hunk": {
                "range": f"${scene_base:06X}-${scene_base + scene_segment['size_bytes'] - 1:06X}",
                "segment": 42,
                "classification": "verified_static_scene_control_candidate",
                "qualification": "Control records select projection handlers and payloads; this does not identify a model by itself.",
                "snapshot_comparisons": scene_comparisons,
            },
            "exact_inline_edge_lists": [
                {"range": "$C37EA0-$C37EA5", "evidence": "analysis/data/c37ea0_bridge_silhouette_projected_edge_list.md"},
                {"range": "$C38B0A-$C38B1F", "evidence": "analysis/data/c38b0a_external_view_projected_edge_list.md"},
                {"range": "$C3985A-$C39883", "evidence": "analysis/data/c3985a_projected_edge_list.md"},
            ],
        },
        "observed_external_frame_subdivision": subdivision,
        "exact_static_record_source_scan": static_record_matches,
        "conclusion": "No named 3D model is asserted. The next valid model-identification trace must observe an immutable upstream coordinate source feeding the mutable C48390 geometry table, then correlate its projected edges with a frame.",
    }
    args.output.write_text(json.dumps(report, indent=2) + "\n")
    lines = ["# Model geometry boundaries", "", "This report separates immutable model-input candidates from the mutable triples consumed by perspective projection. It does **not** assign a model name without an upstream coordinate-source trace and frame correlation.", "", "## Projection consumer workspace", "", f"`{report['projection_consumer_workspace']['range']}` is Hunk 72, declared CODE but positive-data referenced. `$C212B0` reads selected triples here into `$C4C592`, then `$C2EE4A` projects them. It is not a source-model export.", "", f"Byte-exact `$C21C2E` writes midpoint-derived triples into selected records at `+$1E` and `+$24`; therefore this range is a mutable geometry working table.", "", "| Capture | Non-relocation bytes different from original Hunk payload |", "| --- | ---: |"]
    lines += [f"| {row['capture']} | {row['non_relocation_difference_bytes']:,} |" for row in comparisons]
    staging = report["preprojection_record_workspace"]
    lines += ["", "## Pre-projection record workspace", "", f"`{staging['range']}` is Hunk 71. `$C21C2E` selects its observed `$C47628` member before writing midpoint-derived values. It also differs from the original payload in every sampled state and is not an immutable model export.", "", "| Capture | Non-relocation bytes different from original Hunk payload |", "| --- | ---: |"]
    lines += [f"| {row['capture']} | {row['non_relocation_difference_bytes']:,} |" for row in staging["snapshot_comparisons"]]
    watch = staging["ordinary_replay_cpu_write_watch"]
    lines += ["", f"A CPU write watch at `{watch['address']}` is an expected **miss** during {watch['scenario']}. {watch['qualification']}"]
    stable_hunks = report["immutable_upstream_candidates"]["verified_byte_stable_scene_hunks"]
    lines += ["", "## Verified byte-stable scene-family candidates", "", "The following Hunk payloads match their original bytes in every sampled state after complete relocation longwords are excluded. They are immutable scene-family candidates, not whole-model exports: original HUNK_CODE remains authoritative and segment 46 begins with valid 68000 instructions. Individual data records still need producer-to-projection traces before model names can be assigned.", "", "| Hunk | Range | Bytes | Snapshot result |", "| ---: | --- | ---: | --- |"]
    lines += [f"| {row['segment']} | `{row['range']}` | {row['size_bytes']:,} | zero non-relocation differences in {len(row['comparisons'])} snapshots |" for row in stable_hunks]
    lines += ["", "## Upstream immutable candidates", "", f"Hunk 42 `{report['immutable_upstream_candidates']['verified_scene_control_hunk']['range']}` is verified at runtime and supplies scene-control streams. It is an immutable candidate boundary, but controls dispatch/offsets rather than proving a particular model.", "", "The exact static edge-list ranges below select pairs from the mutable working table and are useful topology evidence:", ""]
    lines += [f"- `{row['range']}` — `{row['evidence']}`" for row in report["immutable_upstream_candidates"]["exact_inline_edge_lists"]]
    lines += ["", "## Observed external-frame subdivision", "", f"The existing `{subdivision['trace']}` trace executes `$C21C2E` twice. Its second offset is read from `{subdivision['control_cursor_before_second_offset']}` and selects the mutable `$C48390` base.", "", "| Selected record | Address | Six signed words before midpoint writes |", "| --- | --- | --- |", f"| first | `{subdivision['first_record']['address']}` | `{subdivision['first_record']['words']}` |", f"| second | `{subdivision['second_record']['address']}` | `{subdivision['second_record']['words']}` |", "", "Both selected inputs are mutable Hunk-71/Hunk-72 workspace records, so this operation supplies no immutable model coordinates yet.", "", "## Exact static-copy check", "", "The following exact twelve-byte (six-word) records were searched across the original executable. This detects only an unmodified source copy; empty results do not exclude transformed, packed, or generated model coordinates.", "", "| Traced record | Runtime address | Exact matches in original executable |", "| --- | --- | --- |"]
    lines += [f"| {row['label']} | `{row['runtime_address']}` | {len(row['matches'])} |" for row in static_record_matches]
    lines += ["", "## Required next evidence", "", "The sealed run031 state is already after `$C47628` population. Capture an earlier loader/scene-initialization phase, then trace the first writer into `$C46184` and `$C48390`, record its source pointers and pre-write triples, and require the source range to remain byte-identical to its original Hunk payload across snapshots. Only then correlate the resulting projected edges with a frame and name the model.", ""]
    args.markdown.write_text("\n".join(lines))
    print(f"wrote {args.output.relative_to(ROOT)}")
    print(f"wrote {args.markdown.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
