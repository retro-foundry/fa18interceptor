"""Seal the next complete HUD stream consumers from original owner flow."""
import argparse
import json
from audit_projection_readout_scope import ROOT, inventory as source_inventory

ENTRIES=('C30764','C309B6','C30B5C','C30D34','C30F78','C3112A','C31A64','C31ACC')
MANIFEST=ROOT/'analysis/data/hud_parent_scope_inventory.json'

def inventory():
    result=source_inventory(ENTRIES)
    result['classification']={
        'purpose':'Next eight complete HUD stream consumers and display parents',
        'implementation':False, 'os_service_replacement':False}
    return result

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write',action='store_true')
    args=parser.parse_args()
    result=inventory()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('HUD parent source changed')
    print(f"HUD parents: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; eight original incoming-call scopes sealed")

if __name__=='__main__': main()
