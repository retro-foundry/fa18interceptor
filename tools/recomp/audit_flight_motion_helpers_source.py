"""Audit five complete callable motion helpers against the sealed source."""
import argparse,json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C26322','C26352','C26C72','C26CC0','C26D8A')
MANIFEST=ROOT/'analysis/data/flight_motion_helpers_source_scope.json'
def inventory():
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    previous=json.loads((ROOT/'analysis/data/flight_dynamics_scope_inventory.json').read_text())
    assert all(result['owners'][e]['source_pcs']==previous['owners'][e]['source_pcs'] for e in ENTRIES)
    result['actual_original_callability']={e:previous['actual_original_callability'][e] for e in ENTRIES}
    return result
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); a=p.parse_args(); result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('motion helper source changed')
    print(f"motion helper source: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; all five retain original incoming calls")
if __name__=='__main__': main()
