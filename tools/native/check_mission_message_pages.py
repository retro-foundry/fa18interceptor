"""Check actual C322EE returns and predict every bounded message-owner page byte.

Complete source/native frame entries stay different when timing differs. No
captured page, clock or gameplay state is changed by this diagnostic.
"""
import argparse
import copy
import gzip
import json
import os
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT, integer, span
from check_qualification_message_cadence import digest, pages, verify_trace
from check_mission_message_trace import preserved
from compare_flight_traces import read_trace


def predicted_pages(before, after, drawn, mutation=None):
    result = [bytearray(page) for page in pages(before)]
    if not drawn:
        return result
    # This bounded front-cockpit path has no clipping or odd-address glyph
    # skip. Keep those branches outside the claim rather than guessing them.
    assert integer(before, 0xC45785, 1) == 0  # CONTEXT_SELECT
    assert integer(before, 0xC45986, 2) == 0  # SPAN_ORIGIN
    assert integer(before, 0xC45918, 4) == 0  # REDRAW_STATE_LONG
    kind = integer(after, 0xC45862, 1) >> 6
    first = 0 if kind == 0 else 1 if kind == 1 else 3
    planes = [(first, True)]
    if integer(before, 0xC45861, 1) < 128:
        planes += [(3 if kind < 2 else 1, False), (1 if kind == 0 else 0, False)]
    if mutation == 'wrong_plane':
        planes[0] = ((first + 1) % 4, True)
    if mutation == 'lost_clear':
        planes = [(plane, mode) for plane, mode in planes if mode]
    for i, ch in enumerate(span(after, 0xC4580A, 26)):
        column = integer(before, 0xC31998 + 4 * i, 2)
        mode = integer(before, 0xC3199A + 4 * i, 2)
        assert 0 <= 12 + column < 40 and column % 2 == 0
        assert mode & 0x0FFF == 0, 'Unexpected glyph mode needs its own assessment'
        offset = integer(before, 0xC3D790 + (ch - 32) * 2, 2)
        if offset & 0x8000:
            offset -= 0x10000
        glyph = span(before, 0xC3D790 + offset, 5)
        shift = ((0x0FCA | mode) >> 12) & 15
        cell = 0xE0000000 >> shift
        for plane, drawing in planes:
            for row, bits in enumerate(glyph):
                dest = 0x1E0C + column + 40 * row
                assert 0 <= dest <= 7996
                old = int.from_bytes(result[plane][dest:dest + 4], 'big')
                pixels = ((bits << 24) >> shift) & cell if drawing else 0
                if mutation == 'wrong_glyph' and drawing:
                    pixels ^= cell
                result[plane][dest:dest + 4] = ((old & (0xFFFFFFFF ^ cell)) | pixels).to_bytes(4, 'big')
    return result


