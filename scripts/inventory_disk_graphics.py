"""Inventory ILBM graphics stored in the original ADF without modifying it."""
from __future__ import annotations

import argparse
import hashlib
import json
import shutil
import struct
import subprocess
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PIX_FILES = ("pix/frnt5", "pix/inst5", "pix/splsh")
LOADER_RESULT_SLOTS = {
    "pix/splsh": {"call": "$C0E2EE", "store": "$C0E2F6", "slot": "$C1AADC"},
    "pix/inst5": {"call": "$C0E57E", "store": "$C0E586", "slot": "$C1AB08"},
    "pix/frnt5": {"call": "$C0E59E", "store": "$C0E5A6", "slot": "$C1AB38"},
}


def parse_ilbm(data: bytes) -> dict:
    if len(data) < 12 or data[:4] != b"FORM" or data[8:12] != b"ILBM":
        raise ValueError("expected FORM ILBM")
    chunks = {}
    offset = 12
    while offset + 8 <= len(data):
        kind = data[offset:offset + 4]
        size = struct.unpack_from(">I", data, offset + 4)[0]
        end = offset + 8 + size
        if end > len(data):
            raise ValueError(f"truncated {kind.decode('ascii', 'replace')} chunk")
        chunks[kind] = data[offset + 8:end]
        offset = end + (size & 1)
    bmhd = chunks.get(b"BMHD")
    if bmhd is None or len(bmhd) != 20 or b"BODY" not in chunks:
        raise ValueError("ILBM lacks a valid BMHD or BODY chunk")
    width, height, x, y, planes, masking, compression, _, transparent, x_aspect, y_aspect, page_width, page_height = struct.unpack(
        ">HHhhBBBBHBBHH", bmhd)
    return {
        "width": width, "height": height, "x": x, "y": y, "planes": planes,
        "masking": masking, "compression": compression, "transparent_color": transparent,
        "pixel_aspect": [x_aspect, y_aspect], "page_size": [page_width, page_height],
        "palette_entries": len(chunks.get(b"CMAP", b"")) // 3,
        # Amiga COLORxx values use one high nibble per RGB component.  Preserve
        # the source CMAP separately; these words make a RAM comparison exact.
        "palette_rgb4_words": [f"${((red >> 4) << 8 | (green >> 4) << 4 | (blue >> 4)):04X}"
                               for red, green, blue in zip(chunks.get(b"CMAP", b"")[0::3],
                                                           chunks.get(b"CMAP", b"")[1::3],
                                                           chunks.get(b"CMAP", b"")[2::3])],
        "body_bytes": len(chunks[b"BODY"]),
        "chunks": [{"id": key.decode("ascii", "replace"), "bytes": len(value)}
                   for key, value in chunks.items()],
    }


def extract(xdftool: str, disk: Path, source: str) -> bytes:
    with tempfile.TemporaryDirectory(prefix="fa18-ilbm-") as temporary:
        target = Path(temporary) / source.rsplit("/", 1)[-1]
        subprocess.run([xdftool, str(disk), "read", source, str(target)], check=True,
                       capture_output=True, text=True)
        return target.read_bytes()


def occurrences(data: bytes, needle: bytes) -> list[int]:
    result, offset = [], 0
    while True:
        offset = data.find(needle, offset)
        if offset < 0:
            return result
        result.append(offset)
        offset += 1


