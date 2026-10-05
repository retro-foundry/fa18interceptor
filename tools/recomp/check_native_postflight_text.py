"""Prove the complete native text publisher and signed-width hexadecimal child."""
import argparse
import hashlib
import json
import subprocess
from audit_command_dispatch import source_decoder
from check_record_region_probe import ROOT, build_oracle, default_bash
from recomp import classify, static_target

ENTRIES = (0xc0f812, 0xc0f56a)
CHILDREN = {0xc08f26}


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
        if pc in rows or pc in CHILDREN:
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
    header = "/* Sealed original text, hex and menu-audio closure; validation only. */\n"
    header += "static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } selection_source_bytes[]={\n"
    for pc, row in sorted(rows.items()):
        values = ",".join(f"0x{b:02x}" for b in bytes.fromhex(row["bytes"]))
        header += f"{{0x{pc:06X},{row['length']},{{{values}}}}},\n"
    header += "};\n"
    directory = ROOT / "build/recomp"
    directory.mkdir(parents=True, exist_ok=True)
    (directory / "native_postflight_text_source.h").write_text(header)
    exe = build_oracle("native_postflight_text_oracle", "tools/recomp/native_postflight_text_oracle.c", default_bash())
    reports, visited = [], set()
    for entry in ENTRIES:
        result = subprocess.run([str(exe), str(args.cases), f"{entry:06X}"], cwd=ROOT,
                                capture_output=True, text=True, timeout=240)
        (directory / f"native_postflight_text_{entry:06X}.log").write_text(result.stdout+result.stderr)
        if result.returncode:
            raise RuntimeError(result.stderr or result.stdout or "native text differs")
        for line in result.stdout.splitlines():
            if line.startswith("visited:"):
                visited.update(line.partition(":")[2].split())
        reports.append(result.stdout.splitlines()[0])
        print(reports[-1], flush=True)
    expected = {f"{pc:06X}" for pc in rows}
    print(f"native text: {len(visited)}/{len(rows)} original boundaries", flush=True)
    if args.cases >= 4096:
        if visited != expected:
            raise RuntimeError(f"uncovered original boundaries: {sorted(expected-visited)}")
        paths = ["port/postflight_text.c", "port/postflight_text.h", "port/postflight_text_contract_test.c",
                 "port/hex_field.c", "port/hex_field.h", "port/stage_callback.h", "port/post_input_tick.h",
                 "port/post_input_followup.h", "port/display_palette_assets.c", "port/display_palette_assets.h",
                 "port/menu_record.c", "port/menu_record.h", "port/audio_selection.c", "port/audio_selection.h",
                 "port/voice_selection.c", "port/voice_selection.h",
                 "tools/recomp/check_native_postflight_text.py", "tools/recomp/native_postflight_text_oracle.c",
                 "tools/recomp/native_audio_selection_oracle.c"]
        checkpoint = {
            "status": "validated_native_complete_text_publisher_bootstrap_and_full_runtime_pending",
            "entries": [f"{entry:06X}" for entry in ENTRIES], "cases": args.cases*len(ENTRIES),
            "source_boundaries": len(rows), "covered_boundaries": len(visited),
            "native_cpu_dependency": False,
            "child_contracts": ["C08F26 bootstrap explicitly contracted; native graph not completed. Shared palette pointer, mode, transition, countdown and checksum changes supplied at this boundary; volatile CPU return registers varied independently."],
            "actual_children": ["C0F56A complete formatter", "C17B96 menu sound", "C17B2C sound selection", "C17B08/C0F4A6 voice release", "C4FFB0 acknowledgement"],
            "comparison": "every Chip/Slow RAM byte except CPU ABI stack C7FD00..C7FF00 for parent, C7FD00..C7FF10 for standalone formatter including its rewritten by-value argument slots; full RAM before bootstrap and at ordered audio acknowledgements; final interrupt/custom register state",
            "coverage_cases": "all 256 signed width bytes, source-order overlapping palette copies, bootstrap replacement pointers, all signature-test combinations, last mismatch zero selecting audio, both real menu sound branches and fading gates",
            "original_state_sha256": seal,
            "original_pc_bytes_sha256": hashlib.sha256(b"".join(
                pc.to_bytes(4, "big")+bytes.fromhex(row["bytes"]) for pc,row in sorted(rows.items()))).hexdigest(),
            "reports": reports,
            "source_sha256": {p: hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},
        }
        (ROOT / "analysis/figures/native_postflight_text_checkpoint.json").write_text(json.dumps(checkpoint, indent=2)+"\n")


if __name__ == "__main__":
    main()
