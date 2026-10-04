"""Run original startup-error cleanup with only an ADF available to the fixture."""
import argparse
import hashlib
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--fixture", type=Path, default=ROOT / "build/recomp-cmake/Release/fa18_romfree_startup_failure_test.exe")
    args = parser.parse_args()
    adf = ROOT / "local/media/fa18.adf"
    original_hash = hashlib.sha256(adf.read_bytes()).digest()
    with tempfile.TemporaryDirectory(prefix="romfree-startup-error-") as directory:
        work = Path(directory)
        shutil.copy2(args.fixture, work / "test.exe")
        shutil.copy2(adf, work / "original.adf")
        for stage in ("dos", "intuition"):
            result = subprocess.run([str(work / "test.exe"), "original.adf", "saves", stage],
                                    cwd=work, capture_output=True, text=True, timeout=30)
            assert result.returncode == 0, (result.stdout, result.stderr)
            code = 100 if stage == "dos" else 0
            assert f"restored registers/stack, exit {code}, zero ROM/fault counters" in result.stdout
            if stage == "dos":
                assert "recoverable alert=00030007" in result.stderr
            else:
                assert "Workbench message replied" in result.stdout
            assert hashlib.sha256((work / "original.adf").read_bytes()).digest() == original_hash
            assert not list((work / "saves").rglob("*")), "Startup error wrote a save file"
            print(result.stdout.strip())
    assert hashlib.sha256(adf.read_bytes()).digest() == original_hash


if __name__ == "__main__":
    main()
