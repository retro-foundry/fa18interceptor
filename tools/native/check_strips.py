"""Exercise reached paired model strips and compare their original geometry.

The short pullback scenario ends before the currently failing terrain pass.
It does not establish takeoff or full replay parity. Copper fade is excluded.
"""
import argparse
import json
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    args = parser.parse_args()
    oracles = {}
    for name in ('strips', 'model', 'records'):
        oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
        subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                        '--main', f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
        oracles[name] = oracle
    with tempfile.TemporaryDirectory(prefix='native-strips-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        replay = work / 'pull.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n'
                          'F 3000 K 50 0 0 1\nF 3002 K 50 0 0 0\n'
                          'F 4100 K 13 0 0 1\nF 4102 K 13 0 0 0\n'
                          'F 5000 K 50 0 0 1\nF 5002 K 50 0 0 0\n'
                          'F 5400 K 49 0 0 1\nF 5402 K 49 0 0 0\n'
                          'F 6200 K 61 0 0 1\nF 7000 K 274 0 0 1\n'
                          'F 7150 K 274 0 0 0\n')
        checkpoint = work / '7150.dat'
        result = subprocess.run([str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
                                 '--save-dir', directory, '--headless', '--frames', '7150',
                                 '--replay', str(replay), '--data-out', str(checkpoint)],
                                cwd=ROOT, check=True, capture_output=True, text=True, timeout=25)
        stats = json.loads(result.stdout)
        assert stats['stage'] == 'C10DAE' and stats['model_calls'] > 0, stats
        assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
        subprocess.run([str(oracles['strips']), str(checkpoint)], cwd=ROOT, check=True, timeout=15)
        result = subprocess.run([str(oracles['model']), str(checkpoint)], cwd=ROOT,
                                check=True, capture_output=True, text=True, timeout=20)
        print(result.stdout, end='')
        match = re.search(r'(\d+) positive model strip groups', result.stdout)
        assert match and int(match[1]) > 0, 'reached positive strips were not compared'
        subprocess.run([str(oracles['records']), str(checkpoint), '7150'], cwd=ROOT, check=True, timeout=15)
    print('Native pullback renders paired strips and preserves source view/record updates')


if __name__ == '__main__':
    main()
