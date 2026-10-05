"""Compare native command effects with their complete original instruction closures."""
import argparse
import hashlib
import json
import subprocess
from audit_command_dispatch import source_decoder
from check_record_region_probe import ROOT, build_oracle, default_bash
from recomp import classify, static_target

ENTRIES = (0xc0833e, 0xc25704, 0xc3318e, 0xc33186, 0xc0f4a6, 0xc17f8c)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=2048)
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("cases must be positive")
    state, decoder = source_decoder()
    seal = json.loads((ROOT / "analysis/data/command_dispatch_source_scope.json").read_text())["state_sha256"]
    if hashlib.sha256(state).hexdigest() != seal:
        raise RuntimeError("original state seal differs from audited source")
    rows, pending = {}, list(ENTRIES)
    while pending:
        pc = pending.pop()
        if pc in rows:
            continue
        length, opcode, handler, assembly = decoder.decode(pc)
        raw = b"".join(decoder.word(pc+i).to_bytes(2, "big") for i in range(0, length, 2))
        rows[pc] = {"pc": pc, "length": length, "bytes": raw.hex(), "assembly": assembly}
        kind = classify(handler)
        target = static_target(decoder, pc, opcode, handler, kind)
        if kind == "rts":
            continue
        if kind == "interp":
            raise RuntimeError(f"unexpected exception instruction at {pc:06X}")
        if kind in ("jsr", "bsr", "bcc", "dbcc"):
            if target is None:
                raise RuntimeError(f"unresolved target at {pc:06X}")
            pending.extend((pc+length, target))
        elif kind in ("bra", "jmp"):
            if target is None:
                raise RuntimeError(f"unresolved target at {pc:06X}")
            pending.append(target)
        else:
            pending.append(pc+length)
    header = "/* Sealed original effects and all children, validation only. */\n"
    header += "static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } effect_source_bytes[]={\n"
    for pc, row in sorted(rows.items()):
        values = ",".join(f"0x{b:02x}" for b in bytes.fromhex(row["bytes"]))
        header += f"{{0x{pc:06X},{row['length']},{{{values}}}}},\n"
    header += "};\n"
    directory = ROOT / "build/recomp"
    directory.mkdir(parents=True, exist_ok=True)
    (directory / "native_command_effects_source.h").write_text(header)
    exe = build_oracle("native_command_effects_oracle", "tools/recomp/native_command_effects_oracle.c", default_bash())
    reports, visited = [], set()
    for entry in ENTRIES:
        result = subprocess.run([str(exe), str(args.cases), f"{entry:06X}"], cwd=ROOT,
                                capture_output=True, text=True, timeout=180)
        (directory / f"native_command_effects_{entry:06X}.log").write_text(result.stdout+result.stderr)
        if result.returncode:
            raise RuntimeError(result.stderr or result.stdout)
        for line in result.stdout.splitlines():
            if line.startswith("visited:"):
                visited.update(line.partition(":")[2].split())
        reports.append(result.stdout.splitlines()[0])
        print(reports[-1], flush=True)
    print(f"native command effects: {args.cases*len(ENTRIES)} calls, {len(visited)}/{len(rows)} original boundaries")
    if args.cases >= 2048:
        if visited != {f"{pc:06X}" for pc in rows}:
            raise RuntimeError(f"uncovered original boundaries: {sorted({f'{pc:06X}' for pc in rows}-visited)}")
        paths = ["port/command_effects.c", "port/command_effects.h", "port/command_effects_contract_test.c",
                 "port/command_queue.c", "port/command_queue.h", "port/flight_command_input.h",
                 "port/indexed_controls.h", "tools/recomp/check_native_command_effects.py",
                 "tools/recomp/native_command_effects_oracle.c"]
        checkpoint = {
            "status": "validated_native_command_effects_audio_update_playback_and_runtime_pending",
            "entries": [f"{entry:06X}" for entry in ENTRIES], "cases": args.cases*len(ENTRIES),
            "source_boundaries": len(rows), "covered_boundaries": len(visited),
            "uncovered_boundaries": sorted({f"{pc:06X}" for pc in rows}-visited),
            "native_cpu_dependency": False, "child_contracts": False,
            "comparison": "full Chip/Slow RAM except the original CPU ABI stack C7FD00..C7FF00, full returned event, ordered audio acknowledgements and complete RAM at each acknowledgement; native hardware acknowledgement payloads applied only in validation",
            "original_state_sha256": seal,
            "original_pc_bytes_sha256": hashlib.sha256(b"".join(
                pc.to_bytes(4, "big")+bytes.fromhex(row["bytes"]) for pc,row in sorted(rows.items()))).hexdigest(),
            "reports": reports,
            "source_sha256": {path: hashlib.sha256((ROOT/path).read_bytes()).hexdigest() for path in paths},
        }
        (ROOT / "analysis/figures/native_command_effects_checkpoint.json").write_text(json.dumps(checkpoint, indent=2)+"\n")


if __name__ == "__main__":
    main()
