"""Seal remaining older render parents and original direct/indirect callers."""
import argparse, hashlib, json
from audit_command_dispatch import ROOT, audit, source_decoder
from audit_projection_readout_scope import inventory as direct_inventory

ENTRIES=('C2D16C','C21500','C2122A','C20592','C2168A','C203D0','C201A6','C3019C','C1FB82','C1F99A')
MANIFEST=ROOT/'analysis/data/legacy_render_parents_scope_inventory.json'

def inventory():
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    incoming=direct_inventory(('C2D16C','C3019C','C1FB82','C1F99A'))['actual_original_callability']
    _,decoder=source_decoder()
    caller=[]
    for pc,expected in ((0xc1f934,'02403fff'),(0xc1f938,'41f900c1fce8'),(0xc1f93e,'20700000'),(0xc1f942,'4e90')):
        length,opcode,handler,assembly=decoder.decode(pc)
        raw=''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2))
        assert raw==expected
        caller.append({'pc':f'{pc:06X}','instruction':assembly,'bytes':raw})
    edge_path=ROOT/'port/recomp/generated/recomp_edges.json'
    edges=json.loads(edge_path.read_text())
    for entry,offset in (('C21500',0x28),('C2122A',0x30),('C20592',0xa0),('C2168A',0x20),('C203D0',0x9c),('C201A6',0x10)):
        slot=0xc1fce8+offset
        target=decoder.word(slot)<<16|decoder.word(slot+2)
        assert target==int(entry,16)
        observed=[edge for edge in edges if edge==['C1F944',entry]]
        assert observed,entry+' lacks original observed indirect edge'
        incoming[entry]=[{'kind':'original table and observed indirect return edge','call_pc':'C1F942','return_pc':'C1F944','table':'C1FCE8','selector_offset':f'{offset:04X}','slot':f'{slot:06X}','bytes':f'{target:08x}','target':entry,'caller_instructions':caller,'observed_edges':observed}]
    result['actual_original_callability']=incoming
    result['indirect_edge_authority']={'path':str(edge_path.relative_to(ROOT)).replace('\\','/'),'sha256':hashlib.sha256(edge_path.read_bytes()).hexdigest()}
    result['classification']={'purpose':'Next complete older render-parent upgrades','implementation':False,'os_service_replacement':False,'whole_dispatch_table_extent_claimed':False,'selector_guard_added':False}
    return result

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write',action='store_true')
    args=parser.parse_args();result=inventory()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('older render source or original callers changed')
    print(f"Older render parents: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; original callers sealed")

if __name__=='__main__': main()
