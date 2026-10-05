"""Compare direct static translations with opcode-table reference execution.

Every sealed recording is replayed completely, comparing diagnostics (including
CPU state/timing), all final RAM and every RGB444 frame. One frame stream is
kept at a time. --reference accepts a pre-change executable; otherwise build
the same source with only the static opcode binding disabled.
"""
from __future__ import annotations

import argparse
import ctypes
import hashlib
import json
import subprocess
import tempfile
from pathlib import Path

from recomp import handler_names
from static_recomp import GEN, ROOT, STATIC_FILE, partition


def sha256(path: Path) -> str:
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def check_bindings() -> dict:
    partition(check=True)
    inventory = json.loads((GEN / "recomp_deferred.json").read_text())
    # Verify the Python ordered-mask implementation against the actual
    # interpreter's initialized table, independently of generated C.
    lib = ctypes.CDLL(str(ROOT / "build/recomp/dasm_helper.dll"))
    lib.fa18_dasm_init()
    names = handler_names()
    for opcode, name in inventory["opcodes"].items():
        index = lib.fa18_handler_index(int(opcode, 16))
        if index < 0 or names[index] != name:
            raise ValueError(f"{opcode}: static helper {name} differs from original table")
    return inventory


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runner", type=Path, default=ROOT / "build/recomp/fa18_recomp.exe")
    parser.add_argument("--reference", type=Path)
    parser.add_argument("--bindings-only", action="store_true")
    args = parser.parse_args()
    inventory = check_bindings()
    print(f"{len(inventory['opcodes'])} native opcode bindings match the original interpreter table", flush=True)
    if args.bindings_only:
        return
    if args.reference:
        reference = args.reference.resolve()
    else:
        source = ROOT / "build/recomp/static_reference.c"
        source.write_text('#define FA18_STATIC_RECOMP_REFERENCE\n'
                          '#include "../../port/recomp/generated/recomp_static_deferred.c"\n')
        reference = ROOT / "build/recomp/fa18_static_reference.exe"
        subprocess.run(["python", "scripts/build_recomp.py", "--output", str(reference.relative_to(ROOT)),
                        "--replace-source", "port/recomp/generated/" + STATIC_FILE + "=" +
                        str(source.relative_to(ROOT))], cwd=ROOT, check=True)
    runner = args.runner.resolve()
    recordings = sorted(path for path in (ROOT / "captures/native").iterdir() if (path / "run.json").is_file())
    if not recordings:
        raise ValueError("no sealed native recordings")
    results = []
    for recording in recordings:
        runs = []
        for name, exe in (("reference", reference), ("static", runner)):
            with tempfile.TemporaryDirectory(prefix="static-recomp-", dir=ROOT / "build/recomp") as directory:
                work = Path(directory)
                ram, rgb = work / "ram.bin", work / "frames.bin"
                command = [str(exe), "--state", str(recording / "state.bin"),
                           "--input", str(recording / "input.fa18in"), "--to-end",
                           "--rom", str(ROOT / "local/system/kick13.rom"), "--ports", "off",
                           "--ram-out", str(ram), "--rgb444", str(rgb)]
                print(f"{recording.name}: {name} complete replay", flush=True)
                completed = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, check=True)
                diagnostics = [json.loads(line) for line in completed.stdout.splitlines() if line.strip()]
                runs.append({"diagnostics": diagnostics, "ram_sha256": sha256(ram),
                             "rgb444_sha256": sha256(rgb), "rgb444_bytes": rgb.stat().st_size})
        if runs[0] != runs[1]:
            raise ValueError(f"{recording.name}: direct static execution differs: {runs}")
        seal = json.loads((recording / "run.json").read_text())
        if runs[1]["ram_sha256"] != seal["replay"]["final_ram_sha256"]:
            raise ValueError(f"{recording.name}: final RAM differs from recording seal")
        results.append({"recording": recording.name, **runs[1]})
        print(f"{recording.name}: CPU/timing, sealed RAM and every RGB444 frame match", flush=True)
    report = ROOT / "build/recomp/static_recomp_gate.json"
    report.write_text(json.dumps({"opcode_bindings": len(inventory["opcodes"]),
                                  "static_entries": inventory["static_recompiled_entries"],
                                  "recordings": results}, indent=2) + "\n")


if __name__ == "__main__":
    main()
