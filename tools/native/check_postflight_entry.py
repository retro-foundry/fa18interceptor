"""Validate native display waits and postflight reset against source owners."""
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
    oracles = []
    for name in ('display', 'postflight_entry'):
        oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
        subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                        '--main', f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
        oracles.append(oracle)
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
        checkpoint = work / 'data.bin'
        previous = None
        for frames in (7400, 7404, 8200):
            result = subprocess.run([str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
                                     '--save-dir', directory, '--headless', '--frames', str(frames),
                                     '--replay', str(replay), '--data-out', str(checkpoint)],
                                    cwd=ROOT, check=True, capture_output=True, text=True, timeout=25)
            stats = json.loads(result.stdout)
            assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
            data = checkpoint.read_bytes()
            activity = data[0xC45899 - 0xC00000 + 0x80000]
            if frames < 8200:
                assert stats['stage'] == 'C11788' and stats['display_pending'], stats
                assert 0 < activity < 40 and stats['postflight_resets'] == 0, stats
                if previous:
                    before, old_data, old_activity = previous
                    assert activity == old_activity - 1, 'activity must decrement after four PAL waits'
                    for field in ('game_tick', 'record_updates', 'scene_frames', 'hud_frames', 'glyphs',
                                  'control_frames', 'timer_yields', 'display_publications', 'displayed_page'):
                        assert stats[field] == before[field], f'{field} repeated while display was waiting'
                    assert stats['display_yields'] == before['display_yields'] + 4
                    root = 0xC46184 - 0xC00000 + 0x80000
                    assert data[root:root+512] == old_data[root:root+512], 'physics repeated during flash'
                    assert data[0x10000:0x1A000] == old_data[0x10000:0x1A000]
                    assert data[0x40000:0x4A000] == old_data[0x40000:0x4A000]
                previous = stats, data, activity
            else:
                assert stats['stage'] == 'C10DAE' and stats['postflight_resets'] == 1, stats
                assert stats['postflight_callbacks'] == 1 and activity == 0, stats
                assert stats['record_updates'] > previous[0]['record_updates'], 'reset did not resume gameplay'
                for oracle in oracles:
                    subprocess.run([str(oracle), str(checkpoint)], cwd=ROOT, check=True, timeout=15)
    print('Native display consumes four PAL waits per activity decrement; postflight resets and resumes gameplay')


if __name__ == '__main__':
    main()
