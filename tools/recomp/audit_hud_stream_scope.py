"""Seal the next six original stream-store, numeric and bounded drawing owners."""
import argparse,json
from audit_projection_readout_scope import ROOT,inventory as source_inventory
ENTRIES=('C308D8','C308F4','C30F46','C31B76','C33AD6','C33B06')
MANIFEST=ROOT/'analysis/data/hud_stream_scope_inventory.json'

def inventory():
    result=source_inventory(ENTRIES)
    result['classification']['purpose']='Next six complete stream-store, numeric and bounded drawing owners'
    return result

def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); a=p.parse_args(); result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('HUD stream source changed')
    print(f"HUD streams: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; all six owners have sealed original calls")
if __name__=='__main__': main()
