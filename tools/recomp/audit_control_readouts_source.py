"""Seal whole control-record readout owners and both bounded magnitude jumps."""
import argparse,json,re
from audit_command_dispatch import ROOT,audit,source_decoder
from recomp import classify,static_target

ENTRIES=('C12950','C131BE','C13176','C133B2','C13396','C52EC8')
MANIFEST=ROOT/'analysis/data/control_readouts_source_scope.json'
TARGETS={0xc13252:tuple(range(0xc13256,0xc1325e,2)),
         0xc132ba:tuple(range(0xc132be,0xc132c6,2))}

def inventory():
 result=audit(ENTRIES,dynamic_targets=TARGETS,additional_cold_entries=ENTRIES)
 _,decoder=source_decoder()
 guards={0xc13252:(0xc13242,0xc13246,0xc13248,0xc1324e,0xc13250,0xc13252),
         0xc132ba:(0xc1329c,0xc132a2,0xc132a4,0xc132a6,0xc132ac,0xc132b0,0xc132b6,0xc132b8,0xc132ba)}
 jumps={}
 for pc,targets in TARGETS.items():
  rows=[]
  for at in guards[pc]+targets:
   length,opcode,handler,text=decoder.decode(at)
   rows.append({'pc':f'{at:06X}','instruction':text,
       'bytes':''.join(f'{decoder.word(at+i):04x}' for i in range(0,length,2))})
  jumps[f'{pc:06X}']={'targets':[f'{at:06X}' for at in targets],'original_guard_and_stubs':rows,
   'guard':'unsigned D0 < 4 before ASL.L #1' if pc==0xc13252 else 'signed 0 <= D0 < 4 before ASL.L #1',
   'shared_frame_owner':'C131BE','standalone_callability':False}
 result['original_computed_jumps']=jumps
 result['shared_branch_entries']=[f'{pc:06X}' for values in TARGETS.values() for pc in values]
 result['new_registered_owners']=['C12950','C131BE']
 result['upgraded_registered_owners']=list(ENTRIES[2:])
 result['shared_entry_integration']='The eight two-byte branch stubs retain the LINK frame of C131BE; complete parent ownership covers them. They are not standalone callable functions.'
 incoming={e:[] for e in ENTRIES};candidates=set()
 for path in (ROOT/'port/recomp/generated').glob('recomp_*.c'):
  candidates.update(int(pc,16) for pc in re.findall(r'/\* ([0-9A-F]{6}): (?:jsr|bsr)\s',path.read_text()))
 for pc in sorted(candidates):
  length,opcode,handler,text=decoder.decode(pc);kind=classify(handler)
  target=static_target(decoder,pc,opcode,handler,kind);entry=f'{target:06X}' if target is not None else ''
  if entry in incoming:incoming[entry].append({'pc':f'{pc:06X}','return_pc':f'{pc+length:06X}','kind':kind,'instruction':text,
   'bytes':''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2))})
 result['original_static_incoming']=incoming
 fixtures=(ROOT/'tools/recomp/control_readouts_fixture.h').read_text()
 for entry in ENTRIES:
  case=re.search(r'case 0x'+entry.lower()+r'u:REG_PPC=0x([a-f0-9]{6})u;',fixtures)
  assert case and any(row['pc']==case[1].upper() for row in incoming[entry]),entry+' fixture caller differs'
 sites={(int(c['target'],16),int(c['return_pc'],16)) for o in result['owners'].values() for c in o['child_call_sites']}
 for path in ('port/game/glue/glue_control_readouts.c','tools/recomp/control_readouts_contract_children.c'):
  contents=(ROOT/path).read_text();pairs={(int(e,16),int(r,16)) for e,r in re.findall(r'\{0x([a-f0-9]{6}),0x([a-f0-9]{6})\}',contents)}
  assert pairs==sites,path+' child contracts differ'
 owned={r['pc'] for r in result['instructions']}
 for name in ('oracle','contract_oracle','dispatch_oracle'):
  contents=(ROOT/'tools/recomp'/f'control_readouts_{name}.c').read_text();pcs=re.search(r'source_boundaries\[\]=\{([^}]+)\}',contents).group(1)
  assert set(re.findall(r'0x([A-F0-9]{6})u',pcs))==owned,name+' ownership differs'
 result['sealed_native_child_contracts']=len(sites);result['sealed_oracle_ownership_arrays']=3
 # Every signed word survives the original absolute-word operation and
 # arithmetic shift by nine as -64 or 0..63. The signed BLE must be taken.
 values=set()
 for value in range(65536):
  signed=value if value<32768 else value-65536
  if signed<0:
   absolute=(-signed)&65535;signed=absolute if absolute<32768 else absolute-65536
  values.add(signed>>9)
 assert values=={-64}|set(range(64))
 sequence=(0xc133e0,0xc133e2,0xc133e4,0xc133e8,0xc133ea,0xc133ee,0xc133f0,0xc133f4,0xc133f8,0xc133fa)
 result['unreachable_entry_paths']={'C133FA':{'owner':'C133B2','exhaustive_word_inputs':65536,
  'possible_signed_results':sorted(values),'signed_upper_bound':63,
  'source_guard':[row for row in result['instructions'] if int(row['pc'],16) in sequence],
  'reason':'Original NEG.W leaves -32768 negative. ASR.W #9 gives -64 or 0..63; CMP.W #63 / BLE always takes the branch. The source body and timing boundary remain implemented.'}}
 return result

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--write',action='store_true');args=p.parse_args();r=inventory()
 if args.write:MANIFEST.write_text(json.dumps(r,indent=2)+'\n')
 elif json.loads(MANIFEST.read_text())!=r:raise ValueError('Control-readout source scope changed')
 print('Control readouts:',r['unique_instruction_count'],'unique /',r['shared_instruction_count'],'shared; both original computed guards sealed')
 for e,o in r['owners'].items():print(e,o['instruction_count'],'PCs,',o['additional_cold_instructions'],'cold,',len(o['child_call_sites']),'children')
 for pc,j in r['original_computed_jumps'].items():
  print(pc,j['guard'],'->',','.join(j['targets']))
  for row in j['original_guard_and_stubs']:print(row['pc'],row['bytes'],row['instruction'])
if __name__=='__main__':main()
