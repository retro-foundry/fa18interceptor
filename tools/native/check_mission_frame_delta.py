"""Capture complete actual native bodies compactly and check original execution.

The games start independently. Existing input-boundary traces, complete final
RAM, counters and earned pilot remain unchanged. No captured state feeds the
native runner. Only the external instruction oracle consumes decoded bodies.
"""
import argparse
import gzip
import json
import os
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT
from check_qualification_message_cadence import digest, verify_trace
from check_recorded_original_mission_trace import verified_update_mapping, verified_continuation
from compare_flight_traces import read_trace, number
from frame_delta import bodies


def native_recording_plan(args, reference, original, rows):
    """Verify a native-owned control recording without weakening old parity.

    The successful original flight still authorizes/proves the ordinary
    earned prefix. Its later controls and world are not declared equal to
    this native flight. Only the external body oracle receives native RAM.
    """
    assert reference['mission_mode'] == original['mission_mode'] == 4
    assert reference['mission_success'] and reference['qualification_accepted']
    assert reference['canonical_complete_trace_and_ram_exact']
    assert reference['runner_sha256'] == digest(args.runner.read_bytes())
    assert reference['source_trace_sha256'] == original['driver_trace_sha256']
    assert reference['source_consumed_input_sha256'] == original['consumed_input_sha256']
    assert reference['adf_sha256'] == digest((ROOT/'local/media/fa18.adf').read_bytes())
    from check_original_mission_recording import verified_prefix
    checked, _, _ = verified_prefix(Path(original['source_prefix']['path']), original['input_hashes'])
    assert checked == original['source_prefix'] and original['source_prefix_execution_exact']
    mapping, update = verified_update_mapping(args.source_updates, args.source_evidence/'driver.jsonl.gz', original)
    from check_recorded_original_mission_trace import source_event_plan
    plan = source_event_plan(args.source_evidence/'driver.jsonl.gz', mapping, 4)
    assert reference['escort_anchors'] == plan
    anchors = json.loads((args.reference/'anchors.json').read_text())
    assert anchors['complete'] and not anchors['failed']
    assert len(anchors['anchors']) == len(plan)
    for expected, actual in zip(plan, anchors['anchors']):
        assert all(expected[k] == actual[k] for k in ('source_first','mode','stage','game_tick'))
        if actual['native_first'] != 1:
            row = rows[actual['native_first']]
            assert row['frame'] == actual['frame']
            assert (number(row,'mode'),number(row,'stage'),number(row,'game_tick')) == (
                expected['mode'],int(expected['stage'],16),expected['game_tick'])
    for name, key in (('physical.e9k','physical_input_sha256'),
                      ('prefix.fa18in','prefix_sha256'),('replay-prefix.fa18in','canonical_prefix_sha256')):
        assert digest((args.reference/name).read_bytes()) == reference[key], f'Native controls changed: {name}'
    prefix = (args.reference/'prefix.fa18in').read_text().splitlines()
    replay = (args.reference/'replay-prefix.fa18in').read_text().splitlines()
    end = reference['source_prefix_last']
    expected = ['FA18_GAME_INPUT_V1']+[line for line in (args.source_updates/'update-consumed.fa18in').read_text().splitlines()[1:]
        if ' K ' in line and int(line.split()[0]) <= end]+[f'end {end} 0']
    assert prefix == expected and prefix[:-1] == replay[:-1], 'Original prefix keys changed'
    assert (args.reference/'input.anchors').read_text() == 'FA18_REPLAY_ANCHORS_V1\n'+''.join(
        f'{p["source_first"]} {p["mode"]} {p["stage"]} {p["game_tick"]}\n' for p in plan)
    assert anchors['source_position'] == int(replay[-1].split()[1])
    assert anchors['native_updates'] == reference['canonical']['replay_iterations']
    assert anchors['keys_consumed'] == reference['driver']['prefix_keys'] == reference['canonical']['replay_events']
    for suffix in ('jsonl','dat'):
        actual, driver = (gzip.decompress((args.reference/f'{name}.{suffix}.gz').read_bytes()) for name in ('native','driver'))
        assert actual == driver and digest(actual) == reference['complete_native_artifacts'][suffix+'_sha256']
    nf, nl = anchors['anchors'][-1]['native_first'], max(rows)
    assert set(range(nf,nl+1)) <= set(rows), 'Native flight trace has missing updates'
    assert reference['canonical']['screen'] == 'menu' and reference['canonical']['stage'] == 'C0FCB4'
    assert not reference['canonical']['postflight_resets']
    return nf, nl, update


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('runner', 'reference', 'source-evidence', 'source-updates', 'out'):
        parser.add_argument('--' + name, type=Path, required=True)
    parser.add_argument('--first', type=int, help='Original flight observation; defaults to entire accepted flight')
    parser.add_argument('--count', type=int, help='Original observations, including real duplicate dispatch entries')
    parser.add_argument('--window', type=Path, help='Additionally require exact old entry/body RAM and metadata')
    parser.add_argument('--native-prefix', type=Path, help='Verified ordinary qualification/patrol/escort prefix for mission five')
    parser.add_argument('--replay-evidence', type=Path, help='Verified complete later-mission controls and event anchors')
    parser.add_argument('--native-input-reference', action='store_true',
                        help='Check a qualified native-owned escort recording; first/count name native updates. Independent original histories remain separate')
    args = parser.parse_args()
    assert (args.first is None) == (args.count is None)
    assert bool(args.native_prefix) == bool(args.replay_evidence)
    assert not (args.native_input_reference and args.native_prefix)
    runner_hash = digest(args.runner.read_bytes())
    args.out.mkdir(parents=True, exist_ok=True)
    reference = json.loads((args.reference / 'report.json').read_text())
    original = json.loads((args.source_evidence / 'report.json').read_text())
    if not args.native_input_reference:
        assert reference['whole_successful_flight']['strict_gameplay_matching']
    for path, expected in (original if args.native_input_reference else reference)['input_hashes'].items():
        assert digest((ROOT / path).read_bytes()) == expected, path
    trace_path = args.reference / 'native.jsonl.gz'
    trace_hash = reference['complete_native_artifacts']['jsonl_sha256'] if args.native_input_reference else reference['native_trace_sha256']
    assert digest(gzip.decompress(trace_path.read_bytes())) == trace_hash
    header, rows = read_trace(trace_path)
    continuation = prefix_evidence = None
    if args.native_input_reference:
        assert not args.window, 'Original-aligned window is only supported by the original-input mode'
        lower, upper, update = native_recording_plan(args, reference, original, rows)
        first = args.first if args.first is not None else lower
        last = first+args.count-1 if args.count is not None else upper
        assert lower <= first <= last <= upper
        nf, nl = first, last
        baseline = reference['canonical']
        final_hash = reference['complete_native_artifacts']['dat_sha256']
        saved_pilot = reference['earned_save_hex']
    else:
        mapping, update = verified_update_mapping(args.source_updates, args.source_evidence / 'driver.jsonl.gz', original)
        assert update == reference['source_update_evidence']
        if args.native_prefix:
            mapping, continuation, prefix_evidence = verified_continuation(args.native_prefix,
                args.replay_evidence, args.runner, original, mapping,
                args.source_updates / 'update-consumed.fa18in', args.source_evidence / 'driver.jsonl.gz', trace_path)
            assert reference['native_prefix'] == prefix_evidence
            assert reference.get('baseline_native_trace_sha256', reference['native_trace_sha256']) == continuation['native_trace_sha256']
            assert reference['event_report'] == continuation['event_report']
            assert reference['native_run'] == continuation['native_run']
            assert reference['native_final_ram_sha256'] == continuation['native_final_ram_sha256']
            assert reference['final_saved_pilot'] == continuation['final_saved_pilot']
        else:
            assert not reference.get('native_prefix'), 'Later-mission capture requires its verified ordinary prefix'
        first = args.first if args.first is not None else reference['whole_successful_flight']['first']
        last = first + args.count - 1 if args.count is not None else reference['whole_successful_flight']['last']
        assert reference['whole_successful_flight']['first'] <= first <= last <= reference['whole_successful_flight']['last']
        nf, nl = mapping[first], mapping[last]
        assert set(mapping[i] for i in range(first, last + 1)) == set(range(nf, nl + 1))
        baseline = reference['native_run']
        final_hash = reference['native_final_ram_sha256']
        saved_pilot = reference['final_saved_pilot']
    sealed_window = None
    if args.window:
        sealed_window = json.loads((args.window / 'report.json').read_text())
        assert (sealed_window['first'], sealed_window['last']) == (first, last)
    oracle = ROOT / 'build/recomp/native_frame_body_oracle.exe'
    oracle_sources = ('tools/native/native_frame_body_oracle.c', 'tools/native/native_records_oracle.c')
    oracle_source_hashes = {name: digest((ROOT / name).read_bytes()) for name in oracle_sources}
    with (args.out / 'oracle-build.log').open('w') as log:
        subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                        '--main', 'tools/native/native_frame_body_oracle.c'], cwd=ROOT, stdout=log,
                       stderr=subprocess.STDOUT, check=True)
    oracle_hash = digest(oracle.read_bytes())
    env = {k: v for k, v in os.environ.items() if not k.startswith(('FA18_FRAME_', 'FA18_TRACE_', 'FA18_LOOP_', 'FA18_MISSION_', 'FA18_ORIGINAL_PILOT_'))}
    if 'message_shown' in {f['name'] for f in header['fields']}:
        env['FA18_TRACE_MESSAGE_FIELDS'] = '1'
    identities, failures = [], []
    with tempfile.TemporaryDirectory(prefix='mission-frame-delta-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        pilot, stream, final = work / 'pilot', work / 'frames.delta', work / 'final.dat'
        def run(arguments, log_name):
            result = subprocess.run([str(args.runner.resolve()), '--headless', *arguments], cwd=ROOT,
                                    env=env, capture_output=True, text=True, timeout=300)
            (args.out / log_name).write_text(result.stdout + result.stderr)
            assert result.returncode == 0, log_name
            return json.loads(result.stdout)
        enlist = run(['--frames', '9000', '--replay', str(ROOT / 'tools/native/fixtures/region-pilot-enlist.e9k'),
                      '--save-dir', str(pilot)], 'enlist.log')
        if not args.native_input_reference:
            assert enlist == reference['enlist_run'], 'Enlistment counters changed'
        initial = (pilot / 'config').read_bytes()
        assert len(initial) == 78 and initial[:4] == bytes(4)
        assert initial[18:28] == bytes(10) and initial[56:58] == bytes(2), 'Pilot was not newly enlisted'
        if args.native_input_reference:
            assert digest(initial) == reference['enlisted_save_sha256'], 'Ordinarily enlisted pilot changed'
        intro = work / 'intro.e9k'
        intro.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        replay_arguments = ['--replay', str(intro), '--input', str((args.source_updates/'update-consumed.fa18in').resolve()),
                            '--iterations', str(update['real_update_calls'])]
        if args.native_input_reference:
            end = int((args.reference/'replay-prefix.fa18in').read_text().splitlines()[-1].split()[1])
            replay_arguments = ['--replay', str((args.reference/'physical.e9k').resolve()),
                '--input', str((args.reference/'replay-prefix.fa18in').resolve()), '--iterations', str(end),
                '--input-anchors', str((args.reference/'input.anchors').resolve()),
                '--input-anchors-out', str(work/'anchors.json')]
        if continuation:
            replay_arguments = ['--replay', str((args.native_prefix/'physical.e9k').resolve()),
                '--input', str((args.replay_evidence/'input.segment.fa18in').resolve()),
                '--iterations', str(continuation['input_segment']['replay_source_end']),
                '--input-anchors', str((args.replay_evidence/'input.anchors').resolve()),
                '--input-anchors-out', str(work/'anchors.json')]
        summary = run(['--frames', str(baseline['frames']), *replay_arguments, '--save-dir', str(pilot),
                       '--frame-delta', f'{nf}+{nl - nf + 1}', str(stream), '--data-out', str(final),
                       '--memory-report', str(args.out.resolve() / 'memory.json')], 'flight.log')
        assert summary == baseline, 'Diagnostic changed runtime counters'
        assert digest(final.read_bytes()) == final_hash
        assert (pilot / 'config').read_bytes().hex() == saved_pilot
        if continuation:
            assert json.loads((work/'anchors.json').read_text()) == continuation['event_report']
        if args.native_input_reference:
            assert json.loads((work/'anchors.json').read_text()) == json.loads((args.reference/'anchors.json').read_text())
        stream_bytes, stream_hash = stream.stat().st_size, digest(stream.read_bytes())
        old = {row['native_iteration']: row for row in sealed_window['rows']} if sealed_window else {}
        with (args.out / 'original-bodies.log').open('w') as log:
            for body in bodies(stream):
                entry, before, after = (body[key] for key in ('entry', 'before', 'after'))
                j = entry['iteration']
                assert nf <= j <= nl and j == nf + len(identities)
                assert entry['frame'] == rows[j]['frame']
                verify_trace(entry['data'], header, rows[j])
                if sealed_window:
                    expected = old[j]
                    assert entry['ram_sha256'] == expected['ram_sha256']['native']
                    for name, snapshot in (('before', before), ('after', after)):
                        assert snapshot['data'] == gzip.decompress((args.window / f'native.{j}.{name}.dat.gz').read_bytes())
                    assert (before['frame'], after['frame'], before['saved_tick']) == tuple(
                        expected['body_timing'][name] for name in ('before_tick', 'after_tick', 'saved_tick'))
                input_path, output_path = work / 'before.dat', work / 'after.dat'
                input_path.write_bytes(before['data']); output_path.write_bytes(after['data'])
                command = [str(oracle), str(input_path), str(output_path), str(before['frame']),
                           str(after['frame']), str(before['saved_tick'])]
                if after['boundary'] == 3:
                    command += [str(work / 'original-owner-exit.dat'), 'owner-exit']
                result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, text=True, timeout=20)
                log.write(f'Iteration {j}\n' + result.stdout + result.stderr)
                if result.returncode or '0 gameplay differences, 0 display bytes' not in result.stdout:
                    for name, snapshot in body.items():
                        (args.out / f'failed-{j}.{name}.dat.gz').write_bytes(gzip.compress(snapshot['data'], mtime=0))
                    failures.append(j)
                identities.append(dict(iteration=j, entry_sha256=entry['ram_sha256'],
                                       before_sha256=before['ram_sha256'], after_sha256=after['ram_sha256'],
                                       before_tick=before['frame'], after_tick=after['frame'],
                                       saved_tick=before['saved_tick'], original_body_matching=j not in failures))
                if len(identities) % 100 == 0:
                    print(f'{len(identities)}/{nl - nf + 1} complete bodies checked; failures={len(failures)}', flush=True)
        assert len(identities) == nl - nf + 1, 'Incomplete streamed flight range'
        retained = args.out / 'frames.delta.gz'
        with stream.open('rb') as source, gzip.GzipFile(filename=str(retained), mode='wb', mtime=0) as target:
            while chunk := source.read(1024 * 1024):
                target.write(chunk)
        assert digest(oracle.read_bytes()) == oracle_hash, 'Original body oracle changed during comparison'
        assert {name: digest((ROOT / name).read_bytes()) for name in oracle_sources} == oracle_source_hashes
    assert digest(args.runner.read_bytes()) == runner_hash, 'Runner changed during verification'
    report = dict(first=first, last=last, native_first=nf, native_last=nl, bodies=len(identities),
                  runner_sha256=runner_hash, reference_report_sha256=digest((args.reference / 'report.json').read_bytes()),
                  original_body_oracle_sha256=oracle_hash, original_body_oracle_sources_sha256=oracle_source_hashes,
                  native_trace_sha256=trace_hash, native_final_ram_sha256=final_hash,
                  native_owned_input_reference=args.native_input_reference,
                  source_update_evidence=update,
                  runtime_counters_preserved=True, final_ram_preserved=True, earned_save_preserved=True,
                  original_body_failures=failures, original_bodies_matching=not failures,
                  stream_bytes=stream_bytes, stream_sha256=stream_hash, compressed_bytes=retained.stat().st_size,
                  conventional_capture_bytes=3 * 0x100000 * len(identities), snapshots=3 * len(identities),
                  old_window_ram_and_metadata_identical=bool(sealed_window), identities=identities,
                  scope='Complete actual native input/begin/end RAM with exact old input trace and complete final flight preservation. Every decoded native body executes original instructions under the existing explicit frame-body exclusions and supplied original API timer contract. Independent source-versus-native cache histories and other full flights remain separate.')
    if continuation:
        report.update(native_prefix=prefix_evidence, event_report=continuation['event_report'],
            input_segment=continuation['input_segment'], ordinary_native_prefix_replayed=True)
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'{len(identities)} complete bodies: {len(failures)} original execution failures; delta {stream_bytes} bytes versus {report["conventional_capture_bytes"]} separate raw bytes')
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
    return 1 if failures else 0


if __name__ == '__main__':
    raise SystemExit(main())
