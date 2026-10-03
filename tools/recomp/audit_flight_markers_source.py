"""Audit five complete scene/grid/marker owners against original bytes."""
import argparse,json,re
from audit_flight_action_setup_scope import ROOT,ENTRIES,inventory as remaining_inventory
MANIFEST=ROOT/'analysis/data/flight_markers_source_scope.json'
def inventory():
    result=remaining_inventory(); result.pop('classification'); result.pop('related_unsealed_owner')
    result['new_registered_owners']=list(ENTRIES)
    expected={(int(c['target'],16),int(c['return_pc'],16)) for o in result['owners'].values() for c in o['child_call_sites']}
    t=(ROOT/'port/game/glue/glue_flight_markers.c').read_text()
    actual={(int(e,16),int(r,16)) for e,r in re.findall(r'\{0x([a-f0-9]{6}),0x([a-f0-9]{6})\}',t)}
    assert actual==expected,'flight-markers child sites differ from original source'
    contract=(ROOT/'tools/recomp/flight_markers_contract_children.c').read_text()
    controlled={(int(e,16),int(r,16)) for e,r in re.findall(r'\{0x([a-f0-9]{6}),0x([a-f0-9]{6})\}',contract)}
    assert controlled==expected,'controlled child contracts differ from original sites'
    owned={r['pc'] for r in result['instructions']}
    for name in ('contract_oracle','oracle','dispatch_oracle'):
        body=(ROOT/'tools/recomp'/f'flight_markers_{name}.c').read_text()
        boundaries=re.search(r'source_boundaries\[\]=\{([^}]+)\}',body).group(1)
        assert set(re.findall(r'0x([A-F0-9]{6})u',boundaries))==owned,name+' source ownership differs'
    return result
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); a=p.parse_args(); result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('flight markers source changed')
    print(f"flight markers: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared PCs; all original incoming and child calls sealed")
if __name__=='__main__': main()
