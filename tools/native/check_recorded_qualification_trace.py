"""Trace the sealed carrier qualification from independent original/native starts.

Retains compressed complete traces and reports, never passing raw RAM. The
native game receives consumed control edges, not original runtime state.
"""
import argparse
import gzip
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT, integer, span
from compare_flight_traces import compare, compare_stage_events, number, read_trace


def assess(report, source, native):
    def flight_start(rows):
        return next(i for i, r in rows.items() if number(r, 'mode') == 9 and
                    number(r, 'stage') == 0xC10D8A and number(r, 'game_tick') == 1)
    sf, nf = flight_start(source), flight_start(native)
    # The qualification's existing consumed-input alignment is zero. Require
    # the actual initialization transition to establish this, not an image fit.
    assert sf == nf, ('different qualification input origins', sf, nf)
    def next_stage(rows, first, stage):
        return next(i for i, r in rows.items() if i > first and number(r, 'stage') == stage)
    stop_s, stop_n = next_stage(source, sf, 0xC0F946), next_stage(native, nf, 0xC0F946)
    assert stop_s - sf == stop_n - nf, ('different first-flight lengths', stop_s, stop_n)
    report['flight_first'] = sf
    report['flight_stop'] = dict(source=stop_s, native=stop_n)
    report['complete_first_flight'] = compare(source, native, sf, nf, stop_s - sf)
    restart_s = next_stage(source, stop_s, 0xC0F992)
    restart_n = next_stage(native, stop_n, 0xC0F992)
    report['flight_result_and_restart'] = compare_stage_events(source, native,
        sf, restart_s, nf, restart_n)
    report['restart_callback'] = dict(source=restart_s, native=restart_n)
    # Preserve the full recording's fixed-offset failure diagnostic, even if
    # a wait changes iteration counts. Never select visually similar states.
    missing = [i for i in range(sf, max(source) + 1) if i not in native]
    report['native_observation_gaps'] = missing
    fixed_stop = missing[0] if missing else max(source) + 1
    report['fixed_offset_observed_prefix'] = compare(source, native, sf, nf, fixed_stop - sf)
    report['recording_end'] = {
        name: dict(iteration=max(rows), stage=rows[max(rows)]['fields']['stage'],
                   game_tick=number(rows[max(rows)], 'game_tick'))
        for name, rows in (('source', source), ('native', native))}
    report['drawing_differences'] = []
    for i in range(sf, stop_s):
        a, b = source[i], native[i]
        planes = [p for p in range(8) if a['pages'][p] != b['pages'][p]]
        if planes:
            report['drawing_differences'].append(dict(iteration=i, game_tick=number(a, 'game_tick'),
                planes=planes, source_message=bytes.fromhex(a['fields']['message_line']).decode('ascii'),
                native_message=bytes.fromhex(b['fields']['message_line']).decode('ascii'),
                changed_fields={k: [v, b['fields'][k]] for k, v in a['fields'].items()
                                if v != b['fields'][k]}))
    assert report['complete_first_flight']['record_cores_matching'] == 16 * (stop_s - sf)
    assert report['complete_first_flight']['camera_and_controls_matching'] == stop_s - sf
    events = report['flight_result_and_restart']
    assert events['matching_runs'] == len(events['runs'])
    changed = dict(native)
    mutation = json.loads(json.dumps(native[stop_n + 1]))
    core = bytearray.fromhex(mutation['records'][15])
    core[0] ^= 1
    mutation['records'][15] = core.hex()
    changed[stop_n + 1] = mutation
    rejected = compare_stage_events(source, changed, sf, restart_s, nf, restart_n)
    assert rejected['matching_runs'] < len(rejected['runs']), 'lost a changed state during a wait'
    report['wait_state_mutation_rejected'] = True
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--assess-existing', action='store_true')
    parser.add_argument('--source-evidence', type=Path,
                        help='reuse hashed original evidence; start this native build independently')
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    if args.assess_existing:
        report = json.loads((args.out / 'report.json').read_text())
        for path, digest in report['input_hashes'].items():
            assert hashlib.sha256((ROOT / path).read_bytes()).hexdigest() == digest, 'sealed input/media changed'
        assert hashlib.sha256((args.out / 'consumed.fa18in').read_bytes()).hexdigest() == report['consumed_input_sha256']
        rows = {}
        headers = {}
        for name in ('source', 'native'):
            path = args.out / f'{name}.jsonl.gz'
            assert hashlib.sha256(gzip.decompress(path.read_bytes())).hexdigest() == report[f'{name}_trace_sha256']
            headers[name], rows[name] = read_trace(path)
        assert headers['source'] == headers['native']
        assess(report, rows['source'], rows['native'])
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
        print('Retained independent qualification: full flight, result/restart and strict drawing assessed')
        return
    recording = ROOT / 'captures/native/qual_carrier_success/input.fa18in'
    seal = json.loads(recording.with_name('run.json').read_text())
    media = (recording, recording.with_name('state.bin'), ROOT / 'local/media/fa18.adf',
             ROOT / 'local/system/kick13.rom')
    hashes = {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in media}
    assert hashes[str(recording.relative_to(ROOT))] == seal['input_sha256']
    assert hashes[str(recording.with_name('state.bin').relative_to(ROOT))] == seal['start_state']['sha256']
    assert hashes[str((ROOT / 'local/system/kick13.rom').relative_to(ROOT))] == seal['rom_sha256']
    env = {k: v for k, v in os.environ.items() if not k.startswith('FA18_LOOP_')}
    with tempfile.TemporaryDirectory(prefix='qualification-trace-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        consumed = work / 'consumed.fa18in'
        source_path, native_path = work / 'source.jsonl', work / 'native.jsonl'
        evidence = None
        if args.source_evidence:
            evidence = json.loads((args.source_evidence / 'report.json').read_text())
            assert evidence['input_hashes'] == hashes, 'original input/media changed'
            data = gzip.decompress((args.source_evidence / 'source.jsonl.gz').read_bytes())
            assert hashlib.sha256(data).hexdigest() == evidence['source_trace_sha256']
            source_path.write_bytes(data)
            controls = (args.source_evidence / 'consumed.fa18in').read_bytes()
            assert hashlib.sha256(controls).hexdigest() == evidence['consumed_input_sha256']
            consumed.write_bytes(controls)
            source_stats = evidence['source_run']
        else:
            original = subprocess.run([str(ROOT / 'build/recomp/fa18_recomp.exe'),
                '--state', str(recording.with_name('state.bin')), '--rom', str(ROOT / 'local/system/kick13.rom'),
                '--ports', 'off', '--input', str(recording), '--to-end', '--frames', '40000',
                '--game-input-out', str(consumed), '--ram-out', str(work / 'source.dat')],
                cwd=ROOT, env=dict(env, FA18_LOOP_TRACE=str(source_path)),
                capture_output=True, text=True, check=True, timeout=180)
            source_stats = json.loads(original.stdout)
        assert source_stats['iterations'] == 8038, source_stats
        assert consumed.read_text().splitlines()[0] == 'FA18_GAME_INPUT_V1'
        # Reuse the actual original's consumed edges. Preserve all simultaneous
        # keys, delayed releases and source-defined queue semantics.
        assert len([line for line in consumed.read_text().splitlines() if ' K ' in line]) == 554
        warmup = work / 'intro.e9k'
        warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        result = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '50000',
            '--input', str(consumed), '--iterations', '8038', '--replay', str(warmup),
            '--save-dir', str(work / 'pilot'), '--flight-trace', str(native_path),
            '--data-out', str(work / 'native.dat')], cwd=ROOT, env=env,
            capture_output=True, text=True, check=True, timeout=120)
        native_stats = json.loads(result.stdout)
        assert native_stats['replay_iterations'] == 8038 and native_stats['replay_events'] == 554
        assert not native_stats['cpu_emulation'] and not native_stats['chipset_emulation']
        assert not native_stats['postflight_resets'] and not native_stats['input_queued']
        sh, sr = read_trace(source_path)
        nh, nr = read_trace(native_path)
        assert sh == nh and max(sr) == max(nr) == 8038
        report = dict(source_run=source_stats, native_run=native_stats, input_hashes=hashes,
                      consumed_input_sha256=hashlib.sha256(consumed.read_bytes()).hexdigest())
        for name, path in (('source', source_path), ('native', native_path)):
            data = path.read_bytes()
            (args.out / f'{name}.jsonl.gz').write_bytes(gzip.compress(data, mtime=0))
            report[f'{name}_trace_sha256'] = hashlib.sha256(data).hexdigest()
            if name == 'source' and evidence:
                report['source_final_ram_sha256'] = evidence['source_final_ram_sha256']
                report['source_pilot'] = evidence['source_pilot']
            else:
                ram = (work / f'{name}.dat').read_bytes()
                report[f'{name}_final_ram_sha256'] = hashlib.sha256(ram).hexdigest()
                pilot = span(ram, integer(ram, 0xC1AB74, 4), 78)
                assert pilot[:2] == b'\0\1', f'{name}: final pilot is not qualified'
                report[f'{name}_pilot'] = pilot.hex()
        saved = (work / 'pilot/config').read_bytes()
        assert saved.hex() == report['native_pilot'] and len(saved) == 78
        assert report['source_final_ram_sha256'] == seal['replay']['final_ram_sha256'], 'original sealed outcome changed'
        report['native_save_sha256'] = hashlib.sha256(saved).hexdigest()
        report['acceptance'] = ('Independent complete qualification flight, result and C0F992 restart callback. '
            'Both pilots are qualified at recording end; this does not establish newly earned qualification. '
            'Initial pilot contexts differ. Strict drawing/timing differences remain explicit; '
            'assess the six message differences with check_qualification_message_cadence.py. '
            'The source recording ends during second-flight setup. Native flight-only observation '
            'omits menu iteration 7453; fixed-offset comparison ends before that gap. '
            'Menu return, independent all-mission flights and full drawing acceptance remain separate.')
        if evidence:
            report['source_evidence_reused'] = str(args.source_evidence.resolve())
        # Publish complete evidence before assessment so a failed state or
        # callback assertion remains reviewable without repeating the flight.
        (args.out / 'consumed.fa18in').write_bytes(consumed.read_bytes())
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
        assess(report, sr, nr)
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
        summary = report['complete_first_flight']
        print(json.dumps({k: v for k, v in summary.items() if k != 'first_differences'}))
    for path, digest in hashes.items():
        assert hashlib.sha256((ROOT / path).read_bytes()).hexdigest() == digest, 'sealed media/input changed'
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
