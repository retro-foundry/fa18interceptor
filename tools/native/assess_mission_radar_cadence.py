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
from check_qualification_message_cadence import digest, pages, verify_trace, instrument_panel_refresh
from compare_flight_traces import read_trace


def points(row):
    return sorted([{k:v for k,v in point.items() if k not in ('phase', 'prefix', 'selected_record')} for point in row['points']],
                  key=lambda point: point['record_offset'])


def selected_points(state):
    return [point for point in state['points']
            if point['record_offset'] == point.get('selected_record', state['selected_record'])]


def verify_prefix_contacts(state, flipped, equivalent):
    """Each complete tail can scan a new selection before the next tail."""
    selectors = state['prefix_selected_records']
    assert len(selectors) == state['prefix_calls']
    assert selectors == flipped['prefix_selected_records'] == equivalent['prefix_selected_records']
    assert state['selected_record_after'] == flipped['selected_record_after'] == equivalent['selected_record_after']
    if selectors:
        assert selectors[0] == state['selected_record']
    for case in (state, flipped, equivalent):
        assert len(case['prefix_selected_records']) == case['prefix_calls']
        for point in case['points']:
            prefix = point['prefix']
            assert 1 <= prefix <= case['prefix_calls']
            selected = case['prefix_selected_records'][prefix - 1]
            assert point['selected_record'] == selected
            assert point['phase'] == (case['phase_before'] + prefix) % 256
            if point['record_offset'] == selected:
                assert point['phase'] & 1
    for prefix, selected in enumerate(selectors, 1):
        regular, markers = [], []
        for case in (state, flipped):
            submitted = points(dict(points=[p for p in case['points'] if p['prefix'] == prefix]))
            regular.append([p for p in submitted if p['record_offset'] != selected])
            markers.append([p for p in submitted if p['record_offset'] == selected])
        assert regular[0] == regular[1], 'Regular contacts differ within a radar prefix'
        eligible = bool(markers[0] or markers[1])
        for case, marker in zip((state, flipped), markers):
            assert bool(marker) == (eligible and bool((case['phase_before'] + prefix) & 1))
        assert points(dict(points=[p for p in state['points'] if p['prefix'] == prefix])) == points(
            dict(points=[p for p in equivalent['points'] if p['prefix'] == prefix])), 'Equivalent radar prefix contacts differ'


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


def line_pixels(paint):
    """C2FA7E line terms: omit the first row, step downward in 4x scale.

    The bounded radar heads are inside the view, so no clipping or signed
    overflow branch is inferred. Original instructions validate all output.
    """
    x0, y0, x1, y1 = (paint[n] for n in ('x', 'y', 'x1', 'y1'))
    assert all(0 <= x < 320 for x in (x0, x1))
    assert all(0 <= y < 199 for y in (y0, y1)), 'Clipped radar head needs its own assessment'
    if y1 >= y0:
        x, y, dx = x0, y0 + 1, x1 - x0
    else:
        x, y, dx = x1, y1 + 1, x0 - x1
    rows = max(abs(y1 - y0) - 1, 0)
    x_major = abs(dx) >= rows
    major, minor = (abs(dx), rows) if x_major else (rows, abs(dx))
    error = 4 * minor - 2 * major
    direction = -1 if dx < 0 else 1
    for _ in range(major + 1):
        yield x, y
        negative = error < 0
        error += 4 * minor if negative else 4 * minor - 4 * major
        if x_major:
            x += direction
            if not negative:
                y += 1
        else:
            if not negative:
                x += direction
            y += 1


def painted_pages(data, paints):
    """Predict full pages from ordered original head/erase/marker operations.

    Expected buffers are separate from the captured pages. In particular,
    colour zero erases background bits too; a colour-one marker can erase
    an existing colour-three crosshair's bit two until a later head redraw.
    """
    expected = [bytearray(page) for page in pages(data)]
    apply_paints(expected, paints)
    return expected


def apply_paints(expected, paints):
    for paint in paints:
        if paint['kind'] == 'line':
            pixels = line_pixels(paint)
        else:
            assert paint['kind'] in ('point', 'pair')
            pixels = [(paint['x'], paint['y'])]
            if paint['kind'] == 'pair':
                pixels.append((paint['x'] - 1, paint['y']))
        for x, y in pixels:
            if y <= 0:  # C2F5F4/C2F60A no-draw return.
                continue
            assert 0 <= x < 320 and y < 200
            offset, bit = y * 40 + x // 8, 0x80 >> (x & 7)
            for plane in range(4):
                if paint['colour'] & (1 << (3 - plane)):
                    expected[plane][offset] |= bit
                else:
                    expected[plane][offset] &= 255 ^ bit


