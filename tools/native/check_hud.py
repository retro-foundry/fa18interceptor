"""Exercise the connected native HUD/panels and compare their source owners.

This bounded component comparison excludes source stack only. It does not
certify complete frame ordering or recorded gameplay acceptance.
"""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    args = parser.parse_args()
    oracle = ROOT / 'build/recomp/native_hud_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_hud_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-hud-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        replay = work / 'flight.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n'
                          'F 3000 K 50 0 0 1\nF 3002 K 50 0 0 0\n'
                          'F 4100 K 13 0 0 1\nF 4102 K 13 0 0 0\n'
                          'F 5000 K 50 0 0 1\nF 5002 K 50 0 0 0\n'
                          'F 5400 K 49 0 0 1\nF 5402 K 49 0 0 0\n'
                          'F 6200 K 61 0 0 1\nF 6600 K 61 0 0 0\n')
        data = work / 'data.bin'
        for frames, stage in ((5300, 'C10AE6'), (6100, 'C10DAE'), (6500, 'C10DAE')):
            result = subprocess.run([str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
                '--save-dir', directory, '--headless', '--frames', str(frames), '--replay', str(replay),
                '--data-out', str(data)], cwd=ROOT, check=True, capture_output=True, text=True, timeout=20)
            stats = json.loads(result.stdout)
            assert stats['stage'] == stage and stats['hud_frames'] > 0, stats
            assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
            subprocess.run([str(oracle), str(data)], cwd=ROOT, check=True, timeout=20)
            print(f"{frames}: {stats['hud_frames']} connected HUD/panel frames")
    print('Native HUD/panel owners match focused original comparisons; full frame parity remains open')


if __name__ == '__main__':
    main()
