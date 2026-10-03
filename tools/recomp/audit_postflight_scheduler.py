"""Seal complete scheduler flow; retain shared tails and real child boundaries."""
import argparse
import json
import audit_command_dispatch as source_audit

ENTRIES=("C09E06","C09E98","C09EC4","C0A002","C0A12E","C0A15C",
         "C0A1E0","C0A2F0","C0A334","C0A364","C0A3EA")
MANIFEST=source_audit.ROOT/"analysis/data/postflight_scheduler_source_scope.json"

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write",action="store_true")
    args=parser.parse_args()
    result=source_audit.audit(ENTRIES)
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+"\n")
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError("scheduler source audit changed")
    print(f"scheduler source audit: {result['unique_instruction_count']} unique / "
          f"{result['shared_instruction_count']} shared boundaries, no missing static paths")

if __name__=="__main__": main()
