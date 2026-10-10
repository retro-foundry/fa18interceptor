"""Qualify live-state patrol/escort input in the actual native runner.

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
    parser.add_argument('--source-updates', type=Path,
                        help='Verified original update identities required for an escort prefix')
    parser.add_argument('--assess-failed-flight', action='store_true',
                        help='Replay and retain a rejected escort flight; failed qualification still exits nonzero')
    parser.add_argument('--level-final', action='store_true',
                        help='Escort test keys aim at observed touchdown height while retaining wire-centerline steering')
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    original = json.loads((args.source_evidence / 'report.json').read_text())
    mode = original.get('mission_mode', 3)
    assert mode in (3, 4)
    assert (mode == 4) == (args.source_updates is not None)
    assert not args.assess_failed_flight or mode == 4
    assert not args.level_final or mode == 4
    assert original['unmodified_replay_exact']
    for path, expected in original['input_hashes'].items():
        assert digest((ROOT / path).read_bytes()) == expected
    data = gzip.decompress((args.source_evidence / 'driver.jsonl.gz').read_bytes())
    assert digest(data) == original['driver_trace_sha256']
    _, rows = read_trace(args.source_evidence / 'driver.jsonl.gz')
    first = next(i for i, r in rows.items() if number(r, 'mode') == mode and
                 number(r, 'stage') == 0xC10DAE)
    controls = (args.source_evidence / 'consumed.fa18in').read_bytes()
    assert digest(controls) == original['consumed_input_sha256']
    anchors = None
    if mode == 4:
        from check_recorded_original_mission_trace import verified_update_mapping, source_event_plan
        from check_original_mission_recording import verified_prefix
        checked, _, _ = verified_prefix(Path(original['source_prefix']['path']), original['input_hashes'])
        assert checked == original['source_prefix'] and original['source_prefix_execution_exact']
        mapping, _ = verified_update_mapping(args.source_updates, args.source_evidence/'driver.jsonl.gz', original)
        anchors = source_event_plan(args.source_evidence/'driver.jsonl.gz', mapping, mode)
        controls = (args.source_updates/'update-consumed.fa18in').read_bytes()
        first = mapping[first]
        (args.out/'input.anchors').write_text('FA18_REPLAY_ANCHORS_V1\n'+''.join(
            f'{p["source_first"]} {p["mode"]} {p["stage"]} {p["game_tick"]}\n' for p in anchors))
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
            if result.returncode and not (name=='driver' and args.assess_failed_flight and result.returncode==1):
                for path in work.glob('*.jsonl'):
                    (args.out/(path.name+'.failed.gz')).write_bytes(gzip.compress(path.read_bytes(),mtime=0))
                for path in work.glob('*.dat'):
                    (args.out/(path.name+'.failed.gz')).write_bytes(gzip.compress(path.read_bytes(),mtime=0))
                raise AssertionError(f'{name}: {result.stderr or result.stdout}')
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
        extra = []
        if anchors:
            extra = ['--escort-anchors', (args.out/'input.anchors').resolve(),
                '--flight-trace', work/'driver.jsonl', '--data-out', work/'driver.dat']
            if args.level_final: extra.append('--level-final')
        result = run('driver', [args.test.resolve(), ROOT / 'local/media/fa18.adf', pilot,
            (args.out / 'prefix.fa18in').resolve(), (args.out / 'physical.e9k').resolve(), *extra])
        summary, = [json.loads(l) for l in result.stdout.splitlines() if '"patrol_input_result"' in l]
        success = result.returncode == 0
        if success:
            assert all(summary[k] for k in ('objective', 'airborne', 'gear_up', 'gear_down', 'landed'))
            assert summary['completions'] == mode-2 and summary['grade'] == 1
            assert summary['phase'] == (0 if anchors else 252) and summary['speed'] == summary['resets'] == 0
        else:
            assert args.assess_failed_flight and summary['mode']==4 and summary['prefix_native_end']
            assert summary['resets'] or summary['phase'] in (2,254) or summary['tick']==100000 or summary['landed']
            assert summary['completions']==1 and not summary['grade'], 'Rejected flight unexpectedly earned escort'
        if anchors:
            assert not summary['anchor_failed']
            if success: assert summary['wire'] and summary['menu']
            assert summary['prefix_source_end'] == first-1
        saved = (pilot / 'config').read_bytes()
        assert len(saved) == 78
        if success: assert saved[18+mode] == 1 and int.from_bytes(saved[56:58], 'big') == mode-2
        # Only extend the diagnostic end bound for canonical replay. All
        # qualification/menu controls remain byte-for-byte the same rows.
        replay_input = args.out / 'replay-prefix.fa18in'
        end = first-1+summary['iteration']-summary['prefix_native_end'] if anchors else summary['iteration']
        replay_input.write_text('\n'.join(prefix + [f'end {end} 0']) + '\n')
        replay_pilot, replay_initial = enlist('replay-enlist')
        assert initial == replay_initial
        extra = []
        if anchors:
            extra = ['--input-anchors', (args.out/'input.anchors').resolve(),
                '--input-anchors-out', (args.out/'anchors.json').resolve(),
                '--flight-trace', work/'native.jsonl', '--data-out', work/'native.dat']
        replay = run('replay', [args.runner.resolve(), '--headless', '--frames', summary['tick'],
            '--iterations', end, '--input', replay_input.resolve(), '--replay',
            (args.out / 'physical.e9k').resolve(), '--save-dir', replay_pilot, *extra])
        stats = json.loads(replay.stdout)
        assert not stats['cpu_emulation'] and not stats['chipset_emulation']
        assert stats['postflight_resets']==summary['resets']
        assert not stats['input_queued'] and not stats['host_replay_pending']
        actual = (replay_pilot / 'config').read_bytes()
        assert actual == saved, 'canonical runner earned a different pilot save'
        exact = {}
        if anchors:
            if success: assert (stats['screen'],stats['mode'],stats['stage']) == ('menu',0,'C0FCB4')
            for name in ('jsonl','dat'):
                driven, canonical = (work/f'driver.{name}').read_bytes(), (work/f'native.{name}').read_bytes()
                for label, data in (('driver',driven),('native',canonical)):
                    (args.out/f'{label}.{name}.gz').write_bytes(gzip.compress(data,mtime=0))
                assert driven == canonical, f'canonical complete {name} differs from adaptive input driver'
                exact[name+'_sha256'] = digest(driven)
        report = dict(scope=f'Native mode-{mode} live-state input qualification; independent original flight remains separate',
            mission_success=success, qualification_accepted=success,
            native_level_final_input=args.level_final,
            input_profile='wire_level_height' if args.level_final else 'wire_curve' if anchors else 'patrol_runway',
            mission_mode=mode, escort_anchors=anchors, canonical_complete_trace_and_ram_exact=bool(anchors),
            complete_native_artifacts=exact,
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
        print(f'Mode {mode}: canonical complete replay exact; qualification {"accepted" if success else "rejected"}')
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
    return 0 if success else 1


if __name__ == '__main__':
    raise SystemExit(main())
