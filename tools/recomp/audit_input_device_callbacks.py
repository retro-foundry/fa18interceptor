"""Seal next complete game-side input-device callbacks and setup owners."""
import argparse,json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C1718E','C17456','C1748C','C174A0','C16CD8','C16B8C','C17104','C1712C')
MANIFEST=ROOT/'analysis/data/input_device_callbacks_scope_inventory.json'
def inventory():
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    result['classification']='next complete game input callback and setup owners; not an implementation or OS-service replacement'
    return result
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=inventory()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('input device callback inventory changed')
    print(f"next input device callbacks: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
