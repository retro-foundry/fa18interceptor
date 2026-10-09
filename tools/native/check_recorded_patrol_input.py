"""Qualify patrol input in the actual native runner before original recording.

Both native games begin with ordinarily enlisted saves and replay the actual
original qualification/menu controls. The controller records ordinary keys;
the canonical runner must earn exactly the same complete pilot save.
"""
import argparse
import gzip
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT
from compare_flight_traces import number, read_trace


def digest(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', type=Path, required=True)
    parser.add_argument('--runner', type=Path, required=True)
    parser.add_argument('--source-evidence', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    original = json.loads((args.source_evidence / 'report.json').read_text())
    assert original['unmodified_replay_exact']
    for path, expected in original['input_hashes'].items():
        assert digest((ROOT / path).read_bytes()) == expected
    data = gzip.decompress((args.source_evidence / 'driver.jsonl.gz').read_bytes())
    assert digest(data) == original['driver_trace_sha256']
    _, rows = read_trace(args.source_evidence / 'driver.jsonl.gz')
    first = next(i for i, r in rows.items() if number(r, 'mode') == 3 and
                 number(r, 'stage') == 0xC10DAE)
    controls = (args.source_evidence / 'consumed.fa18in').read_bytes()
    assert digest(controls) == original['consumed_input_sha256']
    lines = controls.decode('ascii').splitlines()
    prefix = [lines[0]] + [l for l in lines[1:] if ' K ' in l and int(l.split()[0]) < first]
    (args.out / 'prefix.fa18in').write_text('\n'.join(prefix + [f'end {first - 1} 0']) + '\n')
    env = {k: v for k, v in os.environ.items()
           if not k.startswith(('FA18_MISSION_', 'FA18_LOOP_', 'FA18_ORIGINAL_PILOT_'))}
    with tempfile.TemporaryDirectory(prefix='recorded-patrol-input-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        def run(name, command):
            result = subprocess.run(list(map(str, command)), cwd=ROOT, env=env,
                                    capture_output=True, text=True, timeout=120)
            (args.out / (name + '.log')).write_text(result.stdout + result.stderr)
            assert result.returncode == 0, f'{name}: {result.stderr or result.stdout}'
            return result
        def enlist(name):
            pilot = work / name
            run(name, [args.runner.resolve(), '--headless', '--frames', 9000, '--replay',
                ROOT / 'tools/native/fixtures/region-pilot-enlist.e9k', '--save-dir', pilot])
            initial = (pilot / 'config').read_bytes()
            assert len(initial) == 78 and initial[:4] == bytes(4)
            assert initial[18:28] == bytes(10) and initial[56:58] == bytes(2)
            return pilot, initial
        pilot, initial = enlist('driver-enlist')
        result = run('driver', [args.test.resolve(), ROOT / 'local/media/fa18.adf', pilot,
            (args.out / 'prefix.fa18in').resolve(), (args.out / 'physical.e9k').resolve()])
        summary, = [json.loads(l) for l in result.stdout.splitlines() if '"patrol_input_result"' in l]
        assert all(summary[k] for k in ('objective', 'airborne', 'gear_up', 'gear_down', 'landed'))
        assert summary['completions'] == summary['grade'] == 1
        assert summary['phase'] == 252 and summary['speed'] == summary['resets'] == 0
        saved = (pilot / 'config').read_bytes()
        assert len(saved) == 78 and saved[21] == 1 and int.from_bytes(saved[56:58], 'big') == 1
        # Only extend the diagnostic end bound for canonical replay. All
        # qualification/menu controls remain byte-for-byte the same rows.
        replay_input = args.out / 'replay-prefix.fa18in'
        replay_input.write_text('\n'.join(prefix + [f'end {summary["iteration"]} 0']) + '\n')
        replay_pilot, replay_initial = enlist('replay-enlist')
        assert initial == replay_initial
        replay = run('replay', [args.runner.resolve(), '--headless', '--frames', summary['tick'],
            '--iterations', summary['iteration'], '--input', replay_input.resolve(), '--replay',
            (args.out / 'physical.e9k').resolve(), '--save-dir', replay_pilot])
        stats = json.loads(replay.stdout)
        assert not stats['cpu_emulation'] and not stats['chipset_emulation'] and not stats['postflight_resets']
        assert not stats['input_queued'] and not stats['host_replay_pending']
        actual = (replay_pilot / 'config').read_bytes()
        assert actual == saved, 'canonical runner earned a different pilot save'
        report = dict(scope='Native patrol input qualification only; independent original flight remains separate',
            source_prefix_last=first - 1, source_trace_sha256=original['driver_trace_sha256'],
            source_consumed_input_sha256=original['consumed_input_sha256'],
            adf_sha256=digest((ROOT / 'local/media/fa18.adf').read_bytes()),
            runner_sha256=digest(args.runner.read_bytes()), test_sha256=digest(args.test.read_bytes()),
            controller_ticks_per_update=4, driver=summary, canonical=stats,
            enlisted_save_sha256=digest(initial), earned_save_sha256=digest(saved), earned_save_hex=saved.hex(),
            physical_input_sha256=digest((args.out / 'physical.e9k').read_bytes()),
            prefix_sha256=digest((args.out / 'prefix.fa18in').read_bytes()),
            canonical_prefix_sha256=digest(replay_input.read_bytes()),
            canonical_earned_save_exact=True, flight_state_seeded=False)
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
        print('Patrol input earns objective, geared runway landing and grade in driver and canonical runner')
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
