"""Build the native intro/menu runner without CPU or chipset objects."""
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]

def main():
    build = ROOT / "build/native-cmake"
    subprocess.run([sys.executable, "scripts/prune_build_artifacts.py", "--quiet"],
                   cwd=ROOT, check=True)
    subprocess.run(["cmake", "-S", "port/recomp", "-B", str(build),
                    "-DFA18_NATIVE_ONLY=ON", "-DBUILD_TESTING=OFF"], cwd=ROOT, check=True)
    subprocess.run(["cmake", "--build", str(build), "--config", "Release",
                    "--target", "fa18_native", "--parallel", "8"], cwd=ROOT, check=True)
    name = "fa18_native.exe" if sys.platform == "win32" else "fa18_native"
    candidates = [build / "native/Release" / name, build / "native" / name]
    source = next((path for path in candidates if path.is_file()), None)
    if source is None:
        raise SystemExit("Native build did not produce the executable")
    output = ROOT / "build/native" / name
    output.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, output)
    print(output)

if __name__ == "__main__":
    main()
