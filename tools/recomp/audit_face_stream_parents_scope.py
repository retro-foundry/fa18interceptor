"""Seal the next complete segment, face, grid and block-stream parents."""
import argparse,hashlib,json,re
from audit_command_dispatch import ROOT,audit,source_decoder
from recomp import classify,static_target

ENTRIES=('C2129C','C212B0','C211DC','C2131C','C20E4E','C20E40','C21490','C2139E','C21412','C20F10','C20EC4','C20D68','C20904','C21A20','C217EA','C2159E','C210E6')
MANIFEST=ROOT/'analysis/data/face_stream_parents_scope_inventory.json'

def inventory():
 result=audit(ENTRIES,additional_cold_entries=ENTRIES)
 _,decoder=source_decoder();incoming={e:[] for e in ENTRIES};candidates=set()
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
 result['classification']={'purpose':'Next complete seventeen face/segment/grid/block stream-parent upgrades','implementation':False,'os_service_replacement':False,'whole_dispatch_table_extent_claimed':False,'selector_guard_added':False}
 return result

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--write',action='store_true');args=p.parse_args();result=inventory()
 if args.write:MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
 elif json.loads(MANIFEST.read_text())!=result:raise ValueError('face-stream source or original callability changed')
 print(f"Face/stream parents: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; all original callers sealed")

if __name__=='__main__':main()
