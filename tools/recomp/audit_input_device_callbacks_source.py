"""Seal complete game input-device owners against original source bytes."""
import argparse,json
from audit_input_device_callbacks import ROOT,ENTRIES,inventory
MANIFEST=ROOT/'analysis/data/input_device_callbacks_source_scope.json'
def scope():
    result=inventory(); del result['classification']; return result
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--write',action='store_true'); args=parser.parse_args()
    result=scope()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('input device source changed')
    print(f"input device callbacks: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
