"""Seal original delayed-menu and outcome owners; this inventory implements none."""
import argparse
import json
from audit_command_dispatch import ROOT,audit,source_decoder

ENTRIES=('C104C2','C105F4','C1072E','C1078A')
MANIFEST=ROOT/'analysis/data/menu_outcome_scope_inventory.json'

def inventory():
    _,decoder=source_decoder()
    table=[]
    for address,key,expected in [(0xc10828,9,0xc1086e),(0xc10830,0x7d,0xc10862),
                                 (0xc10838,2,0xc10856),(0xc10840,1,0xc10848)]:
        actual=(decoder.word(address)<<16)|decoder.word(address+2)
        decoded=decoder.decode(address+4)
        target=address+6+int.from_bytes(decoder.word(address+6).to_bytes(2,'big'),'big',signed=True)
        if actual!=key or decoded[2]!='m68k_op_bra_16' or target!=expected:
            raise ValueError(f'outcome source table changed at {address:06X}')
        table.append({'key':actual,'record':f'{address:06X}','branch_pc':f'{address+4:06X}',
                      'target':f'{target:06X}',
                      'bytes':''.join(f'{decoder.word(address+i):04x}' for i in range(0,8,2))})
    result=audit(ENTRIES,dynamic_targets={0xc10824:[int(row['branch_pc'],16) for row in table]},
                 additional_cold_entries=ENTRIES)
    result['classification']='next delayed-menu/outcome source inventory; not an implementation'
    result['dynamic_jump_tables']={'C10824':{
        'table':'C10828','index_register':'D1.l','record_bytes':8,
        'source_guard':'D1 starts at 32, subtracts 8 before each search, exits on negative; matches D0 to record key',
        'records':table}}
    return result

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=inventory()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('menu outcome inventory changed')
    print(f"next menu outcome scope: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; four sealed table arms")

if __name__=='__main__': main()
