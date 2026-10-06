"""Compare reached negative terrain visibility indices with the original.

Ends at the repaired tick; later postflight/setup ownership remains open.
Copper fade is excluded by comparing plane buffers.
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
    oracles = {}
    for name in ('map_limits', 'raster'):
        oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
        subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                        '--main', f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
        oracles[name] = oracle
    with tempfile.TemporaryDirectory(prefix='native-map-limits-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        replay = work / 'pull.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n'
                          'F 3000 K 50 0 0 1\nF 3002 K 50 0 0 0\n'
                          'F 4100 K 13 0 0 1\nF 4102 K 13 0 0 0\n'
                          'F 5000 K 50 0 0 1\nF 5002 K 50 0 0 0\n'
                          'F 5400 K 49 0 0 1\nF 5402 K 49 0 0 0\n'
                          'F 6200 K 61 0 0 1\nF 7000 K 274 0 0 1\n'
                          'F 7150 K 274 0 0 0\n')
        checkpoint = work / '7294.dat'
        result = subprocess.run([str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
                                 '--save-dir', directory, '--headless', '--frames', '7294',
                                 '--replay', str(replay), '--data-out', str(checkpoint)],
                                cwd=ROOT, check=True, capture_output=True, text=True, timeout=25)
        stats = json.loads(result.stdout)
        assert stats['stage'] == 'C10DAE' and stats['terrain_polygons'] > 0, stats
        assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
        data = checkpoint.read_bytes()
        assert int.from_bytes(data[0x3fd8:0x3fdc], 'big', signed=True) < 0, 'negative map metric not exercised'
        for oracle in oracles.values():
            subprocess.run([str(oracle), str(checkpoint)], cwd=ROOT, check=True, timeout=15)
    print('Native negative-metric terrain pass completes and matches original plane buffers')


if __name__ == '__main__':
    main()
