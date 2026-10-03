"""Seal complete menu follow-up and source file-loading owner scopes."""
import argparse
import json
from audit_command_dispatch import ROOT,audit
from audit_menu_followup import ENTRIES
MANIFEST=ROOT/'analysis/data/menu_followup_source_scope.json'
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('menu follow-up source scope changed')
    print(f"menu follow-up source: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
