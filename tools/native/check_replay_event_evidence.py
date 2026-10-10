"""Validate declared replay events against retained independent flight evidence."""
import argparse
import copy
import gzip
import json
from pathlib import Path

import check_recorded_original_mission_trace as flight


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source-evidence', type=Path, required=True)
    parser.add_argument('--source-updates', type=Path, required=True)
    parser.add_argument('--native-evidence', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    original = json.loads((args.source_evidence / 'report.json').read_text())
    source = args.source_evidence / 'driver.jsonl.gz'
    mapping, _ = flight.verified_update_mapping(args.source_updates, source, original)
    plan = flight.source_event_plan(source, mapping, original['mission_mode'])
    report = json.loads((args.native_evidence / 'report.json').read_text())
    evidence = json.loads((args.native_evidence / 'anchors.json').read_text())
    assert plan == report['event_plan'] and evidence == report['event_report']
    assert flight.digest((args.native_evidence / 'anchors.json').read_bytes()) == report['anchor_report_sha256']
    native_path = args.native_evidence / 'native.jsonl.gz'
    native_bytes = gzip.decompress(native_path.read_bytes())
    assert flight.digest(native_bytes) == report['native_trace_sha256']
    assert evidence['keys_consumed'] == report['native_run']['replay_events']
    # Parse the real retained trace once; mutations alter only the claimed
    # event report. Each claim must still match this unchanged native trace.
    trace = flight.read_trace(native_path)
    flight.read_trace = lambda path: trace
    valid = flight.event_mapping(plan, evidence, mapping, native_path)
    mutations = {}
    for name in ('format', 'complete', 'failed', 'source_position', 'native_updates',
                 'source_first', 'mode', 'stage', 'game_tick', 'native_first', 'frame', 'missing_anchor'):
        changed = copy.deepcopy(evidence)
        if name == 'format':
            changed[name] = 'OTHER'
        elif name in ('complete', 'failed'):
            changed[name] = not changed[name]
        elif name in ('source_position', 'native_updates'):
            changed[name] += 1
        elif name == 'missing_anchor':
            changed['anchors'].pop()
        elif name == 'stage':
            changed['anchors'][-1][name] = 'C10DAE'
        else:
            changed['anchors'][-1][name] += 1
        try:
            flight.event_mapping(plan, changed, mapping, native_path)
        except (AssertionError, KeyError):
            mutations[name] = True
        else:
            raise AssertionError(f'counterfeit event accepted: {name}')
    result = dict(source_trace_sha256=original['driver_trace_sha256'],
        native_trace_sha256=report['native_trace_sha256'],
        anchor_report_sha256=report['anchor_report_sha256'],
        flight_observations=len(valid), real_flight_updates=len(set(valid.values())),
        event_plan=plan, mutation_rejections=mutations,
        scope='Event ownership only; every native trace row remains unchanged. Gameplay parity is a separate check.')
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(dict(flight_observations=len(valid), mutation_rejections=mutations)))


if __name__ == '__main__':
    main()
