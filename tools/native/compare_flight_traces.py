"""Compare explicitly aligned, independently recorded C0EFD4 flight traces.

All sixteen complete record cores, named camera/control state and all eight
complete page hashes are checked. Clock/HUD differences remain reported;
this tool does not accept them by masking pixels or searching for alignment.
"""
import argparse
import gzip
import hashlib
import json
from pathlib import Path


GAME_FIELDS = ('stage', 'game_tick', 'phase', 'mouse_coordinates', 'selected_record',
               'mode', 'target_record', 'controls',
               'observer', 'camera_matrix', 'view_matrix', 'view_pan_rotate',
               'view_attitude', 'view_side')


def read_trace(path):
    opener = gzip.open if path.suffix == '.gz' else open
    with opener(path, 'rt', encoding='ascii') as stream:
        header = json.loads(next(stream))
        assert header['format'] in ('FA18_FLIGHT_TRACE_V1', 'FA18_FLIGHT_TRACE_V2')
        assert header['boundary'] == 'C0EFD4/pre-input'
        assert (header['record_address'], header['record_stride'], header['record_size'],
                header['record_count'], header['plane_bytes']) == (0xC46184, 512, 164, 16, 8000)
        fields = header['fields']
        if header['format'] == 'FA18_FLIGHT_TRACE_V1':
            # V1's mislabeled field is INPUT_X/INPUT_Y, not SELECTED_RECORD.
            # Keep the old bytes and their addresses; do not invent missing
            # target-selection evidence when reading retained V1 recordings.
            legacy = next(f for f in fields if f['name'] == 'selected_record')
            assert (legacy['address'], legacy['size']) == (0xC45776, 4)
            legacy['name'] = 'mouse_coordinates'
        else:
            contract = {f['name']: (f['address'], f['size']) for f in fields}
            assert contract['mouse_coordinates'] == (0xC45776, 4)
            assert contract['selected_record'] == (0xC459C0, 2)
        assert len({f['name'] for f in fields}) == len(fields)
        rows, ended, previous = {}, False, 0
        for line in stream:
            row = json.loads(line)
            assert not ended, 'data follows the trace footer'
            if row.get('end'):
                assert row['rows'] == len(rows), 'incomplete trace row count'
                ended = True
                continue
            iteration = row['iteration']
            assert iteration > previous, 'duplicate/out-of-order trace iteration'
            previous = iteration
            assert len(row['records']) == 16 and all(len(bytes.fromhex(r)) == 164 for r in row['records'])
            assert len(row['fields']) == len(fields)
            assert all(len(bytes.fromhex(v)) == f['size'] for f, v in zip(fields, row['fields']))
            assert len(row['pages']) == (8 if row['pages_valid'] else 0)
            assert all(len(bytes.fromhex(v)) == 32 for v in row['pages'])
            if row['pages_valid']:
                assert (row['width'], row['height']) == (320, 200)
                assert row['draw_page'] in (0, 1) and row['page_table'] == 0xC4566E + 16 * row['draw_page']
            row['fields'] = {f['name']: value for f, value in zip(fields, row['fields'])}
            rows[iteration] = row
        assert ended, 'flight trace has no successful completion footer'
    return header, rows


def number(row, name):
    return int(row['fields'][name], 16)


def compare(source, native, source_first, native_first, count):
    assert source[source_first]['fields'].keys() == native[native_first]['fields'].keys()
    game_fields = [name for name in GAME_FIELDS if name in source[source_first]['fields']]
    report = dict(compared=count, record_cores_compared=16 * count,
                  record_cores_matching=0, complete_record_boundaries_matching=0,
                  camera_and_controls_matching=0, complete_pages_matching=0,
                  timer_and_hud_matching=0, first_differences=[],
                  first_record_difference=None, first_game_difference=None, first_page_difference=None,
                  game_fields_compared=game_fields)
    for offset in range(count):
        a, b = source[source_first + offset], native[native_first + offset]
        assert a['pages_valid'] and b['pages_valid'], 'missing complete drawing page evidence'
        record_changes = [i for i in range(16) if a['records'][i] != b['records'][i]]
        game_changes = [name for name in game_fields if a['fields'][name] != b['fields'][name]]
        timed_changes = [name for name in a['fields'] if name not in GAME_FIELDS and a['fields'][name] != b['fields'][name]]
        page_changes = [i for i in range(8) if a['pages'][i] != b['pages'][i]]
        report['record_cores_matching'] += 16 - len(record_changes)
        report['complete_record_boundaries_matching'] += not record_changes
        report['camera_and_controls_matching'] += not game_changes
        report['complete_pages_matching'] += not page_changes
        report['timer_and_hud_matching'] += not timed_changes
        for key, changes in (('first_record_difference', record_changes),
                             ('first_game_difference', game_changes), ('first_page_difference', page_changes)):
            if changes and report[key] is None:
                report[key] = dict(source_iteration=a['iteration'], native_iteration=b['iteration'],
                                  source_tick=number(a, 'game_tick'), changes=changes)
        if (record_changes or game_changes or page_changes or timed_changes) and len(report['first_differences']) < 16:
            report['first_differences'].append(dict(source_iteration=a['iteration'], native_iteration=b['iteration'],
                source_tick=number(a, 'game_tick'), native_tick=number(b, 'game_tick'),
                record_slots=record_changes, game_fields=game_changes, page_planes=page_changes,
                timed_fields={name: [a['fields'][name], b['fields'][name]] for name in timed_changes}))
    return report


