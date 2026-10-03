"""Seal complete control/flight parents and existing helper upgrades; no implementation."""
import argparse,json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C149BE','C083E2','C25754','C23A7E','C24568','C2436A')
MANIFEST=ROOT/'analysis/data/main_loop_flight_controls_scope_inventory.json'
def inventory():
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    result['classification']='next complete control/flight parents and helper CPU/timing upgrades; no implementation or OS-service replacement'
    return result
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); a=p.parse_args(); result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('control/flight source inventory changed')
    print(f"next control/flight owners: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
