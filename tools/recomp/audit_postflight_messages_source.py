"""Seal complete postflight message owners and their original branch table."""
import argparse,json
from audit_postflight_messages import ROOT,ENTRIES,inventory
from audit_command_dispatch import source_decoder,static_target
MANIFEST=ROOT/'analysis/data/postflight_messages_source_scope.json'
def scope():
    result=inventory(); del result['classification']
    _,decoder=source_decoder(); pc=0xc15c36
    length,opcode,handler,assembly=decoder.decode(pc)
    if length!=6 or handler!='m68k_op_jsr_32_al' or static_target(decoder,pc,opcode,handler,'jsr')!=0xc0f4d8:
        raise ValueError('original external C0F4D8 call changed')
    result['external_callability']={'C0F4D8':{'pc':f'{pc:06X}','instruction':assembly,
        'bytes':''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2)),
        'target':'C0F4D8','return_pc':f'{pc+length:06X}'}}
    return result
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=scope()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('postflight message source changed')
    print(f"postflight messages source: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
