"""Seal eight complete HUD readout, cue and status parents."""
import argparse,json,re
from audit_hud_text_helper_scope import ROOT,ENTRIES,inventory as source_inventory
MANIFEST=ROOT/'analysis/data/hud_text_helpers_source_scope.json'
def inventory():
    result=source_inventory(); result.pop('classification')
    result['new_registered_owners']=[]; result['upgraded_registered_owners']=list(ENTRIES)
    expected={(int(c['target'],16),int(c['return_pc'],16)) for o in result['owners'].values() for c in o['child_call_sites']}
    for path in ('port/game/glue/glue_hud_text_helpers.c','tools/recomp/hud_text_helpers_contract_children.c'):
        pairs={(int(e,16),int(r,16)) for e,r in re.findall(r'\{0x([a-f0-9]{6}),0x([a-f0-9]{6})\}',(ROOT/path).read_text())}
        assert pairs==expected,path+' child sites differ from source'
    owned={r['pc'] for r in result['instructions']}
    for name in ('contract_oracle','oracle','dispatch_oracle'):
        text=(ROOT/'tools/recomp'/f'hud_text_helpers_{name}.c').read_text()
        pcs=re.search(r'source_boundaries\[\]=\{([^}]+)\}',text).group(1)
        assert set(re.findall(r'0x([A-F0-9]{6})u',pcs))==owned,name+' ownership differs'
    return result
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); a=p.parse_args(); result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('HUD text helper source changed')
    print(f"HUD text helpers: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared source boundaries; original incoming calls and child sites sealed")
if __name__=='__main__': main()
