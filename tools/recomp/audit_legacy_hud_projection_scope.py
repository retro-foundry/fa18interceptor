"""Seal older projection, postflight HUD and conditional text parent upgrades."""
import argparse,json
from audit_projection_readout_scope import ROOT,inventory as source_inventory
ENTRIES=('C0DAEE','C0CFFA','C0D04C','C33CD2','C33B38','C33370','C332BC','C32662')
MANIFEST=ROOT/'analysis/data/legacy_hud_projection_scope_inventory.json'
def inventory():
    result=source_inventory(ENTRIES)
    result['classification']={'purpose':'Next eight complete projection and postflight HUD parent upgrades','implementation':False,'os_service_replacement':False}
    return result
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--write',action='store_true');a=p.parse_args();r=inventory()
    if a.write:MANIFEST.write_text(json.dumps(r,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=r:raise ValueError('legacy HUD projection source changed')
    print(f"Legacy HUD projection: {r['unique_instruction_count']} unique / {r['shared_instruction_count']} shared boundaries; original incoming calls sealed")
if __name__=='__main__':main()
