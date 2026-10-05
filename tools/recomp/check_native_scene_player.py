"""Prove complete native player setup leaves and the actual bootstrap clear block."""
import argparse
import hashlib
import json
import subprocess
from audit_command_dispatch import source_decoder
from check_record_region_probe import ROOT, build_oracle, default_bash
from recomp import classify, static_target

ENTRIES = (0xc0840e, 0xc09620, 0xc095c0, 0xc0910c, 0xc0915a, 0xc08f76)
STOPS = {0xc08faa}


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
        if pc in rows or pc in STOPS:
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
    header = "/* Sealed original player leaves/clear block; validation only. */\n"
    header += "static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } scene_source_bytes[]={\n"
    for pc, row in sorted(rows.items()):
        values = ",".join(f"0x{b:02x}" for b in bytes.fromhex(row["bytes"]))
        header += f"{{0x{pc:06X},{row['length']},{{{values}}}}},\n"
    header += "};\n"
    directory = ROOT / "build/recomp"
    directory.mkdir(parents=True, exist_ok=True)
    (directory / "native_scene_player_source.h").write_text(header)
    exe = build_oracle("native_scene_player_oracle", "tools/recomp/native_scene_player_oracle.c", default_bash())
    reports, visited = [], set()
    for entry in ENTRIES:
        result = subprocess.run([str(exe), str(args.cases), f"{entry:06X}"], cwd=ROOT,
                                capture_output=True, text=True, timeout=180)
        (directory / f"native_scene_player_{entry:06X}.log").write_text(result.stdout+result.stderr)
        if result.returncode:
            raise RuntimeError(result.stderr or result.stdout or "native scene differs")
        for line in result.stdout.splitlines():
            if line.startswith("visited:"):
                visited.update(line.partition(":")[2].split())
        reports.append(result.stdout.splitlines()[0])
        print(reports[-1], flush=True)
    expected = {f"{pc:06X}" for pc in rows}
    print(f"native scene: {len(visited)}/{len(rows)} original boundaries", flush=True)
    if args.cases >= 4096:
        if visited != expected:
            raise RuntimeError(f"uncovered original boundaries: {sorted(expected-visited)}")
        paths = ["port/native_scene_records.c", "port/native_scene_records.h", "port/scene_player_setup.c",
                 "port/scene_player_setup.h", "port/scene_player_contract_test.c", "port/context_command_controls.c",
                 "port/context_command_input.h", "port/command_queue.c", "port/command_queue.h", "port/field_bytes.h",
                 "tools/recomp/check_native_scene_player.py", "tools/recomp/native_scene_player_oracle.c"]
        checkpoint = {
            "status": "validated_native_player_leaves_and_shared_record_clear_full_bootstrap_pending",
            "complete_entries": [f"{entry:06X}" for entry in ENTRIES if entry!=0xc08f76],
            "bounded_blocks": ["C08F76..C08FAA all 16 control prefixes and workspace records; not complete C08F26"],
            "cases": args.cases*len(ENTRIES), "source_boundaries": len(rows), "covered_boundaries": len(visited),
            "native_cpu_dependency": False, "child_contracts": [],
            "actual_children": ["C0840E complete real child of C09620", "C0915A existing native observer reused"],
            "comparison": "every Chip/Slow RAM byte, excluding only CPU ABI stack C7FD00..C7FF00 for C0840E/C09620; no exclusions for C095C0, C0910C/C0915A or C08F76 block; independently mapped named record owners and explicit tuple results",
            "coverage_cases": "all byte-phase branches, full original record/workspace roundtrips, live shared aircraft flags/position/angle/inverse fields, preserve +A4..+1FF, all 16 records and all work records, initial/global-state variation",
            "ownership_limit": "Record slots and mapped fields are shared ordinary objects; unported record positions require supplied original imports. Complete startup viewed/selected identity, placement/update/refresh children, asset loading and game-loop composition remain pending.",
            "original_state_sha256": seal,
            "original_pc_bytes_sha256": hashlib.sha256(b"".join(
                pc.to_bytes(4, "big")+bytes.fromhex(row["bytes"]) for pc,row in sorted(rows.items()))).hexdigest(),
            "reports": reports, "source_sha256": {p: hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},
        }
        (ROOT / "analysis/figures/native_scene_player_checkpoint.json").write_text(json.dumps(checkpoint, indent=2)+"\n")


if __name__ == "__main__":
    main()
