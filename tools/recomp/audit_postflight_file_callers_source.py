"""Seal complete formatter and game-side file caller owners against original bytes."""
import argparse,json
from audit_postflight_file_callers import ROOT,ENTRIES,inventory
MANIFEST=ROOT/'analysis/data/postflight_file_callers_source_scope.json'
def scope():
    result=inventory(); del result['classification']
    result['timing_reuse']={'C0F56A':'glue_C24E2C_step'}
    return result
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=scope()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('postflight file caller source changed')
    print(f"postflight file callers: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; formatter timing reused")
if __name__=='__main__': main()
