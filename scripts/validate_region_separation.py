"""Validate the evidence-backed code, data, asset, and display boundaries."""
from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def address(text: str) -> int:
    return int(text.removeprefix("$"), 16)


def owner(segments: list[dict], start: int, end: int) -> dict:
    matches = [segment for segment in segments if "runtime_start" in segment
               and address(segment["runtime_start"]) <= start
               and end < address(segment["runtime_end_exclusive"])]
    if len(matches) != 1:
        raise AssertionError(f"expected one owning Hunk for ${start:06X}-${end:06X}, got {matches}")
    return matches[0]


def main() -> None:
    regions = json.loads((ROOT / "analysis" / "runtime_region_manifest.json").read_text())
    graphics = json.loads((ROOT / "analysis" / "disk_graphics_assets.json").read_text())
    disk_resources = json.loads((ROOT / "analysis" / "disk_game_resources.json").read_text())
    geometry = json.loads((ROOT / "analysis" / "model_geometry_boundaries.json").read_text())
    segments = regions["hunk_segments"]
    checks: list[dict] = []

    # The complete disk-resource classification gives graphics an external
    # boundary too: only the pix/ ILBMs belong to the game's immutable graphics
    # set. The Workbench .info file is a separate desktop DiskObject container.
    disk_ilbms = {row["adf_path"] for row in disk_resources["resources"]
                  if row["classification"] == "ILBM_graphics"}
    detailed_ilbms = {row["adf_path"] for row in graphics["assets"]}
    assert disk_ilbms == detailed_ilbms == {"pix/frnt5", "pix/inst5", "pix/splsh"}
    icon = next(row for row in disk_resources["resources"]
                if row["adf_path"] == "F-18 Interceptor.info")
    assert icon["classification"] == "workbench_icon_container"
    assert icon["format"] == "Amiga DiskObject" and icon["format_magic"] == "E310"
    checks.append({"check": "disk ILBM set matches detailed graphics inventory",
                   "assets": sorted(disk_ilbms)})
    checks.append({"check": "desktop icon remains outside game ILBM graphics",
                   "path": icon["adf_path"], "format": icon["format"]})

    # The container declaration is the primary extraction/allocation boundary.
    # Verify that the generated inventory neither drops nor retypes any
    # original DATA/BSS hunk, including zero-length declarations that preserve
    # segment numbering for relocations.
    inventory = json.loads((ROOT / "analysis" / "hunk_inventory.json").read_text())
    for kind, classification in (("DATA", "initialized_data"), ("BSS", "uninitialized_data")):
        original = [row for row in inventory["segments"] if row["kind"] == kind]
        listed = [row for row in segments if row["hunk_kind"] == kind]
        assert len(listed) == len(original), (kind, len(listed), len(original))
        assert {row["segment"] for row in listed} == {row["index"] for row in original}
        assert all(row["classification"] == classification for row in listed)
        assert sum(row["size_bytes"] for row in listed) == sum(row["size_bytes"] for row in original)
        checks.append({"check": f"all original {kind} hunks retain their container classification",
                       "hunks": len(original), "bytes": sum(row["size_bytes"] for row in original)})

    for record in regions["runtime_display_state_regions"]:
        start, end = address(record["start"]), address(record["end_inclusive"])
        segment = owner(segments, start, end)
        assert segment["hunk_kind"] == "BSS"
        checks.append({"check": "runtime display state belongs to BSS", "range": f"{record['start']}-{record['end_inclusive']}", "segment": segment["segment"]})

    for record in regions["runtime_geometry_workspace_regions"]:
        start, end = address(record["start"]), address(record["end_inclusive"])
        segment = owner(segments, start, end)
        assert segment["segment"] in (71, 72) and segment["hunk_kind"] == "CODE"
        assert segment["classification"] == "data_in_code_hunk"
        assert (ROOT / record["evidence"]).is_file()
        checks.append({"check": "mutable geometry workspace remains separated from model inputs",
                       "range": f"{record['start']}-{record['end_inclusive']}", "segment": segment["segment"]})

    candidates = regions["immutable_scene_source_candidate_regions"]
    assert len(candidates) == 9
    for record in candidates:
        start, end = address(record["start"]), address(record["end_inclusive"])
        segment = owner(segments, start, end)
        assert segment["segment"] == record["segment"] and segment["hunk_kind"] == "CODE"
        assert record["classification"] == "byte_stable_scene_family_candidate_in_CODE_hunk"
        assert (ROOT / record["evidence"]).is_file()
    compared = geometry["immutable_upstream_candidates"]["verified_byte_stable_scene_hunks"]
    assert {row["segment"] for row in candidates} == {row["segment"] for row in compared}
    for row in compared:
        assert all(sample["non_relocation_difference_bytes"] == 0
                   for sample in row["comparisons"]), row
    checks.append({"check": "byte-stable scene-family candidates remain distinct from geometry workspaces",
                   "segments": [record["segment"] for record in candidates],
                   "snapshot_comparisons": len(compared[0]["comparisons"])})

    for record in regions["inline_data_regions"]:
        start, end = address(record["start"]), address(record["end_inclusive"])
        matches = [segment for segment in segments if "runtime_start" in segment
                   and address(segment["runtime_start"]) <= start
                   and end < address(segment["runtime_end_exclusive"])]
        if matches:
            assert len(matches) == 1 and matches[0]["hunk_kind"] == "CODE", matches
            checks.append({"check": "inline data remains bounded inside CODE", "range": f"{record['start']}-{record['end_inclusive']}", "segment": matches[0]["segment"]})
        else:
            # This is not a reclassification.  The inline boundary has its
            # own evidence report, but the generic Hunk runtime resolver has
            # no containing range for it yet.
            assert (ROOT / record["evidence"]).is_file(), record
            checks.append({"check": "inline data has evidence; Hunk owner unresolved", "range": f"{record['start']}-{record['end_inclusive']}", "evidence": record["evidence"]})

    # The segment-16 scene table is more than a location claim: it contains
    # relocation operands in repeated adjacent triplets.  Verify that source
    # shape against the original Hunk before accepting the inline-data range.
    executable = (ROOT / "local" / "extracted" / "f18_interceptor").read_bytes()

    # The control-stream inventory records words observed directly at C1F6F8.
    # Check that every promoted 24-byte prefix equals its original CODE payload;
    # this distinguishes static stream data from the dynamic C3B/C4BF entries
    # intentionally excluded by the inventory script.
    live_inventory = json.loads((ROOT / "analysis" / "live_control_streams.json").read_text())["streams"]
    observed_words = {address(row["stream"]): row["observations"][0]["first_words"]
                      for row in live_inventory}
    live_prefixes = [record for record in regions["inline_data_regions"]
                     if record["role"] == "live_renderer_control_stream_prefix"]
    assert len(live_prefixes) == len(observed_words) - 1  # C3925A is retained as its six-byte specialized record.
    for record in live_prefixes:
        start, end = address(record["start"]), address(record["end_inclusive"])
        enclosing = owner(segments, start, end)
        source_segment = inventory["segments"][enclosing["segment"]]
        source_offset = start - address(enclosing["runtime_start"])
        source = source_segment["payload_file_offset"] + source_offset
        payload = executable[source:source + 24]
        words = [f"${int.from_bytes(payload[offset:offset + 2], 'big'):04X}"
                 for offset in range(0, 24, 2)]
        assert words == observed_words[start], record
        checks.append({"check": "live renderer control prefix is original static data inside CODE",
                       "range": f"{record['start']}-{record['end_inclusive']}",
                       "segment": enclosing["segment"]})

    # Projected edge lists are a particularly important code/data boundary:
    # their enclosing original Hunk is executable, but the exact selector plus
    # offset-pair payload is data consumed by C212B0. Verify both its compact
    # wire format and that it is an unchanged static payload in every saved
    # scene state used for geometry-boundary comparison.
    edge_snapshots = (
        ROOT / "captures" / "baseline_menu" / "slow.bin",
        ROOT / "build" / "attract_focus_600" / "slow.bin",
        ROOT / "build" / "attract_focus_1800" / "slow.bin",
        ROOT / "build" / "run031_frame12000_golden_gate_c1f6f8_probe" / "slow.bin",
        ROOT / "build" / "run031_frame14500_c1f6f8_probe" / "slow.bin",
    )
    edge_records = [record for record in regions["inline_data_regions"]
                    if record["role"] == "projected_edge_list_candidate"]
    assert len(edge_records) >= 7
    for record in edge_records:
        start, end = address(record["start"]), address(record["end_inclusive"])
        size = end - start + 1
        assert size >= 6 and (size - 2) % 4 == 0, record
        enclosing = owner(segments, start, end)
        source_segment = inventory["segments"][enclosing["segment"]]
        source_offset = start - address(enclosing["runtime_start"])
        source_start = source_segment["payload_file_offset"] + source_offset
        payload = executable[source_start:source_start + size]
        assert len(payload) == size
        pairs = [int.from_bytes(payload[offset + 2:offset + 4], "big")
                 for offset in range(2, size, 4)]
        assert pairs[-1] & 0x8000 and all(not (value & 0x8000) for value in pairs[:-1]), record
        for snapshot in edge_snapshots:
            memory = snapshot.read_bytes()
            captured = memory[start - 0xC00000:start - 0xC00000 + size]
            assert captured == payload, (record, snapshot)
        checks.append({"check": "projected edge list is static structured data inside CODE",
                       "range": f"{record['start']}-{record['end_inclusive']}",
                       "pairs": len(pairs), "snapshots": len(edge_snapshots),
                       "segment": enclosing["segment"]})

    source = inventory["segments"][16]
    raw = executable[source["payload_file_offset"]:source["payload_file_offset"] + source["size_bytes"]]
    sites = []
    for relocation in source["reloc32"]:
        if 41 <= relocation["target_segment"] <= 50:
            sites.extend(relocation["offsets"])
    table_sites = sorted(offset for offset in sites if 0x360 <= offset <= 0x7A3)
    triplets = sum(table_sites[index:index + 3] == list(range(table_sites[index], table_sites[index] + 12, 4))
                   for index in range(len(table_sites) - 2))
    assert len(table_sites) >= 100 and triplets >= 30
    checks.append({"check": "scene pointer table has relocation triplets", "range": "$C223A8-$C227EB",
                   "relocation_sites": len(table_sites), "adjacent_triplets": triplets})

    for asset in graphics["assets"]:
        for location in asset["hunk_data_locations"]:
            segment = next(segment for segment in segments if segment["segment"] == location["segment"])
            assert segment["hunk_kind"] == "DATA"
        for reference in asset["static_full_path_references"]:
            segment = next(segment for segment in segments if segment["segment"] == reference["source_segment"])
            assert segment["hunk_kind"] == "CODE"
        result_slot = address(asset["static_loader_result"]["slot"])
        slot_owner = owner(segments, result_slot, result_slot + 3)
        assert slot_owner["hunk_kind"] == "BSS"
        # The calls/stores reside in verified segment 0.  Confirm both the
        # BSR target and the absolute-store relocation in original bytes.
        code0 = inventory["segments"][0]
        code0_base = address(next(segment for segment in segments if segment["segment"] == 0)["runtime_start"])
        raw0 = executable[code0["payload_file_offset"]:code0["payload_file_offset"] + code0["size_bytes"]]
        call = address(asset["static_loader_result"]["call"])
        call_offset = call - code0_base
        assert raw0[call_offset:call_offset + 2] == b"\x61\x00"
        displacement = int.from_bytes(raw0[call_offset + 2:call_offset + 4], "big", signed=True)
        assert call + 2 + displacement == 0xC0E078
        store = address(asset["static_loader_result"]["store"])
        store_offset = store - code0_base
        assert raw0[store_offset:store_offset + 2] == b"\x23\xC0"
        relocation = next(group for group in code0["reloc32"] if store_offset + 2 in group["offsets"])
        addend = int.from_bytes(raw0[store_offset + 2:store_offset + 6], "big")
        assert relocation["target_segment"] == 2 and result_slot == address(segments[2]["runtime_start"]) + addend
        checks.append({"check": "asset identifier DATA and loader reference CODE", "asset": asset["adf_path"]})
        checks.append({"check": "loader result slot belongs to BSS", "asset": asset["adf_path"],
                       "slot": asset["static_loader_result"]["slot"], "segment": slot_owner["segment"]})
        checks.append({"check": "loader call and result store are byte-backed", "asset": asset["adf_path"]})

    for record in regions["active_display_regions"]:
        for plane in record["planes"]:
            start, end = address(plane["start"]), address(plane["end_inclusive"])
            assert 0 <= start <= end < 0x80000, plane
        checks.append({"check": "active display targets are in Chip RAM", "capture": record["capture_state"]})

    output = {"status": "pass", "checks": checks,
              "scope": "Hunk ownership, ILBM identifier/reference placement, and Chip-RAM display target ranges"}
    target = ROOT / "analysis" / "region_separation_audit.json"
    target.write_text(json.dumps(output, indent=2) + "\n")
    print(f"passed {len(checks)} separation checks")


if __name__ == "__main__":
    main()
