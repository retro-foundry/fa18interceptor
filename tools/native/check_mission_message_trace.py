"""Acquire optional message-owner fields while preserving entire flight evidence.

Both games start independently. Every old field, core, page and final RAM byte
must reproduce its accepted recording before the extra fields can be assessed.
An explicit PCM-processing change may alter only the nonzero-sample counter;
all game/voice counters remain strict, and both loops must pass the heap guard.
"""
import argparse
import gzip
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile

from check_gameplay_checkpoint import ROOT
from check_qualification_message_cadence import digest
from check_recorded_original_mission_trace import (assess, event_mapping,
    source_input_segment, verified_native_prefix, verified_update_mapping)
from compare_flight_traces import read_trace


def preserved(path, baseline):
    header, rows = read_trace(path)
    old_header, old = read_trace(baseline)
    assert {k: v for k, v in header.items() if k not in ('fields', 'drawing_bands')} == {
        k: v for k, v in old_header.items() if k not in ('fields', 'drawing_bands')}
    if 'drawing_bands' in old_header:
        assert header['drawing_bands'] == old_header['drawing_bands']
    assert header['fields'][:len(old_header['fields'])] == old_header['fields']
    assert rows.keys() == old.keys()
    for i, row in rows.items():
        prior = old[i]
        for key in ('frame', 'records', 'pages', 'pages_valid', 'draw_page', 'page_table', 'width', 'height'):
            assert row[key] == prior[key], (i, key)
        assert all(row['fields'][name] == value for name, value in prior['fields'].items()), i
        if 'drawing_bands' in prior:
            assert row['drawing_bands'] == prior['drawing_bands'], i
    return len(rows)


