"""Check event replay ownership, key retention, fixed storage and default playback."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def digest(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, required=True)
    parser.add_argument('--baseline', type=Path, help='explicit previous executable for default playback equality')
    parser.add_argument('--out', type=Path)
    args = parser.parse_args()
    results = {}
    with tempfile.TemporaryDirectory(prefix='native-event-replay-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        source = ROOT / 'captures/native/qual_fail_crashes/input.fa18in'
        keys = [line for line in source.read_text().splitlines()[1:]
                if line and not line.startswith('end') and int(line.split()[0]) <= 1960]
        inputs = work / 'input.fa18in'
        inputs.write_text('FA18_LOOP_INPUT_V1\n' + '\n'.join(keys) + '\nend 1960 0\n')
        warmup = work / 'intro.e9k'
        warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        header = 'FA18_REPLAY_ANCHORS_V1\n'
        cold = header + '1 0 C0FCB4 0\n'

        def run(name, anchors=None, runner=None, success=True, frames=10000, outputs=False):
            out = work / name
            out.mkdir()
            command = [str((runner or args.runner).resolve()), '--headless', '--frames', str(frames),
                '--input', str(inputs), '--replay', str(warmup), '--save-dir', str(out / 'pilot'),
                '--memory-report', str(out / 'memory.json')]
            if outputs:
                command += ['--data-out', str(out / 'game.dat'), '--ppm', str(out / 'game.ppm'), '--wav', str(out / 'game.wav')]
            if anchors is not None:
                (out / 'input.anchors').write_text(anchors)
                command += ['--input-anchors', str(out / 'input.anchors'), '--input-anchors-out', str(out / 'anchors.json')]
            result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True, timeout=30)
            assert (result.returncode == 0) == success, (name, result.stdout, result.stderr)
            if (out / 'memory.json').exists():
                memory = json.loads((out / 'memory.json').read_text())
                assert memory['project_gameplay_heap_violations'] == 0 and memory['sdl_failures'] == 0
            return out, result

        legacy, old_result = run('legacy', outputs=True)
        anchored, new_result = run('cold-anchor', cold, outputs=True)
        assert json.loads(old_result.stdout) == json.loads(new_result.stdout)
        fingerprints = {}
        for name in ('game.dat', 'game.ppm', 'game.wav'):
            expected = (legacy / name).read_bytes()
            assert expected == (anchored / name).read_bytes(), f'cold anchor changed {name}'
            fingerprints[name] = digest(expected)
        event = json.loads((anchored / 'anchors.json').read_text())
        assert event['complete'] and not event['failed'] and event['native_updates'] == event['source_position'] == 1960
        assert event['keys_consumed'] == len(keys) and event['anchors'][0]['native_first'] == 1
        results['cold_anchor_default_exact'] = True
        if args.baseline:
            previous, previous_result = run('previous', runner=args.baseline, outputs=True)
            assert json.loads(previous_result.stdout) == json.loads(old_result.stdout)
            for name in fingerprints:
                assert (previous / name).read_bytes() == (legacy / name).read_bytes(), f'default changed {name}'
            results['previous_default_exact'] = True

        # The early next event would skip the ordinary menu selection's
        # pending key release. Reject it instead of dropping that edge.
        out, result = run('skip-key', cold + '900 9 C0FECE 0\n', success=False)
        event = json.loads((out / 'anchors.json').read_text())
        assert event['failed'] and not event['complete'] and event['keys_consumed'] == 1
        assert 'skipped a key' in result.stderr
        results['skipped_key_rejected'] = True
        out, result = run('missing-event', cold + '1800 1 C10D8A 1\n', success=False, frames=5000)
        event = json.loads((out / 'anchors.json').read_text())
        assert not event['complete'] and not event['failed'] and event['keys_consumed'] == 6
        assert '--frames limit reached' in result.stderr
        results['unreached_event_rejected'] = True

        invalid = ['BAD\n', header, header + '2 0 C0FCB4 0\n', header + '1 1 C0FCB4 0\n',
            cold + '2 256 C10C08 36\n', cold + '2 4 C10C08 65536\n', cold + '2 4 Q10C08 36\n',
            cold + '0 4 C10C08 36\n', cold + '1961 4 C10C08 36\n', cold + '1 4 C10C08 36\n',
            cold + '2 4 C10C08 36 extra\n', cold + ''.join(f'{i} 4 C10C08 36\n' for i in range(2, 34))]
        for index, text in enumerate(invalid):
            _, result = run(f'invalid-{index}', text, success=False)
            assert 'Native anchors' in result.stderr and 'row' in result.stderr
        results['malformed_anchors_rejected'] = len(invalid)
    report = dict(results=results, default_fingerprints=fingerprints,
        runner_sha256=digest(args.runner.read_bytes()),
        baseline_sha256=digest(args.baseline.read_bytes()) if args.baseline else None)
    if args.out:
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(results))


if __name__ == '__main__':
    main()
