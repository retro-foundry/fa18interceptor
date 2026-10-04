"""Verify complete render-entry fixtures, cold segments and recording evidence."""
import hashlib
import json
import re
from audit_render_entry_helpers_source import ROOT, ENTRIES, inventory
from check_render_entry_helpers_segments import SEGMENTS

RECORDINGS = ('demo01', 'qual_carrier_success', 'qual_fail_crashes')
KEYS = ('calls', 'compared', 'matched', 'mismatched', 'hardware', 'incomplete')

def seal(path):
    return {'path': path, 'sha256': hashlib.sha256((ROOT/path).read_bytes()).hexdigest()}

def recording_group(entries):
    totals = {mode: {entry: dict.fromkeys(KEYS, 0) for entry in entries}
              for mode in ('shadow', 'sandbox')}
    reports = []
    for mode in totals:
        for name in RECORDINGS:
            path = f'build/recomp/whole_call_{"_".join(entries)}_{name}_{mode}.json'
            rows = [row for row in json.loads((ROOT/path).read_text()) if row['entry'] in entries]
            assert len(rows) == len(entries) and not any(row['mismatched'] for row in rows)
            for row in rows:
                for key in KEYS:
                    totals[mode][row['entry']][key] += row[key]
            reports.append({'mode': mode, 'recording': name, 'rows': rows, 'report': seal(path)})
    # The unchanged generic checker isolates entries absorbed by a parent.
    # Select exactly one independent report for such entries, matching it.
    for entry in entries:
        if any(totals[mode][entry]['matched'] for mode in totals):
            continue
        if len(entries) == 1 or not any(totals[mode][entry]['calls'] for mode in totals):
            continue
        isolated, evidence = recording_group((entry,))
        for mode in totals:
            totals[mode][entry] = isolated[mode][entry]
        reports.extend(evidence)
    return totals, reports

def verify():
    source = inventory()
    fixtures, dispatch, segments = [], [], []
    unions = {kind: set() for kind in ('contract', 'real')}
    cold = {'C30266', 'C3027A', 'C30296', 'C302E4'}
    for kind in unions:
        for entry in ENTRIES:
            path = f'build/recomp/render_entry_helpers_{kind}_{entry}.log'
            text = (ROOT/path).read_text()
            assert f'oracle {entry}: 16384 complete calls matched all registers, PC, full SR and all RAM' in text
            owned = set(source['owners'][entry]['source_pcs'])
            observed = owned & set(text.split('visited:')[1].splitlines()[0].split())
            if kind == 'contract':
                assert owned-observed == owned & cold, (entry, owned-observed)
            unions[kind] |= observed
            fixtures.append({'owner': entry, 'kind': kind, 'completed_calls': 16384,
                             'owner_pcs': len(owned), 'observed_owner_pcs': len(observed),
                             'unobserved_owner_pcs': sorted(owned-observed),
                             'ordered_custom_writes': int(re.search(r'hardware: (\d+) ordered Custom writes validated', text)[1]),
                             'log': seal(path)})
    segment_observed = set()
    for entry, owned in SEGMENTS.items():
        path = f'build/recomp/render_entry_helpers_segment_{entry}.log'
        text = (ROOT/path).read_text()
        assert f'oracle {entry}: 16384 production segment calls matched all registers, PC, full SR and all RAM' in text
        observed = set(text.split('visited:')[1].splitlines()[0].split())
        assert observed == owned
        segment_observed |= observed
        segments.append({'entry': entry, 'completed_calls': 16384, 'source_pcs': sorted(observed), 'log': seal(path)})
    all_pcs = {row['pc'] for row in source['instructions']}
    assert len(unions['contract']) == 546 and unions['contract'] | cold == all_pcs
    assert cold <= segment_observed
    for entry in ENTRIES:
        for mode in ('on', 'shadow', 'sandbox'):
            path = f'build/recomp/render_entry_helpers_dispatch_{entry}_{mode}.log'
            text = (ROOT/path).read_text()
            assert f'oracle {entry}: 1024 complete calls matched all registers, PC, full SR and all RAM' in text, (entry, mode)
            owned = set(source['owners'][entry]['source_pcs'])
            observed = owned & set(text.split('visited:')[1].splitlines()[0].split())
            hw, free = map(int, re.search(r'classification: (\d+) hardware-bearing source calls, (\d+) hardware-free source calls', text).groups())
            assert hw + free == 1024
            dispatch.append({'owner': entry, 'mode': mode, 'completed_calls': 1024,
                             'hardware_bearing': hw, 'hardware_free': free,
                             'owner_pcs': len(owned), 'observed_owner_pcs': len(observed),
                             'unobserved_owner_pcs': sorted(owned-observed),
                             'ordered_custom_writes': int(re.search(r'hardware: (\d+) ordered Custom writes validated', text)[1]),
                             'log': seal(path)})
    source_only = [e for e in source['new_registered_owners'] if e != 'C2F688']
    assert len(source_only) == 7
    groups = (list(ENTRIES[:5]), [e for e in ENTRIES[5:] if e not in source_only], source_only)
    totals = {mode: {} for mode in ('shadow', 'sandbox')}
    reports = []
    for entries in groups:
        rows, evidence = recording_group(entries)
        for mode in totals:
            totals[mode].update(rows[mode])
        reports.extend(evidence)
    for entry in ENTRIES:
        if entry in source_only:
            assert all(totals[mode][entry]['calls'] == totals[mode][entry]['matched'] == 0 for mode in totals)
        else:
            assert any(totals[mode][entry]['matched'] for mode in totals), entry
    leaf_entries = list(reversed(json.loads((ROOT/'analysis/data/render_leaf_helpers_source_scope.json').read_text())['owners']))
    leaf_totals, leaf_reports = recording_group(leaf_entries)
    assert all(any(leaf_totals[mode][entry]['matched'] for mode in leaf_totals) for entry in leaf_entries)
    return {'whole_entry_completed_calls': 1212416, 'controlled_child_completed_calls': 606208,
            'real_child_completed_calls': 606208, 'controlled_union_pcs': len(unions['contract']),
            'real_child_union_pcs': len(unions['real']), 'fixture_coverage': fixtures,
            'whole_call_cold_pcs': sorted(cold), 'cold_segments': segments,
            'additional_production_segment_calls': 65536, 'actual_dispatch_completed_calls': 113664,
            'dispatch_fixtures': dispatch,
            'normal_c_recordings': {'totals': totals, 'reports': reports,
                'shadow_matches': sum(row['matched'] for row in totals['shadow'].values()),
                'sandbox_matches': sum(row['matched'] for row in totals['sandbox'].values()),
                'owners_with_completed_recording_comparisons': [e for e in ENTRIES if e not in source_only],
                'uncalled_source_only_owners': source_only, 'cold_owner_fixture_proofs_separate': True},
            'shared_leaf_regression': {'totals': leaf_totals, 'reports': leaf_reports,
                'shadow_matches': sum(row['matched'] for row in leaf_totals['shadow'].values()),
                'sandbox_matches': sum(row['matched'] for row in leaf_totals['sandbox'].values())}}

if __name__ == '__main__':
    print(json.dumps(verify(), indent=2))
