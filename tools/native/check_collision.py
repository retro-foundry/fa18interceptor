"""Exercise collision effects and postflight resets from a sealed input prefix."""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    runner = parser.parse_args().runner.resolve()
    oracles = []
    for name in ('collision', 'records'):
        oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
        subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                        '--main', f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
        oracles.append(oracle)
    with tempfile.TemporaryDirectory(prefix='native-collision-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        warmup = work / 'intro.e9k'
        warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        for iteration in (2022, 2150):
            data = work / f'{iteration}.dat'
            result = subprocess.run([str(runner), '--adf', str(ROOT / 'local/media/fa18.adf'),
                                     '--save-dir', str(work / 'pilot'), '--headless', '--frames', '10000',
                                     '--input', str(ROOT / 'captures/native/qual_fail_crashes/input.fa18in'),
                                     '--iterations', str(iteration), '--replay', str(warmup), '--data-out', str(data)],
                                    cwd=ROOT, check=True, capture_output=True, text=True, timeout=25)
            stats = json.loads(result.stdout)
            assert stats['stage'] == 'C10DAE' and stats['mode'] == 9, stats
            assert stats['replay_iterations'] == iteration and stats['input_queued'] == 0, stats
            assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
            if iteration == 2022:
                assert stats['postflight_resets'] == 0 and stats['record_updates'] == 160, stats
            else:
                assert stats['postflight_resets'] == 2 and stats['record_updates'] == 288, stats
                assert stats['replay_events'] == 122, stats
                contents = data.read_bytes()
                root = 0xC46184 - 0xC00000 + 0x80000
                assert int.from_bytes(contents[root+2:root+4], 'big') & 0x80, 'reset aircraft is not grounded'
                assert contents[root+0x6C:root+0x6E] == b'\0\0', 'reset speed is not zero'
            for oracle in oracles:
                subprocess.run([str(oracle), str(data)], cwd=ROOT, check=True, timeout=15)
    print('Native crash prefix reaches two source postflight resets; collision and view/record comparisons pass')


if __name__ == '__main__':
    main()
