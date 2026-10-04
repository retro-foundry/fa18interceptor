"""Verify independent whole C, original loops, dispatch and recording limits."""
import json,re
from audit_corner_view_source import ROOT,ENTRIES,inventory
from verify_render_entry_helpers_recordings import seal,recording_group

def verify():
 source=inventory();all_pcs={r['pc'] for r in source['instructions']}
 fixtures=[];unions={kind:set() for kind in ('contract','real')}
 for kind in unions:
  for e in ENTRIES:
   path=f'build/recomp/corner_view_{kind}_{e}.log';text=(ROOT/path).read_text()
   assert f'oracle {e}: 16384 complete calls matched all registers, PC, full SR and all RAM' in text,(e,kind)
   owned=set(source['owners'][e]['source_pcs']);observed=owned&set(text.split('visited:')[1].splitlines()[0].split())
   unions[kind]|=observed
   fixtures.append({'owner':e,'kind':kind,'completed_calls':16384,'owner_pcs':len(owned),'observed_owner_pcs':len(observed),
    'unobserved_owner_pcs':sorted(owned-observed),'ordered_custom_writes':int(re.search(r'hardware: (\d+) ordered Custom writes validated',text)[1]),'log':seal(path)})
 assert unions['contract']==all_pcs,sorted(all_pcs-unions['contract'])
 # The layered record parent reaches a shared layer body with type zero;
 # the independent layer owner covers all of that body's other branches.
 for row in fixtures:
  if row['kind']=='contract' and row['unobserved_owner_pcs']:
   assert row['owner']=='C2CD28'
   layer=next(x for x in fixtures if x['owner']=='C2D082' and x['kind']=='contract')
   assert not layer['unobserved_owner_pcs']
   assert set(row['unobserved_owner_pcs'])<=set(source['owners']['C2D082']['source_pcs'])
 path='build/recomp/corner_view_loop_C2E758.log';text=(ROOT/path).read_text()
 assert '16384 non-returning 64-branch observations matched all registers, PC, full SR and all RAM' in text
 assert 'C2EA02' in text.split('visited:')[1].splitlines()[0].split()
 loop={'owner':'C2E758','loop_pc':'C2EA02','non_returning_observations':16384,'branches_per_observation':64,'log':seal(path),
       'child_contract':'controlled child supplies the original nonpositive CLIP_POINT domain; no return is supplied for the parent'}
 dispatch=[]
 for e in ENTRIES:
  for mode in (('on',) if e=='C200F6' else ('on','shadow','sandbox')):
   path=f'build/recomp/corner_view_dispatch_{e}_{mode}.log';text=(ROOT/path).read_text()
   assert f'oracle {e}: 1024 complete calls matched all registers, PC, full SR and all RAM' in text,(e,mode)
   hw,free=map(int,re.search(r'classification: (\d+) hardware-bearing source calls, (\d+) hardware-free source calls',text).groups());assert hw+free==1024
   owned=set(source['owners'][e]['source_pcs']);observed=owned&set(text.split('visited:')[1].splitlines()[0].split())
   dispatch.append({'owner':e,'mode':mode,'completed_calls':1024,'hardware_bearing':hw,'hardware_free':free,
    'owner_pcs':len(owned),'observed_owner_pcs':len(observed),'unobserved_owner_pcs':sorted(owned-observed),
    'ordered_custom_writes':int(re.search(r'hardware: (\d+) ordered Custom writes validated',text)[1]),'log':seal(path)})
 totals,reports=recording_group(list(ENTRIES))
 positive=[];cold=[]
 for e in ENTRIES:
  assert not any(totals[mode][e]['mismatched'] for mode in totals)
  if any(totals[mode][e]['matched'] for mode in totals):positive.append(e)
  else:
   assert not any(totals[mode][e]['calls'] for mode in totals),e+' called without a completed comparison'
   cold.append(e)
 assert positive==['C2E758','C2CE82','C2D082']
 return {'whole_entry_completed_calls':len(ENTRIES)*16384*2,'controlled_child_completed_calls':len(ENTRIES)*16384,
  'real_child_completed_calls':len(ENTRIES)*16384,'controlled_union_pcs':len(unions['contract']),'real_child_union_pcs':len(unions['real']),
  'fixture_coverage':fixtures,'non_returning_source_loop':loop,'non_returning_observations':16384,
  'actual_dispatch_completed_calls':sum(x['completed_calls'] for x in dispatch),'dispatch_fixtures':dispatch,
  'standalone_tail_timing_limit':'C200F6 restores A1/A5 above its RTS return. Complete whole C and ON are proven; aggregate base charge is 40 plus 16 MOVEM cycles. C20100 already owns all three resumable boundaries.',
  'normal_c_recordings':{'totals':totals,'reports':reports,
   'shadow_matches':sum(r['matched'] for r in totals['shadow'].values()),'sandbox_matches':sum(r['matched'] for r in totals['sandbox'].values()),
   'owners_with_completed_recording_comparisons':positive,'uncalled_owners_with_independent_whole_fixtures':cold,
   'unchanged_checker_zero_call_rejection':'C2CCA0: no completed comparisons; retained in build/recomp/corner_view_normal_all.log'}}
if __name__=='__main__':print(json.dumps(verify(),indent=2))
