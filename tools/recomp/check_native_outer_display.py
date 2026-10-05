"""Prove the complete native C1612C owner against sealed original instructions."""
import argparse
import hashlib
import json
import subprocess
from audit_command_dispatch import source_decoder
from check_record_region_probe import ROOT, build_oracle, default_bash
from recomp import classify, static_target


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=4096)
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("cases must be positive")
    state, decoder = source_decoder()
    seal = json.loads((ROOT / "analysis/data/command_dispatch_source_scope.json").read_text())["state_sha256"]
    if hashlib.sha256(state).hexdigest() != seal:
        raise RuntimeError("original state seal differs from audited source")
    rows, pending = {}, [0xc1612c]
    children = {0xc53f88, 0xc53f30, 0xc53f44, 0xc53ec0}
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
                raise RuntimeError(f"unresolved target at {pc:06X}")
            pending.append(pc+length)
            if target not in children:
                pending.append(target)
        elif kind in ("bra", "jmp"):
            if target is None:
                raise RuntimeError(f"unresolved target at {pc:06X}")
            pending.append(target)
        else:
            pending.append(pc+length)
    header = "/* Sealed original outer display owner; validation only. */\n"
    header += "static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } outer_source_bytes[]={\n"
    for pc, row in sorted(rows.items()):
        values = ",".join(f"0x{b:02x}" for b in bytes.fromhex(row["bytes"]))
        header += f"{{0x{pc:06X},{row['length']},{{{values}}}}},\n"
    header += "};\n"
    directory = ROOT / "build/recomp"
    directory.mkdir(parents=True, exist_ok=True)
    (directory / "native_outer_display_source.h").write_text(header)
    exe = build_oracle("native_outer_display_oracle", "tools/recomp/native_outer_display_oracle.c", default_bash())
    expected = {f"{pc:06X}" for pc in rows}
    reports, coverage = [], []
    for mode in ([], ["--real-palette"]):
        result = subprocess.run([str(exe), str(args.cases), *mode], cwd=ROOT,
                                capture_output=True, text=True, timeout=240)
        suffix = "_rgb4" if mode else ""
        (directory / f"native_outer_display{suffix}.log").write_text(result.stdout+result.stderr)
        if result.returncode:
            raise RuntimeError(result.stderr or result.stdout)
        visited = set()
        for line in result.stdout.splitlines():
            if line.startswith("visited:"):
                visited.update(line.partition(":")[2].split())
        reports.append(result.stdout.splitlines()[0])
        coverage.append(len(visited))
        print(reports[-1], flush=True)
        print(f"native outer display{suffix}: {len(visited)}/{len(rows)} original boundaries", flush=True)
        if args.cases >= 4096 and visited != expected:
            raise RuntimeError(f"uncovered original boundaries{suffix}: {sorted(expected-visited)}")
    if args.cases >= 4096:
        paths = ["port/outer_display.c", "port/outer_display.h", "port/outer_display_contract_test.c",
                 "port/input_callback.c", "port/input_callback.h", "port/input_palette.c", "port/input_palette.h",
                 "port/viewport_transition.c", "port/viewport_transition.h", "port/viewport_mode_state.h",
                 "port/renderer_page_setup.c", "port/five_plane_chip_binding.c", "port/five_plane_page.c", "port/copper_page.c",
                 "port/voice_program.c", "port/audio_update.c", "port/command_queue.c",
                 "port/amiga/rgb4.c", "port/amiga/rgb4.h", "port/amiga/viewport_list.c", "port/amiga/viewport_list.h",
                 "port/amiga/host_graphics.c", "port/amiga/host_compat.c", "port/amiga/guest_memory.c",
                 "tools/recomp/command_publication_oracle.c",
                 "tools/recomp/check_native_outer_display.py", "tools/recomp/native_outer_display_oracle.c"]
        checkpoint = {
            "status": "validated_native_outer_display_game_setup_and_scheduler_pending",
            "entry": "C1612C", "cases": args.cases*2,
            "source_boundaries": len(rows), "covered_boundaries": coverage,
            "native_cpu_dependency": False,
            "child_contracts": ["WaitBOVP, WaitBlit and LoadView supplied synchronization/presentation contracts; mutate shared owners, never ABI locals"],
            "rgb4_proof": "second run executes the real 32-word native palette backend versus the independently frozen-validated packed host service",
            "comparison": "full Chip/Slow RAM except CPU ABI stack C7FD00..C7FF00; ordered service phases, actual object/palette identities, all 32 words and every mutable fixture owner/list buffer at each call; final original D0/D1 word toggle outputs",
            "original_state_sha256": seal,
            "original_pc_bytes_sha256": hashlib.sha256(b"".join(
                pc.to_bytes(4, "big")+bytes.fromhex(row["bytes"]) for pc,row in sorted(rows.items()))).hexdigest(),
            "reports": reports,
            "source_sha256": {path: hashlib.sha256((ROOT/path).read_bytes()).hexdigest() for path in paths},
        }
        (ROOT / "analysis/figures/native_outer_display_checkpoint.json").write_text(json.dumps(checkpoint, indent=2)+"\n")


if __name__ == "__main__":
    main()
