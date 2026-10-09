"""Assess the recorded mode-three target-info episode from actual clock events.

Never supplies clocks/state to either game or searches for matching pictures.
The first information page and subsequent selection loss define the interval.
Both complete drawing pages remain the separate strict diagnostic.
"""
import argparse
import copy
import gzip
import hashlib
import json
from pathlib import Path

from check_gameplay_checkpoint import ROOT, span
from compare_flight_traces import number, read_trace


def data_bytes(path):
    data = path.read_bytes()
    return gzip.decompress(data) if path.suffix == '.gz' else data


def text_asset(data, address):
    return span(data, address, 27).split(b'\0', 1)[0]


def signed(value, bits):
    value &= (1 << bits) - 1
    return value - (1 << bits) if value & (1 << (bits - 1)) else value


def divu_word(value, divisor):
    dividend = value & 0xFFFFFFFF
    quotient = dividend // divisor
    return signed(dividend if quotient > 0xFFFF else quotient, 16)


def elapsed_delta(before, after):
    # C25312 elapsed(): MULU.W seconds and DIVS.W fractional microseconds.
    seconds = (number(after, 'previous_seconds') - number(before, 'previous_seconds')) & 0xFFFF
    fraction = signed(number(after, 'previous_fraction') - number(before, 'previous_fraction'), 32)
    quotient = (abs(fraction) // 1000) * (-1 if fraction < 0 else 1)
    fraction_ms = signed(quotient if -32768 <= quotient <= 32767 else fraction, 16)
    return (seconds * 1000 + fraction_ms) & 0xFFFFFFFF


def formatted_value(value, width, keep_zeros):
    # C3267A/C327C0: packed decimal accumulation, including 32-bit carry.
    bcd, digit = 0, 0x10000000
    value &= 0xFFFFFFFF
    for place in (10000000, 1000000, 100000, 10000, 1000, 100, 10, 1):
        quotient, value = divmod(value, place)
        bcd = (bcd + quotient * digit) & 0xFFFFFFFF
        digit >>= 4
    output = bytes(48 + ((bcd >> (4 * i)) & 15) for i in reversed(range(width)))
    if not keep_zeros:
        output = output[:-1].lstrip(b'0').rjust(width - 1, b' ') + output[-1:]
    return output


def information_line(row, page, assets):
    offset = number(row, 'selected_record')
    assert offset < 16 * 512 and offset % 512 == 0, 'invalid selected record'
    record = bytes.fromhex(row['records'][offset // 512])
    line = bytearray(text_asset(assets, 0xC326B8))
    assert len(line) == 26
    names = {0x12: 0xC326E2, 0x13: 0xC326E9, 0x14: 0xC326F0,
             0x16: 0xC3270C, 0x17: 0xC32713, 0x10: 0xC326F7, 0x15: 0xC326FE}
    name_address = names.get(record[0x62]) if record[0x20] & 0x40 else 0xC32705
    if name_address:
        name = text_asset(assets, name_address)
        line[4:4 + len(name)] = name
    if page == 1:
        value = (int.from_bytes(record[0x18:0x1C], 'big', signed=True) >> 10) * 5
        label, end, width, zeros = 0xC326D3, 23, 5, False
    elif page == 2:
        value = divu_word(int.from_bytes(record[0x68:0x6A], 'big', signed=True) >> 3, 10)
        label, end, width, zeros = 0xC326D8, 22, 3, True
    else:
        assert page == 3
        speed = int.from_bytes(record[0x6E:0x70], 'big', signed=True)
        speed = signed(-speed, 16) if speed < 0 else speed
        value = divu_word(0 if record[0] & 0x80 else speed, 12)
        label, end, width, zeros = 0xC326DD, 22, 4, False
    label = text_asset(assets, label)
    line[13:13 + len(label)] = label
    line[end - width:end] = formatted_value(value, width, zeros)
    assert len(line) == 26
    return line.hex()


def assess_episode(rows, flight_first, flight_last, assets):
    first = next(i for i in range(flight_first, flight_last + 1)
                 if number(rows[i], 'info_page') == 1)
    lost = next(i for i in range(first, flight_last + 1)
                if number(rows[i], 'selected_record') == 0xFFFF)
    last = lost + 1  # Include both the blanking and -1 -> 0 page reset.
    start = rows[first - 1]
    assert number(start, 'info_page') == 0 and number(start, 'info_request') == 255
    assert number(start, 'selected_record') == 0xFFFF
    assert number(rows[first], 'selected_record') == 10 * 512
    report = dict(first=first, last=last, transitions_checked=last - first + 1,
                  sampled_second_changes=0, fresh_lines_checked=0,
                  elapsed_intervals_checked=0,
                  cached_lines_checked=0, context_suppressed=0, redraw_events=[],
                  page_events=[], first_context=None)
    for i in range(first - 1, last):
        a, b = rows[i], rows[i + 1]
        assert all(number(r, 'stage') == 0xC10DAE and number(r, 'mode') == 3 and
                   number(r, 'post_input_aux') for r in (a, b))
        assert not number(a, 'timer_flags') & 0x100, 'timer reset needs separate assessment'
        assert number(b, 'elapsed_total') == (number(a, 'elapsed_total') + elapsed_delta(a, b)) & 0xFFFFFFFF, (
            f'elapsed accumulator mismatch at {i + 1}')
        report['elapsed_intervals_checked'] += 1
        page, request, redraws = (number(a, n) for n in ('info_page', 'info_request', 'info_redraws'))
        if i + 1 == first:
            # C3180C's selected-record scan publishes page one/request one
            # before the HUD on this acquisition, rather than cycling page zero.
            page, request = 1, 1
        # C12098/C1B906 and view commands request the same three-pass redraw.
        # These are observed state changes, not inferred from pixel similarity.
        redraw = a['fields']['context_select'] != b['fields']['context_select'] or (
            a['fields']['view_mode'] != b['fields']['view_mode']) or (
            not number(a, 'context_select') and number(a, 'view_hold') == 0)
        if redraw:
            redraws = 3
            report['redraw_events'].append(i + 1)
        # C0F138 chooses the context branch before panel counters decrement.
        # Mode three omits the message owner in that branch unless redrawing.
        called = not number(b, 'context_select') or number(a, 'update_hud_mode') != 0 or redraw
        if not called:
            report['context_suppressed'] += 1
        if report['first_context'] is None and number(b, 'context_select'):
            report['first_context'] = i + 1
        fresh = False
        expected_line = a['fields']['message_line']
        if called:
            if number(b, 'selected_record') == 0xFFFF:
                page = 0xFFFF if 0 < page < 0x8000 else 0
                expected_line = text_asset(assets, 0xC326B8).hex()
            else:
                assert number(b, 'info_delay') == 255 and not number(b, 'cockpit_flags') & 0x81
                if signed(redraws, 8) <= 0:
                    if signed(request, 8) < 0:
                        request = 1
                        page = page + 1 if signed(page + 1, 16) <= 3 else 1
                        fresh = True
                    elif request & 1:
                        fresh = True
                if fresh:
                    expected_line = information_line(b, page, assets)
        if fresh:
            report['fresh_lines_checked'] += 1
        else:
            report['cached_lines_checked'] += 1
        before_timer = report['sampled_second_changes']
        changed = a['fields']['previous_seconds'] != b['fields']['previous_seconds']
        if changed:
            report['sampled_second_changes'] += 1
            if signed(request, 8) >= 0:
                request = (request - 1) & 255  # C25312 -> C25482, once.
        assert (page, request) == (number(b, 'info_page'), number(b, 'info_request')), (
            f'countdown/page mismatch at {i + 1}: expected {page}/{request}')
        assert b['fields']['message_line'] == expected_line, f'message mismatch at {i + 1}'
        if page != number(a, 'info_page'):
            report['page_events'].append(dict(iteration=i + 1, tick=number(b, 'game_tick'),
                page=page, sampled_second_events_before_hud=before_timer,
                elapsed_ms=(number(b, 'elapsed_total') - number(rows[first], 'elapsed_total')) & 0xFFFFFFFF,
                prelude_seconds=number(b, 'previous_seconds'),
                prelude_fraction=number(b, 'previous_fraction'),
                message=bytes.fromhex(b['fields']['message_line']).decode('latin1')))
    report['first_page_request'] = number(rows[first], 'info_request')
    report['initial_second_change'] = rows[first - 1]['fields']['previous_seconds'] != rows[first]['fields']['previous_seconds']
    report['game_ticks'] = [number(start, 'game_tick'), number(rows[last], 'game_tick')]
    return report


def verify_previous(directory, previous, current):
    old_report = json.loads((previous / 'report.json').read_text())
    report = json.loads((directory / 'report.json').read_text())
    checked = {}
    for name, rows in current.items():
        _, old = read_trace(previous / f'{name}.jsonl.gz')
        assert rows.keys() == old.keys()
        assert report[f'{name}_run'] == old_report[f'{name}_run']
        assert report[f'{name}_final_ram_sha256'] == old_report[f'{name}_final_ram_sha256']
        for i, row in rows.items():
            a = old[i]
            for field in ('frame', 'records', 'pages', 'pages_valid', 'draw_page', 'page_table'):
                assert a[field] == row[field], f'{name}/{i}: changed {field}'
            for field, value in a['fields'].items():
                assert row['fields'][field] == value, f'{name}/{i}: changed {field}'
        checked[name] = len(rows)
    return checked


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--traces', type=Path, required=True)
    parser.add_argument('--previous', type=Path, required=True)
    parser.add_argument('--assets', type=Path, default=ROOT / 'build/native-flight/demo-later-review/source.2401.dat.gz')
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    headers, rows = {}, {}
    reference = json.loads((args.traces / 'report.json').read_text())
    for name in ('source', 'native'):
        path = args.traces / f'{name}.jsonl.gz'
        assert hashlib.sha256(data_bytes(path)).hexdigest() == reference[f'{name}_trace_sha256']
        headers[name], rows[name] = read_trace(path)
        assert headers[name]['format'] == 'FA18_FLIGHT_TRACE_V2'
    assert headers['source'] == headers['native']
    assets = data_bytes(args.assets)
    report = dict(previous_execution_preserved=verify_previous(args.traces, args.previous, rows),
                  assets_sha256=hashlib.sha256(assets).hexdigest(), input_hashes=reference['input_hashes'],
                  trace_sha256={name: reference[f'{name}_trace_sha256'] for name in ('source', 'native')})
    for name in ('source', 'native'):
        first = reference[f'{name}_first']
        report[name] = assess_episode(rows[name], first, first + reference['first_flight']['compared'] - 1, assets)
    # Compare the first uninterrupted cycling interval at equivalent countdown
    # events, including the source's decrement on the very first ALT frame.
    def uninterrupted(result):
        return [(e['page'], e['sampled_second_events_before_hud']) for e in result['page_events']
                if result['first'] < e['iteration'] < result['first_context']]
    se, ne = uninterrupted(report['source']), uninterrupted(report['native'])
    assert se and ne and len(se) >= len(ne), 'missing equivalent countdown-event coverage'
    assert se[:len(ne)] == ne
    report['equivalent_initial_countdown_events'] = len(ne)
    # Deliberate wrong pages, clock events, refreshed values and held text must
    # all fail. These are the defects a broad HUD exclusion would have hidden.
    cases = [('premature_page', 2415, 'info_page', '0002'),
             ('lost_second_event', 2429, 'previous_seconds', rows['native'][2428]['fields']['previous_seconds']),
             ('wrong_refreshed_text', 2430, 'message_line', '20' * 26),
             ('wrong_held_text', 2740, 'message_line', '20' * 26)]
    rejected = []
    for name, iteration, field, value in cases:
        changed = dict(rows['native'])
        changed[iteration] = copy.deepcopy(changed[iteration])
        changed[iteration]['fields'][field] = value
        try:
            assess_episode(changed, reference['native_first'],
                reference['native_first'] + reference['first_flight']['compared'] - 1, assets)
        except AssertionError:
            rejected.append(name)
        else:
            raise AssertionError(f'accepted {name}')
    report['negative_controls_rejected'] = rejected
    report['strict_drawing'] = reference['first_flight']['complete_pages_matching']
    report['strict_drawing_boundaries'] = reference['first_flight']['compared']
    report['acceptance'] = ('Mode-three recorded target-info episode follows original sampled-second, '
        'context, redraw and formatting rules. Initial ALT/HDG divergence is allowed cadence '
        'under the existing timing policy. Complete flight drawing and all missions remain separate.')
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({name: {k: report[name][k] for k in ('transitions_checked', 'fresh_lines_checked',
          'cached_lines_checked', 'context_suppressed')} for name in ('source', 'native')}))
    print('Original target-info timers and cached/refreshed text verified; four wrong-result mutations rejected')


if __name__ == '__main__':
    main()
