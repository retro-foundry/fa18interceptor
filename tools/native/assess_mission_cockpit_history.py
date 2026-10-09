"""Compose certified radar, bitmap and message writes without masking pages.

This diagnostic reads actual independent entries and separately validated owner
captures. Only independent expected buffers receive writes; all eight complete
page XORs must follow the history from an identical initial state.
"""
import argparse
import gzip
import json
from pathlib import Path

from assess_mission_radar_cadence import apply_paints, painted_pages, validate_controls
from check_gameplay_checkpoint import ROOT, integer
from check_mission_message_pages import predicted_pages, verify_return
from check_mission_message_trace import preserved
from check_qualification_message_cadence import digest, pages, verify_trace, instrument_panel_refresh
from compare_flight_traces import read_trace


def ram(path):
    return gzip.decompress(path.read_bytes())


def validate_history_controls(report):
    controls = report['history_mutation_rejections']
    assert set(controls) == {'lost_panel_refresh', 'lost_radar_writes', 'lost_message_writes'}
    source_variants = {'lost_panel_refresh': 'lost_source_panel_refresh', 'lost_radar_writes': 'lost_source_radar_writes'}
    for kind, control in controls.items():
        assert control['rejected'] is True
        assert report['first'] <= control['first_difference'] <= report['last']
        if 'actual_mutation' in control:
            assert kind in source_variants and control['actual_mutation'] == source_variants[kind]
            assert control['balanced_mutation_unobservable'] is True
        else:
            assert 'balanced_mutation_unobservable' not in control


