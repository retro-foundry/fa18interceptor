"""Validate five native context actions and their actual geometry/observer children."""
import argparse
import hashlib
import json
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash
from check_context_commands import ENTRIES, RANGES
from audit_command_dispatch import source_decoder
from recomp import classify, static_target


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=4096)
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("cases must be positive")
    scope = ROOT / "analysis/data/command_dispatch_source_scope.json"
    source = json.loads(scope.read_text())
    parent = [row for row in source["instructions"]
              if int(row["pc"], 16) < 0xc1c23c and
              any(low <= int(row["pc"], 16) < high for low, high in RANGES)]
    state, decoder = source_decoder()
    if hashlib.sha256(state).hexdigest() != source["state_sha256"]:
        raise RuntimeError("original state seal differs from the audited source")
    rows = {int(row["pc"], 16): row for row in parent}
    pending, seen = [0xc091e0, 0xc0915a], set()
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
        if kind in ("jsr", "bsr", "interp"):
            raise RuntimeError(f"unexpected child/exception at {pc:06X}")
        if kind in ("bra", "jmp"):
            pending.append(target)
        elif kind in ("bcc", "dbcc"):
            pending.extend((pc + length, target))
        else:
            pending.append(pc + length)
    header = "/* Original parent/child bytes; validation only. */\n"
    header += "static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } source_bytes[]={\n"
    for pc, row in sorted(rows.items()):
        values = ",".join(f"0x{byte:02x}" for byte in bytes.fromhex(row["bytes"]))
        header += f"{{0x{pc:06X},{row['length']},{{{values}}}}},\n"
    header += "};\n"
    (ROOT / "build/recomp").mkdir(parents=True, exist_ok=True)
    (ROOT / "build/recomp/native_context_command_input_source.h").write_text(header)
    exe = build_oracle("native_context_command_input_oracle", "tools/recomp/native_context_command_input_oracle.c", default_bash())
    visited, controls, reports = set(), set(), []
    for entry in ENTRIES:
        result = subprocess.run([str(exe), str(args.cases), entry], cwd=ROOT,
                                capture_output=True, text=True, timeout=180)
        (ROOT / f"build/recomp/native_context_command_input_{entry}.log").write_text(result.stdout + result.stderr)
        if result.returncode:
            raise RuntimeError(result.stderr or result.stdout)
        for line in result.stdout.splitlines():
            if line.startswith("visited:"):
                visited.update(line.partition(":")[2].split())
            elif line.startswith("control_visited:"):
                controls.update(line.partition(":")[2].split())
        reports.append(result.stdout.splitlines()[0])
        print(reports[-1], flush=True)
    expected = {row["pc"] for row in parent}
    expected_controls = {f"{pc:06X}" for pc in seen}
    if args.cases >= 4096 and (visited != expected or controls != expected_controls):
        raise RuntimeError(f"boundary mismatch: parent missing {sorted(expected-visited)}, extra {sorted(visited-expected)}, children missing {sorted(expected_controls-controls)}, extra {sorted(controls-expected_controls)}")
    print(f"native context actions: {len(ENTRIES)*args.cases} calls, {len(visited)}/{len(expected)} parent boundaries, {len(controls)}/{len(expected_controls)} actual geometry/observer boundaries")
    if args.cases >= 4096:
        paths = ["port/context_command_input.c", "port/context_command_input.h", "port/context_command_types.h",
                 "port/context_command_controls.c", "port/context_command_input_contract_test.c",
                 "port/flight_command_input.h", "port/view_command_input.h", "port/command_input.h",
                 "port/command_types.h", "port/indexed_controls.h",
                 "tools/recomp/check_native_context_command_input.py", "tools/recomp/native_context_command_input_oracle.c",
                 "tools/recomp/context_commands_oracle.c", str(scope.relative_to(ROOT))]
        data = {
            "status": "validated_native_context_actions_full_runtime_integration_pending",
            "actions": len(ENTRIES), "cases": len(ENTRIES)*args.cases,
            "source_boundaries": len(expected), "covered_boundaries": len(visited),
            "child_source_boundaries": len(expected_controls), "covered_child_boundaries": len(controls),
            "native_cpu_dependency": False, "actual_children": ["C091E0", "C0915A"],
            "original_state_sha256": source["state_sha256"],
            "original_pc_bytes_sha256": hashlib.sha256(b"".join(
                pc.to_bytes(4, "big") + bytes.fromhex(row["bytes"])
                for pc, row in sorted(rows.items()))).hexdigest(),
            "child_scope": "actual geometry/observer compared against original instructions; two voice-release calls use explicit test contracts",
            "reports": reports,
            "source_sha256": {path: hashlib.sha256((ROOT/path).read_bytes()).hexdigest() for path in paths},
        }
        (ROOT / "analysis/figures/native_context_command_input_checkpoint.json").write_text(json.dumps(data, indent=2) + "\n")


if __name__ == "__main__":
    main()
