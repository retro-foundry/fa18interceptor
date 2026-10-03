"""Seal the four enclosing flight-dynamics owners after their helper batch."""
import argparse,json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C25B66','C266AE','C28996','C28B16')
MANIFEST=ROOT/'analysis/data/flight_dynamics_remaining_scope_inventory.json'
def inventory():
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    previous=json.loads((ROOT/'analysis/data/flight_dynamics_scope_inventory.json').read_text())
    assert all(result['owners'][e]['source_pcs']==previous['owners'][e]['source_pcs'] for e in ENTRIES)
    result['actual_original_callability']={e:previous['actual_original_callability'][e] for e in ENTRIES}
    result['classification']={'purpose':'Next four complete enclosing flight-dynamics owners','implementation':False,'os_service_replacement':False}
    result['related_unsealed_owner']=previous['related_unsealed_owner']
    return result
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); a=p.parse_args(); result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('remaining dynamics scope changed')
    print(f"remaining flight dynamics: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; all four have original calls")
if __name__=='__main__': main()
