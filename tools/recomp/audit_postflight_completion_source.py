"""Seal complete postflight/reset/restart C owners against original bytes."""
import argparse,json
from audit_postflight_completion import ROOT,ENTRIES,audit
MANIFEST=ROOT/'analysis/data/postflight_completion_source_scope.json'
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('postflight completion source changed')
    print(f"postflight completion source: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
