"""Seal complete menu callbacks, their six original table arms and helpers."""
import argparse
import json
from audit_command_dispatch import ROOT, audit, source_decoder

ENTRIES=("C0FCB4","C0FECE","C0FFE2","C1000A","C17C2A","C24E8A")
MANIFEST=ROOT/"analysis/data/menu_transition_source_scope.json"

def inspect_source():
    _,decoder=source_decoder()
    table=[]
    for offset in range(0,48,8):
        pc=0xc0ffde+offset
        mode=(decoder.word(pc)<<16)|decoder.word(pc+2)
        raw=b"".join(decoder.word(pc+i).to_bytes(2,"big") for i in range(0,8,2))
        length,opcode,_,assembly=decoder.decode(pc+4)
        if length!=4 or opcode!=0x6000: raise ValueError("menu table arm is not BRA.W")
        table.append({"pc":f"{pc:06X}","mode":mode,"bytes":raw.hex(),
                      "arm":f"{pc+4:06X}","target":f"{pc+6+(int.from_bytes(raw[6:8],'big',signed=True)):06X}"})
    if [r['mode'] for r in table]!=[9,125,3,2,1,127]: raise ValueError("menu mode table changed")
    result=audit(ENTRIES,{0xc0ffda:[int(r['arm'],16) for r in table]},("C0FECE",))
    result['scope']='complete owner flow, including all six sealed indexed-jump arms; calls are child boundaries'
    result['indexed_jump_table']=table
    return result

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=inspect_source()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('menu-transition source audit changed')
    print(f"menu source audit: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; all six table arms")

if __name__=='__main__': main()
