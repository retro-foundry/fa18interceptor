"""Aggregate completed whole-owner evidence without claiming cold PCs were entered."""
import hashlib,json,re
from audit_hud_render_parents_source import ROOT,ENTRIES,inventory
from verify_hud_render_cold_segments import verify as verify_cold

def seal(path):
 return {'path':path,'sha256':hashlib.sha256((ROOT/path).read_bytes()).hexdigest()}

def verify():
 source=inventory();fixtures=[];dispatch=[];unions={k:set() for k in ('contract','real')}
 for kind in unions:
  for entry in ENTRIES:
   path=f'build/recomp/hud_render_parents_{kind}_{entry}.log';text=(ROOT/path).read_text()
   assert f'oracle {entry}: 16384 complete calls matched all registers, PC, full SR and all RAM' in text,(kind,entry)
   owned=set(source['owners'][entry]['source_pcs'])
   observed=owned&set(text.split('visited:')[1].splitlines()[0].split())
   missing=owned-observed
   if kind=='contract':assert missing==({'C3429E'} if entry=='C34146' else {'C32380'} if entry=='C322EE' else set()),(entry,missing)
   writes=int(re.search(r'hardware: (\d+) ordered Custom writes validated',text)[1])
   unions[kind]|=observed
   fixtures.append({'owner':entry,'kind':kind,'completed_calls':16384,'owner_pcs':len(owned),'observed_owner_pcs':len(observed),'unobserved_owner_pcs':sorted(missing),'ordered_custom_writes':writes,'log':seal(path)})
 for entry in ENTRIES:
  for mode in ('on','shadow','sandbox'):
   path=f'build/recomp/hud_render_parents_dispatch_{entry}_{mode}.log';text=(ROOT/path).read_text()
   assert f'oracle {entry}: 1024 complete calls matched all registers, PC, full SR and all RAM' in text,(entry,mode)
   owned=set(source['owners'][entry]['source_pcs']);observed=owned&set(text.split('visited:')[1].splitlines()[0].split())
   hw,free=map(int,re.search(r'classification: (\d+) hardware-bearing source calls, (\d+) hardware-free source calls',text).groups())
   assert hw+free==1024
   writes=int(re.search(r'hardware: (\d+) ordered Custom writes validated',text)[1])
   dispatch.append({'owner':entry,'mode':mode,'completed_calls':1024,'hardware_bearing':hw,'hardware_free':free,'owner_pcs':len(owned),'observed_owner_pcs':len(observed),'unobserved_owner_pcs':sorted(owned-observed),'ordered_custom_writes':writes,'log':seal(path)})
 cold=verify_cold();assert len(unions['contract'])==1350
 assert unions['contract']|set(cold['whole_entry_cold_pcs'])=={r['pc'] for r in source['instructions']}
 totals={mode:{e:{k:0 for k in ('calls','compared','matched','mismatched','hardware','incomplete')} for e in ENTRIES} for mode in ('shadow','sandbox')};reports=[]
 for mode in totals:
  for name in ('demo01','qual_carrier_success','qual_fail_crashes'):
   path=f'build/recomp/whole_call_{"_".join(ENTRIES)}_{name}_{mode}.json'
   rows=[r for r in json.loads((ROOT/path).read_text()) if r['entry'] in ENTRIES]
   assert len(rows)==15 and not any(r['mismatched'] for r in rows)
   for row in rows:
    for k in totals[mode][row['entry']]:totals[mode][row['entry']][k]+=row[k]
   reports.append({'mode':mode,'recording':name,'rows':rows,'report':seal(path)})
 assert all(any(totals[mode][e]['matched'] for mode in totals) for e in ENTRIES)
 return {'whole_entry_completed_calls':491520,'controlled_child_completed_calls':245760,'real_child_completed_calls':245760,'controlled_union_pcs':len(unions['contract']),'real_child_union_pcs':len(unions['real']),'fixture_coverage':fixtures,'cold_segments':cold,'additional_production_segment_calls':32768,'actual_dispatch_completed_calls':46080,'dispatch_fixtures':dispatch,'normal_c_recordings':{'totals':totals,'reports':reports,'shadow_matches':sum(r['matched'] for r in totals['shadow'].values()),'sandbox_matches':sum(r['matched'] for r in totals['sandbox'].values()),'each_owner_has_completed_comparisons':True}}

if __name__=='__main__':
 result=verify()
 print(json.dumps(result,indent=2))
