"""Exercise native qualification briefing -> carrier startup -> short takeoff.

Source owner and record/render checkpoints are bounded original-code comparisons,
not acceptance of the complete sealed carrier-success recording. Copper fade is
excluded. No reference capture supplies native runtime state.
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
    for name in ('qualification', 'records', 'model'):
        oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
        subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                        '--main', f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
        oracles[name] = oracle
    with tempfile.TemporaryDirectory(prefix='native-qualification-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        replay = work / 'keys.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n'
                          'F 3000 K 53 0 0 1\nF 3002 K 53 0 0 0\n'
                          'F 4100 K 13 0 0 1\nF 4102 K 13 0 0 0\n'
                          'F 5000 K 32 0 0 1\nF 5002 K 32 0 0 0\n'
                          'F 6000 K 291 0 0 1\nF 6002 K 291 0 0 0\n'
                          'F 6800 K 274 0 0 1\nF 6850 K 274 0 0 0\n')
        previous = None
        for frames, stage in ((3300, 'C1072E'), (4800, 'C0FBB6'), (5800, 'C10DAE'), (7000, 'C10DAE')):
            checkpoint = work / f'{frames}.dat'
            result = subprocess.run([str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
                                     '--save-dir', str(work / 'pilot'), '--headless', '--frames', str(frames),
                                     '--replay', str(replay), '--data-out', str(checkpoint)],
                                    cwd=ROOT, check=True, capture_output=True, text=True, timeout=25)
            stats = json.loads(result.stdout)
            assert stats['mode'] == 9 and stats['screen'] == 'scene-setup' and stats['stage'] == stage, stats
            assert stats['scene_selected'] and stats['record_updates'] > 1 and stats['model_calls'] > 0, stats
            assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
            data = checkpoint.read_bytes()
            def field(address, count):
                start = address - 0xC00000 + 0x80000
                return int.from_bytes(data[start:start + count], 'big')
            assert field(0xC45848, 1) == 3 and field(0xC461E6, 1) == 0x11, 'carrier pose/aircraft'
            if frames == 5800:
                assert field(0xC46186, 2) & 0x80 and field(0xC461F0, 2) == 0, 'stationary carrier start'
                previous = stats, field(0xC4619C, 4)
            if frames == 7000:
                assert not field(0xC46186, 2) & 0x80, 'takeoff did not clear grounded flag'
                assert field(0xC4619C, 4) > previous[1] and field(0xC461F0, 2) > 0, 'no climb/speed'
                assert field(0xC461AF, 1) > 9 and stats['record_updates'] > previous[0]['record_updates'], stats
            if frames != 4800:
                for oracle in oracles.values():
                    subprocess.run([str(oracle), str(checkpoint)], cwd=ROOT, check=True, timeout=15)
            print(f'{frames}: native qualification {stage}; {stats["record_updates"]} record updates')
    print('Native qualification briefing, carrier startup and short takeoff pass bounded source comparisons')


if __name__ == '__main__':
    main()
