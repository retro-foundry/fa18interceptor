"""Extract bounded actual flight/owner RAM from already verified full streams.

No game is rerun and no captured state feeds native gameplay. The existing
drawing checkers receive the same complete entry/body/owner fixture layout.
"""
import argparse
import gzip
import json
from pathlib import Path

from check_gameplay_checkpoint import integer
from check_original_frame_delta import BOUNDARIES
from check_qualification_message_cadence import digest, pages, verify_trace, FIELDS
from check_recorded_original_mission_trace import verified_update_mapping
from compare_flight_traces import read_trace
from frame_delta import bodies, snapshots


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('source-delta', 'native-delta', 'reference', 'source-evidence', 'source-updates', 'out'):
        parser.add_argument('--' + name, type=Path, required=True)
    parser.add_argument('--first', type=int, required=True)
    parser.add_argument('--count', type=int, required=True)
    parser.add_argument('--previous-window', type=Path, help='Require exact old entry/body hashes and timing')
    args = parser.parse_args()
    assert 1 <= args.count <= 128
    first, last = args.first, args.first + args.count - 1
    reference = json.loads((args.reference / 'report.json').read_text())
    original = json.loads((args.source_evidence / 'report.json').read_text())
    source = json.loads((args.source_delta / 'report.json').read_text())
    native = json.loads((args.native_delta / 'report.json').read_text())
    assert source['complete_trace_preserved'] and source['final_ram_preserved'] and source['runtime_counters_preserved']
    assert native['original_bodies_matching'] and native['final_ram_preserved'] and native['earned_save_preserved']
    assert native['runner_sha256'] == reference['runner_sha256']
    assert source['source_trace_sha256'] == reference['source_trace_sha256']
    assert native['native_trace_sha256'] == reference['native_trace_sha256']
    assert source['first'] <= first <= last <= source['last']
    mapping, updates = verified_update_mapping(args.source_updates, args.source_evidence / 'driver.jsonl.gz', original)
    assert updates == reference['source_update_evidence']
    source_header, source_rows = read_trace(args.reference / 'source.jsonl.gz')
    native_header, native_rows = read_trace(args.reference / 'native.jsonl.gz')
    assert source_header == native_header
    for name in ('source', 'native'):
        assert digest(gzip.decompress((args.reference / f'{name}.jsonl.gz').read_bytes())) == reference[name + '_trace_sha256']
    nf, nl = mapping[first], mapping[last]
    assert native['native_first'] <= nf <= nl <= native['native_last']
    assert len(set(mapping[i] for i in range(first, last + 1))) == args.count, 'Duplicate dispatch needs its own owner assessment'
    previous = None
    if args.previous_window:
        previous = json.loads((args.previous_window / 'report.json').read_text())
        assert (previous['first'], previous['last']) == (first, last)
    for name in ('window', 'radar', 'message'):
        (args.out / name).mkdir(parents=True, exist_ok=True)
    saved_bytes = 0
    def retain(directory, name, data):
        nonlocal saved_bytes
        encoded = gzip.compress(data, mtime=0)
        saved_bytes += len(encoded)
        assert saved_bytes <= 512 * 1024 * 1024, 'Bounded extraction exceeds capture budget'
        (args.out / directory / name).write_bytes(encoded)
    identities = {(r['iteration'], r['pc']): r for r in source['identities']}
    selected, registers = {}, gzip.decompress((args.source_delta / 'registers.jsonl.gz').read_bytes())
    if 'metadata_sha256' in source:
        assert digest(registers) == source['metadata_sha256']
    else:
        # The first sealed window predates the metadata digest; every one of
        # its registers was independently compared with both old owner probes.
        assert source['old_window_snapshots_identical'] == 8 * source['observations']
    lines = iter(registers.splitlines())
    observed = 0
    for snapshot in snapshots(args.source_delta / 'frames.delta.gz', BOUNDARIES):
        state = json.loads(next(lines)); observed += 1
        i, pc = snapshot['iteration'], snapshot['boundary']
        expected = identities[i, pc]
        assert (state['iteration'], state['pc'], state['frame']) == (i, pc, snapshot['frame'])
        assert expected['ram_sha256'] == snapshot['ram_sha256'] and expected['frame'] == state['frame']
        if first <= i <= last:
            assert pc not in selected.setdefault(i, {})
            selected[i][pc] = dict(ram_sha256=snapshot['ram_sha256'], registers=state)
            data = snapshot['data']
            if pc == 0xC0EFD4:
                assert snapshot['frame'] == source_rows[i]['frame']
                verify_trace(data, source_header, source_rows[i])
                retain('window', f'source.{i}.dat.gz', data)
            for owner, entry, returned in (('radar', 0xC31226, 0xC0F18E), ('message', 0xC322EE, 0xC0F286)):
                suffix = {0xC0EFEA: 'before', 0xC0F3C0: 'after', entry: 'owner', returned: 'owner-after'}.get(pc)
                if suffix is not None:
                    if pc == entry:
                        actual_return = integer(data, state['registers'][15], 4)
                        assert actual_return == returned, 'Alternate owner path needs its own assessment'
                        state['stack_return'] = actual_return
                    retain(owner, f'source-body.{i}.{suffix}.dat.gz', data)
    assert next(lines, None) is None and observed == source['snapshots']
    rows, native_count = [], 0
    native_identities = {r['iteration']: r for r in native['identities']}
    for body in bodies(args.native_delta / 'frames.delta.gz'):
        native_count += 1
        j = body['entry']['iteration']
        expected = native_identities[j]
        assert expected['original_body_matching']
        for suffix in ('entry', 'before', 'after'):
            assert body[suffix]['ram_sha256'] == expected[suffix + '_sha256']
        assert (body['before']['frame'], body['after']['frame'], body['before']['saved_tick']) == (
            expected['before_tick'], expected['after_tick'], expected['saved_tick'])
        if not nf <= j <= nl:
            continue
        i = next(i for i in range(first, last + 1) if mapping[i] == j)
        a = gzip.decompress((args.out / 'window' / f'source.{i}.dat.gz').read_bytes())
        b = body['entry']['data']
        assert body['entry']['frame'] == native_rows[j]['frame']
        verify_trace(b, native_header, native_rows[j])
        assert source_rows[i]['records'] == native_rows[j]['records']
        differences = []
        for plane, (av, bv) in enumerate(zip(pages(a), pages(b))):
            changed = [dict(byte=k, x=k % 40 * 8, y=k // 40, source=x, native=y)
                       for k, (x, y) in enumerate(zip(av, bv)) if x != y]
            if changed:
                differences.append(dict(plane=plane, bytes=len(changed), changes=changed,
                    bit_pixels=sum((p['source'] ^ p['native']).bit_count() for p in changed),
                    byte_bbox=[min(p['x'] for p in changed), min(p['y'] for p in changed),
                               max(p['x'] for p in changed) + 7, max(p['y'] for p in changed)]))
        timing = dict(iteration=j, before_tick=body['before']['frame'], after_tick=body['after']['frame'],
                      saved_tick=body['before']['saved_tick'], owner_exit=body['after']['boundary'] == 3)
        hashes = dict(source=digest(a), native=body['entry']['ram_sha256'])
        if previous:
            old = previous['rows'][i - first]
            assert hashes == old['ram_sha256'] and timing == old['body_timing']
            for suffix in ('before', 'after'):
                assert body[suffix]['data'] == gzip.decompress((args.previous_window / f'native.{j}.{suffix}.dat.gz').read_bytes())
        for suffix in ('entry', 'before', 'after'):
            retain('window', f'native.{j}' + ('.dat.gz' if suffix == 'entry' else f'.{suffix}.dat.gz'), body[suffix]['data'])
        rows.append(dict(source_iteration=i, native_iteration=j, ram_sha256=hashes, page_differences=differences,
            body_timing=timing, state={name: {k: integer(data, address, size) for k, (address, size) in FIELDS.items()}
                for name, data in (('source', a), ('native', b))}, changed_trace_fields={k: [v, native_rows[j]['fields'][k]]
                for k, v in source_rows[i]['fields'].items() if v != native_rows[j]['fields'][k]}))
    assert native_count == native['bodies'] and len(rows) == args.count
    for owner, entry, returned in (('radar', 0xC31226, 0xC0F18E), ('message', 0xC322EE, 0xC0F286)):
        captures = []
        for i in range(first, last + 1):
            states = selected[i]
            captures.append(dict(iteration=i, snapshots={suffix: states[pc] for suffix, pc in
                (('before', 0xC0EFEA), ('after', 0xC0F3C0), ('owner', entry), ('owner-after', returned))}))
        report = dict(iteration=first, probe_sha256=source['probe_sha256'],
            source_trace_sha256=reference['baseline_source_trace_sha256'], source_input_sha256=source['source_input_sha256'],
            unchanged_observations=source['unchanged_observations'], last_observation=source['unchanged_observations'],
            captures=captures, stream_report_sha256=digest((args.source_delta / 'report.json').read_bytes()),
            scope='Actual independent original owner RAM/registers extracted unchanged from the verified complete stream.')
        (args.out / owner / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    report = dict(first=first, last=last, rows=rows, runner_sha256=reference['runner_sha256'],
        source_trace_sha256=reference['source_trace_sha256'], native_trace_sha256=reference['native_trace_sha256'],
        source_update_mapping_sha256=updates['mapping_sha256'], previous_window_identical=bool(previous), retained_bytes=saved_bytes,
        source_delta_report_sha256=digest((args.source_delta / 'report.json').read_bytes()),
        native_delta_report_sha256=digest((args.native_delta / 'report.json').read_bytes()),
        scope='Bounded complete actual RAM/owners extracted from verified independent full streams. Every trace field, core and page stays exact; all differences remain reported. No drawing-history acceptance is granted here.')
    (args.out / 'window' / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'{len(rows)} independent complete windows/owners extracted; {saved_bytes} compressed bytes; old window exact={bool(previous)}')


if __name__ == '__main__':
    main()
