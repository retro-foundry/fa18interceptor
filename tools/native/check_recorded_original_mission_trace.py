"""Compare recorded original mission-three flights from independent starts.

Enlist a native pilot using ordinary keys, then cold-load its actual saved
config. Original RAM never initializes the native game. Complete drawings and
timers remain strict diagnostics; a failed original flight is not mission success.
"""
import argparse
import copy
import gzip
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT, integer, span
from compare_flight_traces import compare, number, read_trace


def digest(data):
    return hashlib.sha256(data).hexdigest()


def context(ram):
    pilot = span(ram, integer(ram, 0xC1AB74, 4), 78)
    assert len(pilot) == 78
    return dict(pilot=pilot.hex(), qualified=int.from_bytes(pilot[:2], 'big'),
                saved_level=int.from_bytes(pilot[2:4], 'big'), grades=list(pilot[18:28]),
                completions=int.from_bytes(pilot[56:58], 'big'),
                scene_level=integer(ram, 0xC458A7, 1),
                player_phase=integer(ram, 0xC45798, 1))


def assess(source_path, native_path, success=False):
    sh, source = read_trace(source_path)
    nh, native = read_trace(native_path)
    assert sh == nh
    def first(rows):
        return next(i for i, r in rows.items() if number(r, 'mode') == 3 and
                    number(r, 'stage') == 0xC10D8A and number(r, 'game_tick') == 1)
    sf, nf = first(source), first(native)
    assert sf == nf, ('different initialization/input origins', sf, nf)
    def stop(rows, start):
        return next(i for i in range(start + 1, max(rows) + 1)
                    if number(rows[i], 'stage') == 0xC11788)
    if success:
        ss = next(i for i, r in source.items() if i > sf and number(r, 'mode') == 0) - 1
        ns = max(native)
        assert ss == ns, 'different final observed flight boundary'
    else:
        ss, ns = stop(source, sf), stop(native, nf)
        assert ss == ns == max(source) == max(native), 'different crash callback or recording end'
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
    report['native_menu_observation_gaps'] = [i for i in range(ss + 1, max(source) + 1) if i not in native]
    # Ensure the diagnostic observes an NPC byte, controls and a full page.
    report['mutation_rejections'] = {}
    for kind in ('npc_core', 'controls', 'page'):
        changed = dict(native)
        changed[nf] = copy.deepcopy(native[nf])
        row = changed[nf]
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
        rejected = compare(source, changed, sf, nf, count)
        assert rejected[field] == report[field] - 1, f'{kind} difference lost'
        report['mutation_rejections'][kind] = True
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, required=True)
    parser.add_argument('--source-evidence', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--assess-existing', action='store_true')
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    evidence = args.source_evidence
    original = json.loads((evidence / 'report.json').read_text())
    assert original['unmodified_replay_exact'], 'original recording has not reproduced independently'
    success = original['mission_success']
    assert original['original_outcome'] == ('success' if success else 'crash/reset')
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
        for name, key in (('native.jsonl.gz', 'native_trace_sha256'),
                          ('native.dat.gz', 'native_final_ram_sha256')):
            assert digest(gzip.decompress((args.out / name).read_bytes())) == report[key]
        assert context(gzip.decompress((args.out / 'native.dat.gz').read_bytes())) == report['native_context']
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
                assert result.returncode == 0, f'native run failed: {log_name}'
                return json.loads(result.stdout)
            enlist_stats = run(['--frames', '9000', '--replay', str(enlist),
                                '--save-dir', str(pilot)], 'enlist.log')
            initial = (pilot / 'config').read_bytes()
            assert len(initial) == 78 and initial[:4] == bytes(4)
            assert initial[18:28] == bytes(10) and initial[56:58] == bytes(2)
            warmup = work / 'intro.e9k'
            warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
            stats = run(['--frames', '100000', '--replay', str(warmup),
                '--input', str(evidence.resolve() / 'consumed.fa18in'),
                '--iterations', str(original['iterations']), '--save-dir', str(pilot),
                '--flight-trace', str(work / 'native.jsonl'), '--data-out', str(work / 'native.dat')], 'native.log')
            assert stats['replay_iterations'] == original['iterations']
            events = sum(' K ' in line for line in (evidence / 'consumed.fa18in').read_text().splitlines())
            assert stats['replay_events'] == events and stats['input_queued'] == 0
            assert stats['postflight_resets'] == (0 if success else 1)
            if success:
                assert stats['screen'] == 'menu' and stats['mode'] == 0 and stats['stage'] == 'C0FCB4'
            assert not stats['cpu_emulation'] and not stats['chipset_emulation']
            ram = (work / 'native.dat').read_bytes()
            assert len(ram) == 0x100000
            report = dict(native_input_hashes=hashes,
                source_trace_sha256=original['driver_trace_sha256'],
                consumed_input_sha256=original['consumed_input_sha256'],
                runner_sha256=digest(args.runner.read_bytes()),
                enlisted_pilot=initial.hex(), enlisted_save_sha256=digest(initial),
                enlist_run=enlist_stats, native_run=stats, native_context=context(ram),
                final_saved_pilot=(pilot / 'config').read_bytes().hex())
            for name in ('native.jsonl', 'native.dat'):
                data = (work / name).read_bytes()
                (args.out / (name + '.gz')).write_bytes(gzip.compress(data, mtime=0))
                key = 'native_trace_sha256' if name.endswith('jsonl') else 'native_final_ram_sha256'
                report[key] = digest(data)
            (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    report['source_context'] = context(gzip.decompress((evidence / 'driver.dat.gz').read_bytes()))
    a, b = report['source_context'], report['native_context']
    for field in ('qualified', 'saved_level', 'grades', 'completions', 'scene_level', 'player_phase'):
        assert a[field] == b[field], f'unmatched earned-pilot context: {field}'
    report['pilot_record_difference_offsets'] = [i for i, (x, y) in enumerate(
        zip(bytes.fromhex(a['pilot']), bytes.fromhex(b['pilot']))) if x != y]
    key = 'whole_successful_flight' if success else 'whole_failed_flight'
    report[key] = assess(evidence / 'driver.jsonl.gz', args.out / 'native.jsonl.gz', success)
    report['mission_success'] = success
    report['scope'] = ('Independently started, equally qualified zero-grade pilots; complete mission-three '
        + ('successful flight, grade and menu return. Raw fixed-offset state parity remains unaccepted '
           'if a source update repeats. ' if success else 'failed flight through the first C11788 crash/reset boundary. ')
        + 'No reference RAM supplied to native. Full pilot records differ in naming/date/history bytes. '
        'Strict drawing and clock differences remain unaccepted diagnostics.')
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    comparison = report[key]
    print(json.dumps({k: v for k, v in comparison.items() if k.endswith('matching') or k == 'compared'}))
    for name, expected in hashes.items():
        assert digest((ROOT / name).read_bytes()) == expected, f'native input changed: {name}'
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
    assert comparison['strict_gameplay_matching'], 'whole-flight state parity remains unaccepted; see retained report'


if __name__ == '__main__':
    main()
