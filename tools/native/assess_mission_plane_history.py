"""Account for every difference in one complete plane of the independent M3 flight.

Matching SHA-256 page hashes prove zero delta; every nonzero delta must be
covered by a separately validated, bounded paint history with retained pages.
Other planes remain outside this acceptance. No pixels or clocks are changed.
"""
import argparse
import copy
import gzip
import json
from pathlib import Path

from check_qualification_message_cadence import digest, pages
from check_recorded_original_mission_trace import verified_update_mapping
from check_mission_message_trace import preserved
from compare_flight_traces import read_trace


def coverage(source, native, mapping, first, last, histories, plane):
    planes = (plane, plane + 4)
    strict, different, covered = {p: 0 for p in planes}, [], []
    for i in range(first, last + 1):
        a, b = source[i], native[mapping[i]]
        assert a['pages_valid'] and b['pages_valid']
        assert a['records'] == b['records'], (i, 'Complete record cores differ')
        changed = [p for p in planes if a['pages'][p] != b['pages'][p]]
        for p in planes:
            strict[p] += p not in changed
        if changed:
            assert i in histories, (i, 'Missing bounded history for a complete-plane difference')
            predicted = histories[i]
            assert all(predicted['difference_bytes'][predicted['planes'].index(p)] > 0 for p in changed)
            different.append(dict(iteration=i, native_iteration=mapping[i], planes=changed,
                                  history=predicted['history']))
        if i in histories:
            predicted = histories[i]
            for p in planes:
                assert bool(predicted['difference_bytes'][predicted['planes'].index(p)]) == (p in changed)
            covered.append(i)
    return dict(strict_matching_by_role={str(p): n for p, n in strict.items()},
                different_observations=different, bounded_history_observations=covered,
                observations=last - first + 1, complete_plane_observations=2 * (last - first + 1),
                complete_plane_bytes_accounted=16000 * (last - first + 1))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('traces', 'source-evidence', 'native-evidence', 'source-updates', 'out'):
        parser.add_argument('--' + name, type=Path, required=True)
    parser.add_argument('--history', nargs=2, action='append', required=True,
                        metavar=('WINDOW', 'REPORT'), help='Bounded retained window and validated history report')
    parser.add_argument('--plane', type=int, choices=range(4), default=1)
    args = parser.parse_args()
    reference = json.loads((args.traces / 'report.json').read_text())
    original = json.loads((args.source_evidence / 'report.json').read_text())
    assert reference['whole_successful_flight']['strict_gameplay_matching']
    assert digest(gzip.decompress((args.source_evidence / 'driver.jsonl.gz').read_bytes())) == original['driver_trace_sha256']
    assert preserved(args.traces / 'source.jsonl.gz', args.source_evidence / 'driver.jsonl.gz') == reference['source_rows_preserved']
    assert preserved(args.traces / 'native.jsonl.gz', args.native_evidence / 'native.jsonl.gz') == reference['native_rows_preserved']
    mapping, updates = verified_update_mapping(args.source_updates, args.source_evidence / 'driver.jsonl.gz', original)
    assert updates == reference['source_update_evidence']
    traces, headers = {}, {}
    for name in ('source', 'native'):
        path = args.traces / (name + '.jsonl.gz')
        assert digest(gzip.decompress(path.read_bytes())) == reference[name + '_trace_sha256']
        headers[name], traces[name] = read_trace(path)
    assert headers['source'] == headers['native']
    pairs = [(reference['source_trace_sha256'], reference['native_trace_sha256']),
             (reference['baseline_source_trace_sha256'], reference['baseline_native_trace_sha256'])]
    histories, evidence = {}, []
    for window_path, report_path in args.history:
        window_path, report_path = Path(window_path), Path(report_path)
        window = json.loads((window_path / 'report.json').read_text())
        report = json.loads(report_path.read_text())
        assert (window['source_trace_sha256'], window['native_trace_sha256']) in pairs
        assert (report['source_trace_sha256'], report['native_trace_sha256']) == (window['source_trace_sha256'], window['native_trace_sha256'])
        assert report['first'] == window['first'] and report['last'] == window['last']
        if 'complete_plane_history' in report:
            rows = report['complete_plane_history']
            assert all(report['mutation_rejections'].values()) and report['wrong_owner_return_capture_rejected']
            assert report['live_original_pages_matching'] == report['live_native_pages_matching'] == report['observations']
        else:
            rows = report['cross_paint_history']['rows']
            assert all(report['paint_mutation_rejections'].values()) and report['strict_live_owner_pages_matching']
            assert report['live_original_pages_matching'] == report['live_native_pages_matching'] == report['boundaries']
        indexed = {r['iteration']: r for r in rows}
        assert sorted(indexed) == list(range(window['first'], window['last'] + 1))
        for observed in window['rows']:
            i, j = observed['source_iteration'], observed['native_iteration']
            assert j == mapping[i] and i not in histories
            actual_pages = {}
            for name, index in (('source', i), ('native', j)):
                path = window_path / f'{name}.{index}.dat.gz'
                if not path.exists():
                    # Only a passing native entry may have been pruned. Its
                    # retained body-begin must prove all eight complete pages.
                    assert name == 'native' and not observed['page_differences']
                    path = window_path / f'native.{index}.before.dat.gz'
                data = gzip.decompress(path.read_bytes())
                actual_pages[name] = pages(data)
                assert [digest(p) for p in actual_pages[name]] == traces[name][index]['pages']
            row = indexed[i]
            assert all(p in row['planes'] for p in (args.plane, args.plane + 4))
            deltas = [bytes(a ^ b for a, b in zip(actual_pages['source'][p], actual_pages['native'][p])) for p in row['planes']]
            assert row['compared_bytes'] == 8000 * len(deltas)
            assert digest(b''.join(deltas)) == row['delta_sha256']
            assert [sum(bool(byte) for byte in d) for d in deltas] == row['difference_bytes']
            histories[i] = dict(row, history=report_path.as_posix())
        evidence.append(dict(window=window_path.as_posix(), report=report_path.as_posix(),
                             report_sha256=digest(report_path.read_bytes()), first=window['first'], last=window['last']))
    flight = reference['whole_successful_flight']
    first, last = flight['first'], flight['last']
    result = coverage(traces['source'], traces['native'], mapping, first, last, histories, args.plane)
    missing = copy.deepcopy(histories)
    last_difference = result['different_observations'][-1]['iteration']
    del missing[last_difference]
    try:
        coverage(traces['source'], traces['native'], mapping, first, last, missing, args.plane)
    except AssertionError as error:
        assert error.args[0] == (last_difference, 'Missing bounded history for a complete-plane difference')
    else:
        raise AssertionError('Accepted an unexplained late complete-plane difference')
    result.update(first=first, last=last, plane=args.plane, runner_sha256=reference['runner_sha256'],
        source_trace_sha256=reference['source_trace_sha256'], native_trace_sha256=reference['native_trace_sha256'],
        histories=evidence, missing_late_history_rejected=True,
        strict_complete_pages_matching=flight['complete_pages_matching'],
        scope=f'All complete plane-{args.plane} bytes on both pages across the recorded independent successful mission-three flight: exact matching hashes or actual validated bounded paint-history deltas. Source-defined cache/blink rules and complete record cores remain strict. Other planes, flights, recorded sound and performance remain open; no pixels or clocks are altered.')
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(result, indent=2) + '\n')
    print(f"All {result['complete_plane_observations']} complete plane-{args.plane} observations accounted for; {len(result['different_observations'])} differing flight observations have validated paint histories; missing late history rejected")


if __name__ == '__main__':
    main()
