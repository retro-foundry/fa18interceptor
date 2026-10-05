"""Prove ordinary indexed-control state against original instructions.

The oracle uses the sealed demo state and Kickstart as validation inputs only.
The separate native contract target links just the ordinary C implementation.
$C3318E is a controlled child contract, not part of this component's proof.
"""
import argparse
import hashlib
import json
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=65536)
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("cases must be positive")
    scope = ROOT / "analysis/data/command_dispatch_source_scope.json"
    rows = [row for row in json.loads(scope.read_text())["instructions"]
            if 0xc1bc50 <= int(row["pc"], 16) <= 0xc1bee4 or
            0xc1c214 <= int(row["pc"], 16) <= 0xc1c222]
    header = "/* Original instruction bytes, validation inputs only. */\n"
    header += "static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } source_bytes[]={\n"
    for row in rows:
        values = ",".join(f"0x{byte:02x}" for byte in bytes.fromhex(row["bytes"]))
        header += f"{{0x{row['pc']},{row['length']},{{{values}}}}},\n"
    header += "};\n"
    (ROOT / "build/recomp").mkdir(parents=True, exist_ok=True)
    (ROOT / "build/recomp/native_indexed_controls_source.h").write_text(header)
    executable = build_oracle("native_indexed_controls_oracle",
                              "tools/recomp/native_indexed_controls_oracle.c", default_bash())
    result = subprocess.run([str(executable), str(args.cases)], cwd=ROOT,
                            capture_output=True, text=True, timeout=180)
    (ROOT / "build/recomp/native_indexed_controls_oracle.log").write_text(result.stdout + result.stderr)
    if result.returncode:
        raise RuntimeError(result.stderr or result.stdout)
    expected = {row["pc"] for row in rows}
    seen = set(result.stdout.split("visited:")[1].split())
    if args.cases >= 65536 and expected - seen:
        raise RuntimeError(f"uncovered indexed controls: {sorted(expected - seen)}")
    print(result.stdout.splitlines()[0])
    print(f"original indexed-control boundaries: {len(expected & seen)}/{len(expected)}")
    if args.cases >= 65536:
        paths = ["port/indexed_controls.c", "port/indexed_controls.h",
                 "tools/recomp/native_indexed_controls_oracle.c", str(scope.relative_to(ROOT))]
        evidence = {
            "status": "validated_native_component_runtime_integration_pending",
            "cases": args.cases, "source_boundaries": len(expected),
            "covered_boundaries": len(expected & seen),
            "comparison": "full Chip/Slow RAM, published event, ordered child inputs/effects",
            "native_cpu_dependency": False,
            "child_contract": "$C3318E status tone; controlled validation effects, child not ported here",
            "source_sha256": {path: hashlib.sha256((ROOT / path).read_bytes()).hexdigest()
                              for path in paths},
            "result": result.stdout.splitlines()[0],
        }
        (ROOT / "analysis/figures/native_indexed_controls_checkpoint.json").write_text(
            json.dumps(evidence, indent=2) + "\n")


if __name__ == "__main__":
    main()
