"""Assess a complete event-anchored flight's original message/timer rules.

Uses actual independently recorded inputs and proven JSR identities. Neither
clock samples nor drawing bytes are changed to obtain equivalent events.
"""
import argparse
import copy
import gzip
import json
from pathlib import Path

from assess_mission_message_timing import assess_sequence
from check_gameplay_checkpoint import ROOT
from check_mission_message_trace import preserved
from check_qualification_message_cadence import digest
from check_recorded_original_mission_trace import (event_mapping,
    source_input_segment, verified_native_prefix, verified_update_mapping)
from compare_flight_traces import number, read_trace


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--traces', type=Path, required=True)
    parser.add_argument('--source-evidence', type=Path, required=True)
    parser.add_argument('--native-evidence', type=Path, required=True)
    parser.add_argument('--source-updates', type=Path, required=True)
    parser.add_argument('--native-prefix', type=Path, required=True)
    parser.add_argument('--runner', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    original = json.loads((args.source_evidence / 'report.json').read_text())
    baseline = json.loads((args.native_evidence / 'report.json').read_text())
    evidence = json.loads((args.traces / 'report.json').read_text())
    assert original['mission_mode'] == evidence['mission_mode'] == 5
    assert evidence['baseline_source_trace_sha256'] == original['driver_trace_sha256']
    assert evidence['baseline_native_trace_sha256'] == baseline['native_trace_sha256']
    assert baseline['native_mission_success'] and baseline['whole_successful_flight']['strict_gameplay_matching']
    _, prefix = verified_native_prefix(args.native_prefix, args.runner)
    assert prefix == evidence['native_prefix'] == baseline['native_prefix']
    assert evidence['runner_sha256'] == digest(args.runner.read_bytes())
    for name, expected in evidence['input_hashes'].items():
        assert digest((ROOT / name).read_bytes()) == expected
    paths = {name: args.traces / (name + '.jsonl.gz') for name in ('source', 'native')}
    for name, path in paths.items():
        assert digest(gzip.decompress(path.read_bytes())) == evidence[name + '_trace_sha256']
    preservation = dict(source=preserved(paths['source'], args.source_evidence / 'driver.jsonl.gz'),
        native=preserved(paths['native'], args.native_evidence / 'native.jsonl.gz'))
    source_header, source = read_trace(paths['source'])
    native_header, raw_native = read_trace(paths['native'])
    assert source_header == native_header
    mapping, updates = verified_update_mapping(args.source_updates,
        args.source_evidence / 'driver.jsonl.gz', original)
    assert updates == evidence['source_update_evidence']
    localized, _, origin = source_input_segment(mapping,
        (args.source_updates / 'update-consumed.fa18in').read_bytes(), evidence['input_segment']['first_source_observation'])
    assert origin == evidence['input_segment']['first_actual_source_update']
    shifted = {i: u + evidence['input_segment']['native_prefix_source_end'] for i, u in localized.items()}
    prefix_record = json.loads((args.native_prefix / 'report.json').read_text())
    menu = (prefix_record['canonical']['replay_iterations'], prefix_record['canonical']['frames'])
    aligned = event_mapping(evidence['event_plan'], evidence['event_report'], shifted, paths['native'], menu)
    comparison = evidence['whole_successful_flight']
    assert comparison['strict_gameplay_matching']
    first, last = comparison['first'], comparison['last']
    native = {i: raw_native[aligned[i]] for i in range(first, last + 1)}
    assets = gzip.decompress((args.source_evidence / 'driver.dat.gz').read_bytes())
    assert digest(assets) == original['driver_final_ram_sha256']
    report = dict(mission_mode=5, first=first, last=last, observations=last-first+1,
        trace_sha256={name: evidence[name + '_trace_sha256'] for name in paths},
        preserved_observations=preservation, assets_sha256=digest(assets),
        runner_sha256=evidence['runner_sha256'], native_prefix=prefix,
        execution_alignment=comparison['execution_alignment'],
        strict_pages_matching=comparison['complete_pages_matching'],
        executed_tool_sha256=digest(Path(__file__).read_bytes()))
    try:
        report['source'] = assess_sequence(source, first, last, assets, mapping)
        report['native'] = assess_sequence(native, first, last, assets, mapping)
    except AssertionError as error:
        report.update(accepted_evidence=False, rejection=str(error),
            scope='Complete message assessment rejected; original and native captures retained unchanged')
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
        raise
    shared = ('notification_countdown', 'message_code', 'message_loaded',
              'message_shown', 'message_countdown', 'message_time',
              'cockpit_flags', 'threat_events', 'player_phase')
    def check_producers(other):
        for i in range(first, last + 1):
            assert all(source[i]['fields'][name] == other[i]['fields'][name] for name in shared), ('message producer differs', i)
    check_producers(native)
    report.update(producer_fields=list(shared), producer_boundaries_matching=last-first+1)
    first_cycle = {}
    for name, rows in (('source', source), ('native', native)):
        acquired = next(e['iteration'] for e in report[name]['page_events'] if e['page'] == 1)
        heading = next(e['iteration'] for e in report[name]['page_events'] if e['page'] == 2 and e['iteration'] > acquired)
        selected = number(rows[acquired], 'selected_record')
        assert all(number(rows[i], 'selected_record') == selected for i in range(acquired, heading+1)), 'first heading interval changed selection'
        seconds_before_hud = sum(rows[i-1]['fields']['previous_seconds'] != rows[i]['fields']['previous_seconds']
            for i in range(acquired, heading) if mapping[i] != mapping[i-1])
        assert seconds_before_hud == 2, 'first heading did not follow the original request countdown'
        first_cycle[name] = dict(acquisition_observation=acquired, acquisition_tick=number(rows[acquired], 'game_tick'),
            heading_observation=heading, heading_tick=number(rows[heading], 'game_tick'),
            sampled_second_changes_before_hud=seconds_before_hud,
            elapsed_ms=(number(rows[heading], 'elapsed_total')-number(rows[acquired], 'elapsed_total')) & 0xffffffff,
            heading_text=bytes.fromhex(rows[heading]['fields']['message_line']).decode('latin1'))
    report['equivalent_first_heading_event'] = first_cycle
    # Probe actual checked transitions, rather than fitting observation numbers
    # from the earlier mission-three recording to this different flight.
    active = next(i for i in range(first+1, last+1) if number(native[i], 'post_input_aux') and mapping[i] != mapping[i-1])
    seconds = next(i for i in range(first+1, last+1) if number(native[i], 'post_input_aux') and
        native[i]['fields']['previous_seconds'] != native[i-1]['fields']['previous_seconds'])
    fresh = next(e['iteration'] for e in report['native']['page_events'] if e['page'] in (1, 2, 3))
    paused = next(i for i in range(first+1, last+1) if not number(native[i], 'post_input_aux') and mapping[i] != mapping[i-1])
    lost = report['native']['selection_losses'][0]
    cases = [('wrong_notification', active, 'notification_countdown'),
             ('lost_sampled_second', seconds, 'previous_seconds'),
             ('premature_info_page', fresh, 'info_page'),
             ('wrong_fresh_text', fresh, 'message_line'),
             ('changed_paused_text', paused, 'message_line'),
             ('lost_selection_page_reset', lost, 'info_page'),
             ('wrong_selection_loss_blank', lost, 'message_line'),
             ('wrong_message_colour', active, 'message_kind')]
    rejected = {}
    for name, i, field in cases:
        changed = dict(native)
        changed[i] = copy.deepcopy(changed[i])
        value = bytes.fromhex(changed[i]['fields'][field])
        changed[i]['fields'][field] = (native[i-1]['fields'][field] if name == 'lost_sampled_second'
            else bytes([value[0] ^ 1, *value[1:]]).hex())
        try:
            assess_sequence(changed, first, last, assets, mapping)
        except AssertionError as error:
            rejected[name] = dict(iteration=i, reason=str(error))
        else:
            raise AssertionError(f'{name}: wrong result accepted')
    changed = dict(native)
    changed[active] = copy.deepcopy(changed[active])
    value = number(changed[active], 'message_code') ^ 1
    changed[active]['fields']['message_code'] = f'{value:04x}'
    try:
        check_producers(changed)
    except AssertionError as error:
        rejected['wrong_priority_producer'] = dict(iteration=active, reason=str(error))
    else:
        raise AssertionError('wrong priority producer accepted')
    report.update(accepted_evidence=True, negative_controls_rejected=rejected,
        scope='Complete recorded flight: original elapsed/countdown, delay/redraw, full text cache/formatting and message-kind rules on each independent runtime. Every transition and duplicate identity remains represented. Priority producers agree. Strict full pages remain unchanged diagnostics; other cockpit drawing and original sound timing are not accepted by this check.')
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'{last-first+1} complete flight observations assessed; {len(rejected)} wrong message/timer results rejected')


if __name__ == '__main__':
    main()
