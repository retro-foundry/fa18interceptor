"""Generate an original mission recording, then replay it without the pilot.

The external validation pilot chooses ordinary physical keyboard edges only.
Original replay must reproduce every trace byte and final RAM before evidence
is accepted. A failed pilot route remains a failed route, never mission success.
"""
import argparse
import gzip
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT, integer
from compare_flight_traces import read_trace, number


def digest(data):
    return hashlib.sha256(data).hexdigest()


def verified_prefix(path, hashes, expected_mode=3):
    assert expected_mode in (3, 4), 'unsupported earned original prefix'
    report = json.loads((path / 'report.json').read_text())
    assert report.get('mission_mode', 3) == expected_mode
    assert report['input_hashes'] == hashes, 'prefix started from different original media'
    assert report['mission_success'] and report['original_outcome'] == 'success'
    assert (report['final_mode'], report['final_phase'], report['final_completions']) == (0, 0, expected_mode - 2)
    assert report['unmodified_replay_exact']
    for name, key in (('driver.jsonl.gz', 'driver_trace_sha256'),
                      ('driver.dat.gz', 'driver_final_ram_sha256'),
                      ('input.fa18in', 'generated_input_sha256'),
                      ('consumed.fa18in', 'consumed_input_sha256')):
        data = (path / name).read_bytes()
        if name.endswith('.gz'):
            data = gzip.decompress(data)
        assert digest(data) == report[key], f'prefix changed: {name}'
    for replay_key, key in (('trace_sha256', 'driver_trace_sha256'),
                            ('final_ram_sha256', 'driver_final_ram_sha256'),
                            ('consumed_input_sha256', 'consumed_input_sha256')):
        assert report['unmodified_replay'][replay_key] == report[key]
    header, rows = read_trace(path / 'driver.jsonl.gz')
    assert max(rows) == report['iterations']
    end = (path / 'input.fa18in').read_text().splitlines()[-1].split()
    assert end[0] == 'end' and int(end[1]) == max(rows)
    ram = gzip.decompress((path / 'driver.dat.gz').read_bytes())
    assert len(ram) == 0x100048, 'incomplete original prefix RAM/register export'
    assert integer(ram, 0xC45798, 1) == report['final_phase']
    assert integer(ram, 0xC458A6, 1) == report['final_mode']
    pilot = integer(ram, 0xC1AB74, 4)
    assert integer(ram, pilot, 2), 'prefix did not earn qualification'
    for mode in range(3, expected_mode + 1):
        assert integer(ram, pilot + 18 + mode, 1), f'prefix did not earn mission {mode}'
    assert integer(ram, pilot + 56, 2) == report['final_completions']
    if expected_mode == 4:
        assert report['source_prefix_execution_exact'], 'escort prefix execution was not verified'
        parent_path = Path(report['source_prefix']['path'])
        parent, parent_header, parent_rows = verified_prefix(parent_path, hashes)
        assert report['source_prefix'] == parent, 'escort parent evidence changed'
        assert header == parent_header
        assert {i: r for i, r in rows.items() if i <= parent['iterations']} == parent_rows
        assert consumed_keys(path / 'consumed.fa18in', parent['iterations']) == consumed_keys(
            parent_path / 'consumed.fa18in', parent['iterations'])
    evidence = dict(path=str(path.resolve()), report_sha256=digest((path / 'report.json').read_bytes()),
                    iterations=max(rows), trace_sha256=report['driver_trace_sha256'],
                    input_sha256=report['generated_input_sha256'],
                    consumed_sha256=report['consumed_input_sha256'],
                    final_ram_sha256=report['driver_final_ram_sha256'])
    return evidence, header, rows


