"""Seal the remaining numeric, cue and status HUD parents from original bytes."""
import argparse,json
from audit_projection_readout_scope import ROOT,inventory as source_inventory
ENTRIES=('C31F4C','C3201A','C3212A','C32178','C321D2','C32260','C31EB6','C31C60','C31D16','C31E6C','C31D64','C33F54','C328A8')
MANIFEST=ROOT/'analysis/data/hud_readout_parent_scope_inventory.json'
def inventory():
    result=source_inventory(ENTRIES)
    result['classification']={'purpose':'Next thirteen complete HUD numeric, cue and status parent upgrades','implementation':False,'os_service_replacement':False}
    return result
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); args=p.parse_args(); result=inventory()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('HUD readout parent source changed')
    print(f"HUD readout parents: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; original incoming calls sealed")
if __name__=='__main__': main()