def pixel_colour(data, x, y):
    bit, offset = 0x80 >> (x & 7), y * 40 + x // 8
    buffers = pages(data)
    return [sum(1 << (3 - plane) for plane in range(4)
                if buffers[4 * role + plane][offset] & bit)
            for role in range(2)]


def cross_paint_history(window_path, window, history, selected_planes, original_bodies, captures, body_rows,
                        omit_panel_refresh=False):
    """Predict complete requested-plane XOR from common initial pages and paints.

    Other cockpit/message planes remain in the strict difference report. No
    fitted phase, discarded pixel or altered captured page participates here.
    """
    events = {(row['iteration'], row['runtime']): row['paints'] for row in history}
    expected, previous_draw, rows = {}, {}, []
    panel_refreshes = []
    body_hashes = {row['iteration']: row['native_body_input_sha256'] for row in body_rows}
    planes = [4 * role + plane for role in range(2) for plane in selected_planes]
    for observed in window['rows']:
        i, j = observed['source_iteration'], observed['native_iteration']
        live = {}
        for name, index in (('source', i), ('native', j)):
            path = window_path / f'{name}.{index}.dat.gz'
            if not path.exists():
                # Passing entry RAM is deliberately pruned. Require its
                # actual retained body-begin pages to equal the complete
                # source entry too; never synthesize missing/differing bytes.
                assert name == 'native' and not observed['page_differences']
                data = gzip.decompress((window_path / f'native.{index}.before.dat.gz').read_bytes())
                assert pages(data) == live['source'], 'Retained body pages do not prove the passing entry'
            else:
                data = gzip.decompress(path.read_bytes())
            live[name] = pages(data)
            draw = integer(data, 0xC4566C, 2)
            if name not in expected:
                expected[name] = [bytearray(page) for page in live[name]]
            elif previous_draw[name] != draw:
                expected[name] = expected[name][4:] + expected[name][:4]
            previous_draw[name] = draw
        if not rows:
            assert all(live['source'][plane] == live['native'][plane] for plane in planes), 'History must start at common requested planes'
        actual_delta = []
        for plane in planes:
            actual = bytes(x ^ y for x, y in zip(live['source'][plane], live['native'][plane]))
            predicted = bytes(x ^ y for x, y in zip(expected['source'][plane], expected['native'][plane]))
            assert actual == predicted, (i, plane, 'Radar paint history does not explain complete plane difference')
            actual_delta.append(actual)
        rows.append(dict(iteration=i, planes=planes, compared_bytes=8000 * len(planes),
            difference_bytes=[sum(bool(byte) for byte in page) for page in actual_delta],
            delta_sha256=digest(b''.join(actual_delta))))
        for name in ('source', 'native'):
            if name == 'source':
                body = gzip.decompress((original_bodies / f'source-body.{i}.before.dat.gz').read_bytes())
                assert digest(body) == captures[i]['before']['ram_sha256']
            else:
                body = gzip.decompress((window_path / f'native.{j}.before.dat.gz').read_bytes())
                assert digest(body) == body_hashes[i]
            images = instrument_panel_refresh(body, expected[name], apply=not omit_panel_refresh)
            if images is not None:
                panel_refreshes.append(dict(iteration=i, runtime=name, image_sha256=images))
            apply_paints(expected[name], events[i, name])
    for iteration in {row['iteration'] for row in panel_refreshes}:
        copies = [row for row in panel_refreshes if row['iteration'] == iteration]
        assert {row['runtime'] for row in copies} == {'source', 'native'}
        assert copies[0]['image_sha256'] == copies[1]['image_sha256'], 'Instrument bitmap bytes differ'
    return dict(rows=rows, panel_refreshes=panel_refreshes)


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
            equivalent = row[other] if offset % 2 == 0 else row[other + '_flipped_phase']
            if 'prefix_selected_records' in state:
                verify_prefix_contacts(state, flipped, equivalent)
                continue
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


