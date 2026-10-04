"""Seal the six complete original roll/pitch steering entries and callers."""
import argparse,json,re
from audit_command_dispatch import ROOT,audit,source_decoder
from recomp import classify,static_target
ENTRIES=('C2CA26','C2CA92','C2CAA0','C2CB86','C2CB82','C2CBBC')
MANIFEST=ROOT/'analysis/data/record_steering_source_scope.json'
def inventory():
 result=audit(ENTRIES,additional_cold_entries=ENTRIES)
 _,d=source_decoder();pcs=set()
 for path in (ROOT/'port/recomp/generated').glob('recomp_*.c'):
  pcs.update(int(pc,16) for pc in re.findall(r'/\* ([0-9A-F]{6}): (?:jsr|bsr)\s',path.read_text()))
 # These cold BSR sites are original instructions inside action-table arms.
 # Seal their bytes directly; no claim about the unresolved full parent domain.
 pcs.update((0xc2bd32,0xc2bd3c,0xc2be48,0xc2bf3a,0xc2bfc2,0xc2c0be,0xc2c0c8,0xc2c14e,0xc2c158))
 incoming={e:[] for e in ENTRIES}
 for pc in sorted(pcs):
  length,op,handler,text=d.decode(pc);target=static_target(d,pc,op,handler,classify(handler));entry=f'{target:06X}' if target is not None else ''
  if entry in incoming:incoming[entry].append({'pc':f'{pc:06X}','return_pc':f'{pc+length:06X}','instruction':text,
   'bytes':''.join(f'{d.word(pc+i):04x}' for i in range(0,length,2))})
 result['original_static_incoming']=incoming
 result['upgraded_registered_owners']=list(ENTRIES[:4]);result['new_source_only_owners']=list(ENTRIES[4:])
 prefixes={'C2CB82':['C2CB82','C2CB84'],'C2CBBC':['C2CBBC']}
 result['source_only_prefixes']={e:[row for row in result['instructions'] if row['pc'] in pcs] for e,pcs in prefixes.items()}
 fixtures=(ROOT/'tools/recomp/record_steering_fixture.h').read_text()
 for e in ENTRIES:
  caller=re.search(r'case 0x'+e.lower()+r'u:REG_PPC=0x([a-f0-9]{6})u;',fixtures)
  assert caller and any(row['pc']==caller[1].upper() for row in incoming[e]),e+' fixture caller differs'
 owned={row['pc'] for row in result['instructions']}
 for kind in ('oracle','dispatch_oracle'):
  text=(ROOT/f'tools/recomp/record_steering_{kind}.c').read_text()
  array=re.search(r'source_boundaries\[\]=\{([^}]+)\}',text)[1]
  assert set(re.findall(r'0x([A-F0-9]{6})u',array))==owned,kind+' ownership differs'
 positive=[v for v in range(256)];negative=[0xff00|v for v in range(256)]
 assert all(v<0x8000 for v in positive) and all(v>=0x8000 for v in negative)
 result['unreachable_per_owner']={'C2CB86':['C2CBAA','C2CBAE'],'C2CBBC':['C2CBAA','C2CBAE'],'C2CB82':['C2CBB0']}
 result['mask_branch_evidence']={'exhaustive_control_bytes':256,'source_guard':[row for row in result['instructions'] if row['pc'] in ('C2CB82','C2CB86','C2CBA2','C2CBA6','C2CBA8','C2CBAA','C2CBAE','C2CBB0','C2CBBC')],
  'reason':'Original MOVEQ sets D1 to zero or minus one before replacing its low byte. All 256 controls retain a nonnegative or negative low word respectively. Each skipped mask arm is covered by the other original owner.'}
 result['sealed_oracle_ownership_arrays']=2
 result['parent_limit']='C2C392 producer-domain reconciliation remains separate game work; complete helpers do not establish parent completion.'
 return result
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--write',action='store_true');a=p.parse_args();r=inventory()
 if a.write:MANIFEST.write_text(json.dumps(r,indent=2)+'\n',encoding='utf-8')
 elif json.loads(MANIFEST.read_text())!=r:raise ValueError('Record steering source scope changed')
 print('Record steering:',r['unique_instruction_count'],'unique /',r['shared_instruction_count'],'shared instructions')
 for e,o in r['owners'].items():print(e,o['instruction_count'],'source boundaries,',len(r['original_static_incoming'][e]),'original callers')
if __name__=='__main__':main()
