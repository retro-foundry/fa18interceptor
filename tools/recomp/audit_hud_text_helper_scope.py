"""Seal the remaining cache and callable small/8-pixel HUD text helpers."""
import argparse,json
from audit_projection_readout_scope import ROOT,inventory as source_inventory
ENTRIES=('C31C20','C3271A','C32726','C32736','C32794','C32AA4','C32AA6','C32AB4')
MANIFEST=ROOT/'analysis/data/hud_text_helper_scope_inventory.json'
def inventory():
    result=source_inventory(ENTRIES)
    result['classification']={'purpose':'Next eight complete original HUD cache and text helper upgrades','implementation':False,'os_service_replacement':False}
    return result
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--write',action='store_true');a=p.parse_args();result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('HUD text helper source changed')
    print(f"HUD text helpers: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; original incoming calls sealed")
if __name__=='__main__': main()
