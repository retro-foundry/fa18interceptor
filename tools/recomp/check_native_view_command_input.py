"""Validate all 16 native view actions and both actual native children."""
import argparse
import hashlib
import json
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash
from check_view_commands import ENTRIES
from audit_command_dispatch import source_decoder
from recomp import classify, static_target


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=2048)
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("cases must be positive")
    scope = ROOT / "analysis/data/command_dispatch_source_scope.json"
    parent = [row for row in json.loads(scope.read_text())["instructions"]
              if 0xc1b77c <= int(row["pc"], 16) < 0xc1bb7a]
    _, decoder = source_decoder()
    rows = {int(row["pc"], 16): row for row in parent}
    pending, seen = [0xc08324, 0xc082b8], set()
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
    (ROOT / "build/recomp/native_view_command_input_source.h").write_text(header)
    exe = build_oracle("native_view_command_input_oracle", "tools/recomp/native_view_command_input_oracle.c", default_bash())
    visited, controls, reports = set(), set(), []
    for entry in ENTRIES:
        result = subprocess.run([str(exe), str(args.cases), entry], cwd=ROOT,
                                capture_output=True, text=True, timeout=180)
        (ROOT / f"build/recomp/native_view_command_input_{entry}.log").write_text(result.stdout + result.stderr)
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
    if args.cases >= 2048 and (visited != expected or controls != expected_controls):
        raise RuntimeError(f"boundary mismatch: parent missing {sorted(expected-visited)}, extra {sorted(visited-expected)}, children missing {sorted(expected_controls-controls)}, extra {sorted(controls-expected_controls)}")
    print(f"native view actions: {len(ENTRIES)*args.cases} calls, {len(visited)}/{len(expected)} parent boundaries, {len(controls)}/{len(expected_controls)} actual child boundaries")
    if args.cases >= 2048:
        paths = ["port/view_command_input.c", "port/view_command_input.h", "port/view_command_controls.c",
                 "port/view_command_input_contract_test.c", "port/flight_command_input.h",
                 "port/command_input.h", "port/command_types.h", "port/indexed_controls.h",
                 "tools/recomp/check_native_view_command_input.py",
                 "tools/recomp/native_view_command_input_oracle.c", "tools/recomp/view_commands_oracle.c",
                 str(scope.relative_to(ROOT))]
        data = {
            "status": "validated_native_view_actions_full_runtime_integration_pending",
            "actions": len(ENTRIES), "cases": len(ENTRIES)*args.cases,
            "source_boundaries": len(expected), "covered_boundaries": len(visited),
            "child_source_boundaries": len(expected_controls), "covered_child_boundaries": len(controls),
            "native_cpu_dependency": False, "children": ["C08324", "C082B8"],
            "child_scope": "both actual native children compared against original instructions; no controlled child replacements",
            "reports": reports,
            "source_sha256": {path: hashlib.sha256((ROOT/path).read_bytes()).hexdigest() for path in paths},
        }
        (ROOT / "analysis/figures/native_view_command_input_checkpoint.json").write_text(json.dumps(data, indent=2) + "\n")


if __name__ == "__main__":
    main()
