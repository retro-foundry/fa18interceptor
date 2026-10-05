"""Validate both ordinary-state input selectors against original instruction bytes."""
import argparse
import hashlib
import json
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=16384)
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("cases must be positive")
    scope = ROOT / "analysis/data/command_dispatch_source_scope.json"
    source = json.loads(scope.read_text())
    rows = [row for row in source["instructions"]
            if 0xc1ac28 <= int(row["pc"], 16) < 0xc1ad70 or
            0xc1ad74 <= int(row["pc"], 16) < 0xc1b126]
    header = "/* Original selection bytes, validation inputs only. */\n"
    header += "static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } source_bytes[]={\n"
    for row in rows:
        values = ",".join(f"0x{byte:02x}" for byte in bytes.fromhex(row["bytes"]))
        header += f"{{0x{row['pc']},{row['length']},{{{values}}}}},\n"
    header += "};\n"
    (ROOT / "build/recomp").mkdir(parents=True, exist_ok=True)
    (ROOT / "build/recomp/native_command_input_source.h").write_text(header)
    exe = build_oracle("native_command_input_oracle", "tools/recomp/native_command_input_oracle.c", default_bash())
    reports = []
    for entry, low, high in (("C1AC28", 0xc1ac28, 0xc1ad70), ("C1AD74", 0xc1ad74, 0xc1b126)):
        result = subprocess.run([str(exe), str(args.cases), entry], cwd=ROOT,
                                capture_output=True, text=True, timeout=180)
        (ROOT / f"build/recomp/native_command_input_{entry}.log").write_text(result.stdout + result.stderr)
        if result.returncode:
            raise RuntimeError(result.stderr or result.stdout)
        seen = set(result.stdout.split("visited:")[1].split())
        expected = {row["pc"] for row in rows if low <= int(row["pc"], 16) < high}
        if args.cases >= 8192 and seen != expected:
            raise RuntimeError(f"{entry}: boundary mismatch; missing {sorted(expected - seen)}, extra {sorted(seen - expected)}")
        reports.append({"entry": entry, "cases": args.cases, "boundaries": len(expected),
                        "covered": len(expected & seen), "result": result.stdout.splitlines()[0]})
        print(reports[-1]["result"], flush=True)
        print(f"original selection boundaries: {len(expected & seen)}/{len(expected)}", flush=True)
    if args.cases >= 8192:
        paths = ["port/command_input.c", "port/command_input.h", "port/command_types.h",
                 "port/indexed_controls.c", "port/indexed_controls.h",
                 "tools/recomp/native_command_input_oracle.c", "tools/recomp/command_selection_oracle.c",
                 str(scope.relative_to(ROOT))]
        evidence = {
            "status": "validated_native_selection_indexed_composition_runtime_integration_pending",
            "reports": reports, "native_cpu_dependency": False,
            "scope": "complete keyboard/pending selection prefixes; indexed composition; other actions/publication pending",
            "source_sha256": {path: hashlib.sha256((ROOT / path).read_bytes()).hexdigest() for path in paths},
        }
        (ROOT / "analysis/figures/native_command_input_checkpoint.json").write_text(json.dumps(evidence, indent=2) + "\n")


if __name__ == "__main__":
    main()