def verify_return(data, snapshots):
    registers = snapshots['owner']['registers']
    assert registers['pc'] == 0xC322EE
    actual_return = integer(data, registers['registers'][15], 4)
    assert actual_return in (0xC0F286, 0xC0F2DC)
    assert actual_return == snapshots['owner-after']['registers']['pc'], 'Capture includes another caller instruction'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('window', 'original-bodies', 'trace-evidence', 'source-evidence', 'out'):
        parser.add_argument('--' + name, type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    window = json.loads((args.window / 'report.json').read_text())
    original = json.loads((args.original_bodies / 'report.json').read_text())
    reference = json.loads((args.trace_evidence / 'report.json').read_text())
    assert 1 <= len(window['rows']) <= 32
    assert reference['baseline_source_trace_sha256'] == original['source_trace_sha256']
    assert reference['runner_sha256'] == window['runner_sha256']
    baseline = args.source_evidence / 'driver.jsonl.gz'
    assert digest(gzip.decompress(baseline.read_bytes())) == original['source_trace_sha256']
    assert preserved(args.trace_evidence / 'source.jsonl.gz', baseline) == reference['source_rows_preserved']
    headers, traces = {}, {}
    for name in ('source', 'native'):
        path = args.trace_evidence / (name + '.jsonl.gz')
        assert digest(gzip.decompress(path.read_bytes())) == reference[name + '_trace_sha256'] == window[name + '_trace_sha256']
        headers[name], traces[name] = read_trace(path)
    assert headers['source'] == headers['native']
    captures = {row['iteration']: row['snapshots'] for row in original['captures']}
    assert sorted(captures) == list(range(window['first'], window['last'] + 1))
    assert original['last_observation'] >= window['last'] + 1
    assert original['unchanged_observations'] >= window['last'] + 1
    for path, expected in reference['input_hashes'].items():
        assert digest((ROOT / path).read_bytes()) == expected, 'Sealed input/media changed'
    executables = {}
    for name in ('native_frame_body_oracle', 'native_message_cadence_oracle'):
        executables[name] = ROOT / ('build/recomp/' + name + '.exe')
        with (args.out / (name + '.build.log')).open('w') as log:
            subprocess.run(['python', 'scripts/build_recomp.py', '--output',
                str(executables[name].relative_to(ROOT)), '--main', 'tools/native/' + name + '.c'],
                cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, check=True)
    env = {k: v for k, v in os.environ.items() if not k.startswith('FA18_FRAME_')}
    rows, rejections = [], dict(wrong_plane=False, lost_clear=False, wrong_glyph=False)
    with tempfile.TemporaryDirectory(prefix='message-pages-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        for observed in window['rows']:
            i, j = observed['source_iteration'], observed['native_iteration']
            for name, index in (('source', i), ('native', j)):
                data = gzip.decompress((args.window / f'{name}.{index}.dat.gz').read_bytes())
                assert digest(data) == observed['ram_sha256'][name]
                verify_trace(data, headers[name], traces[name][index])
            timing = observed['body_timing']
            before, after, prefix = work / 'before.dat', work / 'after.dat', work / 'message'
            for suffix, path in (('before', before), ('after', after)):
                path.write_bytes(gzip.decompress((args.window / f'native.{j}.{suffix}.dat.gz').read_bytes()))
            log_path = args.out / f'native-body.{j}.log'
            with log_path.open('w') as log:
                subprocess.run([str(executables['native_frame_body_oracle']), str(before), str(after),
                    str(timing['before_tick']), str(timing['after_tick']), str(timing['saved_tick'])],
                    cwd=ROOT, env=dict(env, FA18_FRAME_MESSAGE_DUMP=str(prefix)), stdout=log,
                    stderr=subprocess.STDOUT, check=True, timeout=20)
            assert '0 gameplay differences, 0 display bytes' in log_path.read_text()
            row = dict(iteration=i, native_iteration=j, strict_page_differences=observed['page_differences'],
                       native_body_input_sha256=digest(before.read_bytes()), native_body_output_sha256=digest(after.read_bytes()))
            for name in ('source', 'native'):
                if name == 'source':
                    owner_input, observed_output = [gzip.decompress((args.original_bodies / f'source-body.{i}.{suffix}.dat.gz').read_bytes()) for suffix in ('owner', 'owner-after')]
                    for suffix, data in zip(('owner', 'owner-after'), (owner_input, observed_output)):
                        assert digest(data) == captures[i][suffix]['ram_sha256']
                    verify_return(owner_input, captures[i])
                else:
                    owner_input, observed_output = [(work / f'message.{suffix}.dat').read_bytes() for suffix in ('before', 'after')]
                fixture, output = work / 'owner.dat', work / 'output.dat'
                fixture.write_bytes(owner_input)
                result = subprocess.run([str(executables['native_message_cadence_oracle']), str(fixture), str(output)],
                    cwd=ROOT, env=env, capture_output=True, text=True, check=True, timeout=20)
                state = json.loads(result.stdout)
                actual = output.read_bytes()
                assert state['complete_non_stack_ram_matching']
                if name == 'source':
                    registers = captures[i]['owner']['registers']['registers']
                    actual_return = captures[i]['owner-after']['registers']['registers'][4] & 255
                    expected_return = registers[4] & 255 if state['return_preserved'] else state['return_value']
                    assert actual_return == expected_return, (i, 'Live message return value differs')
                    bad_capture = copy.deepcopy(captures[i])
                    bad_capture['owner-after']['registers']['pc'] += 6
                    try:
                        verify_return(owner_input, bad_capture)
                    except AssertionError:
                        pass
                    else:
                        raise AssertionError('Accepted a capture containing the next caller instruction')
                assert pages(actual) == pages(observed_output), (i, name, 'Complete live owner pages differ')
                for address, size in ((0xC4580A, 26), (0xC45860, 3), (0xC4583C, 1),
                                      (0xC45887, 1), (0xC459C4, 2), (0xC45ADE, 8)):
                    assert span(actual, address, size) == span(observed_output, address, size)
                assert predicted_pages(owner_input, actual, state['text_drawn']) == pages(observed_output), (i, name, 'Glyph prediction differs')
                for mutation in rejections:
                    if predicted_pages(owner_input, actual, state['text_drawn'], mutation) != pages(observed_output):
                        rejections[mutation] = True
                row[name] = dict(owner_input_sha256=digest(owner_input), owner_output_sha256=digest(observed_output),
                    predicted_pages_sha256=[digest(page) for page in pages(actual)], compared_bytes=64000,
                    layout_sha256=digest(span(owner_input, 0xC31998, 104)),
                    **state, message_line=span(actual, 0xC4580A, 26).decode('ascii'),
                    flags_before=integer(owner_input, 0xC45862, 1), flags_after=integer(actual, 0xC45862, 1),
                    redraws_before=integer(owner_input, 0xC45861, 1), redraws_after=integer(actual, 0xC45861, 1))
            assert row['source']['layout_sha256'] == row['native']['layout_sha256']
            rows.append(row)
    assert all(rejections.values()), rejections
    report = dict(first=window['first'], last=window['last'], observations=len(rows), rows=rows,
        runner_sha256=window['runner_sha256'], source_trace_sha256=window['source_trace_sha256'],
        native_trace_sha256=window['native_trace_sha256'], original_probe_sha256=original['probe_sha256'],
        original_unchanged_observations=original['unchanged_observations'], native_complete_body_comparisons=len(rows),
        live_original_pages_matching=len(rows), live_native_pages_matching=len(rows),
        complete_owner_page_predictions=2 * len(rows), predicted_bytes=128000 * len(rows),
        mutation_rejections=rejections, wrong_owner_return_capture_rejected=True,
        live_original_text_returns_matching=len(rows),
        scope='Bounded actual original and native owner inputs, full non-stack component comparisons and defined text returns; complete live owner page gates and independent immutable-glyph page prediction. Whole native bodies use their existing explicit scope. Cross-runtime message page history remains open; strict differences are retained.')
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f"{len(rows)} native bodies, {2 * len(rows)} actual message owners and complete 64,000-byte page predictions pass; three drawing mutations rejected")
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
