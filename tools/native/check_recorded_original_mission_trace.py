"""Compare recorded original flights from independent starts.

Enlist a native pilot using ordinary keys, then cold-load its actual saved
config. Later missions replay the verified ordinary preceding native flights.
Original RAM never initializes the native game. Complete drawings and
timers remain strict diagnostics; a failed original flight is not mission success.
"""
import argparse
import copy
import csv
import gzip
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT, integer, span
from compare_flight_traces import compare, number, read_trace
from check_original_update_entries import content, update_map


def digest(data):
    return hashlib.sha256(data).hexdigest()


def source_event_plan(source_path, mapping, mode, start=1):
    _, rows = read_trace(source_path)
    assert mapping[start] == 1
    if start != 1:
        assert (number(rows[start], 'mode'), number(rows[start], 'stage'), number(rows[start], 'game_tick')) == (0, 0xc0fcb4, 0), 'segment does not start at the actual source menu'
    observations = [start,
        next(i for i, r in rows.items() if number(r, 'mode') == mode and number(r, 'stage') == 0xC10C08),
        next(i for i, r in rows.items() if number(r, 'mode') == mode and
             number(r, 'stage') == 0xC10D8A and number(r, 'game_tick') == 1)]
    return [dict(source_observation=i, source_first=mapping[i], mode=number(rows[i], 'mode'),
                 stage=f'{number(rows[i], "stage"):06X}', game_tick=number(rows[i], 'game_tick'))
            for i in observations]


def event_mapping(plan, evidence, mapping, native_path, menu_boundary=None):
    assert evidence['format'] == 'FA18_REPLAY_ANCHORS_V1'
    assert evidence['complete'] and not evidence['failed'], 'declared event replay incomplete'
    assert len(evidence['anchors']) == len(plan)
    _, native = read_trace(native_path)
    previous, frame = 0, 0
    for expected, actual in zip(plan, evidence['anchors']):
        assert all(actual[k] == expected[k] for k in ('source_first', 'mode', 'stage', 'game_tick'))
        assert actual['native_first'] > previous and actual['frame'] > frame
        previous, frame = actual['native_first'], actual['frame']
        if expected['source_first'] == 1:
            assert actual['native_first'] == 1
        else:
            if actual['native_first'] not in native:
                # Flight traces omit menu-only updates. The preceding complete
                # replay provides this exact menu ordinal/frame independently.
                assert (expected['mode'], expected['stage'], expected['game_tick']) == (0, 'C0FCB4', 0), 'missing flight anchor observation'
                assert menu_boundary == (actual['native_first'], actual['frame']), 'missing verified preceding menu boundary'
                continue
            row = native[actual['native_first']]
            assert row['frame'] == actual['frame']
            assert (number(row, 'mode'), number(row, 'stage'), number(row, 'game_tick')) == (
                expected['mode'], int(expected['stage'], 16), expected['game_tick'])
    assert max(native) <= evidence['native_updates']
    assert evidence['source_position'] == max(mapping.values())
    first = plan[-1]['source_observation']
    actual_first = evidence['anchors'][-1]['native_first']
    assert evidence['native_updates'] == actual_first + max(mapping.values()) - mapping[first]
    # Declared flight-start events establish origins. Every following source
    # observation retains its explicit JSR identity, including resumptions.
    return {i: actual_first + update - mapping[first] for i, update in mapping.items() if i >= first}


def context(ram):
    pilot = span(ram, integer(ram, 0xC1AB74, 4), 78)
    assert len(pilot) == 78
    return dict(pilot=pilot.hex(), qualified=int.from_bytes(pilot[:2], 'big'),
                saved_level=int.from_bytes(pilot[2:4], 'big'), grades=list(pilot[18:28]),
                completions=int.from_bytes(pilot[56:58], 'big'),
                scene_level=integer(ram, 0xC458A7, 1),
                player_phase=integer(ram, 0xC45798, 1))