def predict_history(window_path, window, radar, message, radar_bodies, message_bodies,
                    message_pages, headers, traces, mutation=None):
    radar_rows = {row['iteration']: row for row in radar['rows']}
    message_rows = {row['iteration']: row for row in message['rows']}
    captures = {row['iteration']: row['snapshots'] for row in
                json.loads((radar_bodies / 'report.json').read_text())['captures']}
    message_captures = {row['iteration']: row['snapshots'] for row in
                        json.loads((message_bodies / 'report.json').read_text())['captures']}
    expected, previous_draw, results = {}, {}, []
    for observed in window['rows']:
        i, j = observed['source_iteration'], observed['native_iteration']
        live = {}
        for name, index in (('source', i), ('native', j)):
            path = window_path / f'{name}.{index}.dat.gz'
            if path.exists():
                data = ram(path)
                assert digest(data) == observed['ram_sha256'][name]
                verify_trace(data, headers[name], traces[name][index])
            else:
                assert name == 'native' and not observed['page_differences']
                data = ram(window_path / f'native.{index}.before.dat.gz')
                assert pages(data) == live['source']
            live[name] = pages(data)
            assert [digest(page) for page in live[name]] == traces[name][index]['pages']
            draw = integer(data, 0xC4566C, 2)
            if name not in expected:
                expected[name] = [bytearray(page) for page in live[name]]
            elif previous_draw[name] != draw:
                expected[name] = expected[name][4:] + expected[name][:4]
            previous_draw[name] = draw
        assert traces['source'][i]['records'] == traces['native'][j]['records']
        if not results:
            assert live['source'] == live['native'], 'History must start with all eight complete pages identical'
        deltas = [bytes(a ^ b for a, b in zip(x, y)) for x, y in zip(live['source'], live['native'])]
        predicted = [bytes(a ^ b for a, b in zip(x, y)) for x, y in zip(expected['source'], expected['native'])]
        assert deltas == predicted, (i, 'Combined writes do not explain every complete page byte')
        result = dict(iteration=i, planes=list(range(8)), compared_bytes=64000,
                      difference_bytes=[sum(bool(byte) for byte in delta) for delta in deltas],
                      delta_sha256=digest(b''.join(deltas)))
        refreshes = {}
        for name in ('source', 'native'):
            if name == 'source':
                body = ram(message_bodies / f'source-body.{i}.before.dat.gz')
                assert digest(body) == message_captures[i]['before']['ram_sha256'] == captures[i]['before']['ram_sha256']
                radar_input = ram(radar_bodies / f'source-body.{i}.owner.dat.gz')
                radar_output = ram(radar_bodies / f'source-body.{i}.owner-after.dat.gz')
                assert digest(radar_input) == captures[i]['owner']['ram_sha256'] == radar_rows[i]['source_owner_input_sha256']
                assert digest(radar_output) == captures[i]['owner-after']['ram_sha256'] == radar_rows[i]['source_owner_output_sha256']
                registers = captures[i]['owner']['registers']
                assert registers['pc'] == 0xC31226
                assert integer(radar_input, registers['registers'][15], 4) == captures[i]['owner-after']['registers']['pc'] == 0xC0F18E
                assert painted_pages(radar_input, radar_rows[i][name]['paints']) == pages(radar_output)
                before = ram(message_bodies / f'source-body.{i}.owner.dat.gz')
                after = ram(message_bodies / f'source-body.{i}.owner-after.dat.gz')
                assert digest(before) == message_captures[i]['owner']['ram_sha256']
                assert digest(after) == message_captures[i]['owner-after']['ram_sha256']
                verify_return(before, message_captures[i])
            else:
                body = ram(window_path / f'native.{j}.before.dat.gz')
                assert digest(body) == message_rows[i]['native_body_input_sha256'] == radar_rows[i]['native_body_input_sha256']
                output = ram(window_path / f'native.{j}.after.dat.gz')
                assert digest(output) == message_rows[i]['native_body_output_sha256'] == radar_rows[i]['native_body_output_sha256']
                before = ram(message_pages / f'native-owner.{i}.before.dat.gz')
                after = ram(message_pages / f'native-owner.{i}.after.dat.gz')
            state = message_rows[i][name]
            assert digest(before) == state['owner_input_sha256']
            assert digest(after) == state['owner_output_sha256']
            assert predicted_pages(before, after, state['text_drawn']) == pages(after)
            refreshes[name] = instrument_panel_refresh(body, expected[name], apply=mutation != 'lost_panel_refresh' and
                not (mutation == 'lost_source_panel_refresh' and name == 'source'))
            if mutation != 'lost_radar_writes' and not (mutation == 'lost_source_radar_writes' and name == 'source'):
                apply_paints(expected[name], radar_rows[i][name]['paints'])
            if mutation != 'lost_message_writes':
                expected[name] = predicted_pages(before, after, state['text_drawn'], base=expected[name])
        assert refreshes['source'] == refreshes['native'], 'Instrument redraw requests or images differ'
        result['panel_refresh_sha256'] = refreshes['source']
        results.append(result)
    return results


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('window', 'radar-history', 'radar-bodies', 'message-pages', 'message-bodies',
                 'trace-evidence', 'source-evidence', 'out'):
        parser.add_argument('--' + name, type=Path, required=True)
    args = parser.parse_args()
    window = json.loads((args.window / 'report.json').read_text())
    radar = json.loads(args.radar_history.read_text())
    message = json.loads((args.message_pages / 'report.json').read_text())
    reference = json.loads((args.trace_evidence / 'report.json').read_text())
    assert reference['whole_successful_flight']['strict_gameplay_matching']
    assert reference['runner_sha256'] == window['runner_sha256']
    for path, expected_hash in reference['input_hashes'].items():
        assert digest((ROOT / path).read_bytes()) == expected_hash, path
    assert preserved(args.trace_evidence / 'source.jsonl.gz', args.source_evidence / 'driver.jsonl.gz') == reference['source_rows_preserved']
    headers, traces = {}, {}
    for name in ('source', 'native'):
        path = args.trace_evidence / f'{name}.jsonl.gz'
        assert digest(ram(path)) == reference[name + '_trace_sha256'] == window[name + '_trace_sha256']
        headers[name], traces[name] = read_trace(path)
    assert headers['source'] == headers['native']
    for report, count_key in ((radar, 'boundaries'), (message, 'observations')):
        for key in ('first', 'last', 'runner_sha256', 'source_trace_sha256', 'native_trace_sha256'):
            assert report[key] == window[key], key
        assert report['live_original_pages_matching'] == report['live_native_pages_matching'] == report[count_key] == len(window['rows'])
        assert all(report['mutation_rejections'].values())
    assert radar['strict_live_owner_pages_matching']
    validate_controls(radar)
    assert message['wrong_owner_return_capture_rejected'] and message['native_owner_captures_retained']
    inputs = (args.window, window, radar, message, args.radar_bodies, args.message_bodies,
              args.message_pages, headers, traces)
    history = predict_history(*inputs)
    rejections = {}
    for mutation in ('lost_panel_refresh', 'lost_radar_writes', 'lost_message_writes'):
        try:
            predict_history(*inputs, mutation=mutation)
        except AssertionError as error:
            assert error.args[0][-1] == 'Combined writes do not explain every complete page byte', error
            rejections[mutation] = dict(rejected=True, first_difference=error.args[0][0])
        else:
            assert mutation in ('lost_panel_refresh', 'lost_radar_writes'), f'{mutation} was unobservable in this window'
            # Removing identical writes from both histories can preserve XOR.
            # Removing actual source writes alone must still be observable.
            try:
                actual_mutation = 'lost_source_panel_refresh' if mutation == 'lost_panel_refresh' else 'lost_source_radar_writes'
                predict_history(*inputs, mutation=actual_mutation)
            except AssertionError as error:
                assert error.args[0][-1] == 'Combined writes do not explain every complete page byte', error
                rejections[mutation] = dict(rejected=True, first_difference=error.args[0][0],
                    actual_mutation=actual_mutation, balanced_mutation_unobservable=True)
            else:
                raise AssertionError(f'No observable source omission for {mutation} in this window')
    report = dict(first=window['first'], last=window['last'], observations=len(history),
                  runner_sha256=window['runner_sha256'], source_trace_sha256=window['source_trace_sha256'],
                  native_trace_sha256=window['native_trace_sha256'], complete_plane_history=history,
                  compared_bytes=64000 * len(history), history_mutation_rejections=rejections,
                  live_original_pages_matching=message['live_original_pages_matching'],
                  live_native_pages_matching=message['live_native_pages_matching'],
                  mutation_rejections=message['mutation_rejections'], wrong_owner_return_capture_rejected=True,
                  radar_history_sha256=digest(args.radar_history.read_bytes()),
                  message_pages_sha256=digest((args.message_pages / 'report.json').read_bytes()),
                  radar_phase_mutation_rejections=radar['mutation_rejections'],
                  radar_paint_mutation_rejections=radar['paint_mutation_rejections'],
                  unobservable_message_mutations=message.get('unobservable_drawing_mutations', []),
                  scope='All eight complete page XORs in this bounded independent flight window follow actual original radar, bitmap and message writes from common initial pages. No pixel masks or clock adjustments; other windows and full-flight drawing remain open.')
    if radar.get('unobservable_phase_mutations'):
        report['radar_unobservable_phase_mutations'] = radar['unobservable_phase_mutations']
    if radar.get('unobservable_paint_mutations'):
        report['radar_unobservable_paint_mutations'] = radar['unobservable_paint_mutations']
    validate_history_controls(report)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + '\n')
    print(f'{len(history)} complete eight-plane observations / {report["compared_bytes"]} bytes predicted; three omitted-owner histories rejected')


if __name__ == '__main__':
    main()
