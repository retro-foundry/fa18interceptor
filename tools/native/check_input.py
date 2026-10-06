"""Verify connected source input polling, queued keys and command parity."""
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
    oracle = ROOT / 'build/recomp/native_input_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_input_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-input-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        replay = work / 'keys.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n'
                          'F 3000 K 50 0 0 1\nF 3002 K 50 0 0 0\n'
                          'F 4100 K 13 0 0 1\nF 4102 K 13 0 0 0\n'
                          'F 5000 K 50 0 0 1\nF 5002 K 50 0 0 0\n'
                          'F 5400 K 49 0 0 1\nF 5402 K 49 0 0 0\n'
                          'F 6200 K 61 0 0 1\nF 6501 K 275 0 0 1\n'
                          'F 6511 K 275 0 0 0\nF 6600 K 61 0 0 0\n')
        previous = None
        checkpoint = work / 'data.bin'
        for frames in (6501, 6502, 6510, 6700):
            result = subprocess.run([str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
                                     '--save-dir', directory, '--headless', '--frames', str(frames),
                                     '--replay', str(replay), '--data-out', str(checkpoint)],
                                    cwd=ROOT, check=True, capture_output=True, text=True, timeout=25)
            stats = json.loads(result.stdout)
            assert stats['stage'] == 'C10DAE' and stats['input_passes'] > 0, stats
            assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
            data = checkpoint.read_bytes()
            stick_x = data[0xC45830 - 0xC00000 + 0x80000]
            if frames == 6501:
                assert stats['timer_pending'] and stats['input_queued'] == 0, stats
                assert stick_x == 0
                previous = stats, data
            elif frames == 6502:
                before, old_data = previous
                assert stats['timer_pending'] and stats['input_queued'] == 1, stats
                for field in ('game_tick', 'record_updates', 'hud_frames', 'glyphs', 'input_passes', 'input_events', 'update_iterations'):
                    assert stats[field] == before[field], f'{field} advanced during a pending frame'
                root = 0xC46184 - 0xC00000 + 0x80000
                assert data[root:root+512] == old_data[root:root+512], 'queued press executed before source poll'
                assert stick_x == 0
            elif frames == 6510:
                assert stats['input_queued'] == 0 and stats['input_events'] == previous[0]['input_events'] + 1, stats
                assert stick_x == 4, 'Right must use C1B55C / $04, not the historical child enum name'
            else:
                assert stats['input_queued'] == 0 and stick_x == 0, stats
                subprocess.run([str(oracle), str(checkpoint)], cwd=ROOT, check=True, timeout=15)
    print('Native queued press/release executes at source input boundaries; original input/command comparisons pass')


if __name__ == '__main__':
    main()
