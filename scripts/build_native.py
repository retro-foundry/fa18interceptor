"""Compatibility entry for the moved standalone native port."""
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
STANDALONE = ROOT.parent / "fa18-interceptor-decomp"

def main():
    script = STANDALONE / "scripts/build_native.py"
    if not script.is_file():
        raise SystemExit("Native port moved to https://github.com/retro-foundry/fa18-interceptor-decomp; build it from that repository")
    subprocess.run([sys.executable, str(script)], cwd=STANDALONE, check=True)

if __name__ == "__main__":
    main()
