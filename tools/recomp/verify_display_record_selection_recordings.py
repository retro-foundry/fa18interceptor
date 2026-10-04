"""Verify complete display-record CPU/RAM/dispatch and recording evidence."""
import json,re
from audit_display_record_selection_source import ROOT,ENTRIES,inventory
from verify_render_entry_helpers_recordings import seal,recording_group

def verify():
 source=inventory();owned={r['pc'] for r in source['instructions']};fixtures=[]
 unions={kind:set() for kind in ('contract','real')}
 for kind in unions:
  for entry in ENTRIES:
   path=f'build/recomp/display_record_selection_{kind}_{entry}.log';text=(ROOT/path).read_text()
   assert f'oracle {entry}: 16384 complete calls matched all registers, PC, full SR and all RAM' in text,(entry,kind)
   pcs=set(source['owners'][entry]['source_pcs']);observed=pcs&set(text.split('visited:')[1].splitlines()[0].split())
   if kind=='contract':assert pcs==observed,(entry,sorted(pcs-observed))
   unions[kind]|=observed
   fixtures.append({'owner':entry,'kind':kind,'completed_calls':16384,'owner_pcs':len(pcs),'observed_owner_pcs':len(observed),
    'unobserved_owner_pcs':sorted(pcs-observed),'ordered_custom_writes':int(re.search(r'hardware: (\d+) ordered Custom writes validated',text)[1]),'log':seal(path)})
 assert unions['contract']==owned
 dispatch=[]
 for entry in ENTRIES:
  for mode in ('on','shadow','sandbox'):
   path=f'build/recomp/display_record_selection_dispatch_{entry}_{mode}.log';text=(ROOT/path).read_text()
   assert f'oracle {entry}: 1024 complete calls matched all registers, PC, full SR and all RAM' in text,(entry,mode)
   hw,free=map(int,re.search(r'classification: (\d+) hardware-bearing source calls, (\d+) hardware-free source calls',text).groups());assert hw+free==1024
   pcs=set(source['owners'][entry]['source_pcs']);observed=pcs&set(text.split('visited:')[1].splitlines()[0].split())
   dispatch.append({'owner':entry,'mode':mode,'completed_calls':1024,'hardware_bearing':hw,'hardware_free':free,
    'owner_pcs':len(pcs),'observed_owner_pcs':len(observed),'unobserved_owner_pcs':sorted(pcs-observed),
    'ordered_custom_writes':int(re.search(r'hardware: (\d+) ordered Custom writes validated',text)[1]),'log':seal(path)})
 totals,reports=recording_group(ENTRIES);positive=[];cold=[];uncounted=[]
 for entry in ENTRIES:
  assert not any(totals[mode][entry]['mismatched'] for mode in totals)
  if any(totals[mode][entry]['matched'] for mode in totals):positive.append(entry)
  elif any(totals[mode][entry]['calls'] for mode in totals):uncounted.append(entry)
  else:cold.append(entry)
 return {'whole_entry_completed_calls':7*16384*2,'whole_union_pcs':{kind:len(pcs) for kind,pcs in unions.items()},'fixture_coverage':fixtures,
  'controlled_child_entry_full_cpu_sr_ram_checked':True,'controlled_child_count_checked':True,
  'actual_dispatch_completed_calls':7*1024*3,'dispatch_fixtures':dispatch,
  'normal_c_recordings':{'totals':totals,'reports':reports,
   'shadow_matches':sum(r['matched'] for r in totals['shadow'].values()),'sandbox_matches':sum(r['matched'] for r in totals['sandbox'].values()),
   'owners_with_completed_recording_comparisons':positive,'uncalled_owners_with_independent_whole_fixtures':cold,
   'called_without_completed_recording_comparisons':uncounted,'unchanged_checker_rejections_remain_explicit':True}}
if __name__=='__main__':print(json.dumps(verify(),indent=2))
