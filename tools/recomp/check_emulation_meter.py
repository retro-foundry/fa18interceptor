"""Prove profiling leaves real runner state/output unchanged in all CPU modes."""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runner", type=Path, default=ROOT / "build/recomp/fa18_recomp.exe")
    args = parser.parse_args()
    base = [str(args.runner.resolve()), "--state", str(ROOT / "captures/native/demo01/state.bin"),
            "--rom", str(ROOT / "local/system/kick13.rom"), "--frames", "120"]
    with tempfile.TemporaryDirectory(prefix="meter-check-", dir=ROOT / "build") as temporary:
        out = Path(temporary)
        for name, options in (("off", ["--ports", "off"]), ("on", ["--ports", "on"]),
                              ("interpreter", ["--ports", "off", "--no-recomp"])):
            results = []
            for enabled in (False, True):
                command = base + options + ["--ram-out", str(out / f"{enabled}.ram"),
                                             "--rgb444", str(out / f"{enabled}.rgb")]
                if enabled:
                    command += ["--profile", str(out / "profile.json")]
                results.append(subprocess.run(command, cwd=ROOT, capture_output=True, check=True))
            assert results[0].stdout == results[1].stdout, name
            assert results[0].stderr == results[1].stderr, name
            for suffix in ("ram", "rgb"):
                assert (out / f"False.{suffix}").read_bytes() == (out / f"True.{suffix}").read_bytes(), (name, suffix)
            meter = json.loads((out / "profile.json").read_text())["_emulation"]
            assert meter["frames"] == 120, meter["frames"]
            assert meter["instructions"]["interpreted"] > 0, name
            assert meter["bus"]["interpreted"]["reads"] > 0, name
            assert meter["chipset"]["bitplane_words"] > 0, name
            assert meter["ram_pages"], name
            if name == "interpreter":
                assert meter["instructions"]["generated"] == meter["instructions"]["residual"] == 0
                assert meter["bus"]["generated"]["reads"] == meter["bus"]["residual"]["reads"] == 0
            else:
                assert meter["instructions"]["generated"] > 0, name
            if name == "on":
                assert meter["ports"]["calls"] + meter["ports"]["steps"] > 0
                assert meter["bus"]["port"]["reads"] > 0
            if name == "off":
                assert meter["ports"]["calls"] == meter["ports"]["steps"] == 0
            print(f"{name}: profiling preserves stdout, stderr, all RGB frames and final RAM; origins exercised")


if __name__ == "__main__":
    main()
