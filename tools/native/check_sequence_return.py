"""Finish native crash input, return to the menu and restart qualification."""
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
    oracle = ROOT / 'build/recomp/native_sequence_return_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_sequence_return_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-sequence-return-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        warmup = work / 'intro.e9k'
        warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        source = ROOT / 'captures/native/qual_fail_crashes/input.fa18in'

        def run(path, iterations, name):
            data = work / f'{name}.dat'
            result = subprocess.run([str(runner), '--adf', str(ROOT / 'local/media/fa18.adf'),
                                     '--save-dir', str(work / 'pilot'), '--headless', '--frames', '10000',
                                     '--input', str(path), '--iterations', str(iterations),
                                     '--replay', str(warmup), '--data-out', str(data)],
                                    cwd=ROOT, check=True, capture_output=True, text=True, timeout=25)
            return json.loads(result.stdout), data

        stats, data = run(source, 3082, 'end')
        assert stats['replay_iterations'] == 3082 and stats['replay_events'] == 168, stats
        assert stats['screen'] == 'menu' and stats['stage'] == 'C0FCB4' and stats['mode'] == 0, stats
        assert stats['postflight_resets'] == 3 and stats['postflight_callbacks'] == 3, stats
        assert not stats['timer_pending'] and not stats['display_pending'] and not stats['input_queued'], stats
        assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
        assert data.read_bytes()[0xC4584B - 0xC00000 + 0x80000] == 0, 'recorder mode survived sequence reset'
        subprocess.run([str(oracle), str(data)], cwd=ROOT, check=True, timeout=15)

        # The menu is connected to subsequent play, not just a terminal image.
        restart = work / 'restart.fa18in'
        prefix = source.read_text().split('end ')[0]
        restart.write_text(prefix + '3084 0 K 5 1\n3085 0 K 5 0\nend 3500 0\n')
        restarted, data = run(restart, 3500, 'restart')
        assert restarted['mode'] == 9 and restarted['screen'] == 'scene-setup', restarted
        assert restarted['stage'] == 'C1072E' and restarted['scene_selected'], restarted
        assert restarted['record_updates'] > stats['record_updates'], restarted
        assert restarted['replay_events'] == 170 and restarted['postflight_resets'] == 3, restarted
        subprocess.run([str(oracle), str(data)], cwd=ROOT, check=True, timeout=15)
        # With the original sound flags initialized, re-entry reaches the
        # source code prompt. Acknowledge it and then the carrier briefing.
        restart.write_text(prefix + '3084 0 K 5 1\n3085 0 K 5 0\n'
                           '3502 0 K 68 1\n3503 0 K 68 0\n'
                           '3800 0 K 64 1\n3801 0 K 64 0\nend 4200 0\n')
        playable, data = run(restart, 4200, 'playable')
        assert playable['stage'] == 'C10DAE' and playable['mode'] == 9, playable
        assert playable['record_updates'] > restarted['record_updates'], playable
        assert playable['replay_events'] == 174 and playable['postflight_resets'] == 3, playable
    print('Native crash recording completes all input, returns to the menu and restarts qualification; reset oracle passes')


if __name__ == '__main__':
    main()