def verified_native_prefix(path, runner):
    """Verify an actual earned native disk save and its full replay evidence.

    RAM is read only to check the retained disk export. Neither original RAM
    nor constructed qualification/grade bytes initialize the native game.
    """
    report = json.loads((path / 'report.json').read_text())
    assert report['mission_mode'] == 4, 'native prefix must finish escort'
    assert report['mission_success'] and report['qualification_accepted'], 'native prefix did not earn escort'
    assert report['canonical_complete_trace_and_ram_exact'] and report['canonical_earned_save_exact']
    assert not report['flight_state_seeded'], 'native prefix seeded gameplay state'
    assert report['runner_sha256'] == digest(runner.read_bytes()), 'native prefix belongs to a different runner'
    assert report['adf_sha256'] == digest((ROOT / 'local/media/fa18.adf').read_bytes()), 'native prefix media changed'
    for suffix in ('dat', 'jsonl'):
        driven = content(path / ('driver.' + suffix))
        canonical = content(path / ('native.' + suffix))
        assert driven == canonical, 'native prefix canonical capture differs'
        assert digest(canonical) == report['complete_native_artifacts'][suffix + '_sha256'], 'native prefix capture changed'
    for name, key in (('physical.e9k', 'physical_input_sha256'),
                      ('prefix.fa18in', 'prefix_sha256'),
                      ('replay-prefix.fa18in', 'canonical_prefix_sha256')):
        assert digest((path / name).read_bytes()) == report[key], 'native prefix controls changed'
    saved = bytes.fromhex(report['earned_save_hex'])
    assert len(saved) == 78 and digest(saved) == report['earned_save_sha256'], 'native earned disk save changed'
    ram = content(path / 'native.dat')
    assert len(ram) == 0x100000
    actual = context(ram)
    assert actual['pilot'] == saved.hex(), 'native disk save differs from actual earned pilot'
    assert actual['qualified'] and actual['grades'][3:5] == [1, 1] and actual['completions'] == 2, 'native prefix lacks actual preceding grades'
    assert (integer(ram, 0xc458a6, 1), actual['player_phase'], integer(ram, 0xc1820c, 4)) == (0, 0, 0xc0fcb4), 'native prefix did not return to the menu'
    stats = report['canonical']
    assert (stats['screen'], stats['mode'], stats['stage']) == ('menu', 0, 'C0FCB4')
    assert not stats['cpu_emulation'] and not stats['chipset_emulation'] and not stats['postflight_resets']
    evidence = dict(path=str(path.resolve()), report_sha256=digest((path / 'report.json').read_bytes()),
        save_sha256=digest(saved), trace_sha256=report['complete_native_artifacts']['jsonl_sha256'],
        ram_sha256=report['complete_native_artifacts']['dat_sha256'], runner_sha256=report['runner_sha256'])
    return saved, evidence


def source_input_segment(mapping, source_input, start):
    """Use the proven next JSR as ordinal one; preserve every later key edge."""
    origin = mapping[start]
    assert origin > max(update for observation, update in mapping.items() if observation < start), 'source menu begins inside a pending update'
    held, lines = {}, ['FA18_GAME_INPUT_V1']
    source_lines = source_input.decode('ascii').splitlines()
    assert source_lines[0] == lines[0]
    for line in source_lines[1:]:
        parts = line.split()
        if not parts:
            continue
        if parts[0] == 'end':
            assert int(parts[1]) == max(mapping.values())
            parts[1] = str(int(parts[1]) - origin + 1)
        elif int(parts[0]) < origin:
            assert parts[2] == 'K'
            held[int(parts[3])] = int(parts[4])
            continue
        else:
            parts[0] = str(int(parts[0]) - origin + 1)
        lines.append(' '.join(parts))
    assert not any(held.values()), 'source menu inherits held keys'
    localized = {i: update - origin + 1 for i, update in mapping.items() if i >= start}
    return localized, ('\n'.join(lines) + '\n').encode('ascii'), origin


