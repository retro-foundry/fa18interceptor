"""Validate the full normal-C batch plus its independently recorded split.

The generic checker rejects the absorbed C21C4C row in the combined batch.
Its independent recording run must have actual completed comparisons; no
zero-call exemption or bounded-fixture substitute is accepted here.
"""
import json
from audit_face_list_parents_source import ROOT,ENTRIES

def inventory():
    totals={mode:{e:0 for e in ENTRIES} for mode in ('shadow','sandbox')}
    reports=[]
    for mode in totals:
        for name in ('demo01','qual_carrier_success','qual_fail_crashes'):
            batch=ROOT/'build/recomp'/f'whole_call_{"_".join(ENTRIES)}_{name}_{mode}.json'
            split=ROOT/'build/recomp'/f'whole_call_C21C4C_{name}_{mode}.json'
            rows={r['entry']:r for r in json.loads(batch.read_text()) if r['entry'] in ENTRIES}
            isolated={r['entry']:r for r in json.loads(split.read_text()) if r['entry']=='C21C4C'}
            assert set(rows)==set(ENTRIES) and set(isolated)=={'C21C4C'}
            assert rows['C21C4C']['calls']==0 and rows['C21C4C']['matched']==0
            assert not any(r['mismatched'] for r in list(rows.values())+list(isolated.values()))
            reports.append({'recording':name,'mode':mode,'batch':str(batch.relative_to(ROOT)).replace('\\','/'),'isolated':str(split.relative_to(ROOT)).replace('\\','/'),'batch_rows':list(rows.values()),'isolated_rows':list(isolated.values())})
            rows['C21C4C']=isolated['C21C4C']
            for e,r in rows.items():totals[mode][e]+=r['matched']
    assert all(any(totals[mode][e] for mode in totals) for e in ENTRIES)
    assert totals['shadow']['C21C4C']==165 and totals['sandbox']['C21C4C']==166
    return {'totals':totals,'reports':reports,'shadow_matches':sum(totals['shadow'].values()),'sandbox_matches':sum(totals['sandbox'].values())}

if __name__=='__main__':
    result=inventory()
    print(f"normal C: all fourteen owners complete original recorded comparisons; {result['shadow_matches']} shadow / {result['sandbox_matches']} sandbox matches; split independently 165/166")
