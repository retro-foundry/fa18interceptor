"""Seal complete game input/display setup and synchronization owners."""
import argparse,json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C16D4C','C16FF4','C17066','C1787A','C1612C')
MANIFEST=ROOT/'analysis/data/input_display_setup_scope_inventory.json'

def inventory():
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    result['classification']='next complete game input/display setup and synchronization owners; not an implementation or OS-service replacement'
    return result

def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=inventory()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('input/display setup inventory changed')
    print(f"next input/display setup: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
