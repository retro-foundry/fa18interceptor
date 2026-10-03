"""Seal the next complete menu/context completion owners; inventory only."""
import argparse
import json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C10A24','C10C08','C10C68','C10AB2','C10AE6','C10B1E',
         'C10CFE','C10D8A','C10DAE','C11A26','C11A50','C09192','C16D04','C25070')
MANIFEST=ROOT/'analysis/data/menu_context_finish_scope_inventory.json'

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    result['classification']='next complete menu/context completion scope; not an implementation'
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('menu context completion inventory changed')
    print(f"next menu context completion scope: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
