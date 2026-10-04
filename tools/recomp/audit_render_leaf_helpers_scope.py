"""Seal the next complete plot, span, polygon, lane and glyph helpers."""
import argparse,hashlib,json,re
from audit_command_dispatch import ROOT,audit,source_decoder
from recomp import classify,static_target

ENTRIES=('C2F5C0','C2F5D4','C2F5F4','C2F60A','C2F66E','C310E2','C301F0','C304B2','C32806','C2FA78','C2FA7E','C2FD8C')
MANIFEST=ROOT/'analysis/data/render_leaf_helpers_scope_inventory.json'

def inventory():
 _,decoder=source_decoder()
 # C2F692 masks the colour to four bits, and C2F696/C2F698 multiply
 # it by four before the C2F69A longword load. Seal every original slot.
 evidence=[]
 for pc,expected in ((0xc2f600,'49f900c2f786'),(0xc2f632,'49f900c2f786'),
                     (0xc2f682,'49f900c2f7e6'),(0xc2f692,'0242000f'),
                     (0xc2f696,'d442'),(0xc2f698,'d442'),
                     (0xc2f69a,'28742000'),(0xc2f764,'4ed4')):
  length,opcode,handler,assembly=decoder.decode(pc)
  raw=''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2))
  assert raw==expected,(hex(pc),raw)
  evidence.append({'pc':f'{pc:06X}','instruction':assembly,'bytes':raw})
 tables={}
 for base in (0xc2f786,0xc2f7e6):
  tables[base]=[{'selector':n,'slot':f'{base+4*n:06X}',
                'bytes':f'{decoder.word(base+4*n):04x}{decoder.word(base+4*n+2):04x}',
                'target':f'{decoder.word(base+4*n)<<16|decoder.word(base+4*n+2):06X}'} for n in range(16)]
 rows={};owners={};result=None
 for entry in ENTRIES:
  bases=(0xc2f786,0xc2f7e6) if entry=='C2F66E' else (0xc2f786,)
  targets=sorted({int(slot['target'],16) for base in bases for slot in tables[base]})
  part=audit((entry,),dynamic_targets={0xc2f764:targets},additional_cold_entries=(entry,))
  if result is None:result=part.copy()
  owners.update(part['owners']);rows.update({r['pc']:r for r in part['instructions']})
 result['owners']=owners;result['instructions']=[rows[pc] for pc in sorted(rows)]
 sets=[set(o['source_pcs']) for o in owners.values()];union=set.union(*sets)
 result['unique_instruction_count']=len(rows)
 result['shared_instruction_count']=sum(sum(pc in pcs for pcs in sets)>1 for pc in union)
 packed=b''.join(int(r['pc'],16).to_bytes(4,'big')+bytes.fromhex(r['bytes']) for r in result['instructions'])
 result['owned_pc_and_source_bytes_sha256']=hashlib.sha256(packed).hexdigest()
 result['pixel_writer_dispatch_authority']={'source_instructions':evidence,'tables':{f'{b:06X}':v for b,v in tables.items()},'selector_count':16,'production_guard_added':False,'block_owner_uses_both_tables':True}
 incoming={e:[] for e in ENTRIES};candidates=set()
 for path in (ROOT/'port/recomp/generated').glob('recomp_*.c'):
  candidates.update(int(pc,16) for pc in re.findall(r'/\* ([0-9A-F]{6}): (?:jsr|bsr)\s',path.read_text()))
 for pc in sorted(candidates):
  decoded=decoder.decode(pc)
  if not decoded:continue
  length,opcode,handler,assembly=decoded;kind=classify(handler)
  if kind not in ('jsr','bsr'):continue
  target=static_target(decoder,pc,opcode,handler,kind)
  if target is None or f'{target:06X}' not in incoming:continue
  incoming[f'{target:06X}'].append({'pc':f'{pc:06X}','return_pc':f'{pc+length:06X}','instruction':assembly,'bytes':''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2))})
 caller=[]
 for pc,expected in ((0xc1f934,'02403fff'),(0xc1f938,'41f900c1fce8'),(0xc1f93e,'20700000'),(0xc1f942,'4e90')):
  length,opcode,handler,assembly=decoder.decode(pc);raw=''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2));assert raw==expected
  caller.append({'pc':f'{pc:06X}','instruction':assembly,'bytes':raw})
 edge_path=ROOT/'port/recomp/generated/recomp_edges.json';edges=json.loads(edge_path.read_text())
 # Search existing bounded table evidence for exact slots. This neither
 # claims the whole selector domain nor introduces a production guard.
 for slot in range(0xc1fce8,0xc1fe20,4):
  target=decoder.word(slot)<<16|decoder.word(slot+2);entry=f'{target:06X}'
  if entry not in incoming:continue
  incoming[entry].append({'kind':'original table slot','call_pc':'C1F942','return_pc':'C1F944','table':'C1FCE8','selector_offset':f'{slot-0xc1fce8:04X}','slot':f'{slot:06X}','bytes':f'{target:08x}','target':entry,'caller_instructions':caller,'observed_edges':[edge for edge in edges if edge==['C1F944',entry]]})
 assert all(incoming.values()),[e for e,v in incoming.items() if not v]
 result['actual_original_callability']=incoming
 result['indirect_edge_authority']={'path':str(edge_path.relative_to(ROOT)).replace('\\','/'),'sha256':hashlib.sha256(edge_path.read_bytes()).hexdigest()}
 result['classification']={'purpose':'Next complete plot, span, polygon, lane and glyph helper upgrades','implementation':False,'os_service_replacement':False,'whole_dispatch_table_extent_claimed':False,'selector_guard_added':False}
 return result

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--write',action='store_true');args=p.parse_args();result=inventory()
 if args.write:MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
 elif json.loads(MANIFEST.read_text())!=result:raise ValueError('render-helper source or original callability changed')
 print(f"Render leaf helpers: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; all original callers sealed")

if __name__=='__main__':main()
