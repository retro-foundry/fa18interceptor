"""Seal the next nine complete callable flight-dynamics owners; no implementation."""
import argparse,json,re
from audit_command_dispatch import ROOT,audit,source_decoder
from recomp import classify,static_target
ENTRIES=('C25B66','C26322','C26352','C266AE','C26C72','C26CC0','C26D8A','C28996','C28B16')
MANIFEST=ROOT/'analysis/data/flight_dynamics_scope_inventory.json'

def inventory():
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    _,decoder=source_decoder(); candidates=set()
    for p in (ROOT/'port/recomp/generated').glob('recomp_*.c'):
        candidates.update(int(pc,16) for pc in re.findall(r'/\* ([0-9A-F]{6}): (?:jsr|bsr)\s',p.read_text()))
    for p in (ROOT/'analysis/data').glob('*_source_scope.json'):
        candidates.update(int(r['pc'],16) for r in json.loads(p.read_text()).get('instructions',[]) if r['instruction'].split()[0] in ('jsr','bsr'))
    incoming={e:[] for e in ENTRIES}
    for pc in sorted(candidates):
        decoded=decoder.decode(pc)
        if not decoded: continue
        length,opcode,handler,assembly=decoded; kind=classify(handler)
        if kind not in ('jsr','bsr'): continue
        target=static_target(decoder,pc,opcode,handler,kind)
        if target is not None and f'{target:06X}' in incoming:
            incoming[f'{target:06X}'].append({'pc':f'{pc:06X}','return_pc':f'{pc+length:06X}','instruction':assembly,'bytes':''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2))})
    if not all(incoming.values()): raise ValueError('missing actual original incoming call')
    result['actual_original_callability']=incoming
    result['classification']={'purpose':'Next complete flight dynamics, controls and record motion owners','implementation':False,'os_service_replacement':False,'timing_policy':'retain original children and source instruction boundaries'}
    result['related_unsealed_owner']={'entry':'C2C392','actual_call_pc':'C25C6A','unresolved_dynamic_transfer_pc':'C2C46E','requirement':'Reconcile original computed transfer before assigning complete ownership; no inferred targets or implementation.'}
    return result

def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); a=p.parse_args(); result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('flight-dynamics inventory changed')
    print(f"next flight dynamics: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; all nine have original JSR/BSR evidence")
if __name__=='__main__': main()
