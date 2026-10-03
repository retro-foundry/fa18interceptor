"""Seal complete projection/readout owners and shared projection upgrades."""
import argparse,json,re
from audit_projection_readout_scope import ROOT,inventory as scope_inventory
NEW=('C2ECA8','C32A44','C32AC8','C33F70','C33F8A','C33FB4')
UPGRADED=('C2EC90','C2EC94','C2EC9C','C2ECA4')
ENTRIES=UPGRADED+NEW
MANIFEST=ROOT/'analysis/data/projection_readouts_source_scope.json'
def inventory():
    result=scope_inventory(ENTRIES); result.pop('classification')
    result['new_registered_owners']=list(NEW)
    result['upgraded_registered_owners']=list(UPGRADED)
    expected={(int(c['target'],16),int(c['return_pc'],16)) for o in result['owners'].values() for c in o['child_call_sites']}
    for path in ('port/game/glue/glue_projection_readouts.c','tools/recomp/projection_readouts_contract_children.c'):
        pairs={(int(e,16),int(r,16)) for e,r in re.findall(r'\{0x([a-f0-9]{6}),0x([a-f0-9]{6})\}',(ROOT/path).read_text())}
        assert pairs==expected,path+' child sites differ from source'
    owned={r['pc'] for r in result['instructions']}
    for name in ('contract_oracle','oracle','dispatch_oracle'):
        text=(ROOT/'tools/recomp'/f'projection_readouts_{name}.c').read_text()
        pcs=re.search(r'source_boundaries\[\]=\{([^}]+)\}',text).group(1)
        assert set(re.findall(r'0x([A-F0-9]{6})u',pcs))==owned,name+' ownership differs'
    return result
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); a=p.parse_args(); result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('projection/readout source changed')
    print(f"projection/readouts: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared PCs; all original incoming calls sealed")
if __name__=='__main__': main()
