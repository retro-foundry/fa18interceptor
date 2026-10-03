"""Compare completed owners, internal segments and fault observations separately.

All comparisons retain complete CPU/PC/SR and Chip/Slow RAM. No boundary is
removed from ownership. The nonreturning fault loop is never a completed call.
"""
import argparse,json,subprocess
from audit_flight_record_actions_source import ROOT,ENTRIES,MANIFEST

def observed(output): return set(output.split('visited:')[1].split())

def verify_coverage(source,controlled,segments,fault):
    rows={r['pc']:r['instruction'] for r in source['instructions']}
    # BNE and BEQ consume the same TST flags, with no intervening operation.
    assert rows['C23348']=='tst.b   $c457ae.l'
    assert rows['C2334E']=='bne     $c233a8' and rows['C23350']=='beq     $c233d6'
    recording={pc for pc in rows if 'C23354'<=pc<'C233A8'}
    assert len(recording)==22
    class31={pc for pc in rows if 'C238A8'<=pc<'C2390A'}
    assert len(class31)==27 and rows['C23736']=='move.b  #$31, ($62,A1)'
    # These valid, disjoint-record fixtures assign class 0/1 before the tail.
    # Their class-30 arm is covered in C236AA and the direct internal segment.
    class30={pc for pc in rows if 'C2390A'<=pc<'C2391C' or 'C239E2'<=pc<'C23A24'}
    assert len(class30)==24
    assert rows['C2381C']=='move.b  #$1, ($62,A1)' and rows['C2383C']=='move.b  #$0, ($62,A1)'
    fault_pcs={'C257DC','C257E4','C257EA'}
    expected={'C23228':recording,'C23716':class31,'C2377E':class30,'C257EC':fault_pcs}
    for e in ENTRIES:
        missing=set(source['owners'][e]['source_pcs'])-controlled[e]
        if missing!=expected.get(e,set()): raise RuntimeError(f'{e}: unexpected completed-fixture coverage gaps {sorted(missing)}')
    assert segments['C23354']==recording|{'C233A8'}
    tail={pc for pc in rows if 'C2385A'<=pc<='C23A24'}
    assert segments['C2385A']==tail
    assert fault=={'C257EC','C257F0','C257F6','C257F8','C257DC','C257E4'}
    completed=set().union(*controlled.values())
    # The single back edge never completes; its CPU/bus/cycle behavior has
    # separate instruction proof. Preserve it in the production owner set.
    assert set(rows)-(completed|set().union(*segments.values())|fault)=={'C257EA'}
    print(f'completed owner union {len(completed)}/575; two internal segments and first-fault observations account for all except the separately tested nonreturning back edge')

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--cases',type=int,default=16384); p.add_argument('--contract-cases',type=int,default=16384)
    p.add_argument('--segment-cases',type=int,default=1024); p.add_argument('--fault-cases',type=int,default=1024)
    a=p.parse_args()
    if min(a.cases,a.contract_cases,a.segment_cases,a.fault_cases)<=0: p.error('case counts must be positive')
    subprocess.run(['python','tools/recomp/audit_flight_record_actions_source.py'],cwd=ROOT,check=True)
    source=json.loads(MANIFEST.read_text()); controlled={}; segments={}
    for kind,cases in [('contract',a.contract_cases),('real',a.cases)]:
        name='flight_record_actions_'+('contract_' if kind=='contract' else '')+'oracle'
        exe=ROOT/'build/recomp'/(name+'.exe')
        cmd=['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),'--main','tools/recomp/'+name+'.c','--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c']
        if kind=='contract':
            cmd+=['--replace-source','port/game/glue/glue_child_call.c=tools/recomp/flight_record_actions_contract_children.c',
                  '--replace-source','port/game/glue/glue_flight_record_actions.c=tools/recomp/flight_record_actions_segment_adapter.c']
        else: cmd+=['--replace-source','port/machine/bus.c=tools/recomp/flight_record_actions_bus_budget.c']
        subprocess.run(cmd,cwd=ROOT,check=True)
        jobs=[(e,cases,[]) for e in ENTRIES]
        if kind=='contract': jobs += [('C23354',a.segment_cases,[]),('C2385A',a.segment_cases,[]),('C257EC',a.fault_cases,['fault'])]
        for e,count,extra in jobs:
            r=subprocess.run([str(exe),str(count),e,*extra],cwd=ROOT,capture_output=True,text=True,timeout=600)
            label=e+('_fault' if extra else '')
            (ROOT/'build/recomp'/f'flight_record_actions_{kind}_{label}.log').write_text(r.stdout+r.stderr)
            if r.returncode: raise RuntimeError(f'{e} {kind}: {r.stderr or r.stdout}')
            print(r.stdout.splitlines()[0],flush=True)
            if kind=='contract':
                if extra: fault=observed(r.stdout)
                elif e in ENTRIES: controlled[e]=observed(r.stdout)
                else: segments[e]=observed(r.stdout)
        if kind=='contract' and a.contract_cases>=16384 and a.segment_cases>=1024: verify_coverage(source,controlled,segments,fault)
    print('flight-record action CPU/SR/RAM proof passes; real-child owner coverage is reported separately and remains partial')
if __name__=='__main__': main()
