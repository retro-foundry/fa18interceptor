"""Seal complete delayed-menu/outcome owners and their immediate continuations."""
import argparse
import json
from audit_command_dispatch import ROOT,audit
from audit_menu_outcome import inventory
ENTRIES=('C104C2','C105F4','C1072E','C1078A','C105A6','C10626','C1075A','C108DA','C29368')
MANIFEST=ROOT/'analysis/data/menu_outcome_source_scope.json'
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    tables=inventory()['dynamic_jump_tables']
    result=audit(ENTRIES,dynamic_targets={0xc10824:[int(r['branch_pc'],16) for r in tables['C10824']['records']]},
                 additional_cold_entries=ENTRIES)
    result['dynamic_jump_tables']=tables
    result['timing_reuse']={'C29368':'glue_C29042_step'}
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('menu outcome source changed')
    print(f"menu outcome source: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; four sealed table arms")
if __name__=='__main__': main()
