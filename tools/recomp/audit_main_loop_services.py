"""Seal complete main-loop timer, control and message-service owners."""
import argparse,json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C2527C','C25312','C2548A','C1518C','C32CEE')
MANIFEST=ROOT/'analysis/data/main_loop_services_scope_inventory.json'
def inventory():
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    result['classification']='next complete main-loop game timer/control/message owners; not an implementation or OS-service replacement'
    return result
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=inventory()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('main-loop service inventory changed')
    print(f"next main-loop services: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
