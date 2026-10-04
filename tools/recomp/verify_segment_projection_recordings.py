"""Verify whole CPU/RAM, non-returning branches, dispatch and recordings."""
import json,re
from audit_segment_projection_source import ROOT,ENTRIES,inventory
from verify_render_entry_helpers_recordings import seal,recording_group,RECORDINGS

def verify():
 source=inventory();all_pcs={r['pc'] for r in source['instructions']}
 fixtures=[];unions={kind:set() for kind in ('contract','real')}
 for kind in unions:
  for e in ENTRIES:
   path=f'build/recomp/segment_projection_{kind}_{e}.log';text=(ROOT/path).read_text()
   assert f'oracle {e}: 16384 complete calls matched all registers, PC, full SR and all RAM' in text,(e,kind)
   owned=set(source['owners'][e]['source_pcs']);observed=owned&set(text.split('visited:')[1].splitlines()[0].split())
   if kind=='contract':assert observed==owned,(e,owned-observed)
   unions[kind]|=observed
   fixtures.append({'owner':e,'kind':kind,'completed_calls':16384,'owner_pcs':len(owned),'observed_owner_pcs':len(observed),
    'unobserved_owner_pcs':sorted(owned-observed),'ordered_custom_writes':int(re.search(r'hardware: (\d+) ordered Custom writes validated',text)[1]),'log':seal(path)})
 assert unions['contract']==all_pcs
 parallel=[]
 for e,pc in zip(ENTRIES[4:8],source['unrounded_parallel_source_loops']):
  path=f'build/recomp/segment_projection_parallel_{e}.log';text=(ROOT/path).read_text()
  assert f'oracle {e}: 16384 non-returning 64-branch observations matched all registers, PC, full SR and all RAM' in text,e
  observed=set(text.split('visited:')[1].splitlines()[0].split());assert pc in observed
  parallel.append({'owner':e,'loop_pc':pc,'non_returning_observations':16384,'branches_per_observation':64,'observed_pcs':sorted(observed),'log':seal(path)})
 dispatch=[]
 for e in ENTRIES:
  for mode in ('on','shadow','sandbox'):
   path=f'build/recomp/segment_projection_dispatch_{e}_{mode}.log';text=(ROOT/path).read_text()
   assert f'oracle {e}: 1024 complete calls matched all registers, PC, full SR and all RAM' in text,(e,mode)
   hw,free=map(int,re.search(r'classification: (\d+) hardware-bearing source calls, (\d+) hardware-free source calls',text).groups());assert hw+free==1024
   owned=set(source['owners'][e]['source_pcs']);observed=owned&set(text.split('visited:')[1].splitlines()[0].split())
   dispatch.append({'owner':e,'mode':mode,'completed_calls':1024,'hardware_bearing':hw,'hardware_free':free,'owner_pcs':len(owned),
    'observed_owner_pcs':len(observed),'unobserved_owner_pcs':sorted(owned-observed),
    'ordered_custom_writes':int(re.search(r'hardware: (\d+) ordered Custom writes validated',text)[1]),'log':seal(path)})
 totals,reports=recording_group(list(ENTRIES))
 assert all(any(totals[mode][e]['matched'] for mode in totals) for e in ENTRIES)
 return {'whole_entry_completed_calls':len(ENTRIES)*16384*2,'controlled_child_completed_calls':len(ENTRIES)*16384,
  'real_child_completed_calls':len(ENTRIES)*16384,'controlled_union_pcs':len(unions['contract']),'real_child_union_pcs':len(unions['real']),
  'fixture_coverage':fixtures,'non_returning_source_loops':parallel,'non_returning_observations':65536,
  'actual_dispatch_completed_calls':len(ENTRIES)*1024*3,'dispatch_fixtures':dispatch,
  'normal_c_recordings':{'totals':totals,'reports':reports,
   'shadow_matches':sum(r['matched'] for r in totals['shadow'].values()),'sandbox_matches':sum(r['matched'] for r in totals['sandbox'].values()),
   'owners_with_completed_recording_comparisons':list(ENTRIES)}}
if __name__=='__main__':print(json.dumps(verify(),indent=2))
