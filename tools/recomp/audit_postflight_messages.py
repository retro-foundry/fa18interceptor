"""Seal next complete postflight-message and text callback owners; inventory only."""
import argparse,json
from audit_command_dispatch import ROOT,audit,source_decoder,static_target
ENTRIES=('C0F4D8','C0F812','C11078','C110A4','C11350','C113E4','C1141E',
         'C11446','C11478','C114D2','C1159E','C115BA','C1169A','C116B0','C116CE','C11738')
MANIFEST=ROOT/'analysis/data/postflight_messages_scope_inventory.json'
def inventory():
    _,decoder=source_decoder(); arms=[]
    for address,target in [(0xc111ec,0xc111f4),(0xc111ee,0xc1121c),(0xc111f0,0xc11232),(0xc111f2,0xc11240)]:
        length,opcode,handler,assembly=decoder.decode(address)
        if length!=2 or handler!='m68k_op_bra_8' or static_target(decoder,address,opcode,handler,'bra')!=target:
            raise ValueError(f'postflight table changed at {address:06X}')
        arms.append({'branch_pc':f'{address:06X}','target':f'{target:06X}','bytes':f'{opcode:04x}'})
    result=audit(ENTRIES,dynamic_targets={0xc111e8:[int(r['branch_pc'],16) for r in arms]},additional_cold_entries=ENTRIES)
    result['classification']='next complete postflight-message/text callback scope; not an implementation'
    result['dynamic_jump_tables']={'C111E8':{'table':'C111EC','index_register':'D0.l','record_bytes':2,
        'source_guard':'signed extended mode minus 4 must be >=0 and <4; ASL.L #1 forms the original branch-word index',
        'records':arms}}
    return result
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=inventory()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('postflight message inventory changed')
    print(f"next postflight message scope: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; four original table arms")
if __name__=='__main__': main()
