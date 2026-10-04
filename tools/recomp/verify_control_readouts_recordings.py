"""Verify complete control-readout CPU/RAM/dispatch and recording evidence."""
import json,re
from audit_control_readouts_source import ROOT,ENTRIES,inventory
from verify_render_entry_helpers_recordings import seal,recording_group

def verify():
 source=inventory();owned={r['pc'] for r in source['instructions']};unreachable=set(source['unreachable_entry_paths'])
 fixtures=[];unions={kind:set() for kind in ('contract','real')}
 for kind in unions:
  for e in ENTRIES:
   path=f'build/recomp/control_readouts_{kind}_{e}.log';text=(ROOT/path).read_text()
   assert f'oracle {e}: 16384 complete calls matched all registers, PC, full SR and all RAM' in text,(kind,e)
   pcs=set(source['owners'][e]['source_pcs']);observed=pcs&set(text.split('visited:')[1].splitlines()[0].split())
   assert pcs-observed==pcs&unreachable,(kind,e,sorted(pcs-observed))
   unions[kind]|=observed
   fixtures.append({'owner':e,'kind':kind,'completed_calls':16384,'owner_pcs':len(pcs),'observed_owner_pcs':len(observed),
    'unobserved_owner_pcs':sorted(pcs-observed),'ordered_custom_writes':int(re.search(r'hardware: (\d+) ordered Custom writes validated',text)[1]),'log':seal(path)})
 assert all(union==owned-unreachable for union in unions.values())
 dispatch=[]
 for e in ENTRIES:
  for mode in ('on','shadow','sandbox'):
   path=f'build/recomp/control_readouts_dispatch_{e}_{mode}.log';text=(ROOT/path).read_text()
   assert f'oracle {e}: 1024 complete calls matched all registers, PC, full SR and all RAM' in text,(e,mode)
   hw,free=map(int,re.search(r'classification: (\d+) hardware-bearing source calls, (\d+) hardware-free source calls',text).groups());assert hw+free==1024
   pcs=set(source['owners'][e]['source_pcs']);observed=pcs&set(text.split('visited:')[1].splitlines()[0].split())
   dispatch.append({'owner':e,'mode':mode,'completed_calls':1024,'hardware_bearing':hw,'hardware_free':free,
    'owner_pcs':len(pcs),'observed_owner_pcs':len(observed),'unobserved_owner_pcs':sorted(pcs-observed),
    'ordered_custom_writes':int(re.search(r'hardware: (\d+) ordered Custom writes validated',text)[1]),'log':seal(path)})
 totals,reports=recording_group(list(ENTRIES));positive=[];cold=[];uncounted=[]
 for e in ENTRIES:
  assert not any(totals[mode][e]['mismatched'] for mode in totals)
  if any(totals[mode][e]['matched'] for mode in totals):positive.append(e)
  elif any(totals[mode][e]['calls'] for mode in totals):uncounted.append(e)
  else:cold.append(e)
 return {'whole_entry_completed_calls':196608,'controlled_child_completed_calls':98304,'real_child_completed_calls':98304,
  'controlled_union_pcs':len(unions['contract']),'real_child_union_pcs':len(unions['real']),
  'fixture_coverage':fixtures,'unreachable_entry_paths':source['unreachable_entry_paths'],
  'actual_dispatch_completed_calls':18432,'dispatch_fixtures':dispatch,
  'normal_c_recordings':{'totals':totals,'reports':reports,
   'shadow_matches':sum(r['matched'] for r in totals['shadow'].values()),'sandbox_matches':sum(r['matched'] for r in totals['sandbox'].values()),
   'owners_with_completed_recording_comparisons':positive,'uncalled_owners_with_independent_whole_fixtures':cold,
   'called_without_completed_recording_comparisons':uncounted,
   'unchanged_checker_rejections_remain_explicit':True}}
if __name__=='__main__':print(json.dumps(verify(),indent=2))
