"""Compare complete graphics setup and second-display entry with original code."""
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
        raise RuntimeError("original state seal differs")
    rows, pending = {}, [0xc15db4, 0xc160d6]
    children = {0xc53ca0, 0xc50de8, 0xc53f54, 0xc53ef0, 0xc53f68,
                0xc53edc, 0xc53b30, 0xc53fec, 0xc53f18, 0xc53f04}
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
            raise RuntimeError(f"unexpected instruction at {pc:06X}")
        if kind in ("jsr", "bsr", "bcc", "dbcc"):
            if target is None:
                raise RuntimeError(f"unresolved target at {pc:06X}")
            # C50DE8 -> C522D0 -> C0DFE6 restores the startup stack at
            # C0E028 and returns to the loader, not this parent call site.
            if target != 0xc50de8:
                pending.append(pc+length)
            if target not in children:
                pending.append(target)
        elif kind in ("bra", "jmp"):
            if target is None:
                raise RuntimeError(f"unresolved target at {pc:06X}")
            pending.append(target)
        else:
            pending.append(pc+length)
    length, opcode, handler, assembly = decoder.decode(0xc15d68)
    if classify(handler) != "bsr" or static_target(decoder,0xc15d68,opcode,handler,"bsr") != 0xc160d6:
        raise RuntimeError("second display's original call-site evidence differs")
    if "c07f74" not in decoder.decode(0xc0e028)[3].lower() or "A7" not in decoder.decode(0xc0e028)[3]:
        raise RuntimeError("original termination stack-unwind evidence differs")
    header = "/* Sealed graphics setup, second display and real table child; validation only. */\n"
    header += "static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } graphics_source_bytes[]={\n"
    for pc, row in sorted(rows.items()):
        values = ",".join(f"0x{b:02x}" for b in bytes.fromhex(row["bytes"]))
        header += f"{{0x{pc:06X},{row['length']},{{{values}}}}},\n"
    header += "};\n"
    directory = ROOT / "build/recomp"
    directory.mkdir(parents=True, exist_ok=True)
    (directory / "native_graphics_setup_source.h").write_text(header)
    exe = build_oracle("native_graphics_setup_oracle", "tools/recomp/native_graphics_setup_oracle.c", default_bash())
    result = subprocess.run([str(exe), str(args.cases)], cwd=ROOT,capture_output=True,text=True,timeout=240)
    (directory / "native_graphics_setup.log").write_text(result.stdout+result.stderr)
    if result.returncode:
        raise RuntimeError(result.stderr or result.stdout)
    visited = set()
    for line in result.stdout.splitlines():
        if line.startswith("visited:"):
            visited.update(line.partition(":")[2].split())
    print(result.stdout.splitlines()[0], flush=True)
    print(f"native graphics setup: {len(visited)}/{len(rows)} original boundaries", flush=True)
    if args.cases >= 4096 and visited != {f"{pc:06X}" for pc in rows}:
        raise RuntimeError(f"uncovered: {sorted({f'{pc:06X}' for pc in rows}-visited)}")
    if args.cases >= 4096:
        paths = ["port/graphics_setup.c", "port/graphics_setup.h", "port/graphics_storage.c", "port/graphics_storage.h",
                 "port/graphics_setup_contract_test.c", "port/renderer_page_layout.h", "port/renderer_page_setup.c",
                 "port/input_callback.c", "port/input_callback.h", "port/amiga/viewport_list.c", "port/amiga/rgb4.c",
                 "port/amiga/host_graphics.c", "port/amiga/host_compat.c", "port/amiga/guest_memory.c",
                 "port/five_plane_chip_binding.c", "port/five_plane_page.c", "port/copper_page.c",
                 "tools/recomp/check_native_graphics_setup.py", "tools/recomp/native_graphics_setup_oracle.c"]
        checkpoint = {
            "status": "validated_native_graphics_setup_full_startup_and_scheduler_pending",
            "entries": ["C15DB4", "C160D6"], "case_fixtures": args.cases,
            "actual_child": "C2F4DE renderer table layout",
            "source_boundaries": len(rows), "covered_boundaries": len(visited),
            "native_cpu_dependency": False,
            "comparison": "full Chip/Slow RAM outside ABI stack C7FD00..C7FF00 at each allocation/construction/termination boundary and final return; native object identities serialized only in validation",
            "child_contracts": ["OpenLibrary and initialization data use accepted host semantics", "termination is a controlled nonreturning child; cleanup remains outside this owner"],
            "terminal_fallthrough": {"unreachable_parent_pcs": ["C15DDE", "C15F88", "C15FBE"], "evidence": "C50DE8 -> C522D0 -> C0DFE6, startup stack restoration at C0E028 then RTS at C0E032; these three return-site instructions are static bytes, not returning error paths"},
            "actual_native_backend": "native bounded plane/map/dynamic allocation and record/merge services; compared to explicit source allocation contracts and packed MakeVPort/MrgCop",
            "original_state_sha256": seal,
            "original_pc_bytes_sha256": hashlib.sha256(b"".join(pc.to_bytes(4,'big')+bytes.fromhex(row['bytes']) for pc,row in sorted(rows.items()))).hexdigest(),
            "report": result.stdout.splitlines()[0],
            "source_sha256": {p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},
        }
        (ROOT / "analysis/figures/native_graphics_setup_checkpoint.json").write_text(json.dumps(checkpoint,indent=2)+"\n")


if __name__ == "__main__":
    main()
