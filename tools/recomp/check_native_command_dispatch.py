"""Validate complete ordinary-state keyboard/pending parents and actual control children."""
import argparse
import hashlib
import json
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash
from audit_command_dispatch import source_decoder
from recomp import classify, static_target


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=8192)
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("cases must be positive")
    scope = ROOT / "analysis/data/command_dispatch_source_scope.json"
    source = json.loads(scope.read_text())
    state, decoder = source_decoder()
    if hashlib.sha256(state).hexdigest() != source["state_sha256"]:
        raise RuntimeError("original state seal differs from audited source")
    parents = {int(row["pc"], 16): row for row in source["instructions"]}
    rows = dict(parents)
    pending = [0xc1c214, 0xc08394, 0xc1b50c, 0xc1b510, 0xc1b514,
               0xc1b558, 0xc1b55c, 0xc1b560, 0xc1b602, 0xc08324,
               0xc082b8, 0xc091e0, 0xc0915a, 0xc17456, 0xc1748c, 0xc06c02]
    seen = set()
    while pending:
        pc = pending.pop()
        if pc in seen or pc in parents:
            continue
        seen.add(pc)
        length, opcode, handler, assembly = decoder.decode(pc)
        raw = b"".join(decoder.word(pc+i).to_bytes(2, "big") for i in range(0, length, 2))
        rows[pc] = {"pc": f"{pc:06X}", "length": length, "bytes": raw.hex()}
        kind = classify(handler)
        target = static_target(decoder, pc, opcode, handler, kind)
        if kind == "rts":
            continue
        if kind == "interp":
            raise RuntimeError(f"unexpected exception at {pc:06X}")
        if kind in ("jsr", "bsr"):
            if target not in (0xc53b00, 0xc53b18):
                raise RuntimeError(f"unexpected child at {pc:06X}")
            pending.append(pc+length)
        elif kind in ("bra", "jmp"):
            pending.append(target)
        elif kind in ("bcc", "dbcc"):
            pending.extend((pc+length, target))
        else:
            pending.append(pc+length)
    header = "/* Sealed original parents/control children, validation only. */\n"
    header += "static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } source_bytes[]={\n"
    for pc, row in sorted(rows.items()):
        values = ",".join(f"0x{byte:02x}" for byte in bytes.fromhex(row["bytes"]))
        header += f"{{0x{pc:06X},{row['length']},{{{values}}}}},\n"
    header += "};\n"
    directory = ROOT / "build/recomp"
    directory.mkdir(parents=True, exist_ok=True)
    (directory / "native_command_dispatch_source.h").write_text(header)
    (directory / "command_dispatch_owner_pcs.h").write_text(
        "/* Audited original owner PCs; validation only. */\nstatic const uint32_t owner_pcs[]={" +
        ",".join(f"0x{pc:06X}" for pc in sorted(parents)) + "};\n")
    exe = build_oracle("native_command_dispatch_oracle", "tools/recomp/native_command_dispatch_oracle.c", default_bash())
    visited, controls, reports = set(), set(), []
    for entry in ("C1AC28", "C1AD74"):
        result = subprocess.run([str(exe), str(args.cases), entry], cwd=ROOT,
                                capture_output=True, text=True, timeout=180)
        (directory / f"native_command_dispatch_{entry}.log").write_text(result.stdout+result.stderr)
        if result.returncode:
            raise RuntimeError(result.stderr or result.stdout)
        for line in result.stdout.splitlines():
            if line.startswith("visited:"):
                visited.update(line.partition(":")[2].split())
            elif line.startswith("control_visited:"):
                controls.update(line.partition(":")[2].split())
        reports.append(result.stdout.splitlines()[0])
        print(reports[-1], flush=True)
    print(f"native parents: {args.cases*2} complete calls, {len(visited)}/{len(parents)} parent boundaries, {len(controls)}/{len(seen)} actual-child boundaries")
    if args.cases >= 8192:
        exits = {"C06BF0", "C06BF6", "C06BFA", "C06C00", "C1AC18", "C1AC20", "C1AC26",
                 "C1AD70", "C1AD72", "C1C2B8", "C1C23C", "C1C2B6"}
        if not exits <= visited:
            raise RuntimeError(f"uncovered exits: {sorted(exits-visited)}")
        expected_controls = {f"{pc:06X}" for pc in seen}
        if controls != expected_controls:
            raise RuntimeError(f"child coverage: missing {sorted(expected_controls-controls)}, extra {sorted(controls-expected_controls)}")
        paths = ["port/command_dispatch.c", "port/command_dispatch.h", "port/command_dispatch_controls.c",
                 "port/input_callback_registration.c", "port/input_callback_registration.h",
                 "port/command_dispatch_contract_test.c", "port/command_input.c", "port/command_input.h",
                 "port/command_queue.c", "port/command_queue.h", "port/indexed_controls.c",
                 "port/flight_command_input.c", "port/view_command_input.c", "port/view_command_controls.c",
                 "port/context_command_input.c", "port/context_command_controls.c",
                 "tools/recomp/native_command_dispatch_oracle.c", "tools/recomp/native_command_dispatch_fields.h",
                 "tools/recomp/check_native_command_dispatch.py", "tools/recomp/command_dispatch_oracle.c",
                 str(scope.relative_to(ROOT))]
        checkpoint = {
            "status": "validated_native_parents_remaining_child_backends_and_runtime_integration_pending",
            "cases": args.cases*2, "source_boundaries": len(parents), "covered_boundaries": len(visited),
            "child_source_boundaries": len(seen), "covered_child_boundaries": len(controls),
            "native_cpu_dependency": False,
            "comparison": "all Chip/Slow RAM without exclusions, published events, selector carries, ordered child inputs and full RAM at each child boundary",
            "child_scope": "actual direction/throttle/space-release/eject publication, zoom/redraw, geometry/observer, registration/removal and fault RTS instructions; explicit audio/space-press/spawn/host-registration contracts",
            "original_state_sha256": source["state_sha256"],
            "original_pc_bytes_sha256": hashlib.sha256(b"".join(
                pc.to_bytes(4, "big")+bytes.fromhex(row["bytes"]) for pc,row in sorted(rows.items()))).hexdigest(),
            "reports": reports,
            "source_sha256": {path: hashlib.sha256((ROOT/path).read_bytes()).hexdigest() for path in paths},
        }
        (ROOT / "analysis/figures/native_command_parent_checkpoint.json").write_text(json.dumps(checkpoint, indent=2)+"\n")


if __name__ == "__main__":
    main()
