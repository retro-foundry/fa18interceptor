"""Assess the six qualification message differences without excluding pixels.

Original RAM is retained compressed; native starts independently from disk and
consumed controls. Passing native RAM is temporary. All changed drawing bytes,
source/native message state, live trace agreement and owner oracles are reported.
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
from compare_flight_traces import read_trace


FIELDS = {
    'notification_countdown': (0xC45890, 1), 'notification_code': (0xC4588E, 1),
    'message_countdown': (0xC45892, 1), 'message_time': (0xC45893, 1),
    'message_code': (0xC45AE0, 2), 'message_loaded': (0xC45AE2, 2),
    'message_shown': (0xC45ADE, 2), 'message_drawn': (0xC45AE4, 2),
    'message_state': (0xC458CE, 2), 'post_input_event': (0xC457AE, 1),
    'post_input_aux': (0xC45795, 1),
    'cockpit_flags': (0xC458CC, 2), 'player_phase': (0xC45798, 1),
    'warning_causes': (0xC45B50, 4),
}
WINDOWS = ((4690, 4), (5527, 8), (6229, 71))


def digest(data):
    return hashlib.sha256(data).hexdigest()


def verify_trace(data, header, row):
    for field in header['fields']:
        assert span(data, field['address'], field['size']).hex() == row['fields'][field['name']]
    for slot in range(16):
        assert span(data, 0xC46184 + 512 * slot, 164).hex() == row['records'][slot]
    assert row['pages_valid'] and (row['width'], row['height']) == (320, 200)
    assert [digest(page) for page in pages(data)] == row['pages']
    if 'drawing_bands' in header:
        assert [[digest(page[band['y'] * 40:(band['y'] + band['rows']) * 40])
                 for page in pages(data)] for band in header['drawing_bands']] == row['drawing_bands']


def pages(data):
    draw = integer(data, 0xC4566C, 2)
    assert draw in (0, 1)
    assert integer(data, 0xC456B6, 4) == 0xC4566E + 16 * draw
    return [span(data, integer(data, 0xC4566E + 16 * (draw ^ role) + 4 * plane, 4), 8000)
            for role in range(2) for plane in range(4)]


def assess(rows):
    indexed = {r['iteration']: r for r in rows}
    for first, count in WINDOWS:
        for i in range(first, first + count):
            a, b = indexed[i]['source'], indexed[i]['native']
            assert b['notification_countdown'] == (a['notification_countdown'] - 2) % 8 + 1
            assert a['post_input_event'] == b['post_input_event']
            assert a['warning_causes'] == b['warning_causes']
            for name in ('source', 'native'):
                state = indexed[i][name]
                if state['post_input_aux']:
                    assert state['notification_code'] == 0
                if i > first:
                    before = indexed[i - 1][name]
                    assert state['notification_countdown'] == (before['notification_countdown'] - 2) % 8 + 1
    for name, expiry in (('source', 5531), ('native', 5530)):
        before, after = indexed[expiry - 1][name], indexed[expiry][name]
        assert before['message_countdown'] == 0 and before['message_shown'] == 0x4023
        assert before['notification_countdown'] == 1 and after['notification_countdown'] == 8
        assert after['message_countdown'] == 255 and after['message_shown'] == 0
        assert after['message_code'] == 0 and not after['cockpit_flags'] & 1
    for name, on, off in (('source', 6231, 6235), ('native', 6230, 6234)):
        for i, phase, shown, cadence in ((on, 0x20, 0xC001, 4), (off, 0, 0, 8)):
            state = indexed[i][name]
            assert state['message_code'] == 0xC001 and state['warning_causes'] & 0x80
            assert state['message_state'] & 0x20 == phase
            assert state['message_shown'] == shown and state['notification_countdown'] == cadence
    for name in ('source', 'native'):
        assert [indexed[i][name]['player_phase'] for i in range(6294, 6299)] == [255, 255, 255, 239, 239]
    changed = [r for r in rows if r['pages']]
    assert [r['iteration'] for r in changed] == [5530, 5531, 6230, 6231, 6234, 6235]
    # This is localization evidence, not a drawing mask: every original/native
    # page hash and every changed byte stays reported. Original C322EE/SmallText
    # draws 26 four-pixel glyphs at x120/y192, with five glyph rows.
    for row in changed:
        for plane in row['pages']:
            assert all(120 <= d['x'] < 224 and 192 <= d['y'] < 197 for d in plane['differences'])
    return dict(boundaries=len(rows), source_owners=3 * len(rows),
                native_failure_owners=3 * len(changed), initial_cadence_offset_steps=1,
                message_expiry=dict(source=5531, native=5530),
                stall_on=dict(source=6231, native=6230), stall_off=dict(source=6235, native=6234),
                result_player_phase=[255, 255, 255, 239, 239],
                drawing_difference_boundaries=len(changed),
                changed_drawing_bytes=sum(len(p['differences']) for r in changed for p in r['pages']),
                acceptance='Observed message expiry and blink phase preserve original eight-step cadence. '
                'Strict full-page differences remain six; whole-flight cadence outside these windows is not inferred.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, required=True)
    parser.add_argument('--traces', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    reference = json.loads((args.traces / 'report.json').read_text())
    for path, expected in reference['input_hashes'].items():
        assert digest((ROOT / path).read_bytes()) == expected, 'sealed input/media changed'
    headers, traces = {}, {}
    for name in ('source', 'native'):
        path = args.traces / f'{name}.jsonl.gz'
        assert digest(gzip.decompress(path.read_bytes())) == reference[f'{name}_trace_sha256']
        headers[name], traces[name] = read_trace(path)
    assert headers['source'] == headers['native']
    controls = args.traces / 'consumed.fa18in'
    assert digest(controls.read_bytes()) == reference['consumed_input_sha256']
    env = {k: v for k, v in os.environ.items() if not k.startswith('FA18_LOOP_')}
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', 'build/recomp/native_message_cadence_oracle.exe',
                    '--main', 'tools/native/native_message_cadence_oracle.c'], cwd=ROOT, check=True)
    rows, oracle_cases = [], []
    with tempfile.TemporaryDirectory(prefix='qualification-message-review-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        intro = work / 'intro.e9k'
        intro.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        for first, count in WINDOWS:
            retained = [args.out / f'source.{i}.dat.gz' for i in range(first, first + count)]
            if not all(p.is_file() for p in retained):
                subprocess.run([str(ROOT / 'build/recomp/fa18_recomp.exe'), '--state',
                    str(ROOT / 'captures/native/qual_carrier_success/state.bin'), '--rom',
                    str(ROOT / 'local/system/kick13.rom'), '--ports', 'off', '--input',
                    str(ROOT / 'captures/native/qual_carrier_success/input.fa18in'), '--frames',
                    str(traces['source'][first + count - 1]['frame'] + 1)], cwd=ROOT,
                    env=dict(env, FA18_LOOP_DUMP=f'{first}+{count}:{work / "source"}'),
                    capture_output=True, text=True, check=True, timeout=180)
                for i, path in zip(range(first, first + count), retained):
                    data = (work / f'source.{i}.dat').read_bytes()
                    verify_trace(data, headers['source'], traces['source'][i])
                    path.write_bytes(gzip.compress(data, mtime=0))
            result = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '30000',
                '--input', str(controls.resolve()), '--iterations', str(first + count), '--replay',
                str(intro), '--save-dir', str(work / f'pilot-{first}'), '--frame-capture',
                f'{first}+{count}', str(work / 'native'), '--frame-capture-entry-only'], cwd=ROOT,
                env=env, capture_output=True, text=True, check=True, timeout=120)
            stats = json.loads(result.stdout)
            assert stats['frame_capture_complete'] and not stats['cpu_emulation'] and not stats['chipset_emulation']
            for i, path in zip(range(first, first + count), retained):
                a = gzip.decompress(path.read_bytes())
                b = (work / f'native.{i}.entry.dat').read_bytes()
                for name, data in (('source', a), ('native', b)):
                    verify_trace(data, headers[name], traces[name][i])
                row = dict(iteration=i, game_tick=integer(a, 0xC458DA, 2), pages=[],
                           ram_sha256={name: digest(data) for name, data in (('source', a), ('native', b))})
                for name, data in (('source', a), ('native', b)):
                    row[name] = {k: integer(data, address, size) for k, (address, size) in FIELDS.items()}
                    if i == 4690:
                        row[name]['initial_pilot_qualified'] = integer(data, integer(data, 0xC1AB74, 4), 2)
                for plane, (av, bv) in enumerate(zip(pages(a), pages(b))):
                    changes = [dict(byte=j, x=j % 40 * 8, y=j // 40, source=av[j], native=bv[j])
                               for j in range(8000) if av[j] != bv[j]]
                    if changes:
                        row['pages'].append(dict(plane=plane, differences=changes))
                if row['pages']:
                    (args.out / f'native.{i}.dat.gz').write_bytes(gzip.compress(b, mtime=0))
                for name, data in (('source', a), ('native', b)):
                    if name == 'native' and not row['pages']:
                        continue
                    observed = work / 'observed.dat'
                    observed.write_bytes(data)
                    result = subprocess.run([str(ROOT / 'build/recomp/native_message_cadence_oracle.exe'),
                        str(observed)], cwd=ROOT, check=True, capture_output=True, text=True, timeout=30)
                    oracle_cases.append(dict(capture=f'{name}.{i}.dat.gz', output=result.stdout.strip(),
                                             ram_sha256=digest(data)))
                rows.append(row)
            print(f'{first}+{count}: independent RAM matches every traced core, field and page; actual message owners match', flush=True)
        report = dict(rows=rows, original_owner_cases=oracle_cases,
                      input_hashes=reference['input_hashes'],
                      trace_sha256={name: reference[f'{name}_trace_sha256'] for name in ('source', 'native')},
                      total_boundaries=len(rows), matching_drawing_boundaries=sum(not r['pages'] for r in rows))
        # Preserve observed evidence even if assessment finds a new mismatch.
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
        summary = assess(rows)
        rejected = []
        for iteration, field in ((4690, 'notification_countdown'), (5531, 'message_countdown'),
                                 (6231, 'message_shown'), (6297, 'player_phase')):
            changed = json.loads(json.dumps(rows))
            next(r for r in changed if r['iteration'] == iteration)['source'][field] ^= 1
            try:
                assess(changed)
            except AssertionError:
                rejected.append(field)
            else:
                raise AssertionError(f'accepted wrong {field}')
        summary['wrong_state_mutations_rejected'] = rejected
        report['assessment'] = summary
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
        print(json.dumps(summary))
    for path, expected in reference['input_hashes'].items():
        assert digest((ROOT / path).read_bytes()) == expected, 'sealed input/media changed'
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
