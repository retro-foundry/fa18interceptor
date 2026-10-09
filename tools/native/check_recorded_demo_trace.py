"""Trace the whole sealed demo recording from independent original/native starts.

The recording ends while flight remains active. Report that boundary honestly:
this expands independent-run evidence, not complete-flight qualification.
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
from compare_flight_traces import compare, compare_stage_events, read_trace, timer_events


def assess_flights(report, sr, nr):
    def next_stage(rows, first, stage):
        return next(i for i, r in rows.items() if i > first and r['fields']['stage'] == stage)
    sf, nf = report['source_first'], report['native_first']
    stop_s, stop_n = next_stage(sr, sf, '00c0f946'), next_stage(nr, nf, '00c0f946')
    assert stop_s - sf == stop_n - nf
    report['first_flight'] = compare(sr, nr, sf, nf, stop_s - sf)
    restart_s, restart_n = next_stage(sr, stop_s, '00c10d8a'), next_stage(nr, stop_n, '00c10d8a')
    report['flight_and_restart_events'] = compare_stage_events(sr, nr, sf, restart_s, nf, restart_n)
    count = max(sr) - restart_s + 1
    report['second_flight_prefix'] = compare(sr, nr, restart_s, restart_n, count)
    # A mutation inside an otherwise frozen wait must survive event compression.
    changed = dict(nr)
    altered = json.loads(json.dumps(nr[stop_n + 1]))
    altered['records'][15] = ('ff' if altered['records'][15][:2] != 'ff' else '00') + altered['records'][15][2:]
    changed[stop_n + 1] = altered
    rejected = compare_stage_events(sr, changed, sf, restart_s, nf, restart_n)
    assert rejected['matching_runs'] < len(rejected['runs']), 'lost a changed state during a wait'
    report['wait_state_mutation_rejected'] = True
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--assess-existing', action='store_true', help='reuse the already retained complete trace evidence')
    parser.add_argument('--source-evidence', type=Path,
                        help='reuse a sealed, hashed original trace while independently running this native build')
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    if args.assess_existing:
        report = json.loads((args.out / 'report.json').read_text())
        sh, sr = read_trace(args.out / 'source.jsonl.gz')
        nh, nr = read_trace(args.out / 'native.jsonl.gz')
        assert sh == nh
        for name in ('source', 'native'):
            assert hashlib.sha256(gzip.decompress((args.out / f'{name}.jsonl.gz').read_bytes())).hexdigest() == report[f'{name}_trace_sha256']
        assess_flights(report, sr, nr)
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
        print('Retained complete demo: flight boundaries, ordinal restart events and changed-wait-state rejection checked')
        return
    recording = ROOT / 'captures/native/demo01/input.fa18in'
    hashes = {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in (
        recording, recording.with_name('state.bin'), ROOT / 'local/media/fa18.adf', ROOT / 'local/system/kick13.rom')}
    env = {k: v for k, v in os.environ.items() if not k.startswith('FA18_LOOP_')}
    with tempfile.TemporaryDirectory(prefix='demo-flight-trace-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        source_path, native_path = work / 'source.jsonl', work / 'native.jsonl'
        if args.source_evidence:
            evidence = json.loads((args.source_evidence / 'report.json').read_text())
            assert evidence['input_hashes'] == hashes, 'reference input/media changed'
            source_data = gzip.decompress((args.source_evidence / 'source.jsonl.gz').read_bytes())
            assert hashlib.sha256(source_data).hexdigest() == evidence['source_trace_sha256']
            source_path.write_bytes(source_data)
            source_stats = evidence['source_run']
            source_ram_hash = evidence['source_final_ram_sha256']
            consumed = (args.source_evidence / 'consumed.fa18in').read_text()
        else:
            source = subprocess.run([str(ROOT / 'build/recomp/fa18_recomp.exe'),
                '--state', str(recording.with_name('state.bin')), '--rom', str(ROOT / 'local/system/kick13.rom'),
                '--ports', 'off', '--input', str(recording), '--to-end', '--frames', '40000',
                '--game-input-out', str(work / 'consumed.fa18in'), '--ram-out', str(work / 'source.dat')],
                cwd=ROOT, env=dict(env, FA18_LOOP_TRACE=str(source_path)),
                check=True, capture_output=True, text=True, timeout=180)
            source_stats = json.loads(source.stdout)
            source_ram_hash = hashlib.sha256((work / 'source.dat').read_bytes()).hexdigest()
            consumed = (work / 'consumed.fa18in').read_text()
        assert source_stats['iterations'] == 4892, source_stats
        (args.out / 'consumed.fa18in').write_text(consumed)
        # The established C10D8A flight transition supplies this alignment;
        # no frame/image search or original-state injection is used.
        source_first, native_first = 2180, 2143
        native_end = source_stats['iterations'] - (source_first - native_first)
        intro = work / 'intro.e9k'
        intro.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        native = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '30000',
            '--input', str(recording), '--iterations', str(native_end), '--replay', str(intro),
            '--save-dir', str(work / 'pilot'), '--flight-trace', str(native_path),
            '--data-out', str(work / 'native.dat')], cwd=ROOT, env=env,
            check=True, capture_output=True, text=True, timeout=90)
        native_stats = json.loads(native.stdout)
        assert native_stats['replay_iterations'] == native_end and native_stats['replay_events'] == 2
        assert not native_stats['cpu_emulation'] and not native_stats['chipset_emulation']
        sh, sr = read_trace(source_path)
        nh, nr = read_trace(native_path)
        assert sh == nh and len(sr) == 4892 and max(nr) == native_end
        assert sr[source_first]['fields']['stage'] == nr[native_first]['fields']['stage'] == '00c10d8a'
        count = 4892 - source_first + 1
        report = compare(sr, nr, source_first, native_first, count)
        report.update(source_run=source_stats, native_run=native_stats, input_hashes=hashes,
                      source_first=source_first, native_first=native_first,
                      source_events=timer_events(sr, source_first, count),
                      native_events=timer_events(nr, native_first, count),
                      acceptance='Whole sealed recorded demo interval; flight remains active, timed rendering assessment remains separate')
        assess_flights(report, sr, nr)
        for name, path in (('source', source_path), ('native', native_path)):
            data = path.read_bytes()
            (args.out / f'{name}.jsonl.gz').write_bytes(gzip.compress(data, mtime=0))
            report[f'{name}_trace_sha256'] = hashlib.sha256(data).hexdigest()
            report[f'{name}_final_ram_sha256'] = source_ram_hash if name == 'source' else hashlib.sha256((work / f'{name}.dat').read_bytes()).hexdigest()
        if args.source_evidence:
            report['source_evidence_reused'] = str(args.source_evidence.resolve())
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
        print(json.dumps({k: v for k, v in report.items() if k.endswith('matching') or k.startswith('first_') and k != 'first_differences'}))
    for path, digest in hashes.items():
        assert hashlib.sha256((ROOT / path).read_bytes()).hexdigest() == digest, 'sealed input/media changed'
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
