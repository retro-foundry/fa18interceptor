"""Prove native sound selection and the complete menu pair against original code."""
import argparse
import hashlib
import json
import subprocess
from audit_command_dispatch import source_decoder
from check_record_region_probe import ROOT, build_oracle, default_bash
from recomp import classify, static_target

ENTRIES = (0xc17b96, 0xc17b2c)


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
    header = "/* Sealed original sound selection closure; validation only. */\n"
    header += "static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } selection_source_bytes[]={\n"
    for pc, row in sorted(rows.items()):
        values = ",".join(f"0x{b:02x}" for b in bytes.fromhex(row["bytes"]))
        header += f"{{0x{pc:06X},{row['length']},{{{values}}}}},\n"
    header += "};\n"
    directory = ROOT / "build/recomp"
    directory.mkdir(parents=True, exist_ok=True)
    (directory / "native_audio_selection_source.h").write_text(header)
    exe = build_oracle("native_audio_selection_oracle", "tools/recomp/native_audio_selection_oracle.c", default_bash())
    reports, visited = [], set()
    for entry in ENTRIES:
        result = subprocess.run([str(exe), str(args.cases), f"{entry:06X}"], cwd=ROOT,
                                capture_output=True, text=True, timeout=240)
        (directory / f"native_audio_selection_{entry:06X}.log").write_text(result.stdout+result.stderr)
        if result.returncode:
            raise RuntimeError(result.stderr or result.stdout or "native selection differs")
        for line in result.stdout.splitlines():
            if line.startswith("visited:"):
                visited.update(line.partition(":")[2].split())
        reports.append(result.stdout.splitlines()[0])
        print(reports[-1], flush=True)
    expected = {f"{pc:06X}" for pc in rows}
    print(f"native audio selection: {len(visited)}/{len(rows)} original boundaries", flush=True)
    if args.cases >= 4096:
        if visited != expected:
            raise RuntimeError(f"uncovered original boundaries: {sorted(expected-visited)}")
        paths = ["port/voice_selection.c", "port/voice_selection.h", "port/voice_selection_contract_test.c",
                 "port/voice_program.h", "port/audio_selection.c", "port/audio_selection.h",
                 "port/audio_selection_contract_test.c", "port/command_effects.c", "port/command_effects.h",
                 "tools/recomp/check_native_audio_selection.py", "tools/recomp/native_audio_selection_oracle.c"]
        checkpoint = {
            "status": "validated_native_menu_audio_selection_asset_loading_playback_and_scheduler_pending",
            "entries": [f"{entry:06X}" for entry in ENTRIES], "cases": args.cases*len(ENTRIES),
            "source_boundaries": len(rows), "covered_boundaries": len(visited),
            "native_cpu_dependency": False, "child_contracts": False,
            "host_contract": "real original interrupt acknowledgement instructions execute; supplied shared sound-table/fade/enable/mask mutations are applied in both runs after the acknowledgement",
            "comparison": "every Chip/Slow RAM byte except CPU ABI stack C7FD00..C7FF00; full RAM and ordered channel/mask at every acknowledgement; final interrupt/custom register state",
            "original_state_sha256": seal,
            "original_pc_bytes_sha256": hashlib.sha256(b"".join(
                pc.to_bytes(4, "big")+bytes.fromhex(row["bytes"]) for pc,row in sorted(rows.items()))).hexdigest(),
            "reports": reports,
            "source_sha256": {p: hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},
        }
        (ROOT / "analysis/figures/native_audio_selection_checkpoint.json").write_text(json.dumps(checkpoint, indent=2)+"\n")


if __name__ == "__main__":
    main()
