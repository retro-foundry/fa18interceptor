"""Validate all native aircraft actions; execute the real simple control children."""
import argparse
import hashlib
import json
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash
from check_flight_commands import ENTRIES, RANGES
from audit_command_dispatch import source_decoder
from recomp import classify, static_target

LEAVES = (0xc08394, 0xc1b50c, 0xc1b510, 0xc1b514, 0xc1b558, 0xc1b55c, 0xc1b560, 0xc1b602)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=1024)
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("cases must be positive")
    scope = ROOT / "analysis/data/command_dispatch_source_scope.json"
    parent = [row for row in json.loads(scope.read_text())["instructions"]
              if int(row["pc"], 16) < 0xc1c23c and
              any(low <= int(row["pc"], 16) < high for low, high in RANGES)]
    _, decoder = source_decoder()
    rows = {int(row["pc"], 16): row for row in parent}
    pending = list(LEAVES)
    seen = set()
    while pending:
        pc = pending.pop()
        if pc in seen:
            continue
        seen.add(pc)
        length, opcode, handler, assembly = decoder.decode(pc)
        raw = b"".join(decoder.word(pc + offset).to_bytes(2, "big") for offset in range(0, length, 2))
        rows[pc] = {"pc": f"{pc:06X}", "length": length, "bytes": raw.hex()}
        kind = classify(handler)
        target = static_target(decoder, pc, opcode, handler, kind)
        if kind == "rts":
            continue
        if kind in ("jsr", "bsr"):
            raise RuntimeError(f"unported child inside control leaf at {pc:06X}")
        if kind in ("bra", "jmp"):
            pending.append(target)
        elif kind in ("bcc", "dbcc"):
            pending.extend((pc + length, target))
        else:
            pending.append(pc + length)
    header = "/* Original parent/control-child bytes; validation only. */\n"
    header += "static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } source_bytes[]={\n"
    for pc, row in sorted(rows.items()):
        values = ",".join(f"0x{byte:02x}" for byte in bytes.fromhex(row["bytes"]))
        header += f"{{0x{pc:06X},{row['length']},{{{values}}}}},\n"
    header += "};\n"
    (ROOT / "build/recomp").mkdir(parents=True, exist_ok=True)
    (ROOT / "build/recomp/native_flight_command_input_source.h").write_text(header)
    exe = build_oracle("native_flight_command_input_oracle", "tools/recomp/native_flight_command_input_oracle.c", default_bash())
    visited, leaves, controls, reports = set(), set(), set(), []
    for entry in ENTRIES:
        result = subprocess.run([str(exe), str(args.cases), entry], cwd=ROOT,
                                capture_output=True, text=True, timeout=180)
        (ROOT / f"build/recomp/native_flight_command_input_{entry}.log").write_text(result.stdout + result.stderr)
        if result.returncode:
            raise RuntimeError(result.stderr or result.stdout)
        visited.update(result.stdout.split("visited:")[1].split("leaves:")[0].split())
        leaves.update(result.stdout.split("leaves:")[1].split("control_visited:")[0].split())
        controls.update(result.stdout.split("control_visited:")[1].split())
        reports.append(result.stdout.splitlines()[0])
        print(reports[-1], flush=True)
    expected = {row["pc"] for row in parent}
    expected_controls = {f"{pc:06X}" for pc in seen}
    if args.cases >= 1024 and (visited != expected or controls != expected_controls or
                              leaves != {f"{pc:06X}" for pc in LEAVES}):
        raise RuntimeError(f"boundary mismatch: missing {sorted(expected-visited)}, extra {sorted(visited-expected)}, controls missing {sorted(expected_controls-controls)}, extra {sorted(controls-expected_controls)}, leaves {sorted(leaves)}")
    print(f"native aircraft actions: {len(ENTRIES)*args.cases} calls, {len(visited)}/{len(expected)} parent boundaries, {len(controls)}/{len(expected_controls)} control-child boundaries, {len(leaves)} real control-child entries")
    if args.cases >= 1024:
        paths = ["port/flight_command_input.c", "port/flight_command_input.h", "port/flight_command_types.h",
                 "port/flight_command_input_contract_test.c", "port/command_input.h",
                 "port/command_types.h", "port/indexed_controls.h",
                 "tools/recomp/check_native_flight_command_input.py",
                 "tools/recomp/native_flight_command_input_oracle.c", "tools/recomp/flight_commands_oracle.c",
                 str(scope.relative_to(ROOT))]
        data = {
            "status": "validated_native_aircraft_actions_full_runtime_integration_pending",
            "actions": len(ENTRIES), "cases": len(ENTRIES)*args.cases,
            "source_boundaries": len(expected), "covered_boundaries": len(visited),
            "control_source_boundaries": len(expected_controls), "covered_control_boundaries": len(controls),
            "real_control_child_entries": sorted(leaves), "native_cpu_dependency": False,
            "child_scope": "nine control calls / eight original entries execute real instructions; audio, space-press, spawn and eject-publication children are controlled test contracts",
            "reports": reports,
            "source_sha256": {path: hashlib.sha256((ROOT/path).read_bytes()).hexdigest() for path in paths},
        }
        (ROOT / "analysis/figures/native_flight_command_input_checkpoint.json").write_text(json.dumps(data, indent=2) + "\n")


if __name__ == "__main__":
    main()
