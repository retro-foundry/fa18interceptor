"""Aggregate complete owners and independently executed cold production segments."""
import hashlib
import json
import re
from audit_render_leaf_helpers_source import ROOT, ENTRIES, inventory
from check_render_leaf_helpers_segments import SEGMENTS

def seal(path):
    return {'path': path, 'sha256': hashlib.sha256((ROOT/path).read_bytes()).hexdigest()}

def verify():
    source = inventory()
    fixtures, dispatch, segments = [], [], []
    unions = {kind: set() for kind in ('contract', 'real')}
    cold = {'C30266', 'C3027A', 'C30296', 'C302E4', 'C2FBC2', 'C2FBC4', 'C2FBC6'}
    for kind in unions:
        for entry in ENTRIES:
            path = f'build/recomp/render_leaf_helpers_{kind}_{entry}.log'
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
    for entry, owned in SEGMENTS.items():
        path = f'build/recomp/render_leaf_helpers_segment_{entry}.log'
        text = (ROOT/path).read_text()
        assert f'oracle {entry}: 16384 production segment calls matched all registers, PC, full SR and all RAM' in text
        observed = set(text.split('visited:')[1].splitlines()[0].split())
        assert observed == owned
        segments.append({'entry': entry, 'completed_calls': 16384, 'source_pcs': sorted(owned), 'log': seal(path)})
    all_pcs = {row['pc'] for row in source['instructions']}
    assert len(unions['contract']) == 964
    assert unions['contract'] | cold == all_pcs
    assert cold <= set.union(*SEGMENTS.values())
    for entry in ENTRIES:
        for mode in ('on', 'shadow', 'sandbox'):
            path = f'build/recomp/render_leaf_helpers_dispatch_{entry}_{mode}.log'
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
    totals = {mode: {entry: {key: 0 for key in ('calls', 'compared', 'matched', 'mismatched', 'hardware', 'incomplete')}
                     for entry in ENTRIES} for mode in ('shadow', 'sandbox')}
    reports = []
    for mode in totals:
        for name in ('demo01', 'qual_carrier_success', 'qual_fail_crashes'):
            path = f'build/recomp/whole_call_{"_".join(ENTRIES)}_{name}_{mode}.json'
            rows = [row for row in json.loads((ROOT/path).read_text()) if row['entry'] in ENTRIES]
            assert len(rows) == 12 and not any(row['mismatched'] for row in rows)
            for row in rows:
                for key in totals[mode][row['entry']]:
                    totals[mode][row['entry']][key] += row[key]
            reports.append({'mode': mode, 'recording': name, 'rows': rows, 'report': seal(path)})
    assert all(any(totals[mode][entry]['matched'] for mode in totals) for entry in ENTRIES)
    return {'whole_entry_completed_calls': 393216, 'controlled_child_completed_calls': 196608,
            'real_child_completed_calls': 196608, 'controlled_union_pcs': len(unions['contract']),
            'real_child_union_pcs': len(unions['real']), 'fixture_coverage': fixtures,
            'whole_call_cold_pcs': sorted(cold), 'cold_segments': segments,
            'additional_production_segment_calls': 81920, 'actual_dispatch_completed_calls': 36864,
            'dispatch_fixtures': dispatch,
            'normal_c_recordings': {'totals': totals, 'reports': reports,
                'shadow_matches': sum(row['matched'] for row in totals['shadow'].values()),
                'sandbox_matches': sum(row['matched'] for row in totals['sandbox'].values()),
                'each_owner_has_completed_comparisons': True}}

if __name__ == '__main__':
    result = verify()
    print(json.dumps(result, indent=2))
