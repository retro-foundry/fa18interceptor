"""Acquire optional message-owner fields while preserving entire flight evidence.

Both games start independently. Every old field, core, page and final RAM byte
must reproduce its accepted recording before the extra fields can be assessed.
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
from check_recorded_original_mission_trace import assess, verified_update_mapping
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


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, required=True)
    parser.add_argument('--source-evidence', type=Path, required=True)
    parser.add_argument('--native-evidence', type=Path, required=True)
    parser.add_argument('--source-updates', type=Path, required=True)
    parser.add_argument('--source-reuse', type=Path)
    parser.add_argument('--drawing-bands', action='store_true', help='also locate page differences with adjoining full-width bands')
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    source = json.loads((args.source_evidence / 'report.json').read_text())
    native = json.loads((args.native_evidence / 'report.json').read_text())
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
    with tempfile.TemporaryDirectory(prefix='mission-message-trace-', dir=ROOT / 'build') as directory:
        work = Path(directory)
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
            with (args.out / 'source.log').open('w') as log:
                result = subprocess.run([str(executable), '--state',
                    str(ROOT / 'captures/native/qual_carrier_success/state.bin'), '--rom',
                    str(ROOT / 'local/system/kick13.rom'), '--ports', 'off', '--input',
                    str(input_path.resolve()), '--frames', end[2], '--ram-out', str(ram)],
                    cwd=ROOT, env=dict(env, FA18_LOOP_TRACE=str(trace)),
                    stdout=log, stderr=subprocess.STDOUT, timeout=360)
            assert result.returncode == 0
            source_run, = [json.loads(line) for line in (args.out / 'source.log').read_text().splitlines() if line.startswith('{')]
            previous_run, = [json.loads(line) for line in (args.source_evidence / 'replay.log').read_text().splitlines() if line.startswith('{')]
            assert source_run == previous_run
            source_ram_sha = digest(ram.read_bytes())
            source_runner_sha = digest(executable.read_bytes())
            source_trace.write_bytes(gzip.compress(trace.read_bytes(), mtime=0))
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
                      '--save-dir', str(pilot)], 'enlist.log')
        assert enlist == native['enlist_run'] and (pilot / 'config').read_bytes().hex() == native['enlisted_pilot']
        intro = work / 'intro.e9k'
        intro.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        trace, ram = work / 'native.jsonl', work / 'native.dat'
        native_run = run(['--frames', '100000', '--replay', str(intro), '--input',
            str((args.source_updates / 'update-consumed.fa18in').resolve()), '--iterations',
            str(updates['real_update_calls']), '--save-dir', str(pilot),
            '--flight-trace', str(trace), '--data-out', str(ram)], 'native.log')
        assert native_run == native['native_run']
        native_ram_sha = digest(ram.read_bytes())
        assert native_ram_sha == native['native_final_ram_sha256']
        assert (pilot / 'config').read_bytes().hex() == native['final_saved_pilot']
        native_trace = args.out / 'native.jsonl.gz'
        native_trace.write_bytes(gzip.compress(trace.read_bytes(), mtime=0))
        native_rows = preserved(native_trace, args.native_evidence / 'native.jsonl.gz')
        comparison = assess(source_trace, native_trace, True, mapping)
        assert comparison['strict_gameplay_matching'] and comparison['complete_pages_matching'] == 287
        report = dict(runner_sha256=digest(args.runner.read_bytes()), source_runner_sha256=source_runner_sha,
            baseline_source_trace_sha256=source['driver_trace_sha256'], baseline_native_trace_sha256=native['native_trace_sha256'],
            source_trace_sha256=digest(gzip.decompress(source_trace.read_bytes())),
            native_trace_sha256=digest(gzip.decompress(native_trace.read_bytes())),
            source_final_ram_sha256=source_ram_sha, native_final_ram_sha256=native_ram_sha,
            source_run=source_run, native_run=native_run, source_update_evidence=updates,
            source_rows_preserved=source_rows, native_rows_preserved=native_rows,
            final_saved_pilot=native['final_saved_pilot'], input_hashes={**source['input_hashes'], **native['native_input_hashes']},
            whole_successful_flight=comparison,
            scope='Optional message fields leave every preceding trace field, complete core, page, final RAM byte, '
                  'counter, outcome and save unchanged. Independently started full mission-three flight; '
                  'strict drawing remains open. Message timing is assessed separately.')
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
        print(f'{source_rows} original and {native_rows} native observations preserved; '
              'complete final RAM and saves unchanged; optional owner inputs now recorded')
    subprocess.run([sys.executable, 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
