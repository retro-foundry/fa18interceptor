"""Check the complete recorded mission-three message cache against source owners.

No clocks, input, state or pixels are changed. The diagnostic keeps strict
whole-page differences and requires every observed message transition.
"""
import argparse
import copy
import gzip
import json
from pathlib import Path

from assess_demo_hud_timing import elapsed_delta, information_line, signed
from check_gameplay_checkpoint import ROOT, span
from check_qualification_message_cadence import digest
from check_recorded_original_mission_trace import verified_update_mapping
from check_mission_message_trace import preserved
from compare_flight_traces import number, read_trace


def entry_line(assets, code):
    return span(assets, 0xC3D0A0 + 28 * (code & 255) + 1, 26).hex()


def delay_reset(current, previous):
    # C31722's priority order; missile bits bypass the vicinity delay.
    if current & 0x30:
        return False
    if current & 8:
        return not previous & 8 or (current & 4 and not previous & 4)
    return bool(current & 4 and not previous & 4)


def assess_sequence(rows, first, last, assets, identities=None):
    counts = dict(transitions=0, duplicate_observations=0, idle=0,
                  fresh_information=0, cached_information=0, blanking=0,
                  fresh_message=0, cached_message=0, context_suppressed=0,
                  seconds_changed=0, notification_steps=0, message_kinds=0,
                  delay_resets=[], redraw_events=[], page_events=[])
    cache_names = ('info_page', 'info_request', 'info_redraws', 'info_delay',
                   'message_drawn', 'message_line')
    for i in range(first + 1, last + 1):
        a, b = rows[i - 1], rows[i]
        if identities and identities[i] == identities[i - 1]:
            assert all(a[k] == b[k] for k in a if k not in ('iteration', 'frame')), ('duplicate changed', i)
            counts['duplicate_observations'] += 1
            continue
        counts['transitions'] += 1
        assert number(b, 'notification_countdown') == (number(a, 'notification_countdown') - 2) % 8 + 1, ('notification cadence', i)
        counts['notification_steps'] += 1
        if not number(b, 'post_input_aux'):
            assert all(a['fields'][n] == b['fields'][n] for n in cache_names), ('idle cache changed', i)
            assert all(a['fields'][n] == b['fields'][n] for n in
                       ('elapsed_total', 'previous_seconds', 'previous_fraction')), ('idle timer changed', i)
            counts['idle'] += 1
            continue
        assert not number(a, 'timer_flags') & 0x100, ('timer reset needs assessment', i)
        assert number(b, 'elapsed_total') == (number(a, 'elapsed_total') + elapsed_delta(a, b)) & 0xFFFFFFFF, ('elapsed', i)
        page, request, redraws, delay, drawn = (number(a, n) for n in cache_names[:-1])
        selected = number(b, 'selected_record')
        if selected != number(a, 'selected_record'):
            if signed(selected, 16) >= 0:
                page, request, delay = 1, 1, 255  # C3180C acquisition.
            else:
                # C3180C's empty-list return and release_lost_selection only
                # clear selection. C322EE owns subsequent page blanking; no
                # acquisition stores to INFO_PAGE/REQUEST/DELAY occur here.
                assert selected == 0xffff, ('invalid lost selection', i)
                counts.setdefault('selection_losses', []).append(i)
        if delay_reset(number(b, 'threat_events'), number(a, 'threat_events')):
            delay = 24
            counts['delay_resets'].append(i)
        # C12098 requests three passes throughout its slide, including the
        # final call clearing bit 10. C1B906/C1BA82 cover view changes/expiry.
        redraw = (number(a, 'context_select') != number(b, 'context_select') or
                  number(a, 'view_mode') != number(b, 'view_mode') or
                  (not number(a, 'context_select') and not number(a, 'view_hold')) or
                  bool((number(a, 'cockpit_flags') | number(b, 'cockpit_flags')) & 0x400))
        if redraw:
            redraws = 3
            counts['redraw_events'].append(i)
        called = not number(b, 'context_select') or number(a, 'update_hud_mode') or redraw
        line = a['fields']['message_line']
        # C11BFC loads flags only when the low entry byte changes, then
        # publishes MESSAGE_KIND before C322EE can set a target-info colour.
        kind = number(a, 'message_flags') & 0xC0
        if number(a, 'message_loaded') & 255 != number(b, 'message_loaded') & 255:
            kind = span(assets, 0xC3D0A0 + 28 * (number(b, 'message_loaded') & 255), 1)[0] & 0xC0
        flags, kind_redraws = kind, number(a, 'message_redraws')
        if kind != number(a, 'message_kind'):
            kind_redraws = 2
        drawing = False
        if not called:
            counts['context_suppressed'] += 1
        else:
            eligible = signed(selected, 16) >= 0 and not number(b, 'cockpit_flags') & 0x81
            if eligible:
                delay = (delay - 1) & 255
            if eligible and signed(delay, 8) < 0:
                delay = 255
                fresh = False
                if signed(redraws, 8) <= 0:
                    if signed(request, 8) < 0:
                        request = 1
                        page = (page + 1) & 65535
                        if signed(page, 16) > 3:
                            page = 1
                        fresh = True
                    elif request & 1:
                        fresh = True
                    if fresh:
                        page |= 0x8000
                        redraws = 2
                if page & 0x8000:
                    page &= 0x7FFF
                    line = information_line(b, page, assets)
                    counts['fresh_information'] += 1
                    record = bytes.fromhex(b['records'][selected // 512])
                    flags = (0 if not record[0x20] & 0x40 else
                             0x40 if record[0x62] in (0x10, 0x14, 0x16, 0x17) else 0x80)
                    drawing = True
                else:
                    if signed(redraws, 8) > 0:
                        redraws = (redraws - 1) & 255
                        drawing = True
                    counts['cached_information'] += 1
            elif page:
                page = 65535 if signed(page, 16) > 0 else 0
                line = entry_line(assets, 0)
                counts['blanking'] += 1
                drawing = True
            elif number(b, 'message_shown') != drawn:
                drawn = number(b, 'message_shown')
                line = entry_line(assets, drawn)
                redraws = 2
                counts['fresh_message'] += 1
                drawing = True
            else:
                if signed(redraws, 8) > 0:
                    redraws = (redraws - 1) & 255
                    drawing = True
                counts['cached_message'] += 1
        if drawing and not number(b, 'context_select') and signed(kind_redraws, 8) >= 0:
            kind_redraws = (kind_redraws - 1) & 255
        assert (kind, flags, kind_redraws) == tuple(number(b, n) for n in
            ('message_kind', 'message_flags', 'message_redraws')), ('message kind/cache', i)
        counts['message_kinds'] += 1
        if a['fields']['previous_seconds'] != b['fields']['previous_seconds']:
            counts['seconds_changed'] += 1
            if signed(request, 8) >= 0:
                request = (request - 1) & 255
        if not number(b, 'origin_detail_mode') and number(a, 'game_tick') & 31 == 16:
            redraws = 3  # C0F350 redraw after HUD/timer, using saved tick.
        expected = (page, request, redraws, delay, drawn, line)
        actual = tuple(number(b, n) for n in cache_names[:-1]) + (b['fields']['message_line'],)
        assert expected == actual, (i, dict(zip(cache_names, expected)), dict(zip(cache_names, actual)))
        if page != number(a, 'info_page'):
            counts['page_events'].append(dict(iteration=i, tick=number(b, 'game_tick'),
                page=page, seconds_changed=counts['seconds_changed'], message=bytes.fromhex(line).decode('latin1')))
    return counts


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--traces', type=Path, required=True)
    parser.add_argument('--source-evidence', type=Path, required=True)
    parser.add_argument('--native-evidence', type=Path, required=True)
    parser.add_argument('--source-updates', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    reference = json.loads((args.traces / 'report.json').read_text())
    original = json.loads((args.source_evidence / 'report.json').read_text())
    native_baseline = json.loads((args.native_evidence / 'report.json').read_text())
    assert reference['baseline_source_trace_sha256'] == original['driver_trace_sha256']
    assert reference['baseline_native_trace_sha256'] == native_baseline['native_trace_sha256']
    for path, expected in reference['input_hashes'].items():
        assert digest((ROOT / path).read_bytes()) == expected
    traces, headers = {}, {}
    for name in ('source', 'native'):
        path = args.traces / f'{name}.jsonl.gz'
        assert digest(gzip.decompress(path.read_bytes())) == reference[f'{name}_trace_sha256']
        headers[name], traces[name] = read_trace(path)
    assert headers['source'] == headers['native']
    assets = gzip.decompress((args.source_evidence / 'driver.dat.gz').read_bytes())
    assert digest(assets) == original['driver_final_ram_sha256']
    comparison = reference['whole_successful_flight']
    first, last = comparison['first'], comparison['last']
    mapping, update_evidence = verified_update_mapping(args.source_updates,
        args.source_evidence / 'driver.jsonl.gz', original)
    assert update_evidence == reference['source_update_evidence']
    report = dict(assets_sha256=digest(assets), trace_sha256={n: reference[f'{n}_trace_sha256'] for n in traces})
    report['preserved_observations'] = {
        'source': preserved(args.traces / 'source.jsonl.gz', args.source_evidence / 'driver.jsonl.gz'),
        'native': preserved(args.traces / 'native.jsonl.gz', args.native_evidence / 'native.jsonl.gz')}
    report['source'] = assess_sequence(traces['source'], first, last, assets, mapping)
    native = {i: traces['native'][mapping[i]] for i in range(first, last + 1)}
    report['native'] = assess_sequence(native, first, last, assets, mapping)
    # Unlike elapsed-second target cycling, priority message producers have
    # identical cadence and outputs throughout this independently started run.
    shared = ('notification_countdown', 'message_code', 'message_loaded',
              'message_shown', 'message_countdown', 'message_time',
              'cockpit_flags', 'threat_events', 'player_phase')
    def compare_producers(other):
        for i in range(first, last + 1):
            assert all(traces['source'][i]['fields'][n] == other[i]['fields'][n]
                       for n in shared), ('message producer differs', i)
    compare_producers(native)
    report['complete_producer_boundaries'] = last - first + 1
    report['producer_fields'] = list(shared)
    # Acquisition and the next view redraw define the first uninterrupted
    # page interval. Compare countdown events, never search for later pixels.
    equivalent = {}
    for name in ('source', 'native'):
        state = report[name]
        acquired = state['page_events'][0]
        redraw = next(i for i in state['redraw_events'] if i > acquired['iteration'])
        equivalent[name] = [(e['page'], e['seconds_changed'] - acquired['seconds_changed'])
                           for e in state['page_events'] if e['iteration'] < redraw]
    se, ne = equivalent['source'], equivalent['native']
    assert len(se) >= len(ne) >= 2 and se[:len(ne)] == ne
    report['equivalent_initial_page_events'] = ne
    report['first_heading'] = {name: next(e for e in report[name]['page_events'] if e['page'] == 2)
                               for name in ('source', 'native')}
    rejected = []
    second_change = next(i for i in range(22303, last + 1) if
        native[i]['fields']['previous_seconds'] != native[i - 1]['fields']['previous_seconds'])
    cases = [('premature_page', 22309, 'info_page', '0002'),
             ('lost_second_event', second_change, 'previous_seconds', native[second_change - 1]['fields']['previous_seconds']),
             ('wrong_refreshed_text', 22326, 'message_line', '20' * 26),
             ('wrong_held_text', 23369, 'message_line', '20' * 26),
             ('lost_vicinity_delay', 23311, 'info_delay', 'ff'),
             ('wrong_notification_step', 23369, 'notification_countdown', '00'),
             ('wrong_message_colour', 25698, 'message_kind', 'c0'),
             ('wrong_warning_text', 26233, 'message_line', '20' * 26)]
    for name, i, field, value in cases:
        changed = dict(native)
        changed[i] = copy.deepcopy(changed[i])
        assert changed[i]['fields'][field] != value, ('mutation ineffective', name)
        changed[i]['fields'][field] = value
        try:
            assess_sequence(changed, first, last, assets, mapping)
        except AssertionError:
            rejected.append(name)
        else:
            raise AssertionError(f'{name} mutation accepted')
    changed = dict(native)
    changed[26233] = copy.deepcopy(changed[26233])
    changed[26233]['fields']['message_code'] = '4006'
    try:
        compare_producers(changed)
    except AssertionError:
        rejected.append('wrong_priority_message')
    else:
        raise AssertionError('wrong priority message accepted')
    report['negative_controls_rejected'] = rejected
    report.update(first=first, last=last,
        strict_pages_matching=comparison['complete_pages_matching'],
        strict_page_boundaries=comparison['compared'],
        runner_sha256=reference['runner_sha256'],
        acceptance='The complete recorded mission obeys original elapsed-second page cycling, '
                   'delay/redraw gates, full text caching/formatting and message-kind rules. '
                   'Priority-message producer fields match at all real update identities. '
                   'The initial ALT/HDG difference is assessed under the accepted cadence policy. '
                   'Strict complete pages remain reported; other drawing, other flights and audio '
                   'are not accepted by this check.')
    args.out.mkdir(parents=True, exist_ok=True)
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print('All complete mission message transitions obey source cache, countdown and formatting rules')


if __name__ == '__main__':
    main()
