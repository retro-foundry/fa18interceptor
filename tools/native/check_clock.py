"""Check connected source timer yielding and original timer/readout parity."""
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
    oracle = ROOT / 'build/recomp/native_clock_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_clock_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-clock-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        replay = work / 'flight.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n'
                          'F 3000 K 50 0 0 1\nF 3002 K 50 0 0 0\n'
                          'F 4100 K 13 0 0 1\nF 4102 K 13 0 0 0\n'
                          'F 5000 K 50 0 0 1\nF 5002 K 50 0 0 0\n'
                          'F 5400 K 49 0 0 1\nF 5402 K 49 0 0 0\n'
                          'F 6200 K 61 0 0 1\nF 6600 K 61 0 0 0\n')
        checkpoint = work / 'data.bin'
        previous = None
        for frames in (6501, 6502):
            result = subprocess.run([str(args.runner.resolve()), '--adf', str(ROOT / 'local/media/fa18.adf'),
                '--save-dir', directory, '--headless', '--frames', str(frames), '--replay', str(replay),
                '--data-out', str(checkpoint)], cwd=ROOT, check=True, capture_output=True, text=True, timeout=20)
            stats = json.loads(result.stdout)
            assert stats['stage'] == 'C10DAE' and stats['game_tick'] > 0, stats
            assert stats['timer_pending'] and stats['timer_yields'] > 0, stats
            assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
            data = checkpoint.read_bytes()
            if previous:
                before, old_data = previous
                for field in ('game_tick', 'record_updates', 'scene_frames', 'hud_frames', 'glyphs'):
                    assert stats[field] == before[field], f'{field} repeated while source timer was waiting'
                assert stats['timer_yields'] == before['timer_yields'] + 1, 'poll did not resume'
                root = 0xC46184 - 0xC00000 + 0x80000
                assert data[root:root + 512] == old_data[root:root + 512], 'physics repeated while waiting'
            previous = stats, data
        subprocess.run([str(oracle), str(checkpoint)], cwd=ROOT, check=True, timeout=15)
    print('Native timer resumes clock polls without repeating physics, drawing or final text')


if __name__ == '__main__':
    main()