def timer_events(rows, first, count):
    events, previous = [], None
    start = number(rows[first], 'elapsed_total')
    for i in range(first, first + count):
        row = rows[i]
        current = {name: number(row, name) for name in ('info_page', 'view_hold', 'gauge_refresh')}
        if previous is None or current['info_page'] != previous['info_page'] or (
            current['view_hold'] == 0 and previous['view_hold'] != 0) or (
            current['gauge_refresh'] == 2 and previous['gauge_refresh'] != 2):
            events.append(dict(iteration=i, tick=number(row, 'game_tick'),
                elapsed_ms=(number(row, 'elapsed_total') - start) & 0xFFFFFFFF,
                sample_seconds=number(row, 'sample_seconds'),
                info_request=number(row, 'info_request'), **current,
                message=bytes.fromhex(row['fields']['message_line']).decode('latin1')))
        previous = current
    return events


def compare_stage_events(source, native, source_first, source_last, native_first, native_last):
    """Account for each boundary in ordinal callback runs, including all changes.

    Repeated unchanged gameplay states during an asynchronous wait are counted
    separately. Every distinct consecutive state is compared without exclusions;
    this never chooses a matching later state or changes a game's clock.
    Drawing hashes remain the separate strict, uncompressed diagnostic.
    """
    def runs(rows, first, last):
        result = []
        for i in range(first, last + 1):
            row = rows[i]  # Missing updates cannot silently disappear.
            stage = row['fields']['stage']
            if not result or stage != result[-1]['stage']:
                result.append(dict(stage=stage, first=i, last=i, states=[], changes=[]))
            run = result[-1]
            payload = b''.join(bytes.fromhex(v) for v in row['records']) + b''.join(
                bytes.fromhex(row['fields'][name]) for name in GAME_FIELDS if name in row['fields'])
            digest = hashlib.sha256(payload).hexdigest()
            if not run['states'] or digest != run['states'][-1]:
                run['states'].append(digest)
                run['changes'].append(i)
            run['last'] = i
        return result
    sr, nr = runs(source, source_first, source_last), runs(native, native_first, native_last)
    assert [r['stage'] for r in sr] == [r['stage'] for r in nr], 'different actual callback sequences'
    compared = []
    for a, b in zip(sr, nr):
        compared.append(dict(stage=a['stage'], source_first=a['first'], source_last=a['last'],
            native_first=b['first'], native_last=b['last'],
            source_rows=a['last'] - a['first'] + 1, native_rows=b['last'] - b['first'] + 1,
            source_distinct_states=len(a['states']), native_distinct_states=len(b['states']),
            matching=a['states'] == b['states'],
            source_state_sequence_sha256=hashlib.sha256(''.join(a['states']).encode('ascii')).hexdigest(),
            native_state_sequence_sha256=hashlib.sha256(''.join(b['states']).encode('ascii')).hexdigest(),
            source_pal_interval=[source[a['first']]['frame'], source[a['last']]['frame']],
            native_pal_interval=[native[b['first']]['frame'], native[b['last']]['frame']]))
    return dict(source_rows=source_last - source_first + 1, native_rows=native_last - native_first + 1,
                runs=compared, matching_runs=sum(r['matching'] for r in compared),
                source_state_changes=sum(r['source_distinct_states'] for r in compared),
                native_state_changes=sum(r['native_distinct_states'] for r in compared))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, required=True)
    parser.add_argument('--native', type=Path, required=True)
    parser.add_argument('--source-first', type=int, required=True)
    parser.add_argument('--native-first', type=int, required=True)
    parser.add_argument('--count', type=int, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    assert min(args.source_first, args.native_first, args.count) > 0
    source_header, source = read_trace(args.source)
    native_header, native = read_trace(args.native)
    assert source_header == native_header, 'different trace field contracts'
    report = compare(source, native, args.source_first, args.native_first, args.count)
    report.update(source_sha256=hashlib.sha256(args.source.read_bytes()).hexdigest(),
                  native_sha256=hashlib.sha256(args.native.read_bytes()).hexdigest(),
                  source_first=args.source_first, native_first=args.native_first,
                  source_events=timer_events(source, args.source_first, args.count),
                  native_events=timer_events(native, args.native_first, args.count),
                  acceptance='diagnostic; equivalent elapsed-time assessment remains separate')
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({k: v for k, v in report.items() if k.endswith('matching') or k == 'compared'}))
    raise SystemExit(0 if all(report[k] == args.count for k in
        ('complete_record_boundaries_matching', 'camera_and_controls_matching',
         'complete_pages_matching', 'timer_and_hud_matching')) else 1)


if __name__ == '__main__':
    main()