def verify_prefix_execution(prefix, current_path, anchors, prefix_record):
    """Check the whole frozen prefix before its already-counted menu boundary."""
    opener = gzip.open if current_path.suffix == '.gz' else open
    with gzip.open(prefix / 'native.jsonl.gz', 'rb') as previous, opener(current_path, 'rb') as current:
        rows = 0
        for line in previous:
            row = json.loads(line)
            if row.get('end'):
                break
            assert current.readline() == line, 'native preceding execution changed'
            if 'iteration' in row:
                rows += 1
    menu = anchors['anchors'][len(prefix_record['escort_anchors'])]
    canonical = prefix_record['canonical']
    assert (menu['native_first'], menu['frame']) == (canonical['replay_iterations'], canonical['frames']), 'native preceding menu boundary changed'
    return rows


def verified_update_mapping(path, source_path, original):
    evidence = json.loads((path / 'report.json').read_text())
    assert evidence['source_trace_sha256'] == original['driver_trace_sha256']
    assert evidence['source_final_ram_sha256'] == original['driver_final_ram_sha256']
    assert evidence['source_consumed_input_sha256'] == original['consumed_input_sha256']
    for name, key in (('entries.csv', 'entries_sha256'),
                      ('complete-boundary.csv', 'instruction_trace_sha256'),
                      ('complete-source.jsonl', 'source_trace_sha256'),
                      ('complete-source.dat', 'source_final_ram_sha256'),
                      ('mapping.json', 'mapping_sha256'),
                      ('update-consumed.fa18in', 'remapped_input_sha256')):
        assert digest(content(path / name)) == evidence[key], f'update evidence changed: {name}'
    _, source = read_trace(source_path)
    mapping, repeats, updates = update_map(
        list(csv.DictReader(content(path / 'entries.csv').decode('ascii').splitlines())),
        list(csv.DictReader(content(path / 'complete-boundary.csv').decode('ascii').splitlines())), source)
    assert mapping == {int(k): v for k, v in json.loads((path / 'mapping.json').read_text()).items()}
    assert len(repeats) == evidence['duplicate_observations'] and updates == evidence['real_update_calls']
    expected = ['FA18_GAME_INPUT_V1']
    for line in content(path / 'complete-consumed.fa18in').decode('ascii').splitlines()[1:]:
        parts = line.split()
        if not parts:
            continue
        if parts[0] == 'end':
            parts[1] = str(updates)
        else:
            parts[0] = str(mapping[int(parts[0])])
        expected.append(' '.join(parts))
    assert digest(content(path / 'complete-consumed.fa18in')) == original['consumed_input_sha256']
    assert ('\n'.join(expected) + '\n').encode('ascii') == (path / 'update-consumed.fa18in').read_bytes()
    return mapping, evidence