def consumed_keys(path, last):
    lines = path.read_text().splitlines()
    assert lines[0] == 'FA18_GAME_INPUT_V1'
    return [line for line in lines[1:] if line and not line.startswith('end') and
            int(line.split()[0]) <= last]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--reuse-driver', action='store_true', help='verify an already retained complete driver recording')
    parser.add_argument('--pilot-ticks-per-update', type=int, choices=range(1, 17), default=4,
                        help='validation controller input timing; never changes a game clock')
    parser.add_argument('--patrol-input', action='store_true', help='ordinary approach/landing input without firing')
    parser.add_argument('--repeat-steering', action='store_true',
                        help='validation input: repeat unchanged held steering keys through the original physical keyboard queue')
    parser.add_argument('--wait-for-approach-height', nargs='?', const='final', choices=('final', 'standoff', 'wire'),
                        help='validation carrier-return input: final-mission gate (default), requested standoff height, or standoff height with live wire targeting')
    parser.add_argument('--level-final', action='store_true',
                        help='mode-five wire input: aim at observed aircraft touchdown height during final')
    parser.add_argument('--touchdown-approach', action='store_true',
                        help='mode-five wire input: settle at observed takeoff height at standoff and retain it during final')
    parser.add_argument('--mode', type=int, choices=(3, 4, 5), default=3)
    parser.add_argument('--source-prefix', type=Path,
                        help='verified original recording through the preceding mission, required for modes four and five')
    args = parser.parse_args()
    assert bool(args.source_prefix) == (args.mode > 3), 'later missions require their verified original prefix'
    assert not (args.mode > 3 and args.patrol_input), 'patrol input belongs to mission three'
    assert not (args.wait_for_approach_height and args.mode not in (4, 5)), 'approach-height input requires a carrier-return mission'
    assert not args.level_final or (args.mode == 5 and args.wait_for_approach_height == 'wire'), 'level final requires mode-five wire input'
    assert not args.touchdown_approach or (args.mode == 5 and args.wait_for_approach_height == 'wire' and not args.level_final), 'touchdown approach requires mode-five wire input without level-final override'
    args.out.mkdir(parents=True, exist_ok=True)
    recording = ROOT / 'captures/native/qual_carrier_success/input.fa18in'
    media = (recording, recording.with_name('state.bin'), ROOT / 'local/system/kick13.rom')
    hashes = {str(p.relative_to(ROOT)): digest(p.read_bytes()) for p in media}
    seal = json.loads(recording.with_name('run.json').read_text())
    assert digest(recording.read_bytes()) == seal['input_sha256'], 'sealed input changed'
    assert digest(recording.with_name('state.bin').read_bytes()) == seal['start_state']['sha256']
    assert digest((ROOT / 'local/system/kick13.rom').read_bytes()) == seal['rom_sha256']
    prefix = None
    if args.source_prefix:
        prefix, prefix_header, prefix_rows = verified_prefix(args.source_prefix, hashes, args.mode - 1)
    driver_input = args.source_prefix / 'input.fa18in' if prefix else recording
    env = {k: v for k, v in os.environ.items() if not k.startswith(('FA18_LOOP_', 'FA18_ORIGINAL_PILOT_'))}
    with tempfile.TemporaryDirectory(prefix='original-mission-recording-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        def retain_failure(reason, executable, input_path, paths, returncode=None):
            retained = {}
            for path in paths:
                if path.exists():
                    data = path.read_bytes()
                    (args.out / (path.name + '.partial.gz')).write_bytes(gzip.compress(data, mtime=0))
                    retained[path.name] = dict(bytes=len(data), sha256=digest(data))
            (args.out / 'failure.json').write_text(json.dumps(dict(
                reason=reason, accepted_evidence=False, returncode=returncode,
                executable=str(executable), input=str(input_path), mission_mode=args.mode,
                retained=retained), indent=2) + '\n')
            subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)

        def run(executable, input_path, log_path, trace_path, ram_path, consumed_path, extra_env=None,
                accepted_codes=(0,)):
            try:
                with log_path.open('w') as log:
                    result = subprocess.run([str(executable), '--state', str(recording.with_name('state.bin')),
                        '--rom', str(ROOT / 'local/system/kick13.rom'), '--ports', 'off', '--input', str(input_path),
                        '--to-end', '--frames', str({3: 40000, 4: 65000, 5: 80000}[args.mode]), '--game-input-out', str(consumed_path),
                        '--ram-out', str(ram_path)], cwd=ROOT,
                        env=dict(env, FA18_LOOP_TRACE=str(trace_path), **(extra_env or {})),
                        stdout=log, stderr=subprocess.STDOUT, timeout={3: 300, 4: 600, 5: 900}[args.mode])
            except subprocess.TimeoutExpired:
                retain_failure('Original recording timed out; partial data is not accepted evidence',
                               executable, input_path, (trace_path, ram_path, consumed_path))
                raise
            if result.returncode not in accepted_codes:
                retain_failure('Original process failed; partial data is not accepted evidence',
                               executable, input_path, (trace_path, ram_path, consumed_path), result.returncode)
            return result.returncode
        if not args.reuse_driver:
            subprocess.run(['python', 'scripts/build_recomp.py', '--output', 'build/recomp/fa18_original_mission_pilot.exe',
                '--replace-source', 'port/recomp/loop_input.c=tools/native/original_mission_pilot_loop.c'], cwd=ROOT, check=True)
            # Bind the executable and included controller sources before the
            # process starts; later workspace edits must not relabel a capture.
            driver_identity = dict(
                driver_executable_sha256=digest((ROOT / 'build/recomp/fa18_original_mission_pilot.exe').read_bytes()),
                driver_source_sha256=digest((ROOT / 'tools/native/original_mission_pilot_loop.c').read_bytes()),
                controller_source_sha256=digest((ROOT / 'tools/native/mission_pilot.c').read_bytes()),
                controller_header_sha256=digest((ROOT / 'tools/native/mission_pilot.h').read_bytes()))
            code = run(ROOT / 'build/recomp/fa18_original_mission_pilot.exe', driver_input, args.out / 'driver.log',
                work / 'driver.jsonl', work / 'driver.dat', args.out / 'consumed.fa18in',
                dict(FA18_ORIGINAL_PILOT_INPUT=str((args.out / 'input.fa18in').resolve()),
                     FA18_ORIGINAL_PILOT_KEYS=str((args.out / 'controller-choices.e9k').resolve()),
                     FA18_ORIGINAL_PILOT_TICKS_PER_UPDATE=str(args.pilot_ticks_per_update),
                     FA18_ORIGINAL_PILOT_MODE=str(args.mode),
                     **({'FA18_ORIGINAL_PILOT_PREFIX_END': str(prefix['iterations'])} if prefix else {}),
                     **({'FA18_ORIGINAL_PILOT_PATROL': '1'} if args.patrol_input else {}),
                     **({'FA18_ORIGINAL_PILOT_REPEAT_STEERING': '1'} if args.repeat_steering else {}),
                     **({'FA18_ORIGINAL_PILOT_LEVEL_FINAL': '1'} if args.level_final else {}),
                     **({'FA18_ORIGINAL_PILOT_TOUCHDOWN_APPROACH': '1'} if args.touchdown_approach else {}),
                     **({'FA18_ORIGINAL_PILOT_APPROACH_HEIGHT': '1'} if args.wait_for_approach_height else {}),
                     **({'FA18_ORIGINAL_PILOT_APPROACH_STANDOFF': '1'} if args.wait_for_approach_height in ('standoff', 'wire') else {}),
                     **({'FA18_ORIGINAL_PILOT_WIRE_APPROACH': '1'} if args.wait_for_approach_height == 'wire' else {})),
                accepted_codes=(0, 1))
            assert code in (0, 1), f'original pilot process failed: {code}'
            for name in ('driver.jsonl', 'driver.dat'):
                (args.out / f'{name}.gz').write_bytes(gzip.compress((work / name).read_bytes(), mtime=0))
            # Successful footer and complete final RAM are required even for a
            # diagnosed failed route. Truncated/time-limited output is not proof.
            _, rows = read_trace(work / 'driver.jsonl')
            ram = (work / 'driver.dat').read_bytes()
            log = (args.out / 'driver.log').read_text()
            preliminary = dict(input_hashes=hashes, driver_returncode=code,
                mission_mode=args.mode, source_prefix=prefix,
                **driver_identity,
                controller_ticks_per_update=args.pilot_ticks_per_update,
                patrol_input=args.patrol_input,
                repeat_steering=args.repeat_steering,
                level_final_input=args.level_final,
                touchdown_approach_input=args.touchdown_approach,
                wait_for_approach_height=bool(args.wait_for_approach_height),
                approach_height_target=args.wait_for_approach_height or 'final',
                driver_trace_sha256=digest((work / 'driver.jsonl').read_bytes()),
                driver_final_ram_sha256=digest(ram), generated_input_sha256=digest((args.out / 'input.fa18in').read_bytes()),
                consumed_input_sha256=digest((args.out / 'consumed.fa18in').read_bytes()),
                mission_success=code == 0 and 'complete and menu returned' in log,
                original_outcome='mission failure' if 'Original mission failure outcome' in log else
                                 'landing without earned result' if 'Original landing without earned result' in log else
                                 'crash/reset' if 'Original crash/reset outcome' in log else
                                 'player destroyed' if 'Original player destroyed' in log else 'success' if code == 0 else 'incomplete',
                final_phase=integer(ram, 0xC45798, 1), final_mode=integer(ram, 0xC458A6, 1),
                final_completions=integer(ram, integer(ram, 0xC1AB74, 4) + 56, 2),
                iterations=max(rows))
            if preliminary['mission_success']:
                pilot = integer(ram, 0xC1AB74, 4)
                assert integer(ram, pilot + 18 + args.mode, 1), 'original did not earn the selected mission grade'
                assert preliminary['final_completions'] == args.mode - 2
                assert (preliminary['final_mode'], preliminary['final_phase']) == (0, 0)
            (args.out / 'report.json').write_text(json.dumps(preliminary, indent=2) + '\n')
            print(f'Original driver recording closed: {max(rows)} boundaries; outcome {preliminary["original_outcome"]}', flush=True)
        report = json.loads((args.out / 'report.json').read_text())
        assert report['input_hashes'] == hashes, 'original media changed'
        assert report.get('mission_mode', 3) == args.mode, 'recording belongs to a different mission'
        assert report.get('repeat_steering', False) == args.repeat_steering, 'recording uses different steering inputs'
        assert report.get('level_final_input', False) == args.level_final, 'recording uses different final-height input'
        assert report.get('touchdown_approach_input', False) == args.touchdown_approach, 'recording uses different touchdown-approach input'
        assert report.get('wait_for_approach_height', False) == bool(args.wait_for_approach_height), 'recording uses different approach inputs'
        assert report.get('approach_height_target', 'final') == (args.wait_for_approach_height or 'final'), 'recording uses a different approach height target'
        assert report.get('source_prefix') == prefix, 'recording belongs to a different prefix'
        data = gzip.decompress((args.out / 'driver.jsonl.gz').read_bytes())
        assert digest(data) == report['driver_trace_sha256']
        assert digest(gzip.decompress((args.out / 'driver.dat.gz').read_bytes())) == report['driver_final_ram_sha256']
        assert digest((args.out / 'input.fa18in').read_bytes()) == report['generated_input_sha256']
        assert digest((args.out / 'consumed.fa18in').read_bytes()) == report['consumed_input_sha256']
        header, rows = read_trace(args.out / 'driver.jsonl.gz')
        if prefix:
            assert header == prefix_header, 'prefix trace contract changed'
            assert {i: r for i, r in rows.items() if i <= prefix['iterations']} == prefix_rows, 'original prefix execution changed'
            assert consumed_keys(args.out / 'consumed.fa18in', prefix['iterations']) == consumed_keys(
                args.source_prefix / 'consumed.fa18in', prefix['iterations']), 'original prefix consumed keys changed'
            report['source_prefix_execution_exact'] = True
        result = run(ROOT / 'build/recomp/fa18_recomp.exe', args.out / 'input.fa18in', args.out / 'replay.log',
            work / 'replay.jsonl', work / 'replay.dat', work / 'replay-consumed.fa18in')
        assert result == 0, 'unmodified original replay did not complete'
        replay_trace = (work / 'replay.jsonl').read_bytes()
        replay_ram = (work / 'replay.dat').read_bytes()
        report['unmodified_replay'] = dict(trace_sha256=digest(replay_trace), final_ram_sha256=digest(replay_ram),
            executable_sha256=digest((ROOT / 'build/recomp/fa18_recomp.exe').read_bytes()),
            consumed_input_sha256=digest((work / 'replay-consumed.fa18in').read_bytes()))
        report['unmodified_replay_exact'] = (
            report['unmodified_replay']['trace_sha256'] == report['driver_trace_sha256'] and
            report['unmodified_replay']['final_ram_sha256'] == report['driver_final_ram_sha256'] and
            report['unmodified_replay']['consumed_input_sha256'] == report['consumed_input_sha256'])
        starts = [i for i, r in rows.items() if number(r, 'mode') == args.mode and
                  number(r, 'stage') == 0xC10D8A and number(r, 'game_tick') == 1]
        report['mission_flight_start'] = starts[0] if starts else None
        if args.mode == 3:
            report['mission_three_flight_start'] = report['mission_flight_start']
        report['scope'] = ('Physical-key original recording, independently replayed without the validation pilot. '
                           'Failed route outcomes are preserved. Native whole-flight comparison remains separate.')
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
        if not report['unmodified_replay_exact']:
            for name in ('replay.jsonl', 'replay.dat', 'replay-consumed.fa18in'):
                (args.out / (name + '.gz')).write_bytes(gzip.compress((work / name).read_bytes(), mtime=0))
        assert report['unmodified_replay_exact'], 'pilot adapter changed original execution beyond its recorded keys'
        print(f'{len(rows)} complete original boundaries and consumed keys reproduce exactly without the pilot; '
              f'mission success {report["mission_success"]}', flush=True)
    for path, expected in hashes.items():
        assert digest((ROOT / path).read_bytes()) == expected, 'sealed media changed'
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
