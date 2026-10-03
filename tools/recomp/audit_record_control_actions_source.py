"""Audit complete original control-record actions and alert owners."""
import argparse,json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C153FC','C15688','C159AE','C15AD4','C181A0','C15138')
MANIFEST=ROOT/'analysis/data/record_control_actions_source_scope.json'
def inventory(): return audit(ENTRIES,additional_cold_entries=ENTRIES)
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); a=p.parse_args(); result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('record/control action source changed')
    print(f"record control actions: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
