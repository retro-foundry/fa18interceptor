"""Seal cold menu callbacks and helpers, distinguishing source-only owners."""
import argparse
import json
from audit_command_dispatch import ROOT,audit
from port_info import instructions

ENTRIES=("C0FE36","C1017E","C10272","C103E4","C09120","C29490","C2949A","C09148","C10B90","C16406","C1643A")
MANIFEST=ROOT/"analysis/data/menu_cold_scope_inventory.json"

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write",action="store_true")
    args=parser.parse_args()
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    result["classification"]="original menu callback/helper inventory; not an implementation"
    for entry,owner in result["owners"].items():
        owner["translated_instruction_count"]=len(instructions(entry))
        owner["source_only"]=owner["translated_instruction_count"]==0
    result["source_only_owner_count"]=sum(owner["source_only"] for owner in result["owners"].values())
    result["translated_helper_count"]=len(ENTRIES)-result["source_only_owner_count"]
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+"\n")
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError("cold menu source audit changed")
    print(f"cold menu audit: {result['unique_instruction_count']} unique / "
          f"{result['shared_instruction_count']} shared boundaries; "
          f"{result['source_only_owner_count']} source-only owners / "
          f"{result['translated_helper_count']} translated helpers; no missing static paths")

if __name__=="__main__": main()
