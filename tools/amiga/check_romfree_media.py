"""Verify checksummed ADF discovery beside the executable and direct disk assets."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
ZERO = dict(rom_reads=0, rom_instruction_fetches=0, unsupported_services=0)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runner", type=Path, default=ROOT / "build/recomp-cmake/Release/fa18_romfree.exe")
    parser.add_argument("--variants", type=Path, default=Path("D:/amiga"))
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix="romfree-media-") as directory:
        work = Path(directory)
        beside = work / "game with spaces"
        caller = work / "different working directory"
        beside.mkdir(); caller.mkdir()
        runner = beside / "game.exe"
        shutil.copy2(args.runner, runner)
        shutil.copy2(ROOT / "local/media/fa18.adf", caller / "cwd.adf")

        def invoke(options):
            return subprocess.run([str(runner)] + options, cwd=caller, capture_output=True, text=True, timeout=30)

        def launch(name, extra):
            result = invoke(["--frames", "1000", "--save-dir", "saves", "--ram-out", f"{name}.ram",
                             "--ppm", f"{name}.ppm"] + extra)
            assert result.returncode == 0, result.stderr
            stats = [json.loads(line) for line in result.stdout.splitlines()]
            assert stats[-1] == ZERO and stats[0]["nonblack_pixels"] > 60000, stats
            return stats

        missing = invoke(["--frames", "1"])
        assert missing.returncode == 1 and "No supported ADF beside" in missing.stderr
        disk = beside / "renamed game.ADF"
        shutil.copy2(caller / "cwd.adf", disk)
        original = disk.read_bytes()
        expected = hashlib.sha256(original).hexdigest()
        info = json.loads(invoke(["--identify-adf", str(disk)]).stdout)
        assert info["supported"] and info["disk_sha256"] == expected and "[cr A-Ha]" in info["version"]
        (beside / "unrelated.adf").write_bytes(b"not a game disk")
        auto = launch("auto", [])
        explicit = launch("explicit", ["--adf", str(disk)])
        assert auto == explicit
        for suffix in ("ram", "ppm"):
            assert (caller / f"auto.{suffix}").read_bytes() == (caller / f"explicit.{suffix}").read_bytes()
        # Deliberate stale/extracted asset copies must not replace ADF pixels.
        poison = caller / "saves/pix/splsh"
        poison.parent.mkdir(parents=True, exist_ok=True)
        poison.write_bytes(b"invalid external graphics")
        poisoned = launch("poisoned", [])
        assert poisoned == auto
        for suffix in ("ram", "ppm"):
            assert (caller / f"auto.{suffix}").read_bytes() == (caller / f"poisoned.{suffix}").read_bytes()
        assert poison.read_bytes() == b"invalid external graphics"
        poison.unlink(); poison.parent.rmdir()
        duplicate = beside / "duplicate.adf"
        duplicate.write_bytes(original)
        assert invoke(["--frames", "1"]).returncode == 0
        # Boot bytes do not change the loaded executable or game resources.
        # A changed whole-disk identity is permitted when the executable matches.
        modified = bytearray(original); modified[100] ^= 1
        duplicate.write_bytes(modified)
        ambiguous = invoke(["--frames", "1"])
        assert ambiguous.returncode == 1 and "Multiple distinct supported ADFs" in ambiguous.stderr
        assert invoke(["--adf", str(disk), "--frames", "1"]).returncode == 0
        unidentified = json.loads(invoke(["--identify-adf", str(duplicate)]).stdout)
        assert unidentified["supported"] and unidentified["version"] == "unlisted disk image"
        disk.unlink()
        assert invoke(["--frames", "1"]).returncode == 0
        duplicate.unlink()
        assert not list((caller / "saves").rglob("*")), "Assets were extracted into the save directory"

        manifest = json.loads((ROOT / "analysis/adf_versions.json").read_text())
        identified = compatible = 0
        for row in manifest["images"]:
            source = args.variants / row["file"]
            if not source.is_file():
                continue
            probe = invoke(["--identify-adf", str(source)])
            assert probe.returncode == 0, probe.stderr
            recognized = json.loads(probe.stdout)
            identified += 1
            assert recognized["disk_sha256"] == hashlib.sha256(source.read_bytes()).hexdigest() == row["disk_sha256"]
            assert recognized["supported"] == row["supported"] and recognized["version"] == row["file"]
            if row["supported"]:
                compatible += 1
                shutil.copy2(source, disk)
                launch("variant", [])
                assert disk.read_bytes() == source.read_bytes()
                disk.unlink()
            elif row["ofs"] and row["executable_sha256"]:
                rejected = invoke(["--adf", str(source), "--frames", "1"])
                assert rejected.returncode == 1 and "does not match" in rejected.stderr
        print(f"{identified} catalogued ADF identities match SHA-256; {compatible} supported local variants launch directly from disk")
        print("Executable-directory discovery, renamed/uppercase files, duplicate/ambiguous/unknown images and explicit override pass")
        print("Window-free launch matches complete RAM/CPU/pixels; no extracted assets, ROM or savestate needed")


if __name__ == "__main__":
    main()
