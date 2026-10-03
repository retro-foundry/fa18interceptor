"""Inventory the original text formatter and game-side mode-file callers.

This is Stage D game code. The actual OS children remain separate services.
"""
import argparse,json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C0F56A','C0EF08','C162E4','C1631C','C16386')
MANIFEST=ROOT/'analysis/data/postflight_file_callers_scope_inventory.json'
def inventory():
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    result['classification']='next complete game-side text formatter and mode-file caller owners; no implementation or OS-service proof'
    return result
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=inventory()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('postflight file caller inventory changed')
    print(f"next postflight file callers: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; actual OS children retained")
if __name__=='__main__': main()
