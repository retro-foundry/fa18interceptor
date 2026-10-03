"""Seal the next complete flight-action setup owners; no implementation claim."""
import argparse,json,re
from audit_command_dispatch import ROOT,audit,source_decoder
from recomp import classify,static_target
ENTRIES=('C2AFFA','C2B3C2','C2B564','C2B928','C2B952')
MANIFEST=ROOT/'analysis/data/flight_action_setup_scope_inventory.json'
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
    result['classification']={'purpose':'Next five complete flight-action setup owners',
        'implementation':False,'os_service_replacement':False}
    result['related_unsealed_owner']={'entry':'C2C392','pc':'C2C46E',
        'reason':'Computed JMP uses signed action-byte table indexing; target domain still requires source evidence.'}
    return result
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); a=p.parse_args(); result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('flight-action setup source changed')
    print(f"flight-action setup: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; all five owners have sealed original calls")
if __name__=='__main__': main()
