"""Audit complete main-loop control/message source flow and byte authority."""
import argparse,json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C1518C','C32CEE')
MANIFEST=ROOT/'analysis/data/main_loop_control_messages_source_scope.json'
def inventory(): return audit(ENTRIES,additional_cold_entries=ENTRIES)
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=inventory()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('main-loop control/message source changed')
    print(f"main-loop control/messages: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
