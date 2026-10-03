"""Seal complete menu setup, sound selector and input/message helpers."""
import argparse
import json
from audit_command_dispatch import ROOT,audit

ENTRIES=("C0FBE0","C17B96","C1082C","C11BB0","C24FA4")
MANIFEST=ROOT/"analysis/data/menu_setup_source_scope.json"

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write",action="store_true")
    args=parser.parse_args()
    result=audit(ENTRIES)
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+"\n")
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError("menu setup source audit changed")
    print(f"menu setup audit: {result['unique_instruction_count']} unique / "
          f"{result['shared_instruction_count']} shared boundaries; no missing static paths")

if __name__=="__main__": main()
