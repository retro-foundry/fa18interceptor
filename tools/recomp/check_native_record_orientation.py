"""Prove both complete live-record orientation entries against sealed instructions."""
import argparse
import hashlib
import json
import subprocess
from audit_command_dispatch import source_decoder
from check_record_region_probe import ROOT, build_oracle, default_bash
from recomp import classify, static_target

ENTRIES = (0xc2d954, 0xc2d94e)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=4096)
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("cases must be positive")
    state, decoder = source_decoder()
    seal = json.loads((ROOT / "analysis/data/command_dispatch_source_scope.json").read_text())["state_sha256"]
    if hashlib.sha256(state).hexdigest() != seal:
        raise RuntimeError("original source state seal differs")
    rows, pending = {}, list(ENTRIES)
    while pending:
        pc = pending.pop()
        if pc in rows:
            continue
        length, opcode, handler, assembly = decoder.decode(pc)
        raw = b"".join(decoder.word(pc+i).to_bytes(2, "big") for i in range(0, length, 2))
        rows[pc] = {"length": length, "bytes": raw.hex(), "assembly": assembly}
        kind = classify(handler)
        target = static_target(decoder, pc, opcode, handler, kind)
        if kind == "rts":
            continue
        if kind == "interp":
            raise RuntimeError(f"unexpected exception instruction at {pc:06X}")
        if kind in ("jsr", "bsr", "bcc", "dbcc"):
            if target is None:
                raise RuntimeError(f"unresolved source target at {pc:06X}")
            pending.extend((pc+length, target))
        elif kind in ("bra", "jmp"):
            if target is None:
                raise RuntimeError(f"unresolved source transfer at {pc:06X}")
            pending.append(target)
        else:
            pending.append(pc+length)
    header = "/* Sealed original orientation graph; validation only. */\n"
    header += "static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } scene_source_bytes[]={\n"
    for pc, row in sorted(rows.items()):
        values = ",".join(f"0x{b:02x}" for b in bytes.fromhex(row["bytes"]))
        header += f"{{0x{pc:06X},{row['length']},{{{values}}}}},\n"
    header += "};\n"
    directory = ROOT / "build/recomp"
    directory.mkdir(parents=True, exist_ok=True)
    (directory / "native_record_orientation_source.h").write_text(header)
    exe = build_oracle("native_record_orientation_oracle", "tools/recomp/native_record_orientation_oracle.c", default_bash())
    reports, visited = [], set()
    for entry in ENTRIES:
        result = subprocess.run([str(exe), str(args.cases), f"{entry:06X}"], cwd=ROOT,
                                capture_output=True, text=True, timeout=180)
        (directory / f"native_record_orientation_{entry:06X}.log").write_text(result.stdout+result.stderr)
        if result.returncode:
            raise RuntimeError(result.stderr or result.stdout or "native orientation differs")
        for line in result.stdout.splitlines():
            if line.startswith("visited:"):
                visited.update(line.partition(":")[2].split())
            elif line.startswith(("native orientation", "trig asset:", "trig domain:")):
                reports.append(line)
                print(line, flush=True)
    print(f"native orientation: {len(visited)}/{len(rows)} original boundaries", flush=True)
    if args.cases >= 4096:
        expected = {f"{pc:06X}" for pc in rows}
        if visited != expected:
            raise RuntimeError(f"uncovered boundaries: {sorted(expected-visited)}")
        paths = ["port/native_record_orientation.c", "port/native_record_orientation.h",
                 "port/native_record_orientation_contract_test.c", "port/native_scene_records.c",
                 "port/native_scene_records.h", "port/flight.c", "port/flight.h", "port/two_angle_matrix.c",
                 "port/two_angle_matrix.h", "port/field_bytes.h", "port/record_matrix_update.c",
                 "port/record_matrix_update.h", "port/record_matrix_update_contract_test.c", "port/flight_contract_test.c",
                 "tools/recomp/check_native_record_orientation.py", "tools/recomp/native_record_orientation_oracle.c",
                 "tools/recomp/native_scene_player_oracle.c"]
        checkpoint = {
            "status": "validated_complete_native_record_orientation_full_root_placement_pending",
            "complete_entries": [f"{pc:06X}" for pc in ENTRIES],
            "actual_children": ["C2E47A", "C2E514", "C2E5F6", "C2E6DA"],
            "cases": args.cases*len(ENTRIES), "lookup_angles": 65536,
            "source_boundaries": len(rows), "covered_boundaries": len(visited),
            "native_cpu_dependency": False, "child_contracts": [],
            "comparison": "all Chip/Slow RAM excluding CPU ABI save stack C7FD00..C7FF00; independent live forward/inverse/angle owners for all sixteen records; actual original children executed",
            "coverage_cases": "all word angles in standalone lookup, signed doubled-word wrap, arbitrary signed original table/adjacent words, original Hunk-63 quarter table (1802 bytes), live asset mutation, original D7/high register variation, zero-angle branches, sixteen selected records, low-word intermediate multiply and wrapped long arithmetic, control flag preservation/clear",
            "ownership_limit": "Original assets/adjacent fields must be bound by actual producers; missing reads fail explicitly with preceding stores retained. Native main and full root placement/update/context graphs remain pending. Captured state and Musashi are validation only.",
            "original_state_sha256": seal,
            "original_adf_sha256": hashlib.sha256((ROOT/"FA-18 Interceptor (1988)(Electronic Arts)[cr A-Ha].adf").read_bytes()).hexdigest(),
            "original_pc_bytes_sha256": hashlib.sha256(b"".join(
                pc.to_bytes(4, "big")+bytes.fromhex(row["bytes"]) for pc,row in sorted(rows.items()))).hexdigest(),
            "reports": reports, "source_sha256": {p: hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},
        }
        (ROOT / "analysis/figures/native_record_orientation_checkpoint.json").write_text(json.dumps(checkpoint, indent=2)+"\n")


if __name__ == "__main__":
    main()
