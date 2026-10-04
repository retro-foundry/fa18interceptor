"""Seal complete history/display parents and three original indirect stream entries."""
import argparse,hashlib,json
from audit_command_dispatch import ROOT,audit,source_decoder
from audit_projection_readout_scope import inventory as direct_inventory
ENTRIES=('C0D04C','C33370','C1FE24','C1FE46','C0CF98')
MANIFEST=ROOT/'analysis/data/hud_history_stream_scope_inventory.json'
def inventory():
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    _,decoder=source_decoder()
    incoming=direct_inventory(ENTRIES[:2])['actual_original_callability']
    caller=[]
    for pc in (0xc1f934,0xc1f938,0xc1f93e,0xc1f942):
        length,opcode,handler,assembly=decoder.decode(pc)
        caller.append({'pc':f'{pc:06X}','instruction':assembly,'bytes':''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2))})
    assert caller[0]['bytes']=='02403fff'
    assert caller[1]['bytes']=='41f900c1fce8'
    assert caller[2]['bytes']=='20700000'
    assert caller[3]['bytes']=='4e90'
    edges_path=ROOT/'port/recomp/generated/recomp_edges.json';edges=json.loads(edges_path.read_text())
    for entry,offset in zip(ENTRIES[2:],(4,0xe8,0xf8)):
        slot=0xc1fce8+offset;target=decoder.word(slot)<<16|decoder.word(slot+2)
        assert target==int(entry,16)
        observed=[list(e) for e in edges if e==['C1F944',entry]]
        assert observed,entry+' lacks original observed indirect edge'
        incoming[entry]=[{'kind':'original table and observed indirect return edge','call_pc':'C1F942','return_pc':'C1F944','table':'C1FCE8','selector_offset':f'{offset:04X}','slot':f'{slot:06X}','bytes':f'{target:08x}','target':entry,'caller_instructions':caller,'observed_edges':observed}]
    result['actual_original_callability']=incoming
    result['indirect_edge_authority']={'path':str(edges_path.relative_to(ROOT)).replace('\\','/'),'sha256':hashlib.sha256(edges_path.read_bytes()).hexdigest()}
    result['classification']={'purpose':'Next five complete history/display and indirect stream upgrades','implementation':False,'os_service_replacement':False,'whole_dispatch_table_extent_claimed':False,'selector_guard_added':False}
    return result
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--write',action='store_true');a=p.parse_args();r=inventory()
    if a.write:MANIFEST.write_text(json.dumps(r,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=r:raise ValueError('history/stream original source or indirect callability changed')
    print(f"History/stream: {r['unique_instruction_count']} unique / {r['shared_instruction_count']} shared boundaries; two direct owners and three exact original indirect slots sealed")
if __name__=='__main__':main()
