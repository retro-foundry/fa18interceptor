"""Seal the next complete point-projection, numeric-readout and sweep owners."""
import argparse,json,re
from audit_command_dispatch import ROOT,audit,source_decoder
from recomp import classify,static_target
ENTRIES=('C2ECA8','C32A44','C32AC8','C33F70','C33F8A','C33FB4')
MANIFEST=ROOT/'analysis/data/projection_readout_scope_inventory.json'

def inventory():
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    _,decoder=source_decoder(); incoming={e:[] for e in ENTRIES}; candidates=set()
    for path in (ROOT/'port/recomp/generated').glob('recomp_*.c'):
        candidates.update(int(pc,16) for pc in re.findall(r'/\* ([0-9A-F]{6}): (?:jsr|bsr)\s',path.read_text()))
    for pc in sorted(candidates):
        decoded=decoder.decode(pc)
        if not decoded: continue
        length,opcode,handler,assembly=decoded; kind=classify(handler)
        if kind not in ('jsr','bsr'): continue
        target=static_target(decoder,pc,opcode,handler,kind)
        if target is None or f'{target:06X}' not in incoming: continue
        incoming[f'{target:06X}'].append({'pc':f'{pc:06X}','return_pc':f'{pc+length:06X}',
            'instruction':assembly,'bytes':''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2))})
    assert all(incoming.values())
    result['actual_original_callability']=incoming
    result['classification']={'purpose':'Next six complete projection, numeric readout and bounded sweep owners',
        'implementation':False,'os_service_replacement':False}
    return result

def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); a=p.parse_args(); result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('projection/readout source changed')
    print(f"projection/readout: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; all six owners have sealed original calls")
if __name__=='__main__': main()
