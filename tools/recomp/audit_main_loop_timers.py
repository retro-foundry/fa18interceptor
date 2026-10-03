"""Seal complete game-side timer/readout and bounds-setup owners."""
import argparse,json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C2527C','C25312','C2548A')
MANIFEST=ROOT/'analysis/data/main_loop_timers_source_scope.json'
def inventory(): return audit(ENTRIES,additional_cold_entries=ENTRIES)
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=inventory()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('main-loop timer source changed')
    print(f"main-loop timers: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
