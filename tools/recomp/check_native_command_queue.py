"""Prove native command publication, including every signed queue destination."""
import argparse
import hashlib
import json
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash
from audit_command_dispatch import source_decoder


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=73728)
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("cases must be positive")
    scope = ROOT / "analysis/data/command_dispatch_source_scope.json"
    source = json.loads(scope.read_text())
    state, decoder = source_decoder()
    if hashlib.sha256(state).hexdigest() != source["state_sha256"]:
        raise RuntimeError("original state seal differs from audited source")
    rows = [row for row in source["instructions"] if 0xc1c23c <= int(row["pc"], 16) < 0xc1c2b8]
    header = "/* Sealed original publication bytes; validation only. */\n"
    header += "static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } source_bytes[]={\n"
    for row in rows:
        pc = int(row["pc"], 16)
        raw = b"".join(decoder.word(pc+i).to_bytes(2, "big") for i in range(0, row["length"], 2))
        if raw.hex() != row["bytes"]:
            raise RuntimeError(f"original source bytes differ at {pc:06X}")
        values = ",".join(f"0x{byte:02x}" for byte in raw)
        header += f"{{0x{pc:06X},{row['length']},{{{values}}}}},\n"
    header += "};\n"
    (ROOT / "build/recomp").mkdir(parents=True, exist_ok=True)
    (ROOT / "build/recomp/native_command_queue_source.h").write_text(header)
    exe = build_oracle("native_command_queue_oracle", "tools/recomp/native_command_queue_oracle.c", default_bash())
    result = subprocess.run([str(exe), str(args.cases)], cwd=ROOT,
                            capture_output=True, text=True, timeout=180)
    (ROOT / "build/recomp/native_command_queue.log").write_text(result.stdout + result.stderr)
    if result.returncode:
        raise RuntimeError(result.stderr or result.stdout)
    print(result.stdout, end="", flush=True)
    visited = set()
    for line in result.stdout.splitlines():
        if line.startswith("visited:"):
            visited.update(line.partition(":")[2].split())
    expected = {row["pc"] for row in rows}
    if args.cases >= 73728:
        if visited != expected:
            raise RuntimeError(f"boundary mismatch: missing {sorted(expected-visited)}, extra {sorted(visited-expected)}")
        paths = ["port/command_queue.c", "port/command_queue.h", "port/command_queue_contract_test.c",
                 "port/field_bytes.h", "port/field_bytes_contract_test.c",
                 "port/command_input.h", "port/indexed_controls.h", "port/flight_command_input.h",
                 "port/view_command_input.h", "port/context_command_input.h",
                 "tools/recomp/native_command_queue_oracle.c", "tools/recomp/check_native_command_queue.py",
                 "tools/recomp/command_publication_oracle.c", str(scope.relative_to(ROOT))]
        checkpoint = {
            "status": "validated_native_publication_full_runtime_integration_pending",
            "cases": args.cases, "source_boundaries": len(expected), "covered_boundaries": len(visited),
            "raw_destinations": 138, "translated_destinations": 256,
            "native_cpu_dependency": False, "original_state_sha256": source["state_sha256"],
            "original_pc_bytes_sha256": hashlib.sha256(b"".join(
                int(row["pc"], 16).to_bytes(4, "big") + bytes.fromhex(row["bytes"]) for row in rows)).hexdigest(),
            "comparison": "all Chip/Slow RAM without exclusions, full event, canonical native owners; original instructions without child contracts",
            "report": result.stdout.splitlines()[0],
            "source_sha256": {path: hashlib.sha256((ROOT/path).read_bytes()).hexdigest() for path in paths},
        }
        (ROOT / "analysis/figures/native_command_queue_checkpoint.json").write_text(json.dumps(checkpoint, indent=2) + "\n")


if __name__ == "__main__":
    main()
