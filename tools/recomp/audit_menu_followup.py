"""Seal the next menu follow-up owners and required source file-loading helper."""
import argparse
import json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C1029E','C10418','C10458','C10678','C1643A')
MANIFEST=ROOT/'analysis/data/menu_followup_scope_inventory.json'
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    result['classification']='next menu-followup/source-file owner inventory; not an implementation'
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('menu follow-up inventory changed')
    print(f"next menu follow-up scope: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
