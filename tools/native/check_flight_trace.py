"""Exercise actual native/reference trace callers and verify read-only evidence."""
import argparse
import gzip
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT, integer, span
from compare_flight_traces import compare, read_trace, timer_events


def verify_ram(header, row, data):
    assert row['records'] == [span(data, 0xC46184 + i * 512, 164).hex() for i in range(16)]
    assert row['fields'] == {f['name']: span(data, f['address'], f['size']).hex() for f in header['fields']}
    assert row['pages_valid']
    draw = integer(data, 0xC4566C, 2)
    assert row['draw_page'] == draw
    assert row['page_table'] == integer(data, 0xC456B6, 4) == 0xC4566E + 16 * draw
    assert row['pages'] == [hashlib.sha256(span(data,
        integer(data, 0xC4566E + 16 * (draw ^ role) + 4 * plane, 4), 8000)).hexdigest()
        for role in range(2) for plane in range(4)]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    subprocess.run(['python', 'scripts/build_recomp.py'], cwd=ROOT, check=True)
    reference = ROOT / 'build/recomp/fa18_recomp.exe'
    recording = ROOT / 'captures/native/demo01/input.fa18in'
    seal = hashlib.sha256(recording.read_bytes()).hexdigest()
    env = {k: v for k, v in os.environ.items() if not k.startswith('FA18_LOOP_')}
    with tempfile.TemporaryDirectory(prefix='flight-trace-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        source_trace, native_trace = work / 'source.jsonl', work / 'native.jsonl'
        base = [str(reference), '--state', str(recording.with_name('state.bin')),
                '--rom', str(ROOT / 'local/system/kick13.rom'), '--ports', 'off',
                '--input', str(recording), '--frames', '6000', '--ram-out', str(work / 'source.dat')]
        original_runs, original_ram = [], []
        for enabled in (False, True):
            source_env = dict(env, FA18_LOOP_DUMP=f'2452:{work / "source-checkpoint.dat"}')
            if enabled:
                source_env['FA18_LOOP_TRACE'] = str(source_trace)
            result = subprocess.run(base, cwd=ROOT, env=source_env, check=True,
                                    capture_output=True, text=True, timeout=60)
            original_runs.append(json.loads(result.stdout))
            original_ram.append((work / 'source.dat').read_bytes())
        assert original_runs[0] == original_runs[1] and original_ram[0] == original_ram[1], 'trace changed original execution'
        source_header, source_rows = read_trace(source_trace)
        assert len(source_rows) == original_runs[1]['iterations'] == 2763
        verify_ram(source_header, source_rows[2452], (work / 'source-checkpoint.dat').read_bytes())
        intro = work / 'intro.e9k'
        intro.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        base = [str(args.runner.resolve()), '--headless', '--frames', '20000',
                '--input', str(recording), '--iterations', '2727', '--replay', str(intro),
                '--save-dir', str(work / 'pilot'), '--data-out', str(work / 'native.dat'),
                '--frame-capture', '2415', str(work / 'native-checkpoint'), '--frame-capture-entry-only']
        native_runs, native_ram = [], []
        for enabled in (False, True):
            result = subprocess.run(base + (['--flight-trace', str(native_trace)] if enabled else []),
                                    cwd=ROOT, env=env, check=True, capture_output=True, text=True, timeout=60)
            native_runs.append(json.loads(result.stdout))
            native_ram.append((work / 'native.dat').read_bytes())
        assert native_runs[0] == native_runs[1] and native_ram[0] == native_ram[1], 'trace changed native execution/capture'
        native_header, native_rows = read_trace(native_trace)
        assert source_header == native_header
        assert max(native_rows) == 2727
        verify_ram(native_header, native_rows[2415], (work / 'native-checkpoint.entry.dat').read_bytes())
        report = compare(source_rows, native_rows, 2401, 2364, 363)
        assert report['record_cores_matching'] == 5808
        assert report['complete_record_boundaries_matching'] == report['camera_and_controls_matching'] == 363
        assert report['complete_pages_matching'] == 83, report
        report.update(source_events=timer_events(source_rows, 2401, 363),
                      native_events=timer_events(native_rows, 2364, 363))
        # Truncation must never become accepted evidence.
        truncated = work / 'truncated.jsonl'
        truncated.write_bytes(native_trace.read_bytes().rsplit(b'\n', 2)[0] + b'\n')
        try:
            read_trace(truncated)
        except AssertionError as error:
            assert 'completion footer' in str(error)
        else:
            raise AssertionError('accepted a truncated trace')
        # Native RAM captures and the trace share one diagnostic byte budget.
        rejected = subprocess.run(base + ['--flight-trace', str(work / 'budget.jsonl'),
            '--capture-budget-mib', '1'], cwd=ROOT, env=env, capture_output=True, text=True, timeout=15)
        assert rejected.returncode == 1 and 'exceeds capture budget' in rejected.stderr
        for extra in (['--no-recomp'], ['--ports', 'on']):
            invalid = subprocess.run([str(reference), '--state', str(recording.with_name('state.bin')),
                '--rom', str(ROOT / 'local/system/kick13.rom'), '--frames', '1', '--ports', 'off'] + extra,
                cwd=ROOT, env=dict(env, FA18_LOOP_TRACE=str(work / 'invalid.jsonl')),
                capture_output=True, text=True, timeout=10)
            assert invalid.returncode == 2 and 'requires translated main-loop boundaries and --ports off' in invalid.stderr
            assert not (work / 'invalid.jsonl').exists()
        # Retain compact trace evidence and reports; discard all passing raw RAM.
        for name, path in (('source', source_trace), ('native', native_trace)):
            data = path.read_bytes()
            (args.out / f'{name}.jsonl.gz').write_bytes(gzip.compress(data, mtime=0))
            report[f'{name}_trace_sha256'] = hashlib.sha256(data).hexdigest()
        report.update(source_run=original_runs[1], native_run=native_runs[1],
                      source_final_ram_sha256=hashlib.sha256(original_ram[1]).hexdigest(),
                      native_final_ram_sha256=hashlib.sha256(native_ram[1]).hexdigest(),
                      source_trace_rows=len(source_rows), native_trace_rows=len(native_rows),
                      read_only=True, ram_cross_checks=['source2452', 'native2415'],
                      acceptance='Trace integrity and read-only integration; full-flight/timed-HUD acceptance remains separate')
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    assert hashlib.sha256(recording.read_bytes()).hexdigest() == seal
    print('Both live trace callers preserve RAM/counters; complete cores and full-page hashes reproduce the 363-update comparison')


if __name__ == '__main__':
    main()
