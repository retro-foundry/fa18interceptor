"""Seal next complete postflight/restart callbacks and root transform; inventory only."""
import argparse
import json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C11788','C11830','C11872','C118A0','C118E6','C118FC','C11934',
         'C11958','C119D4','C1104C','C0F946','C0F974','C091E6')
MANIFEST=ROOT/'analysis/data/postflight_completion_scope_inventory.json'
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    result['classification']='next complete postflight/restart scope; not an implementation'
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('postflight completion inventory changed')
    print(f"next postflight completion scope: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
