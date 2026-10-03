"""Seal complete menu/context completion owners against original bytes."""
import argparse
import json
from audit_command_dispatch import ROOT,audit
from audit_menu_context_finish import ENTRIES
MANIFEST=ROOT/'analysis/data/menu_context_finish_source_scope.json'
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('menu context completion source changed')
    print(f"menu context completion source: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
