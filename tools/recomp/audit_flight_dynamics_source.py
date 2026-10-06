"""Audit four complete flight-dynamics parents and the upgraded shared stream."""
import argparse,json
from audit_command_dispatch import ROOT,audit,source_decoder
from recomp import classify,static_target
ENTRIES=('C25B66','C266AE','C28996','C28B16','C28B34')
MANIFEST=ROOT/'analysis/data/flight_dynamics_source_scope.json'
def inventory():
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    previous=json.loads((ROOT/'analysis/data/flight_dynamics_remaining_scope_inventory.json').read_text())
    assert all(result['owners'][e]['source_pcs']==previous['owners'][e]['source_pcs'] for e in ENTRIES[:4])
    result['actual_original_callability']={e:previous['actual_original_callability'][e] for e in ENTRIES[:4]}
    _,decoder=source_decoder(); incoming=[]
    # Complete initializer ownership is original call-site authority.
    import re
    candidates=set()
    for p in (ROOT/'port/recomp/generated').glob('recomp_*.c'):
        candidates.update(int(pc,16) for pc in re.findall(r'/\* ([0-9A-F]{6}): (?:jsr|bsr)\s',p.read_text()))
    for pc in sorted(candidates):
        decoded=decoder.decode(pc)
        if not decoded: continue
        length,opcode,handler,assembly=decoded; kind=classify(handler)
        if kind in ('jsr','bsr') and static_target(decoder,pc,opcode,handler,kind)==0xc28b34:
            incoming.append({'pc':f'{pc:06X}','return_pc':f'{pc+length:06X}','instruction':assembly,'bytes':''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2))})
    assert incoming
    result['actual_original_callability']['C28B34']=incoming
    result['upgraded_registered_owners']=['C28B34']
    fixture=(ROOT/'tools/recomp/flight_dynamics_contract_oracle.c').read_text()
    if fixture:
        sealed_pcs={r['pc'] for r in result['instructions']}
        fixture_pcs=set(re.findall(r'0x([0-9A-F]{6})u',fixture.split('source_boundaries[]={',1)[1].split('};',1)[0]))
        assert fixture_pcs==sealed_pcs,'complete-call fixture ownership differs from original source'
    expected={(int(c['target'],16),int(c['return_pc'],16)) for owner in result['owners'].values() for c in owner['child_call_sites']}
    for path in ('port/game/glue/glue_flight_dynamics.c','tools/recomp/flight_dynamics_contract_children.c'):
        contents=(ROOT/path).read_text()
        if path.endswith('glue_flight_dynamics.c'):
            contents=contents.split('static const DynamicsCallSite sites[]={',1)[1].split('};',1)[0]
        actual={(int(e,16),int(r,16)) for e,r in re.findall(r'\{0x([a-f0-9]{6}),0x([a-f0-9]{6})\}',contents)}
        assert actual==expected,f'{path}: child sites differ from original source'
    return result
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); a=p.parse_args(); result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('flight-dynamics source changed')
    print(f"flight dynamics: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; all original callable owners sealed")
if __name__=='__main__': main()
