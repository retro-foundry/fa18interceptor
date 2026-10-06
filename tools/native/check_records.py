"""Compare the native bootstrap record checkpoint with original C1C63E.

This bounded component check uses CPU/ROM only in the validation oracle.
It does not certify active flight or native frame timing.
"""
import argparse
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    args = parser.parse_args()
    oracle = ROOT / 'build/recomp/native_records_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_records_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-records-', dir=ROOT / 'build') as directory:
        checkpoint = Path(directory) / 'bootstrap.bin'
        subprocess.run([str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
                        '--save-dir', directory, '--headless', '--frames', '1',
                        '--data-out', str(checkpoint)], cwd=ROOT, check=True, timeout=15)
        subprocess.run([str(oracle), str(checkpoint)], cwd=ROOT, check=True, timeout=15)

if __name__ == '__main__':
    main()
