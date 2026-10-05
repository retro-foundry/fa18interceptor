"""Retain the sealed bootstrap-parent proof with three historical child contracts."""
import argparse
import hashlib
import json
import re
import subprocess
from audit_command_dispatch import source_decoder
from check_record_region_probe import ROOT, build_oracle, default_bash
from recomp import classify, static_target

ENTRIES = (0xc08f26, 0xc09620, 0xc090c2, 0xc1c40c)
CHILDREN = {0xc09266, 0xc1c63e, 0xc1c860}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=4096)
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("cases must be positive")
    state, decoder = source_decoder()
    seal = json.loads((ROOT / "analysis/data/command_dispatch_source_scope.json").read_text())["state_sha256"]
    if hashlib.sha256(state).hexdigest() != seal:
        raise RuntimeError("original source seal differs")
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
                raise RuntimeError(f"unresolved target at {pc:06X}")
            pending.extend((pc+length, target))
        elif kind in ("bra", "jmp"):
            if target is None:
                raise RuntimeError(f"unresolved transfer at {pc:06X}")
            pending.append(target)
        else:
            pending.append(pc+length)
    header = "/* Sealed original bootstrap/player/startup instructions; validation only. */\n"
    header += "static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } scene_source_bytes[]={\n"
    for pc, row in sorted(rows.items()):
        values = ",".join(f"0x{b:02x}" for b in bytes.fromhex(row["bytes"]))
        header += f"{{0x{pc:06X},{row['length']},{{{values}}}}},\n"
    header += "};\n"
    directory = ROOT / "build/recomp"
    directory.mkdir(parents=True, exist_ok=True)
    (directory / "native_scene_bootstrap_source.h").write_text(header)
    exe = build_oracle("native_scene_bootstrap_oracle", "tools/recomp/native_scene_bootstrap_oracle.c", default_bash())
    reports, visited, gate_assets = [], set(), None
    for entry in ENTRIES:
        result = subprocess.run([str(exe), str(args.cases), f"{entry:06X}"], cwd=ROOT,
                                capture_output=True, text=True, timeout=180)
        (directory / f"native_scene_bootstrap_{entry:06X}.log").write_text(result.stdout+result.stderr)
        if result.returncode:
            raise RuntimeError(result.stderr or result.stdout or "native bootstrap differs")
        for line in result.stdout.splitlines():
            if line.startswith("visited:"):
                visited.update(line.partition(":")[2].split())
        reports.append(result.stdout.splitlines()[0])
        print(reports[-1], flush=True)
        if entry==0xc1c40c:
            match=re.search(r"gate assets: (\d+) original Hunk-66 bytes; (\d+)/65536 bit indices exercised",result.stdout)
            if not match:
                raise RuntimeError("missing gate asset/bit coverage report")
            gate_assets={"original_hunk_bytes":int(match[1]),"covered_bit_indices":int(match[2])}
            print(match[0],flush=True)
    print(f"native bootstrap: {len(visited)}/{len(rows)} original boundaries", flush=True)
    if args.cases >= 4096:
        expected = {f"{pc:06X}" for pc in rows}
        if visited != expected:
            raise RuntimeError(f"uncovered boundaries: {sorted(expected-visited)}")
        paths = ["port/scene_bootstrap_native.c", "port/scene_bootstrap_native.h",
                 "port/template_bitmask_buffers.c", "port/template_bitmask_buffers.h",
                 "port/template_bitmask_buffers_contract_test.c", "tools/recomp/native_template_gate_fixture.h",
                 "port/romfree/placement.h", "port/disk.c", "port/hunk.c",
                 "port/viewed_record_word.c", "port/viewed_record_word.h",
                 "port/scene_bootstrap_native_contract_test.c", "port/field_bytes.h",
                 "port/field_bytes_contract_test.c", "port/startup_ranges.c", "port/startup_ranges.h",
                 "port/command_queue.c", "port/command_queue.h", "port/renderer_clear.c",
                 "port/renderer_clear.h", "port/native_scene_records.c", "port/native_scene_records.h",
                 "port/scene_player_setup.c", "port/scene_player_setup.h", "port/context_command_controls.c",
                 "tools/recomp/native_scene_player_oracle.c",
                 "tools/recomp/check_native_scene_bootstrap.py", "tools/recomp/native_scene_bootstrap_oracle.c"]
        checkpoint = {
            "status": "historical_parent_proof_with_real_gate_and_three_child_contracts_placement_now_direct_in_product",
            "complete_parent": "C08F26", "supplementary_entries": ["C09620", "C090C2", "C1C40C"],
            "cases": args.cases*len(ENTRIES), "source_boundaries": len(rows), "covered_boundaries": len(visited),
            "native_cpu_dependency": False,
            "gate_assets": gate_assets,
            "original_adf_sha256": hashlib.sha256((ROOT/"FA-18 Interceptor (1988)(Electronic Arts)[cr A-Ha].adf").read_bytes()).hexdigest(),
            "child_contracts": [f"{pc:06X}" for pc in sorted(CHILDREN)],
            "actual_children": ["C090C2", "C090F2", "C2FD22", "C09620 including C0840E", "C0910C", "C0915A", "C1C40C including real C06C02 RTS fault hook"],
            "comparison": "all Chip/Slow RAM; only CPU ABI save/return stack C7FD00..C7FF00 excluded for C08F26/C09620/C1C40C; no exclusion for C090C2. Three ordered historical child-entry snapshots, independent child mutations and named record owners compared.",
            "coverage_cases": "all 16 valid viewed identities; both phase branches; ten renderer planes; real gate child, all 65536 bit indices, negative/odd/0/1/7FFF lengths, across-row/axis/adjacent-owner writes, source fault codes 43/44/45 and real RTS hook, all original Hunk-66 bytes with relocation-aware comparison and direct live stream binding, bound stream changed at placement boundary before real expansion",
            "ownership_limit": "VIEW_RECORD accepts the supplied 16-record bank. Signed gate neighbours require actual bounded field owners; missing data is explicit. This historical oracle still contracts placement and therefore proves only parent sequencing for that boundary; the product now calls its separately tested native placement owner directly. Update/refresh graphs, tenth plane production, loading/checksums, sample output and scheduling remain pending. Native main does not invoke this graph.",
            "original_state_sha256": seal,
            "original_pc_bytes_sha256": hashlib.sha256(b"".join(pc.to_bytes(4, "big")+bytes.fromhex(row["bytes"]) for pc,row in sorted(rows.items()))).hexdigest(),
            "reports": reports, "source_sha256": {p: hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},
        }
        (ROOT / "analysis/figures/native_scene_bootstrap_parent_checkpoint.json").write_text(json.dumps(checkpoint, indent=2)+"\n")


if __name__ == "__main__":
    main()
