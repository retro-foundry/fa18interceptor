"""Check native timing instrumentation without changing game state or pixels.

Performance thresholds are host measurements, not assertions in this contract.
The SDL dummy window exercises conversion/presentation without desktop focus.
"""
import argparse
import csv
import hashlib
import json
import math
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
PHASES = ('input_us', 'game_us', 'audio_us', 'convert_us', 'present_us', 'wait_us',
          'work_us', 'total_us', 'start_interval_us')


def validate(path, stats, presented):
    with path.open(newline='') as file:
        rows = list(csv.DictReader(file))
    assert [int(row['frame']) for row in rows] == list(range(1, stats['frames'] + 1))
    assert all(int(row['presented']) == presented for row in rows)
    assert sum(int(row['scene_updated']) for row in rows) == stats['scene_frames']
    for row in rows:
        values = {name: float(row[name]) for name in PHASES}
        assert all(math.isfinite(value) and value >= 0 for value in values.values()), row
        assert abs(sum(values[name] for name in PHASES[:5]) - values['work_us']) < 0.01, row
        assert abs(values['work_us'] + values['wait_us'] - values['total_us']) < 0.01, row
    return rows


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    runner = parser.parse_args().runner.resolve()
    with tempfile.TemporaryDirectory(prefix='native-frame-times-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        replay = work / 'flight.e9k'
        replay.write_text('E9K_INPUT_V1\n' + ''.join(
            f'F {frame} K {key} 0 0 1\nF {frame + 2} K {key} 0 0 0\n'
            for frame, key in ((1800, 32), (3000, 50), (4100, 13), (5000, 50), (5400, 49))))
        environment = dict(os.environ, SDL_VIDEODRIVER='dummy', SDL_AUDIODRIVER='dummy')

        def run(name, measured=False, window=False):
            command = [str(runner), '--adf', str(ROOT / 'local/media/fa18.adf'),
                       '--save-dir', str(work / name), '--frames', '8' if window else '6500',
                       '--data-out', str(work / f'{name}.dat'), '--ppm', str(work / f'{name}.ppm')]
            command += ['--hidden'] if window else ['--headless', '--replay', str(replay)]
            if measured:
                command += ['--frame-times', str(work / f'{name}.csv')]
            result = subprocess.run(command, cwd=ROOT, env=environment, capture_output=True,
                                    text=True, check=True, timeout=25)
            return json.loads(result.stdout)

        baseline = run('baseline')
        measured = run('measured', measured=True)
        assert baseline == measured and measured['hud_frames'] > 0, (baseline, measured)
        for suffix in ('dat', 'ppm'):
            digest = lambda name: hashlib.sha256((work / f'{name}.{suffix}').read_bytes()).digest()
            assert digest('baseline') == digest('measured'), f'Timing changed native {suffix}'
        rows = validate(work / 'measured.csv', measured, 0)
        assert {row['renderer'] for row in rows} == {'headless'}
        assert all(float(row['convert_us']) == float(row['present_us']) == 0 for row in rows)
        window = run('window', measured=True, window=True)
        rows = validate(work / 'window.csv', window, 1)
        assert all(row['renderer'] != 'headless' for row in rows)

        bad = subprocess.run([str(runner), '--headless', '--frames', '1',
            '--adf', str(ROOT / 'local/media/fa18.adf'), '--save-dir', str(work / 'bad'),
            '--frame-times', str(work / 'missing' / 'times.csv')], cwd=ROOT,
            capture_output=True, text=True, timeout=10)
        assert bad.returncode and 'Cannot create native frame timing report' in bad.stderr, bad.stderr
    print('6500 timed flight rows preserve complete native RAM, pixels and stats; all 8 SDL rows present; timing sums and write failure pass')


if __name__ == '__main__':
    main()
