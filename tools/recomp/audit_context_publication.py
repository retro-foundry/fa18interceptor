"""Seal complete context/selected-record publishers, keeping child calls explicit."""
import argparse
import json
from audit_command_dispatch import ROOT,audit

ENTRIES=("C1B7A6","C1BEE8","C1C214","C083A6","C09DD0")
MANIFEST=ROOT/"analysis/data/context_publication_source_scope.json"

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write",action="store_true")
    args=parser.parse_args()
    result=audit(ENTRIES)
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+"\n")
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError("context publication source audit changed")
    print(f"context publication audit: {result['unique_instruction_count']} unique / "
          f"{result['shared_instruction_count']} shared boundaries; no missing static paths")

if __name__=="__main__": main()
