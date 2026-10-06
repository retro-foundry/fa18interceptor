"""Validate reached native postflight entry separately from display/reset timing."""
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
    oracle = ROOT / 'build/recomp/native_postflight_entry_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_postflight_entry_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-postflight-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        replay = work / 'pull.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n'
                          'F 3000 K 50 0 0 1\nF 3002 K 50 0 0 0\n'
                          'F 4100 K 13 0 0 1\nF 4102 K 13 0 0 0\n'
                          'F 5000 K 50 0 0 1\nF 5002 K 50 0 0 0\n'
                          'F 5400 K 49 0 0 1\nF 5402 K 49 0 0 0\n'
                          'F 6200 K 61 0 0 1\nF 7000 K 274 0 0 1\n'
                          'F 7150 K 274 0 0 0\n')
        checkpoint = work / '8200.dat'
        result = subprocess.run([str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
                                 '--save-dir', directory, '--headless', '--frames', '8200',
                                 '--replay', str(replay), '--data-out', str(checkpoint)],
                                cwd=ROOT, check=True, capture_output=True, text=True, timeout=25)
        stats = json.loads(result.stdout)
        assert stats['stage'] == 'C11788' and stats['record_updates'] > 1000, stats
        assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
        # The reached source callback waits here; the outer display owner must
        # consume this counter before reset completion can be demonstrated.
        data = checkpoint.read_bytes()
        assert data[0xC45899 - 0xC00000 + 0x80000] == 0x28
        subprocess.run([str(oracle), str(checkpoint)], cwd=ROOT, check=True, timeout=15)
    print('Native postflight entry executes; outer-display activity wait remains open')


if __name__ == '__main__':
    main()