def phase_controls(rows):
    """Test counters/regular contacts even when the selected contact is absent."""
    verify(rows)
    selected = next((i for i, row in enumerate(rows) if selected_points(row['source'])), None)
    unobservable, rejections = [], {}
    for kind in ('counter_increment', 'premature_marker', 'marker_coordinate', 'other_marker'):
        changed = copy.deepcopy(rows)
        if kind in ('premature_marker', 'marker_coordinate') and selected is None:
            assert all(not selected_points(row[name])
                for row in rows for name in ('source', 'native', 'source_flipped_phase', 'native_flipped_phase'))
            unobservable.append(kind)
            continue
        index = selected if selected is not None else 0
        if kind == 'other_marker' and selected is None:
            index = next(i for i, row in enumerate(rows) if any(
                p['record_offset'] != row['source']['selected_record'] for p in row['source']['points']))
        state = changed[index]['source']
        if kind == 'counter_increment':
            state['phase_after'] += 1
        elif kind == 'other_marker':
            state['points'] = selected_points(state)
        else:
            marker = selected_points(state)[0]
            if kind == 'premature_marker':
                changed[index]['source_flipped_phase']['points'].append(copy.deepcopy(marker))
            else:
                marker['x'] += 1
        try:
            verify(changed)
        except AssertionError:
            rejections[kind] = True
        else:
            raise AssertionError(f'{kind} mutation accepted')
    return rejections, unobservable


def changed_paints(paints, kind):
    """Mutate actual operations; None means that feature is absent."""
    changed = copy.deepcopy(paints)
    if kind == 'lost_erase':
        return [p for p in changed if p['colour']] if any(not p['colour'] for p in changed) else None
    if kind == 'wrong_head_slope':
        target = next((p for p in changed if p['kind'] == 'line'), None)
        if target is not None:
            target['x1'] += 1
    else:
        assert kind == 'wrong_marker_colour'
        target = next((p for p in changed if p['kind'] != 'line' and p['colour'] in (1, 8)), None)
        if target is None:
            target = next((p for p in changed if p['kind'] != 'line' and p['colour']), None)
        if target is not None:
            target['colour'] = 1 if target['colour'] == 8 else 8
    return changed if target is not None else None


def validate_control_summary(phase, inactive_phase, paints=None, inactive_paints=()):
    """Every defined control needs a rejection or its permitted assessment."""
    inactive = set(inactive_phase)
    required = {'counter_increment', 'premature_marker', 'marker_coordinate', 'other_marker'}
    assert set(phase).isdisjoint(inactive) and set(phase) | inactive == required
    assert all(value is True for value in phase.values())
    assert inactive <= {'premature_marker', 'marker_coordinate'}
    if inactive:
        assert inactive == {'premature_marker', 'marker_coordinate'}
    if paints is not None:
        active = paints
        required = {'lost_erase', 'wrong_marker_colour', 'wrong_head_slope'}
        assert required <= set(active)
        assert all(value is True for value in active.values())
        assert set(inactive_paints) <= {'lost_panel_refresh'}


