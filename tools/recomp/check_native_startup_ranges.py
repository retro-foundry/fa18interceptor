"""Prove both complete startup range leaves on canonical native field views."""
import argparse
import hashlib
import json
import subprocess
from audit_command_dispatch import source_decoder
from check_record_region_probe import ROOT, build_oracle, default_bash

ENTRIES = (0xc090c2, 0xc090f2)


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
    rows, pc = {}, ENTRIES[0]
    while pc < 0xc0910c:
        length, opcode, handler, assembly = decoder.decode(pc)
        raw = b"".join(decoder.word(pc+i).to_bytes(2, "big") for i in range(0, length, 2))
        rows[pc] = {"length": length, "bytes": raw.hex(), "assembly": assembly}
        pc += length
    if pc != 0xc0910c:
        raise RuntimeError("startup source boundary differs")
    header = "/* Sealed original startup leaves; validation only. */\n"
    header += "static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } startup_source_bytes[]={\n"
    for pc, row in sorted(rows.items()):
        values = ",".join(f"0x{b:02x}" for b in bytes.fromhex(row["bytes"]))
        header += f"{{0x{pc:06X},{row['length']},{{{values}}}}},\n"
    header += "};\n"
    directory = ROOT / "build/recomp"
    directory.mkdir(parents=True, exist_ok=True)
    (directory / "native_startup_ranges_source.h").write_text(header)
    exe = build_oracle("native_startup_ranges_oracle", "tools/recomp/native_startup_ranges_oracle.c", default_bash())
    reports, visited = [], set()
    for entry in ENTRIES:
        result = subprocess.run([str(exe), str(args.cases), f"{entry:06X}"], cwd=ROOT,
                                capture_output=True, text=True, timeout=180)
        (directory / f"native_startup_ranges_{entry:06X}.log").write_text(result.stdout+result.stderr)
        if result.returncode:
            raise RuntimeError(result.stderr or result.stdout or "native startup differs")
        for line in result.stdout.splitlines():
            if line.startswith("visited:"):
                visited.update(line.partition(":")[2].split())
        reports.append(result.stdout.splitlines()[0])
        print(reports[-1], flush=True)
    expected = {f"{pc:06X}" for pc in rows}
    print(f"native startup: {len(visited)}/{len(rows)} original boundaries", flush=True)
    if args.cases >= 4096:
        if visited != expected:
            raise RuntimeError(f"uncovered original boundaries: {sorted(expected-visited)}")
        paths = ["port/startup_ranges.c", "port/startup_ranges.h", "port/startup_ranges_contract_test.c",
                 "port/field_bytes.h", "port/field_bytes_contract_test.c", "port/command_queue.c",
                 "port/command_queue.h", "tools/recomp/check_native_startup_ranges.py",
                 "tools/recomp/native_startup_ranges_oracle.c"]
        checkpoint = {
            "status": "validated_native_startup_range_leaves_full_record_ownership_and_bootstrap_pending",
            "entries": [f"{entry:06X}" for entry in ENTRIES], "cases": args.cases*len(ENTRIES),
            "source_boundaries": len(rows), "covered_boundaries": len(visited), "native_cpu_dependency": False,
            "child_contracts": [], "comparison": "every Chip/Slow RAM byte with no exclusions; live queue and supplied typed word owners",
            "ownership_limit": "Word references use existing spawn/command/cockpit/message/redraw owners; other words are supplied imports, including viewed-record offset. Complete native geometry/record identity and startup graph remain pending.",
            "original_state_sha256": seal,
            "original_pc_bytes_sha256": hashlib.sha256(b"".join(
                pc.to_bytes(4, "big")+bytes.fromhex(row["bytes"]) for pc,row in sorted(rows.items()))).hexdigest(),
            "reports": reports, "source_sha256": {p: hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},
        }
        (ROOT / "analysis/figures/native_startup_ranges_checkpoint.json").write_text(json.dumps(checkpoint, indent=2)+"\n")


if __name__ == "__main__":
    main()
