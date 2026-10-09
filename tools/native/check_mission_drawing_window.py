"""Inspect complete original/native pages at a bounded independent flight window.

All captured fields, sixteen complete cores and eight page hashes must reproduce
the accepted flight traces. No pixels are excluded or rewritten for comparison.
"""
import argparse
import gzip
import json
import os
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT, integer
from check_qualification_message_cadence import digest, pages, verify_trace, FIELDS
from check_recorded_original_mission_trace import verified_update_mapping
from compare_flight_traces import read_trace


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, required=True)
    parser.add_argument('--source-evidence', type=Path, required=True)
    parser.add_argument('--source-updates', type=Path, required=True)
    parser.add_argument('--native-evidence', type=Path, required=True)
    parser.add_argument('--first', type=int, required=True)
    parser.add_argument('--count', type=int, default=4)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--body-capture', action='store_true', help='retain bounded before/after fixtures for original owners')
    args = parser.parse_args()
    assert 1 <= args.count <= 128
    args.out.mkdir(parents=True, exist_ok=True)
    original = json.loads((args.source_evidence / 'report.json').read_text())
    native = json.loads((args.native_evidence / 'report.json').read_text())
    assert native['whole_successful_flight']['strict_gameplay_matching']
    mapping, updates = verified_update_mapping(args.source_updates,
        args.source_evidence / 'driver.jsonl.gz', original)
    assert native['source_update_evidence'] == updates
    assert digest(args.runner.read_bytes()) == native['runner_sha256']
    source_path, native_path = (args.source_evidence / 'driver.jsonl.gz', args.native_evidence / 'native.jsonl.gz')
    assert digest(gzip.decompress(source_path.read_bytes())) == original['driver_trace_sha256']
    assert digest(gzip.decompress(native_path.read_bytes())) == native['native_trace_sha256']
    sh, source = read_trace(source_path)
    nh, host = read_trace(native_path)
    assert sh == nh
    first, last = args.first, args.first + args.count - 1
    nf, nl = mapping[first], mapping[last]
    assert all(i in source and mapping[i] in host for i in range(first, last + 1))
    for path, expected in {**original['input_hashes'], **native['native_input_hashes']}.items():
        assert digest((ROOT / path).read_bytes()) == expected
    env = {k: v for k, v in os.environ.items() if not k.startswith(
        ('FA18_LOOP_', 'FA18_BOUNDARY_', 'FA18_UPDATE_ENTRY_', 'FA18_ORIGINAL_PILOT_'))}
    rows = []
    with tempfile.TemporaryDirectory(prefix='mission-drawing-window-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        retained = [args.out / f'source.{i}.dat.gz' for i in range(first, last + 1)]
        if not all(p.exists() for p in retained):
            with (args.out / 'source.log').open('w') as log:
                result = subprocess.run([str(ROOT / 'build/recomp/fa18_recomp.exe'), '--state',
                    str(ROOT / 'captures/native/qual_carrier_success/state.bin'), '--rom',
                    str(ROOT / 'local/system/kick13.rom'), '--ports', 'off', '--input',
                    str((args.source_evidence / 'input.fa18in').resolve()), '--frames',
                    str(source[last]['frame'] + 1)], cwd=ROOT,
                    env=dict(env, FA18_LOOP_DUMP=f'{first}+{args.count}:{work / "source"}'),
                    stdout=log, stderr=subprocess.STDOUT, timeout=300)
            assert result.returncode == 0
            for i, path in zip(range(first, last + 1), retained):
                data = (work / f'source.{i}.dat').read_bytes() if args.count > 1 else (work / 'source').read_bytes()
                verify_trace(data, sh, source[i])
                path.write_bytes(gzip.compress(data, mtime=0))
            subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
        pilot = work / 'pilot'
        def run(arguments, name):
            result = subprocess.run([str(args.runner.resolve()), '--headless', *arguments],
                cwd=ROOT, env=env, capture_output=True, text=True, timeout=120)
            (args.out / name).write_text(result.stdout + result.stderr)
            assert result.returncode == 0, name
            return json.loads(result.stdout)
        run(['--frames', '9000', '--replay', str(ROOT / 'tools/native/fixtures/region-pilot-enlist.e9k'),
             '--save-dir', str(pilot)], 'enlist.log')
        assert (pilot / 'config').read_bytes().hex() == native['enlisted_pilot']
        intro = work / 'intro.e9k'
        intro.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        stats = run(['--frames', '100000', '--replay', str(intro), '--input',
            str((args.source_updates / 'update-consumed.fa18in').resolve()),
            '--iterations', str(nl + 1), '--save-dir', str(pilot), '--frame-capture',
            f'{nf}+{nl - nf + 1}', str(work / 'native'),
            *([] if args.body_capture else ['--frame-capture-entry-only'])], 'native.log')
        assert stats['frame_capture_complete'] and not stats['cpu_emulation'] and not stats['chipset_emulation']
        for i, path in zip(range(first, last + 1), retained):
            a = gzip.decompress(path.read_bytes())
            j = mapping[i]
            b = (work / (f'native.{j}.entry.dat' if nl != nf else 'native.entry.dat')).read_bytes()
            verify_trace(a, sh, source[i])
            verify_trace(b, nh, host[j])
            differences = []
            for plane, (av, bv) in enumerate(zip(pages(a), pages(b))):
                changed = [dict(byte=k, x=k % 40 * 8, y=k // 40, source=x, native=y)
                           for k, (x, y) in enumerate(zip(av, bv)) if x != y]
                if changed:
                    differences.append(dict(plane=plane, bytes=len(changed),
                        bit_pixels=sum((p['source'] ^ p['native']).bit_count() for p in changed),
                        byte_bbox=[min(p['x'] for p in changed), min(p['y'] for p in changed),
                                   max(p['x'] for p in changed) + 7, max(p['y'] for p in changed)],
                        changes=changed))
            row = dict(source_iteration=i, native_iteration=j,
                ram_sha256=dict(source=digest(a), native=digest(b)), page_differences=differences,
                state={name: {k: integer(data, address, size) for k, (address, size) in FIELDS.items()}
                       for name, data in (('source', a), ('native', b))},
                changed_trace_fields={k: [v, host[j]['fields'][k]] for k, v in source[i]['fields'].items()
                                      if v != host[j]['fields'][k]})
            if differences:
                (args.out / f'native.{j}.dat.gz').write_bytes(gzip.compress(b, mtime=0))
            if args.body_capture:
                for suffix in ('before', 'after'):
                    data = (work / (f'native.{j}.{suffix}.dat' if nl != nf else f'native.{suffix}.dat')).read_bytes()
                    (args.out / f'native.{j}.{suffix}.dat.gz').write_bytes(gzip.compress(data, mtime=0))
                row['body_timing'] = (json.loads((work / f'native.{j}.timing.json').read_text()) if nl != nf else
                    {name: stats['frame_' + name] for name in ('before_tick', 'after_tick', 'saved_tick')})
            rows.append(row)
    report = dict(source_trace_sha256=original['driver_trace_sha256'],
        native_trace_sha256=native['native_trace_sha256'], source_update_mapping_sha256=updates['mapping_sha256'],
        runner_sha256=native['runner_sha256'], first=first, last=last, rows=rows,
        scope='Every captured field, core and complete page reproduces its live trace. '
              'All changed bytes are retained; this localizes differences without accepting them.')
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    for row in rows:
        print(row['source_iteration'], [(p['plane'], p['bytes'], p['byte_bbox']) for p in row['page_differences']])
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
