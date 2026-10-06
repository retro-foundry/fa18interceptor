"""Check recorded raw keys at native update boundaries, without full replays."""
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
    with tempfile.TemporaryDirectory(prefix='native-replay-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        warmup = work / 'intro.e9k'
        warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')

        def run(path, iterations, name, frames=10000, success=True):
            data, image = work / f'{name}.dat', work / f'{name}.ppm'
            command = [str(runner), '--adf', str(ROOT / 'local/media/fa18.adf'),
                       '--save-dir', str(work / 'pilot'), '--headless', '--frames', str(frames),
                       '--input', str(path), '--replay', str(warmup), '--data-out', str(data), '--ppm', str(image)]
            if iterations:
                command += ['--iterations', str(iterations)]
            result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, timeout=25)
            assert (result.returncode == 0) == success, (command, result.stdout, result.stderr)
            return result, data, image

        source = ROOT / 'captures/native/qual_fail_crashes/input.fa18in'
        result, original_data, original_image = run(source, 1960, 'sealed')
        stats = json.loads(result.stdout)
        assert stats['replay_iterations'] == 1960 and stats['replay_events'] == 18, stats
        assert stats['mode'] == 9 and stats['stage'] == 'C10DAE' and stats['record_updates'] > 90, stats
        assert stats['replay_started'] and stats['timer_yields'] and stats['display_yields'], stats
        assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats

        # Video-frame metadata is not the event's delivery time. Deliberately
        # replace every frame column with zero and keep the original iterations.
        rows = []
        for line in source.read_text().splitlines()[1:]:
            fields = line.split()
            if fields[0] == 'end':
                break
            fields[1] = '0'
            rows.append(' '.join(fields))
        changed = work / 'frame-metadata.fa18in'
        changed.write_text('FA18_LOOP_INPUT_V1\n' + '\n'.join(rows) + '\nend 3082 0\n')
        result, data, image = run(changed, 1960, 'metadata')
        assert json.loads(result.stdout) == stats, 'video metadata changed update delivery'
        assert data.read_bytes() == original_data.read_bytes(), 'video metadata changed game state'
        assert image.read_bytes() == original_image.read_bytes(), 'video metadata changed pixels'

        # Multiple edges in one update must retain recording order. These keys
        # enter C0F3C4 on the next update after the pending timer completes.
        prefix = [line for line in rows if int(line.split()[0]) <= 1960]
        for edges, expected in (((1, 0), 0), ((0, 1), 0x20)):
            ordered = work / 'ordered.fa18in'
            ordered.write_text('FA18_LOOP_INPUT_V1\n' + '\n'.join(prefix) + '\n' +
                               ''.join(f'1961 0 K 77 {down}\n' for down in edges) + 'end 1961 0\n')
            result, data, _ = run(ordered, None, f'order-{expected}')
            order_stats = json.loads(result.stdout)
            assert order_stats['replay_iterations'] == 1961 and order_stats['replay_events'] == 20, order_stats
            assert order_stats['input_events'] == stats['input_events'] + 2, order_stats
            assert data.read_bytes()[0xC4582E - 0xC00000 + 0x80000] == expected, 'raw edge order lost'

        malformed = ('BAD\n', 'FA18_LOOP_INPUT_V1\n1 0 K 5 1\n',
                     'FA18_LOOP_INPUT_V1\n1 0 M 1 0\nend 2 0\n',
                     'FA18_LOOP_INPUT_V1\n-1 0 K 5 1\nend 2 0\n',
                     'FA18_LOOP_INPUT_V1\n4294967296 0 K 5 1\nend 2 0\n',
                     'FA18_LOOP_INPUT_V1\n1 0 K 128 1\nend 2 0\n',
                     'FA18_LOOP_INPUT_V1\n1 0 K 5 2\nend 2 0\n',
                     'FA18_LOOP_INPUT_V1\n2 0 K 5 1\n1 0 K 5 0\nend 2 0\n',
                     'FA18_LOOP_INPUT_V1\n2 0 K 5 1\nend 1 0\n',
                     'FA18_LOOP_INPUT_V1\nend 2 0\n1 0 K 5 1\n',
                     'FA18_LOOP_INPUT_V1\n1 0 K 5 1 extra\nend 2 0\n')
        for index, text in enumerate(malformed):
            invalid = work / 'invalid.fa18in'
            invalid.write_text(text)
            result, _, _ = run(invalid, None, f'invalid-{index}', success=False)
            assert 'Native input' in result.stderr and 'row' in result.stderr, result.stderr
        result, _, _ = run(source, 1960, 'short', frames=100, success=False)
        assert 'before main-menu anchor' in result.stderr, result.stderr
    print('Native sealed qualification prefix, iteration delivery, raw edge order and input errors pass')


if __name__ == '__main__':
    main()
