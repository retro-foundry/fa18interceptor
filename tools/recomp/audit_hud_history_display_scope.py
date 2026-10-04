"""Seal the remaining complete history-projection and postflight-display parents."""
import argparse,json
from audit_projection_readout_scope import ROOT,inventory as source_inventory
ENTRIES=('C0D04C','C33370')
MANIFEST=ROOT/'analysis/data/hud_history_display_scope_inventory.json'
def inventory():
    result=source_inventory(ENTRIES)
    result['classification']={'purpose':'Complete remaining history projection and postflight display parents','implementation':False,'os_service_replacement':False}
    return result
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--write',action='store_true');a=p.parse_args();r=inventory()
    if a.write:MANIFEST.write_text(json.dumps(r,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=r:raise ValueError('history/display source changed')
    print(f"History/display: {r['unique_instruction_count']} unique / {r['shared_instruction_count']} shared boundaries; original calls sealed")
if __name__=='__main__':main()