def assess(source_path, native_path, success=False, mapping=None, mode=3, outcome=None):
    sh, source = read_trace(source_path)
    nh, native = read_trace(native_path)
    assert sh == nh
    def first(rows):
        return next(i for i, r in rows.items() if number(r, 'mode') == mode and
                    number(r, 'stage') == 0xC10D8A and number(r, 'game_tick') == 1)
    sf, nf = first(source), first(native)
    assert (mapping[sf] if mapping else sf) == nf, ('different initialization/input origins', sf, nf)
    def stop(rows, start):
        return next(i for i in range(start + 1, max(rows) + 1)
                    if number(rows[i], 'stage') == 0xC11788)
    if success:
        ss = next(i for i, r in source.items() if i > sf and number(r, 'mode') == 0) - 1
        ns = max(native)
        assert (mapping[ss] if mapping else ss) == ns, 'different final observed flight boundary'
    elif outcome == 'mission failure':
        ss, ns = max(source), max(native)
        assert number(source[ss], 'mode') == mode
        assert (mapping[ss] if mapping else ss) == ns, 'different final failure observation'
    else:
        ss, ns = stop(source, sf), stop(native, nf)
        assert (mapping[ss] if mapping else ss) == ns == max(native), 'different crash callback or recording end'
        assert ss == max(source), 'original recording continues beyond crash callback'
    actual_native = native
    if mapping:
        native = {i: actual_native[mapping[i]] for i in range(sf, ss + 1)}
        nf = sf
    count = ss - sf + 1
    report = compare(source, native, sf, nf, count)
    report.update(first=sf, last=ss,
        records_matching_per_slot=[sum(source[i]['records'][j] == native[i]['records'][j]
            for i in range(sf, ss + 1)) for j in range(16)],
        drawing_differences=[dict(iteration=i, tick=number(source[i], 'game_tick'),
            planes=[p for p in range(8) if source[i]['pages'][p] != native[i]['pages'][p]],
            changed_fields={k: [v, native[i]['fields'][k]] for k, v in source[i]['fields'].items()
                            if v != native[i]['fields'][k]})
            for i in range(sf, ss + 1) if source[i]['pages'] != native[i]['pages']])
    first_difference = min((r['source_iteration'] for r in
        (report['first_record_difference'], report['first_game_difference']) if r), default=ss + 1)
    report['complete_gameplay_matching_prefix'] = first_difference - sf
    report['strict_gameplay_matching'] = first_difference == ss + 1
    report['native_menu_observation_gaps'] = [i for i in range(ss + 1, max(source) + 1)
        if (mapping[i] if mapping else i) not in actual_native]
    if mapping:
        report['execution_alignment'] = dict(first_native_update=mapping[sf], last_native_update=mapping[ss],
            real_updates_compared=len({mapping[i] for i in range(sf, ss + 1)}),
            duplicate_observations_preserved=[i for i in range(sf + 1, ss + 1) if mapping[i] == mapping[i - 1]],
            basis='Original explicit JSR calls and executed LINK instructions; no state matching or frame search')
    # Probe sensitivity against one actual original boundary. The independent
    # flight may already differ at its first row; changing another byte must
    # not require that already-different row to become "less matching".
    report['mutation_rejections'] = {}
    probe = {sf: source[sf]}
    baseline = compare(probe, probe, sf, sf, 1)
    for kind in ('npc_core', 'controls', 'page'):
        changed = {sf: copy.deepcopy(source[sf])}
        row = changed[sf]
        if kind == 'npc_core':
            data = bytearray.fromhex(row['records'][15]); data[0] ^= 1
            row['records'][15] = data.hex()
            field = 'complete_record_boundaries_matching'
        elif kind == 'controls':
            data = bytearray.fromhex(row['fields']['controls']); data[0] ^= 1
            row['fields']['controls'] = data.hex()
            field = 'camera_and_controls_matching'
        else:
            data = bytearray.fromhex(row['pages'][0]); data[0] ^= 1
            row['pages'][0] = data.hex()
            field = 'complete_pages_matching'
        rejected = compare(probe, changed, sf, sf, 1)
        assert rejected[field] == baseline[field] - 1, f'{kind} difference lost'
        report['mutation_rejections'][kind] = True
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, required=True)
    parser.add_argument('--source-evidence', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--assess-existing', action='store_true')
    parser.add_argument('--source-updates', type=Path,
                        help='verified original JSR/LINK probe; retains every observed boundary')
    parser.add_argument('--event-anchors', action='store_true',
                        help='bind carrier-mission context/flight controls to declared source callback entry events')
    parser.add_argument('--native-prefix', type=Path,
                        help='mode five: replay the verified ordinary native escort inputs before the next original flight')
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    evidence = args.source_evidence
    original = json.loads((evidence / 'report.json').read_text())
    mode = original.get('mission_mode', 3)
    assert mode in (3, 4, 5)
    assert (mode == 5) == bool(args.native_prefix), 'mode five requires its actual earned native prefix'
    assert mode != 5 or (args.source_updates and args.event_anchors), 'mode five requires verified update identities and declared events'
    if mode > 3:
        from check_original_mission_recording import verified_prefix
        prefix = original['source_prefix']
        checked, _, _ = verified_prefix(Path(prefix['path']), original['input_hashes'], mode - 1)
        assert checked == prefix and original['source_prefix_execution_exact']
    assert original['unmodified_replay_exact'], 'original recording has not reproduced independently'
    success = original['mission_success']
    mapping, update_evidence = (verified_update_mapping(args.source_updates, evidence / 'driver.jsonl.gz', original)
        if args.source_updates else (None, None))
    input_path = (args.source_updates / 'update-consumed.fa18in') if mapping else evidence / 'consumed.fa18in'
    iterations = update_evidence['real_update_calls'] if mapping else original['iterations']
    native_prefix, input_segment = None, None
    prefix_record, prefix_end, plan = None, 0, None
    start = 1
    if args.native_prefix:
        _, native_prefix = verified_native_prefix(args.native_prefix, args.runner)
        prefix_record = json.loads((args.native_prefix / 'report.json').read_text())
        prefix_lines = (args.native_prefix / 'replay-prefix.fa18in').read_text().splitlines()
        assert prefix_lines[0] == 'FA18_GAME_INPUT_V1' and prefix_lines[-1].startswith('end ')
        prefix_end = int(prefix_lines[-1].split()[1])
        prefix_plan = prefix_record['escort_anchors']
        expected_anchors = 'FA18_REPLAY_ANCHORS_V1\n' + ''.join(
            f'{p["source_first"]} {p["mode"]} {p["stage"]} {p["game_tick"]}\n' for p in prefix_plan)
        assert (args.native_prefix / 'input.anchors').read_text() == expected_anchors, 'native prefix anchors changed'
        start = original['source_prefix']['iterations'] + 1
        mapping, segment, origin = source_input_segment(mapping, input_path.read_bytes(), start)
        plan = source_event_plan(evidence / 'driver.jsonl.gz', mapping, mode, start)
        plan = prefix_plan + [dict(p, source_first=p['source_first'] + prefix_end) for p in plan]
        mapping = {i: update + prefix_end for i, update in mapping.items()}
        combined = prefix_lines[:-1]
        for line in segment.decode('ascii').splitlines()[1:]:
            parts = line.split()
            at = 1 if parts[0] == 'end' else 0
            parts[at] = str(int(parts[at]) + prefix_end)
            combined.append(' '.join(parts))
        segment = ('\n'.join(combined) + '\n').encode('ascii')
        input_path = args.out / 'input.segment.fa18in'
        if args.assess_existing:
            assert input_path.read_bytes() == segment, 'retained source segment changed'
        else:
            input_path.write_bytes(segment)
        iterations = max(mapping.values())
        input_segment = dict(first_source_observation=start, first_actual_source_update=origin,
                             input_sha256=digest(segment), replay_source_end=iterations,
                             native_prefix_source_end=prefix_end, ordinary_native_prefix_replayed=True)
    assert not args.event_anchors or (mode in (4, 5) and mapping), 'event replay requires verified carrier-mission update identities'
    if args.event_anchors and plan is None:
        plan = source_event_plan(evidence / 'driver.jsonl.gz', mapping, mode, start)
    outcome = original['original_outcome']
    expected_outcomes = ('success',) if success else ('crash/reset', 'mission failure')
    assert outcome in expected_outcomes
    for name, expected in original['input_hashes'].items():
        assert digest((ROOT / name).read_bytes()) == expected, f'original media changed: {name}'
    for name, key, compressed in (
            ('driver.jsonl.gz', 'driver_trace_sha256', True),
            ('driver.dat.gz', 'driver_final_ram_sha256', True),
            ('consumed.fa18in', 'consumed_input_sha256', False),
            ('input.fa18in', 'generated_input_sha256', False)):
        data = (evidence / name).read_bytes()
        assert digest(gzip.decompress(data) if compressed else data) == original[key], name
    # Check the unmodified replay's actual retained fingerprints, not only its flag.
    for key in ('trace_sha256', 'final_ram_sha256', 'consumed_input_sha256'):
        driver_key = 'driver_' + key if key != 'consumed_input_sha256' else key
        assert original['unmodified_replay'][key] == original[driver_key]
    enlist = ROOT / 'tools/native/fixtures/region-pilot-enlist.e9k'
    hashes = {str(p.relative_to(ROOT)): digest(p.read_bytes())
              for p in (enlist, ROOT / 'local/media/fa18.adf')}
    if args.assess_existing:
        report = json.loads((args.out / 'report.json').read_text())
        assert report['native_input_hashes'] == hashes
        assert report['source_trace_sha256'] == original['driver_trace_sha256']
        assert report['consumed_input_sha256'] == original['consumed_input_sha256']
        assert report.get('source_update_evidence') == update_evidence
        assert report.get('event_plan') == plan
        assert report.get('native_prefix') == native_prefix and report.get('input_segment') == input_segment
        if plan:
            assert digest((args.out / 'input.anchors').read_bytes()) == report['anchor_input_sha256']
            assert digest((args.out / 'anchors.json').read_bytes()) == report['anchor_report_sha256']
            assert json.loads((args.out / 'anchors.json').read_text()) == report['event_report']
        if mapping:
            assert report['replay_input_sha256'] == digest(input_path.read_bytes())
        for name, key in (('native.jsonl.gz', 'native_trace_sha256'),
                          ('native.dat.gz', 'native_final_ram_sha256')):
            assert digest(gzip.decompress((args.out / name).read_bytes())) == report[key]
        assert context(gzip.decompress((args.out / 'native.dat.gz').read_bytes())) == report['native_context']
        if args.native_prefix:
            assert report['native_prefix_execution_exact']
            assert verify_prefix_execution(args.native_prefix, args.out / 'native.jsonl.gz', report['event_report'], prefix_record) == report['native_prefix_observations_exact']
    else:
        env = {k: v for k, v in os.environ.items()
               if not k.startswith(('FA18_LOOP_', 'FA18_ORIGINAL_PILOT_'))}
        with tempfile.TemporaryDirectory(prefix='recorded-mission-trace-', dir=ROOT / 'build') as directory:
            work = Path(directory)
            pilot = work / 'pilot'
            def run(arguments, log_name):
                result = subprocess.run([str(args.runner.resolve()), '--headless', *arguments],
                    cwd=ROOT, env=env, capture_output=True, text=True, timeout=120)
                (args.out / log_name).write_text(result.stdout + result.stderr)
                if result.returncode:
                    for name in ('native.jsonl', 'native.dat'):
                        path = work / name
                        if path.exists():
                            (args.out / (name + '.failed.gz')).write_bytes(gzip.compress(path.read_bytes(), mtime=0))
                assert result.returncode == 0, f'native run failed: {log_name}'
                return json.loads(result.stdout)
            enlist_stats = run(['--frames', '9000', '--replay', str(enlist),
                                '--save-dir', str(pilot)], 'enlist.log')
            initial = (pilot / 'config').read_bytes()
            assert len(initial) == 78 and initial[:4] == bytes(4)
            assert initial[18:28] == bytes(10) and initial[56:58] == bytes(2)
            if prefix_record:
                assert digest(initial) == prefix_record['enlisted_save_sha256'], 'native prefix enlistment changed'
            warmup = work / 'intro.e9k'
            warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
            if args.native_prefix:
                warmup = args.native_prefix / 'physical.e9k'
            anchor_args = []
            if plan:
                (args.out / 'input.anchors').write_text('FA18_REPLAY_ANCHORS_V1\n' + ''.join(
                    f'{p["source_first"]} {p["mode"]} {p["stage"]} {p["game_tick"]}\n' for p in plan))
                anchor_args = ['--input-anchors', str((args.out / 'input.anchors').resolve()),
                               '--input-anchors-out', str((args.out / 'anchors.json').resolve())]
            stats = run(['--frames', '180000' if native_prefix else '100000', '--replay', str(warmup),
                '--input', str(input_path.resolve()),
                '--iterations', str(iterations), '--save-dir', str(pilot),
                '--flight-trace', str(work / 'native.jsonl'), '--data-out', str(work / 'native.dat'), *anchor_args], 'native.log')
            # Retain completed runner output before any validation assertion.
            # A rejected check must not discard the flight it was assessing.
            for name in ('native.jsonl', 'native.dat'):
                (args.out / (name + '.gz')).write_bytes(gzip.compress((work / name).read_bytes(), mtime=0))
            if plan:
                anchors = json.loads((args.out / 'anchors.json').read_text())
                assert anchors['complete'] and not anchors['failed'] and anchors['source_position'] == iterations
                assert anchors['native_updates'] == stats['replay_iterations']
            if args.native_prefix:
                prefix_rows = verify_prefix_execution(args.native_prefix, work / 'native.jsonl', anchors, prefix_record)
            if not plan:
                assert stats['replay_iterations'] == iterations
            events = sum(' K ' in line for line in input_path.read_text().splitlines())
            assert stats['replay_events'] == events and stats['input_queued'] == 0
            assert not stats['cpu_emulation'] and not stats['chipset_emulation']
            ram = (work / 'native.dat').read_bytes()
            assert len(ram) == 0x100000
            report = dict(native_input_hashes=hashes, mission_mode=mode, event_plan=plan,
                source_trace_sha256=original['driver_trace_sha256'],
                consumed_input_sha256=original['consumed_input_sha256'],
                source_update_evidence=update_evidence,
                replay_input_sha256=digest(input_path.read_bytes()),
                runner_sha256=digest(args.runner.read_bytes()),
                enlisted_pilot=initial.hex(), enlisted_save_sha256=digest(initial),
                enlist_run=enlist_stats, native_run=stats, native_context=context(ram),
                final_saved_pilot=(pilot / 'config').read_bytes().hex())
            if native_prefix:
                report.update(native_prefix=native_prefix, input_segment=input_segment)
                report['native_prefix_execution_exact'] = True
                report['native_prefix_observations_exact'] = prefix_rows
            if plan:
                assert anchors['keys_consumed'] == stats['replay_events']
                report.update(anchor_input_sha256=digest((args.out / 'input.anchors').read_bytes()),
                    anchor_report_sha256=digest((args.out / 'anchors.json').read_bytes()), event_report=anchors)
            for name in ('native.jsonl', 'native.dat'):
                data = (work / name).read_bytes()
                key = 'native_trace_sha256' if name.endswith('jsonl') else 'native_final_ram_sha256'
                report[key] = digest(data)
            (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    report['source_context'] = context(gzip.decompress((evidence / 'driver.dat.gz').read_bytes()))
    a, b = report['source_context'], report['native_context']
    if outcome == 'mission failure':
        assert a['player_phase'] in (0xfe, 2), 'original final RAM has no mission failure outcome'
    report['context_differences'] = {field: [a[field], b[field]] for field in
        ('qualified', 'saved_level', 'grades', 'completions', 'scene_level', 'player_phase') if a[field] != b[field]}
    stats = report['native_run']
    report['runtime_outcome_matches'] = (
        stats['postflight_resets'] == (1 if outcome == 'crash/reset' else 0) and
        (not success or (stats['screen'], stats['mode'], stats['stage']) == ('menu', 0, 'C0FCB4')))
    report['pilot_record_difference_offsets'] = [i for i, (x, y) in enumerate(
        zip(bytes.fromhex(a['pilot']), bytes.fromhex(b['pilot']))) if x != y]
    key = 'whole_successful_flight' if success else 'whole_failed_flight'
    try:
        menu_boundary = (prefix_record['canonical']['replay_iterations'], prefix_record['canonical']['frames']) if prefix_record else None
        comparison_mapping = event_mapping(plan, report['event_report'], mapping, args.out / 'native.jsonl.gz', menu_boundary) if plan else mapping
        report[key] = assess(evidence / 'driver.jsonl.gz', args.out / 'native.jsonl.gz', success, comparison_mapping, mode, outcome)
        report['comparison_rejected'] = None
    except AssertionError as error:
        report[key] = None
        report['comparison_rejected'] = str(error)
    report['comparison_completed'] = report[key] is not None
    report['mission_success'] = success
    report['native_mission_success'] = bool(b['qualified'] and b['grades'][mode] == 1 and
        b['completions'] == a['completions'] and report['runtime_outcome_matches'])
    report['scope'] = (f'Independently started pilots with no qualification or grades initially; complete mode-{mode} '
        + ('successful flight, grade and menu return. Complete gameplay-state parity requires '
           'every core and named game field to match at the declared execution alignment. '
           if success else f'failed flight through the first original {outcome} boundary. ')
        + 'No reference RAM supplied to native. Full pilot records differ in naming/date/history bytes. '
        'Strict drawing and clock differences remain unaccepted diagnostics.')
    if native_prefix:
        report['scope'] = (f'Independent mode-{mode} flight after ordinary qualification and native preceding missions. '
            'All native preceding controls and observations reproduce the verified complete earlier replay. '
            'Next-mission source controls begin at the actual next menu JSR with no held keys, '
            'retaining every later key edge and source PAL timestamp. No original RAM, constructed saved progress, '
            'fitted world offset or clock enters native gameplay. Full history/state parity remains a strict diagnostic.')
    if mode == 4:
        report['scope'] += (' Both games replay qualification and the complete preceding mission to earn '
            'their pilot progress normally before escort. The original prefix is independently verified; '
            'neither saved pilot fields nor gameplay RAM are synthesized.')
    if mapping:
        report['scope'] += (' Every source observation is compared at its proven original JSR/LINK call identity. '
            'All consumed key edges retain their order and source PAL timestamps; only the '
            'legacy recorder iteration is translated to the actual update call. No game state or clock is written.')
    if plan:
        report['scope'] += (' Source-declared C10C08 and C10D8A events own input segments. All native '
            'updates remain counted; flight observations retain their actual native indices and follow source JSR/LINK ordinals '
            'from the actual flight-start event; no state matching, frame search or fitted offset.')
    if not report['comparison_completed']:
        report['scope'] = ('Retained independent replay attempt; rejected before accepting flight parity: '
            + report['comparison_rejected'] + '. No fitted alignment or native gameplay change. '
            'The declared complete comparison has not run successfully.')
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    comparison = report[key]
    print(json.dumps({k: v for k, v in comparison.items() if k.endswith('matching') or k == 'compared'})
          if comparison else json.dumps(dict(comparison_rejected=report['comparison_rejected'])))
    for name, expected in hashes.items():
        assert digest((ROOT / name).read_bytes()) == expected, f'native input changed: {name}'
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
    assert report['comparison_completed'], f'comparison rejected: {report["comparison_rejected"]}; captures retained'
    assert report['runtime_outcome_matches'], 'native outcome differs; complete comparison retained'
    assert not report['context_differences'], 'earned-pilot context differs; complete comparison retained'
    assert comparison['strict_gameplay_matching'], 'whole-flight state parity remains unaccepted; see retained report'


if __name__ == '__main__':
    main()
