"""Assess observed selected-target radar blink phases without masking pages.

Actual original owner inputs/returns are checked separately from native body
integration. Phase reversal is an external oracle probe, never a runtime fix.
"""
import argparse
import copy
import gzip
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile

from check_gameplay_checkpoint import ROOT, integer, span
from check_qualification_message_cadence import digest, pages, verify_trace
from compare_flight_traces import read_trace


def points(row):
    return sorted([{k:v for k,v in point.items() if k!='phase'} for point in row['points']],
                  key=lambda point: point['record_offset'])


def cached_pixels(data, page):
    result = set()
    for cursor in range(0, 40, 4):
        address = 0xC4E71C + page * 40 + cursor
        x, y = integer(data, address, 2), integer(data, address + 2, 2)
        if x == 0xFFFF:
            break
        pair, x = bool(x & 0x8000), x & 0x7FFF
        assert 0 < x < 320 and 0 < y < 200
        result.add((x, y))
        if pair:
            result.add((x - 1, y))  # Original PAIR_MASKS / plot_pixel_pair.
    return result


def complete_page_delta(source, native, colour):
    """Check every actual XOR byte against the observed cached point delta.

    This predicts differences; neither page is modified or masked, and the
    strict same-phase page comparison remains failed when bytes differ.
    """
    draw = integer(source, 0xC4566C, 2)
    assert draw == integer(native, 0xC4566C, 2) and draw in (0, 1)
    expected = [bytearray(8000) for _ in range(8)]
    changed = []
    for role, page in enumerate((draw, 1 - draw)):
        delta = cached_pixels(source, page) ^ cached_pixels(native, page)
        changed.append(sorted(delta))
        for plane in range(4):
            if colour & (1 << (3 - plane)):
                for x, y in delta:
                    expected[4 * role + plane][y * 40 + x // 8] |= 0x80 >> (x & 7)
    actual = [bytes(x ^ y for x, y in zip(a, b)) for a, b in zip(pages(source), pages(native))]
    assert actual == expected, 'Complete page difference is not the observed radar cache delta'
    return dict(compared_bytes=64000, changed_pixels_by_page=changed,
                delta_sha256=digest(b''.join(actual)), colour=colour)


def verify(rows):
    phase_offset = None
    for i, row in enumerate(rows):
        source, native = row['source'], row['native']
        offset = (native['phase_before'] - source['phase_before']) % 256
        if phase_offset is None:
            phase_offset = offset
        assert offset == phase_offset
        for name, other in (('source', 'native'), ('native', 'source')):
            state, flipped = row[name], row[name + '_flipped_phase']
            assert state['complete_non_stack_ram_matching']
            assert state['prefix_calls'] == flipped['prefix_calls'] == row[other]['prefix_calls']
            assert state['prefix_calls'] in (0,1,2)
            assert state['phase_after'] == (state['phase_before'] + state['prefix_calls']) % 256
            assert flipped['phase_before'] == state['phase_before'] ^ 1
            assert flipped['phase_after'] == (flipped['phase_before'] + flipped['prefix_calls']) % 256
            assert state['selected_record'] == row[other]['selected_record']
            if i:
                assert state['phase_before'] == rows[i - 1][name]['phase_after']
            selected = state['selected_record']
            regular = [p for p in points(state) if p['record_offset'] != selected]
            assert regular == [p for p in points(flipped) if p['record_offset'] != selected]
            marker = [p for p in points(state) if p['record_offset'] == selected]
            alternate = [p for p in points(flipped) if p['record_offset'] == selected]
            eligible = bool(marker or alternate)
            for case in (state,flipped):
                assert all(p['phase']&1 for p in case['points'] if p['record_offset']==selected)
            if state['prefix_calls']==1:
                assert bool(marker) == (eligible and bool(state['phase_after'] & 1))
                assert bool(alternate) == (eligible and bool(flipped['phase_after'] & 1))
            elif state['prefix_calls']==2:
                assert bool(marker)==bool(alternate)
            else:
                assert not marker and not alternate
            # Equivalent blink phases must yield every same record/position/
            # pair/colour. The phase probe is not applied to playable memory.
            equivalent = row[other] if offset % 2 == 0 else row[other + '_flipped_phase']
            assert points(state) == points(equivalent)
    return phase_offset


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--window', type=Path, required=True)
    parser.add_argument('--original-bodies', type=Path, required=True)
    parser.add_argument('--trace-evidence', type=Path,
        help='Verified preserved full-flight traces when the drawing window uses optional owner fields')
    parser.add_argument('--source-evidence', type=Path,
        help='Ordinary original recording required to bind optional trace fields')
    parser.add_argument('--complete-page-delta', action='store_true',
        help='Require every complete-page XOR byte to equal the observed cached selected-marker difference')
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    window = json.loads((args.window / 'report.json').read_text())
    original = json.loads((args.original_bodies / 'report.json').read_text())
    if args.trace_evidence:
        from check_mission_message_trace import preserved
        assert args.source_evidence is not None
        reference = json.loads((args.trace_evidence / 'report.json').read_text())
        assert reference['baseline_source_trace_sha256'] == original['source_trace_sha256']
        assert reference['source_trace_sha256'] == window['source_trace_sha256']
        assert reference['native_trace_sha256'] == window['native_trace_sha256']
        assert reference['runner_sha256'] == window['runner_sha256']
        source_path = args.trace_evidence / 'source.jsonl.gz'
        baseline = args.source_evidence / 'driver.jsonl.gz'
        assert digest(gzip.decompress(baseline.read_bytes())) == original['source_trace_sha256']
        assert digest(gzip.decompress(source_path.read_bytes())) == reference['source_trace_sha256']
        header, source_rows = read_trace(source_path)
        # The owner probe already verifies the ordinary recording row for row.
        # Bind optional fields to that same recording, never another replay.
        assert preserved(source_path, baseline) == reference['source_rows_preserved']
        native_path = args.trace_evidence / 'native.jsonl.gz'
        assert digest(gzip.decompress(native_path.read_bytes())) == reference['native_trace_sha256']
        native_header, native_rows = read_trace(native_path)
        assert header == native_header
        for observed in window['rows']:
            for name, index, trace_rows in (('source', observed['source_iteration'], source_rows),
                                            ('native', observed['native_iteration'], native_rows)):
                data = gzip.decompress((args.window / f'{name}.{index}.dat.gz').read_bytes())
                assert digest(data) == observed['ram_sha256'][name]
                verify_trace(data, header, trace_rows[index])
    else:
        assert window['source_trace_sha256'] == original['source_trace_sha256']
    captures = {row['iteration']: row['snapshots'] for row in original['captures']}
    assert sorted(captures) == list(range(window['first'], window['last'] + 1))
    assert original['last_observation'] >= window['last'] + 1
    body_oracle = ROOT / 'build/recomp/native_frame_body_oracle.exe'
    radar_oracle = ROOT / 'build/recomp/native_radar_cadence_oracle.exe'
    for executable, source in ((body_oracle, 'native_frame_body_oracle.c'),
                                (radar_oracle, 'native_radar_cadence_oracle.c')):
        with (args.out / (source + '.build.log')).open('w') as log:
            subprocess.run(['python', 'scripts/build_recomp.py', '--output',
                str(executable.relative_to(ROOT)), '--main', 'tools/native/' + source],
                cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, check=True)
    env = {k: v for k, v in os.environ.items() if not k.startswith('FA18_FRAME_')}
    rows = []
    with tempfile.TemporaryDirectory(prefix='radar-cadence-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        for observed in window['rows']:
            i, j = observed['source_iteration'], observed['native_iteration']
            timing = observed['body_timing']
            before, after = work / 'native.before.dat', work / 'native.after.dat'
            for suffix, path in (('before', before), ('after', after)):
                path.write_bytes(gzip.decompress((args.window / f'native.{j}.{suffix}.dat.gz').read_bytes()))
            prefix = work / 'radar'
            log_path = args.out / f'native-body.{j}.log'
            with log_path.open('w') as log:
                result = subprocess.run([str(body_oracle), str(before), str(after),
                    str(timing['before_tick']), str(timing['after_tick']), str(timing['saved_tick'])],
                    cwd=ROOT, env=dict(env, FA18_FRAME_RADAR_DUMP=str(prefix)),
                    stdout=log, stderr=subprocess.STDOUT, timeout=20)
            assert result.returncode == 0, log_path
            assert '0 gameplay differences, 0 display bytes' in log_path.read_text()
            row = dict(iteration=i, native_iteration=j,
                       native_body_input_sha256=digest(before.read_bytes()),
                       native_body_output_sha256=digest(after.read_bytes()),
                       native_body_original_matching_with_existing_exclusions=True,
                       strict_page_differences=observed['page_differences'])
            for name in ('source', 'native'):
                if name == 'source':
                    boundary_data = []
                    for suffix in ('owner', 'owner-after'):
                        data = gzip.decompress((args.original_bodies / f'source-body.{i}.{suffix}.dat.gz').read_bytes())
                        assert digest(data) == captures[i][suffix]['ram_sha256']
                        boundary_data.append(data)
                    owner_input, observed_output = boundary_data
                else:
                    owner_input = (work / 'radar.before.dat').read_bytes()
                    observed_output = (work / 'radar.after.dat').read_bytes()
                    # C45883 is unchanged by the preceding mode-three scene.
                    assert integer(owner_input, 0xC45883, 1) == integer(before.read_bytes(), 0xC45883, 1)
                assert len(owner_input) == len(observed_output) == 0x100000
                row[name + '_owner_input_sha256'] = digest(owner_input)
                row[name + '_owner_output_sha256'] = digest(observed_output)
                for flip in (False, True):
                    data = bytearray(owner_input)
                    if flip:
                        data[0x80000 + 0x45883] ^= 1
                    fixture, output = work / 'owner.dat', work / 'output.dat'
                    fixture.write_bytes(data)
                    result = subprocess.run([str(radar_oracle), str(fixture), str(output)],
                        cwd=ROOT, env=env, capture_output=True, text=True, timeout=20)
                    assert result.returncode == 0, result.stderr
                    state = json.loads(result.stdout)
                    row[name + ('_flipped_phase' if flip else '')] = state
                    if not flip:
                        actual = output.read_bytes()
                        # Actual original returns can include asynchronous OS
                        # and real ABI stack changes. Check all complete pages,
                        # sixteen cores and the entire radar-owned list/cache.
                        actual_pages, observed_pages = pages(actual), pages(observed_output)
                        row[name + '_live_pages_matching'] = actual_pages == observed_pages
                        row[name + '_live_page_difference_bytes'] = [
                            sum(a != b for a, b in zip(x, y)) for x, y in zip(actual_pages, observed_pages)]
                        if actual_pages != observed_pages:
                            (args.out / f'failed-{name}-owner.{i}.dat.gz').write_bytes(gzip.compress(actual, mtime=0))
                        for slot in range(16):
                            address = 0xC46184 + 512 * slot
                            assert span(actual, address, 164) == span(observed_output, address, 164)
                        assert span(actual, 0xC4E2BC, 0x4B0) == span(observed_output, 0xC4E2BC, 0x4B0)
                        assert state['phase_after'] == integer(observed_output, 0xC45883, 1)
                        assert span(actual, 0xC4586D, 3) == span(observed_output, 0xC4586D, 3)
                if name == 'source':
                    body = gzip.decompress((args.original_bodies / f'source-body.{i}.before.dat.gz').read_bytes())
                    assert digest(body) == captures[i]['before']['ram_sha256']
                    row['source_body_phase_before'] = integer(body, 0xC45883, 1)
                    assert row['source_body_phase_before'] == row['source']['phase_before']
            rows.append(row)
    phase_offset = verify(rows)
    page_deltas = None
    if args.complete_page_delta:
        assert args.trace_evidence, 'Live trace verification is required for the complete page delta'
        colours = {p['colour'] for row in rows for name in
                   ('source', 'native', 'source_flipped_phase', 'native_flipped_phase')
                   for p in row[name]['points'] if p['record_offset'] == row[name]['selected_record']}
        assert len(colours) == 1, 'This bounded cache-delta check requires a stable selected-marker colour'
        colour, = colours
        page_deltas = []
        for observed in window['rows']:
            i, j = observed['source_iteration'], observed['native_iteration']
            a = gzip.decompress((args.window / f'source.{i}.dat.gz').read_bytes())
            b = gzip.decompress((args.window / f'native.{j}.dat.gz').read_bytes())
            page_deltas.append(dict(source_iteration=i, native_iteration=j,
                                   **complete_page_delta(a, b, colour)))
    rejections = {}
    for kind in ('counter_increment', 'premature_marker', 'marker_coordinate', 'other_marker'):
        changed = copy.deepcopy(rows)
        index = next(i for i, row in enumerate(changed)
                     if any(p['record_offset'] == row['source']['selected_record'] for p in row['source']['points']))
        state = changed[index]['source']
        marker = next(p for p in state['points'] if p['record_offset'] == state['selected_record'])
        if kind == 'counter_increment':
            state['phase_after'] += 1
        elif kind == 'premature_marker':
            changed[index]['source_flipped_phase']['points'].append(copy.deepcopy(marker))
        elif kind == 'marker_coordinate':
            marker['x'] += 1
        else:
            state['points'] = [p for p in state['points'] if p['record_offset'] == state['selected_record']]
        try:
            verify(changed)
        except AssertionError:
            rejections[kind] = True
        else:
            raise AssertionError(f'{kind} mutation accepted')
    report = dict(first=window['first'], last=window['last'], boundaries=len(rows),
        runner_sha256=window['runner_sha256'], original_probe_sha256=original['probe_sha256'],
        original_unchanged_observations=original['unchanged_observations'],
        source_trace_sha256=window['source_trace_sha256'], native_trace_sha256=window['native_trace_sha256'],
        radar_original_non_stack_comparisons=4 * len(rows), native_complete_body_comparisons=len(rows),
        live_original_radar_returns_compared=len(rows), initial_phase_offset=phase_offset,
        equivalent_blink_phase_points_matching=True, mutation_rejections=rejections, rows=rows,
        scope='Selected-target markers obey original alternating-call blink in this window. '
              'Different incoming phases are assessed under the accepted rendering-cadence policy. '
              'Every strict page difference remains reported. Message text and other drawing differences '
              'are not accepted by this check. Native body checks retain their existing explicit exclusions; '
              'actual original owner returns are checked for all complete pages, cores and radar list/cache.')
    report['live_original_pages_matching'] = sum(row['source_live_pages_matching'] for row in rows)
    report['live_native_pages_matching'] = sum(row['native_live_pages_matching'] for row in rows)
    report['strict_live_owner_pages_matching'] = all(
        row[name + '_live_pages_matching'] for row in rows for name in ('source', 'native'))
    if page_deltas is not None:
        report['complete_page_delta'] = page_deltas
        report['scope'] += ' Every full-page XOR byte in this bounded window equals the retained radar point-cache delta; no pixels are excluded.'
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'{len(rows)} original/native radar transitions, {4*len(rows)} complete owner comparisons, '
          f'{len(rows)} native body comparisons; phase offset={phase_offset}; four mutations rejected')
    print(f"Live original owner pages: {report['live_original_pages_matching']}/{len(rows)} match; "
          f"native: {report['live_native_pages_matching']}/{len(rows)} match")
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
    return 0 if report['strict_live_owner_pages_matching'] else 1


if __name__ == '__main__':
    sys.exit(main())