def validate_controls(report):
    """Missing observable controls cannot be relabelled as unobservable."""
    validate_control_summary(report['mutation_rejections'], report.get('unobservable_phase_mutations', []),
        report.get('paint_mutation_rejections'), report.get('unobservable_paint_mutations', []))
    if report.get('unobservable_phase_mutations'):
        assert all(not selected_points(row[name])
            for row in report['rows'] for name in ('source', 'native', 'source_flipped_phase', 'native_flipped_phase'))


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
    parser.add_argument('--paint-history', action='store_true',
        help='Predict complete live owner pages from ordered crosshair/erase/marker operations')
    parser.add_argument('--paint-planes', type=int, nargs='+', choices=range(4), default=[2],
        help='Complete planes whose cross-runtime differences must follow the paint history (default: 2)')
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
            source_entry_pages = None
            for name, index, trace_rows in (('source', observed['source_iteration'], source_rows),
                                            ('native', observed['native_iteration'], native_rows)):
                path = args.window / f'{name}.{index}.dat.gz'
                if path.exists():
                    data = gzip.decompress(path.read_bytes())
                    assert digest(data) == observed['ram_sha256'][name]
                    verify_trace(data, header, trace_rows[index])
                    if name == 'source':
                        source_entry_pages = pages(data)
                else:
                    # Pruned passing native entries can be proved only for
                    # complete page bytes by the retained actual body begin.
                    assert name == 'native' and not observed['page_differences']
                    data = gzip.decompress((args.window / f'native.{index}.before.dat.gz').read_bytes())
                    assert pages(data) == source_entry_pages
                    assert [digest(page) for page in pages(data)] == trace_rows[index]['pages']
    else:
        assert window['source_trace_sha256'] == original['source_trace_sha256']
    captures = {row['iteration']: row['snapshots'] for row in original['captures']}
    assert sorted(captures) == list(range(window['first'], window['last'] + 1))
    assert original['last_observation'] >= window['last'] + 1
    for i, snapshots in captures.items():
        data = gzip.decompress((args.original_bodies / f'source-body.{i}.owner.dat.gz').read_bytes())
        assert digest(data) == snapshots['owner']['ram_sha256']
        registers = snapshots['owner']['registers']
        assert registers['pc'] == 0xC31226
        stack_return = integer(data, registers['registers'][15], 4)
        assert stack_return == snapshots['owner-after']['registers']['pc'], (
            f'Radar {i} returned to {stack_return:06X}; capture includes another caller instruction')
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
    paint_history = []
    paint_rejections = dict(lost_erase=False, wrong_marker_colour=False, wrong_head_slope=False)
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
                        if args.paint_history:
                            expected = painted_pages(owner_input, state['paints'])
                            assert expected == observed_pages, (i, name, 'Ordered radar paints do not predict every live page byte')
                            paint_history.append(dict(iteration=i, runtime=name,
                                owner_input_sha256=digest(owner_input), owner_output_sha256=digest(observed_output),
                                predicted_pages_sha256=[digest(p) for p in expected], compared_bytes=64000,
                                pixel_163_162_before=pixel_colour(owner_input, 163, 162),
                                pixel_163_162_after=pixel_colour(observed_output, 163, 162),
                                paints=state['paints']))
                            for kind in paint_rejections:
                                changed = changed_paints(state['paints'], kind)
                                if changed is None:
                                    continue
                                if painted_pages(owner_input, changed) != observed_pages:
                                    paint_rejections[kind] = True
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
    rejections, unobservable_phase = phase_controls(rows)
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
    if unobservable_phase:
        report['unobservable_phase_mutations'] = unobservable_phase
    report['live_original_pages_matching'] = sum(row['source_live_pages_matching'] for row in rows)
    report['live_native_pages_matching'] = sum(row['native_live_pages_matching'] for row in rows)
    report['strict_live_owner_pages_matching'] = all(
        row[name + '_live_pages_matching'] for row in rows for name in ('source', 'native'))
    if args.paint_history:
        assert all(paint_rejections.values()), paint_rejections
        report['paint_history'] = paint_history
        report['paint_mutation_rejections'] = paint_rejections
        history = cross_paint_history(args.window, window, paint_history, args.paint_planes,
                                     args.original_bodies, captures, rows)
        report['cross_paint_history'] = history
        if history['panel_refreshes']:
            try:
                cross_paint_history(args.window, window, paint_history, args.paint_planes,
                                    args.original_bodies, captures, rows, omit_panel_refresh=True)
            except AssertionError as error:
                assert error.args and error.args[0][-1] == 'Radar paint history does not explain complete plane difference', error
                report['paint_mutation_rejections']['lost_panel_refresh'] = True
            else:
                report.setdefault('unobservable_paint_mutations', []).append('lost_panel_refresh')
        report['scope'] += ' Ordered radar crosshair, erase and marker writes predict every complete live owner page byte, including background-bit erasure.'
        report['scope'] += ' Cross-runtime XOR is predicted for all bytes of the requested planes from a common initial state, including actual instrument bitmap redraws.'
    if page_deltas is not None:
        report['complete_page_delta'] = page_deltas
        report['scope'] += ' Every full-page XOR byte in this bounded window equals the retained radar point-cache delta; no pixels are excluded.'
    validate_controls(report)
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'{len(rows)} original/native radar transitions, {4*len(rows)} complete owner comparisons, '
          f'{len(rows)} native body comparisons; phase offset={phase_offset}; {len(rejections)} phase mutations rejected')
    print(f"Live original owner pages: {report['live_original_pages_matching']}/{len(rows)} match; "
          f"native: {report['live_native_pages_matching']}/{len(rows)} match")
    if args.paint_history:
        print(f"Ordered paints predict {len(paint_history)} complete owner page sets; "
              f"all requested-plane XOR bytes match across {len(history['rows'])} observations")
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
    return 0 if report['strict_live_owner_pages_matching'] else 1


if __name__ == '__main__':
    sys.exit(main())
