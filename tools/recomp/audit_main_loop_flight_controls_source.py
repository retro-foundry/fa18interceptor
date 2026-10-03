"""Audit all complete original control/flight parents and helper upgrades."""
import argparse,json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C149BE','C083E2','C25754','C23A7E','C24568','C2436A')
MANIFEST=ROOT/'analysis/data/main_loop_flight_controls_source_scope.json'
def inventory(): return audit(ENTRIES,additional_cold_entries=ENTRIES)
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); a=p.parse_args(); result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('control/flight source changed')
    print(f"control/flight source: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
