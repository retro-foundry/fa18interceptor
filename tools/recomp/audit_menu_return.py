"""Seal the next complete menu/context return callbacks; inventory only."""
import argparse
import json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C1064C','C108FE','C10900','C10970','C102D8','C0FB70','C0FBB6',
         'C101FC','C10228','C10942','C109AC','C10302','C10BAE','C10362')
MANIFEST=ROOT/'analysis/data/menu_return_scope_inventory.json'
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    result['classification']='next complete menu/context return scope; not an implementation'
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('menu return inventory changed')
    print(f"next menu return scope: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
