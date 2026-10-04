"""Seal complete display-record parents, pair writers and original child sites."""
import argparse,json,re
from audit_command_dispatch import ROOT,audit,source_decoder
from recomp import classify,static_target
ENTRIES=('C0D74A','C0D752','C0DAA0','C0DAD0','C0DAD4','C0DADC','C0DAE6')
MANIFEST=ROOT/'analysis/data/display_record_selection_source_scope.json'
def inventory():
 result=audit(ENTRIES,additional_cold_entries=ENTRIES);_,d=source_decoder();pcs=set()
 for p in (ROOT/'port/recomp/generated').glob('recomp_*.c'):
  pcs.update(int(pc,16) for pc in re.findall(r'/\* ([0-9A-F]{6}): (?:jsr|bsr)\s',p.read_text()))
 incoming={e:[] for e in ENTRIES}
 for pc in sorted(pcs):
  length,op,handler,text=d.decode(pc);target=static_target(d,pc,op,handler,classify(handler));entry=f'{target:06X}' if target is not None else ''
  if entry in incoming:incoming[entry].append({'pc':f'{pc:06X}','return_pc':f'{pc+length:06X}','instruction':text,
   'bytes':''.join(f'{d.word(pc+i):04x}' for i in range(0,length,2))})
 result['original_static_incoming']=incoming;result['upgraded_registered_owners']=list(ENTRIES)
 result['candidate_inputs']={'address':'C0D720','bytes':''.join(f'{d.word(pc):04x}' for pc in range(0xc0d720,0xc0d730,2)),
  'meaning':'Four original signed x/z candidate pairs; y is reloaded from C45A66 and shifted by two.'}
 result['shared_frame_exits']={'C0DA38':'Shared selection arm reached by branches within each parent LINK frame, not a separate call registration.',
  'C0DA90':'Original accepted UNLINK/RTS','C0DA9C':'Original rejected UNLINK/RTS'}
 fixtures=(ROOT/'tools/recomp/display_record_selection_fixture.h').read_text()
 for entry in ENTRIES:
  caller=re.search(r'case 0x'+entry.lower()+r'u:REG_PPC=0x([a-f0-9]{6})u;',fixtures)
  assert caller and any(row['pc']==caller[1].upper() for row in incoming[entry]),entry+' fixture caller differs'
 sites={(int(c['target'],16),int(c['return_pc'],16)) for owner in result['owners'].values() for c in owner['child_call_sites']}
 for path in ('port/game/glue/glue_display_record_selection.c','tools/recomp/display_record_selection_contract_children.c'):
  contents=(ROOT/path).read_text()
  pairs={(int(e,16),int(r,16)) for e,r in re.findall(r'\{0x([a-f0-9]{6}),0x([a-f0-9]{6})\}',contents)}
  assert pairs==sites,path+' child contracts differ'
 owned={row['pc'] for row in result['instructions']}
 for kind in ('oracle','contract_oracle','dispatch_oracle'):
  contents=(ROOT/f'tools/recomp/display_record_selection_{kind}.c').read_text()
  array=re.search(r'source_boundaries\[\]=\{([^}]+)\}',contents)[1]
  assert set(re.findall(r'0x([A-F0-9]{6})u',array))==owned,kind+' ownership differs'
 result['sealed_native_child_contracts']=len(sites)
 result['sealed_oracle_ownership_arrays']=3
 return result
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--write',action='store_true');a=p.parse_args();r=inventory()
 if a.write:MANIFEST.write_text(json.dumps(r,indent=2)+'\n',encoding='utf-8')
 elif json.loads(MANIFEST.read_text())!=r:raise ValueError('Display-record selection source changed')
 print('Display-record selection:',r['unique_instruction_count'],'unique /',r['shared_instruction_count'],'shared instructions')
 for e,o in r['owners'].items():print(e,o['instruction_count'],'boundaries,',len(o['child_call_sites']),'child sites')
if __name__=='__main__':main()