def relocation_references_to(segments: list[dict], runtime: dict, target_segment: int,
                             target_payload_offset: int, executable: bytes) -> list[dict]:
    """Find exact Hunk relocation sites naming one target payload byte."""
    references = []
    for segment in segments:
        source_base = runtime.get(str(segment["index"]), {}).get("runtime_payload_base")
        if source_base is None:
            continue
        for relocation in segment.get("reloc32", []):
            if relocation["target_segment"] != target_segment:
                continue
            for offset in relocation["offsets"]:
                raw = executable[segment["payload_file_offset"] + offset:
                                 segment["payload_file_offset"] + offset + 4]
                if len(raw) != 4 or int.from_bytes(raw, "big") != target_payload_offset:
                    continue
                references.append({
                    "source_segment": segment["index"],
                    "source_kind": segment["kind"],
                    "source_payload_offset": f"${offset:X}",
                    # This is the longword extension, not the instruction PC.
                    "runtime_extension_address": f"${source_base + offset:06X}",
                })
    return references


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--disk", type=Path, default=ROOT / "local" / "media" / "fa18.adf")
    parser.add_argument("--xdftool", default="xdftool.exe")
    parser.add_argument("--output", type=Path, default=ROOT / "analysis" / "disk_graphics_assets.json")
    parser.add_argument("--markdown", type=Path, default=ROOT / "analysis" / "disk_graphics_assets.md")
    parser.add_argument("--executable", type=Path, default=ROOT / "local" / "extracted" / "f18_interceptor")
    parser.add_argument("--slow", type=Path, default=ROOT / "captures" / "baseline_menu" / "slow.bin")
    parser.add_argument("--hunk-inventory", type=Path, default=ROOT / "analysis" / "hunk_inventory.json")
    parser.add_argument("--runtime-mapping", type=Path, default=ROOT / "analysis" / "hunk_runtime_resolved.json")
    parser.add_argument("--extract-dir", type=Path,
                        help="optional output directory for verbatim ILBM files; never defaults into the repository")
    args = parser.parse_args()
    if not args.disk.is_file():
        raise FileNotFoundError(args.disk)
    xdftool = shutil.which(args.xdftool) or args.xdftool
    executable = args.executable.read_bytes()
    slow = args.slow.read_bytes()
    segments = json.loads(args.hunk_inventory.read_text())["segments"]
    runtime = json.loads(args.runtime_mapping.read_text())["resolved"]
    if len(slow) != 0x80000:
        raise ValueError(f"expected 512 KiB Slow-RAM snapshot, got {len(slow)}")
    assets = []
    extracted = {}
    for source in PIX_FILES:
        raw = extract(xdftool, args.disk, source)
        details = parse_ilbm(raw)
        string_offsets = occurrences(executable, source.encode("ascii"))
        hunk_locations = []
        for offset in string_offsets:
            segment = next((row for row in segments if row["payload_file_offset"] <= offset < row["payload_file_offset"] + row["size_bytes"]), None)
            if segment is None:
                continue
            mapped = runtime.get(str(segment["index"]), {})
            hunk_locations.append({"segment": segment["index"], "kind": segment["kind"],
                                   "payload_offset": f"${offset - segment['payload_file_offset']:X}",
                                   "runtime_payload_base": f"${mapped['runtime_payload_base']:06X}" if "runtime_payload_base" in mapped else None})
        details.update({"adf_path": source, "size_bytes": len(raw),
                        "sha256": hashlib.sha256(raw).hexdigest(),
                        "executable_path_string_file_offsets": [f"${offset:X}" for offset in string_offsets],
                        "hunk_data_locations": hunk_locations,
                        "baseline_runtime_path_string_addresses": [f"${0xC00000 + offset:06X}" for offset in occurrences(slow, source.encode("ascii"))]})
        details["static_loader_result"] = LOADER_RESULT_SLOTS[source]
        palette_words = [int(word[1:], 16) for word in details["palette_rgb4_words"]]
        palette_bytes = b"".join(word.to_bytes(2, "big") for word in palette_words)
        details["baseline_runtime_palette_addresses"] = [
            f"${0xC00000 + offset:06X}" for offset in occurrences(slow, palette_bytes)]
        if len(hunk_locations) == 1:
            location = hunk_locations[0]
            # The resource names are preceded by the ``df0:`` volume prefix.
            details["static_full_path_references"] = relocation_references_to(
                segments, runtime, location["segment"],
                int(location["payload_offset"][1:], 16) - 4, executable)
        else:
            details["static_full_path_references"] = []
        assets.append(details)
        extracted[source] = raw
    manifest = {
        "authority_disk": str(args.disk),
        "authority_disk_sha256": hashlib.sha256(args.disk.read_bytes()).hexdigest(),
        "classification": "immutable_disk_graphics_assets",
        "assets": assets,
        "extraction_command": "python scripts/inventory_disk_graphics.py --extract-dir <directory>",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(manifest, indent=2) + "\n")
    lines = ["# Disk graphics assets", "", "The original ADF contains these immutable `FORM ILBM` resources. They are distinct from the mutable Chip-RAM display targets documented in `cockpit_bitplane_assets.md` and the pointer-bearing runtime data records in `runtime_display_pointer_state.md`.", "", "| ADF path | Size | Geometry | Planes | Identifier Hunk | Runtime string | SHA-256 |", "| --- | ---: | --- | ---: | --- | --- | --- |"]
    for asset in assets:
        hunk_locations = ", ".join(f"{row['kind']} {row['segment']}+{row['payload_offset']}" for row in asset["hunk_data_locations"])
        runtime_addresses = ", ".join(asset["baseline_runtime_path_string_addresses"])
        lines.append(f"| `{asset['adf_path']}` | {asset['size_bytes']:,} | {asset['width']}×{asset['height']} | {asset['planes']} | `{hunk_locations}` | `{runtime_addresses}` | `{asset['sha256']}` |")
    lines += ["", "## Runtime palette evidence", ""]
    for asset in assets:
        addresses = ", ".join(asset["baseline_runtime_palette_addresses"])
        lines.append(f"- `{asset['adf_path']}` RGB4 CMAP: `{addresses or 'no exact baseline match'}`.")
    lines += ["", "`pix/inst5` and `pix/frnt5` have identical 32-word RGB4 CMAP data, and the baseline snapshot contains that exact sequence at `$C1AA9C`. This establishes `$C1AA9C-$C1AADB` as mutable decoded palette state, not executable code or a disk-pixel payload. A match does not alone establish which file was loaded most recently.", "", "## Static loader references", ""]
    for asset in assets:
        references = ", ".join(
            f"{row['source_kind']} {row['source_segment']}+{row['source_payload_offset']} "
            f"(relocated extension `{row['runtime_extension_address']}`)"
            for row in asset["static_full_path_references"])
        lines.append(f"- `{asset['adf_path']}`: {references or 'no exact relocated reference found'}.")
    lines += ["", "Each listed address is the relocated longword extension that names the full `df0:pix/...` path. The three references occur as `PEA` arguments to `$C0E078`: `$C0E2EE` for `splsh`, `$C0E57E` for `inst5`, and `$C0E59E` for `frnt5`. Static disassembly shows that helper reads the bitmap header, allocates a plane-pointer array, and reads plane-sized data. It is therefore an observed ILBM resource-loading path; the callback arguments and eventual display presentation remain unassigned.", "", "The loader-return slots are `$C1AADC` for `splsh`, `$C1AB08` for `inst5`, and `$C1AB38` for `frnt5`; the latter two calls share palette destination `$C1AA9C`. For `splsh`, `$C1693A-$C1697E` later copies five pointer fields from the `$C1AADC` object into BSS cache `$C1AAF4-$C1AB04`. See `analysis/runtime_display_pointer_state.md`. This establishes static resource-to-runtime-object-to-cache dataflow, not a frame-specific presentation claim.", "", "Run the recorded extraction command to write byte-for-byte ILBM copies to an explicit directory. The inventory itself does not duplicate copyrighted source pixels into the repository.", ""]
    lines += ["", "The separate BSS-object slot and pointer-cache paths for all three resources are documented in `analysis/runtime_ilbm_object_slots.md`.", ""]
    args.markdown.parent.mkdir(parents=True, exist_ok=True)
    args.markdown.write_text("\n".join(lines))
    if args.extract_dir:
        args.extract_dir.mkdir(parents=True, exist_ok=True)
        for source, raw in extracted.items():
            (args.extract_dir / source.rsplit("/", 1)[-1]).write_bytes(raw)
    print(f"wrote {args.output.relative_to(ROOT)}")
    print(f"wrote {args.markdown.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