def preserved_counters(before, after, pcm_change=False):
    """PCM processing can change zero crossings, never game/voice counters."""
    if not pcm_change:
        assert before == after
        return None
    assert before.keys() == after.keys()
    assert {k: v for k, v in before.items() if k != 'nonzero_sample_frames'} == {
        k: v for k, v in after.items() if k != 'nonzero_sample_frames'}
    assert 0 <= after['nonzero_sample_frames'] <= after['sample_frames']
    return dict(before=before['nonzero_sample_frames'], after=after['nonzero_sample_frames'])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, required=True)
    parser.add_argument('--source-evidence', type=Path, required=True)
    parser.add_argument('--native-evidence', type=Path, required=True)
    parser.add_argument('--source-updates', type=Path, required=True)
    parser.add_argument('--source-reuse', type=Path)
    parser.add_argument('--native-prefix', type=Path,
        help='mission five: verified ordinary native qualification/patrol/escort prefix')
    parser.add_argument('--drawing-bands', action='store_true', help='also locate page differences with adjoining full-width bands')
    parser.add_argument('--pcm-change', action='store_true',
        help='Explicit original-derived PCM change: preserve every game/voice counter, recording the changed nonzero-sample count')
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    source = json.loads((args.source_evidence / 'report.json').read_text())
    native = json.loads((args.native_evidence / 'report.json').read_text())
    mode = source.get('mission_mode', 3)
    assert mode in (3, 5), 'only established complete gameplay comparisons are supported'
    assert (mode == 5) == bool(args.native_prefix), 'mission five requires the verified native prefix'
    prefix_record = None
    if args.native_prefix:
        _, prefix = verified_native_prefix(args.native_prefix, args.runner)
        assert prefix == native['native_prefix'] and native['native_prefix_execution_exact']
        prefix_record = json.loads((args.native_prefix / 'report.json').read_text())
        mapping_start = native['input_segment']['first_source_observation']
        assert native['input_segment']['ordinary_native_prefix_replayed']
    assert source['mission_success'] and native['whole_successful_flight']['strict_gameplay_matching']
    for relative, expected in {**source['input_hashes'], **native['native_input_hashes']}.items():
        assert digest((ROOT / relative).read_bytes()) == expected
    for path, expected in ((args.source_evidence / 'driver.jsonl.gz', source['driver_trace_sha256']),
                           (args.native_evidence / 'native.jsonl.gz', native['native_trace_sha256'])):
        assert digest(gzip.decompress(path.read_bytes())) == expected
    mapping, updates = verified_update_mapping(args.source_updates,
        args.source_evidence / 'driver.jsonl.gz', source)
    env = {k: v for k, v in os.environ.items() if not k.startswith(
        ('FA18_LOOP_', 'FA18_ORIGINAL_', 'FA18_BOUNDARY_', 'FA18_UPDATE_ENTRY_', 'FA18_TRACE_'))}
    env['FA18_TRACE_MESSAGE_FIELDS'] = '1'
    if args.drawing_bands:
        env['FA18_TRACE_DRAWING_BANDS'] = '1'
    # A full later-mission source prefix exceeds the existing 512 MiB budget
    # with band hashes. Optional message fields alone remain within that bound.
    assert not (mode == 5 and args.drawing_bands), 'mission-five full-prefix band capture exceeds the default budget; use bounded drawing-owner captures'
    executed_tool_sha = digest(Path(__file__).read_bytes())
    with tempfile.TemporaryDirectory(prefix='mission-message-trace-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        def retain_failure(reason):
            retained = {}
            for path in work.glob('*'):
                if path.is_file() and path.suffix in ('.jsonl', '.dat', '.fa18in'):
                    data = path.read_bytes()
                    (args.out / (path.name + '.partial.gz')).write_bytes(gzip.compress(data, mtime=0))
                    retained[path.name] = dict(bytes=len(data), sha256=digest(data))
            (args.out / 'failure.json').write_text(json.dumps(dict(accepted_evidence=False,
                reason=reason, mission_mode=mode, executed_tool_sha256=executed_tool_sha,
                retained=retained), indent=2) + '\n')
            subprocess.run([sys.executable, 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
        source_trace = args.out / 'source.jsonl.gz'
        if args.source_reuse:
            reused = json.loads((args.source_reuse / 'report.json').read_text())
            assert reused['baseline_source_trace_sha256'] == source['driver_trace_sha256']
            data = gzip.decompress((args.source_reuse / 'source.jsonl.gz').read_bytes())
            assert digest(data) == reused['source_trace_sha256']
            source_trace.write_bytes(gzip.compress(data, mtime=0))
            source_run = reused['source_run']
            source_ram_sha = reused['source_final_ram_sha256']
            source_runner_sha = reused['source_runner_sha256']
        else:
            input_path = args.source_evidence / 'input.fa18in'
            assert digest(input_path.read_bytes()) == source['generated_input_sha256']
            end = input_path.read_text().splitlines()[-1].split()
            assert end[0] == 'end'
            trace, ram = work / 'source.jsonl', work / 'source.dat'
            executable = ROOT / 'build/recomp/fa18_recomp.exe'
            source_args = [str(executable), '--state',
                    str(ROOT / 'captures/native/qual_carrier_success/state.bin'), '--rom',
                    str(ROOT / 'local/system/kick13.rom'), '--ports', 'off', '--input',
                    str(input_path.resolve()), '--frames', end[2], '--ram-out', str(ram)]
            if mode == 5:
                source_args += ['--to-end', '--game-input-out', str(work / 'source-consumed.fa18in')]
            try:
                with (args.out / 'source.log').open('w') as log:
                    result = subprocess.run(source_args,
                        cwd=ROOT, env=dict(env, FA18_LOOP_TRACE=str(trace)),
                        stdout=log, stderr=subprocess.STDOUT, timeout=900 if mode == 5 else 360)
            except subprocess.TimeoutExpired:
                retain_failure('Original optional-field replay timed out')
                raise
            if result.returncode:
                retain_failure('Original optional-field replay failed')
            assert result.returncode == 0
            if mode == 5:
                assert digest((work / 'source-consumed.fa18in').read_bytes()) == source['consumed_input_sha256']
            source_run, = [json.loads(line) for line in (args.out / 'source.log').read_text().splitlines() if line.startswith('{')]
            previous_run, = [json.loads(line) for line in (args.source_evidence / 'replay.log').read_text().splitlines() if line.startswith('{')]
            assert source_run == previous_run
            source_ram_sha = digest(ram.read_bytes())
            source_runner_sha = digest(executable.read_bytes())
            source_trace.write_bytes(gzip.compress(trace.read_bytes(), mtime=0))
            trace.unlink()
            ram.unlink()
        assert source_ram_sha == source['driver_final_ram_sha256']
        source_rows = preserved(source_trace, args.source_evidence / 'driver.jsonl.gz')
        pilot = work / 'pilot'
        def run(arguments, name):
            result = subprocess.run([str(args.runner.resolve()), '--headless', *arguments],
                cwd=ROOT, env=env, capture_output=True, text=True, timeout=150)
            (args.out / name).write_text(result.stdout + result.stderr)
            assert result.returncode == 0, name
            return json.loads(result.stdout)
        enlist = run(['--frames', '9000', '--replay', str(ROOT / 'tools/native/fixtures/region-pilot-enlist.e9k'),
                      '--save-dir', str(pilot), '--memory-report', str(work / 'enlist-memory.json')], 'enlist.log')
        enlist_pcm = preserved_counters(native['enlist_run'], enlist, args.pcm_change)
        assert (pilot / 'config').read_bytes().hex() == native['enlisted_pilot']
        intro = work / 'intro.e9k'
        intro.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        input_path = args.source_updates / 'update-consumed.fa18in'
        iterations = updates['real_update_calls']
        anchor_args = []
        if args.native_prefix:
            intro = args.native_prefix / 'physical.e9k'
            input_path = args.native_evidence / 'input.segment.fa18in'
            assert digest(input_path.read_bytes()) == native['replay_input_sha256']
            iterations = native['input_segment']['replay_source_end']
            anchor_path = args.native_evidence / 'input.anchors'
            assert digest(anchor_path.read_bytes()) == native['anchor_input_sha256']
            anchor_args = ['--input-anchors', str(anchor_path.resolve()),
                '--input-anchors-out', str((args.out / 'anchors.json').resolve())]
        trace, ram = work / 'native.jsonl', work / 'native.dat'
        native_run = run(['--frames', '180000' if mode == 5 else '100000', '--replay', str(intro), '--input',
            str(input_path.resolve()), '--iterations', str(iterations), '--save-dir', str(pilot),
            '--flight-trace', str(trace), '--data-out', str(ram),
            '--memory-report', str(work / 'flight-memory.json'), *anchor_args], 'native.log')
        flight_pcm = preserved_counters(native['native_run'], native_run, args.pcm_change)
        memory = {name: json.loads((work / f'{name}-memory.json').read_text()) for name in ('enlist', 'flight')}
        for value in memory.values():
            assert value['project_gameplay_heap_violations'] == value['sdl_failures'] == 0, value
        native_ram_sha = digest(ram.read_bytes())
        assert native_ram_sha == native['native_final_ram_sha256']
        assert (pilot / 'config').read_bytes().hex() == native['final_saved_pilot']
        native_trace = args.out / 'native.jsonl.gz'
        native_trace.write_bytes(gzip.compress(trace.read_bytes(), mtime=0))
        native_rows = preserved(native_trace, args.native_evidence / 'native.jsonl.gz')
        aligned = mapping
        anchors = None
        if args.native_prefix:
            mapping, _, _ = source_input_segment(mapping, (args.source_updates / 'update-consumed.fa18in').read_bytes(), mapping_start)
            mapping = {i: u + native['input_segment']['native_prefix_source_end'] for i, u in mapping.items()}
            anchors = json.loads((args.out / 'anchors.json').read_text())
            assert anchors == native['event_report'], 'optional fields changed replay event ownership'
            boundary = (prefix_record['canonical']['replay_iterations'], prefix_record['canonical']['frames'])
            aligned = event_mapping(native['event_plan'], anchors, mapping, native_trace, boundary)
        comparison = assess(source_trace, native_trace, True, aligned, mode)
        assert comparison['strict_gameplay_matching']
        assert all(comparison[k] == native['whole_successful_flight'][k] for k in
            ('compared', 'complete_record_boundaries_matching', 'camera_and_controls_matching', 'complete_pages_matching'))
        report = dict(runner_sha256=digest(args.runner.read_bytes()), source_runner_sha256=source_runner_sha,
            mission_mode=mode, executed_tool_sha256=executed_tool_sha,
            baseline_source_trace_sha256=source['driver_trace_sha256'], baseline_native_trace_sha256=native['native_trace_sha256'],
            source_trace_sha256=digest(gzip.decompress(source_trace.read_bytes())),
            native_trace_sha256=digest(gzip.decompress(native_trace.read_bytes())),
            source_final_ram_sha256=source_ram_sha, native_final_ram_sha256=native_ram_sha,
            source_run=source_run, native_run=native_run, enlist_run=enlist, memory=memory,
            source_update_evidence=updates,
            source_rows_preserved=source_rows, native_rows_preserved=native_rows,
            final_saved_pilot=native['final_saved_pilot'], input_hashes={**source['input_hashes'], **native['native_input_hashes']},
            whole_successful_flight=comparison,
            scope='Optional message fields leave every preceding trace field, complete core, page, final RAM byte, '
                  f'counter, outcome and save unchanged. Independently started full mode-{mode} flight; '
                  'strict drawing remains open. Message timing is assessed separately.')
        if args.native_prefix:
            report.update(native_prefix=prefix, event_plan=native['event_plan'], event_report=anchors,
                          input_segment=native['input_segment'])
        if args.pcm_change:
            report['pcm_change'] = dict(enlist=enlist_pcm, flight=flight_pcm,
                scope='Explicit PCM processing change; only nonzero_sample_frames may differ. '
                      'All game/voice counters, complete traces, final RAM and saves remain strict.')
            report['scope'] = report['scope'].replace('counter, outcome', 'game/voice counter, outcome')
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
        print(f'{source_rows} original and {native_rows} native observations preserved; '
              'complete final RAM and saves unchanged; optional owner inputs now recorded')
    subprocess.run([sys.executable, 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
