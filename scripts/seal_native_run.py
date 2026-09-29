"""Seal a native recording, or verify a sealed one.

A native recording is a start state plus a FA18_LOOP_INPUT_V1 file made with
`fa18_recomp --window --record`. Sealing copies both into
captures/native/<name>/ and replays the input headless to its recorded end,
storing the hashes of the inputs, of the build that replays it (the
executable and the generated translation) and of the final RAM and
registers. The files are then made read-only.

Replays are exact for a given machine model and translation, with or without
ports in shadow mode (shadow keeps the plain run's timing); they are not
exact across changes to the machine's timing or a regenerated translation.
`--verify` replays a sealed run and says whether it still ends identically.

  python scripts/seal_native_run.py NAME --state START.bin --input REC.fa18in [--note TEXT]
  python scripts/seal_native_run.py NAME --verify
"""
import argparse
import datetime
import hashlib
import json
import os
import shutil
import stat
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / "build/recomp/fa18_recomp.exe"
ROM = ROOT / "local/system/kick13.rom"
NATIVE = ROOT / "captures/native"


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def translation_hash():
    h = hashlib.sha256()
    for path in sorted((ROOT / "port/recomp/generated").glob("*.c")):
        h.update(path.name.encode())
        h.update(path.read_bytes())
    return h.hexdigest()


def replay(run, ports="off"):
    """Replay a sealed run to its end; the final RAM hash and the summary."""
    with tempfile.TemporaryDirectory() as tmp:
        ram = Path(tmp) / "ram.bin"
        result = subprocess.run([str(EXE), "--state", str(run / "state.bin"), "--rom", str(ROM),
                                 "--input", str(run / "input.fa18in"), "--to-end", "--ports", ports,
                                 "--ram-out", str(ram)], capture_output=True, text=True)
        if result.returncode != 0:
            sys.exit(f"replay failed ({result.returncode}): {result.stderr.strip()[-400:]}")
        summary = json.loads(result.stdout.strip().splitlines()[-1])
        return sha(ram), summary


def seal(args):
    run = NATIVE / args.name
    if run.exists():
        sys.exit(f"{run} exists; sealed runs are read-only")
    first = Path(args.input).read_text().splitlines()[0] if Path(args.input).exists() else ""
    if first != "FA18_LOOP_INPUT_V1":
        sys.exit(f"{args.input} is not a FA18_LOOP_INPUT_V1 recording")
    run.mkdir(parents=True)
    shutil.copyfile(args.state, run / "state.bin")
    shutil.copyfile(args.input, run / "input.fa18in")
    ram, summary = replay(run)
    commit = subprocess.run(["git", "rev-parse", "HEAD"], cwd=ROOT, capture_output=True, text=True).stdout.strip()
    record = {
        "name": args.name,
        "sealed": datetime.datetime.now().isoformat(timespec="seconds"),
        "note": args.note or "",
        "format": "FA18_LOOP_INPUT_V1",
        "start_state": {"source": str(Path(args.state).resolve()), "sha256": sha(run / "state.bin")},
        "input_sha256": sha(run / "input.fa18in"),
        "rom_sha256": sha(ROM),
        "build": {"git_commit": commit, "executable_sha256": sha(EXE), "translation_sha256": translation_hash()},
        "replay": {"frames": summary["frames"], "iterations": summary["iterations"], "final_ram_sha256": ram},
    }
    (run / "run.json").write_text(json.dumps(record, indent=2) + "\n")
    for path in run.iterdir():
        os.chmod(path, stat.S_IREAD)
    print(f"sealed {run}: {summary['frames']} frames, {summary['iterations']} iterations")


def verify(args):
    run = NATIVE / args.name
    record = json.loads((run / "run.json").read_text())
    ram, summary = replay(run, args.ports)
    same = ram == record["replay"]["final_ram_sha256"]
    build = record["build"]["translation_sha256"] == translation_hash()
    print(f"{args.name}: {'identical' if same else 'DIFFERS'} after {summary['frames']} frames "
          f"(sealed {record['replay']['frames']}), translation {'unchanged' if build else 'changed'}")
    sys.exit(0 if same else 1)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("name")
    parser.add_argument("--state")
    parser.add_argument("--input")
    parser.add_argument("--note")
    parser.add_argument("--verify", action="store_true")
    parser.add_argument("--ports", default="off", help="ports mode for --verify (off or shadow)")
    args = parser.parse_args()
    if "/" in args.name or "\\" in args.name:
        sys.exit("name must be one directory component")
    if args.verify:
        verify(args)
    elif args.state and args.input:
        seal(args)
    else:
        parser.error("seal needs --state and --input; or pass --verify")


if __name__ == "__main__":
    main()
