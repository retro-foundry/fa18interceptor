"""Seal twelve complete rendering parents and all original child contracts."""
import argparse,json,re
from audit_command_dispatch import source_decoder
from audit_render_leaf_helpers_scope import ROOT,ENTRIES,inventory as source_inventory
MANIFEST=ROOT/'analysis/data/render_leaf_helpers_source_scope.json'
def inventory():
    result=source_inventory(); result.pop('classification')
    result['new_registered_owners']=[]; result['upgraded_registered_owners']=list(ENTRIES)
    _,decoder=source_decoder()
    groups=((0xc2fdf0,0xc2fdf4,0xc2fdf8,0xc2fdfe),
            (0xc2fe3a,0xc2fe3e,0xc2fe42,0xc2fe48,0xc2fe4c,0xc2fe52,0xc2fe54),
            (0xc2fe90,0xc2fe94,0xc2fe98,0xc2fe9e),
            (0xc2fe02,0xc2fe08,0xc2fe0a,0xc2fe10))
    timing=[]
    for pcs in groups:
        rows=[]
        for pc in pcs:
            length,opcode,handler,assembly=decoder.decode(pc)
            rows.append({'pc':f'{pc:06X}','instruction':assembly,'base_cycles':decoder.lib.fa18_handler_cycles(opcode)})
        timing.append(rows)
    assert [sum(r['base_cycles'] for r in rows) for rows in timing]==[52,86,52,56]
    # Original 68000 byte BEQ not-taken adjustment is -2. The conditional
    # MOVE costs 8: middle interval 78 (taken) /84 (not-taken), poll loop 54.
    result['counted_poll_timing_authority']={'instructions':timing,'prefix_cycles':[52,{'zero':78,'nonzero':84},52],'busy_loop_cycles':54,'ready_poll_cycles':26,'native_logic_owns_each_counter_increment':True}
    expected={(int(c['target'],16),int(c['return_pc'],16)) for o in result['owners'].values() for c in o['child_call_sites']}
    for path in ('port/game/glue/glue_render_leaf_helpers.c','tools/recomp/render_leaf_helpers_contract_children.c'):
        pairs={(int(e,16),int(r,16)) for e,r in re.findall(r'\{0x([a-f0-9]{6}),0x([a-f0-9]{6})\}',(ROOT/path).read_text())}
        assert pairs==expected,path+' child sites differ from source'
    owned={r['pc'] for r in result['instructions']}
    for name in ('contract_oracle','oracle','dispatch_oracle'):
        text=(ROOT/'tools/recomp'/f'render_leaf_helpers_{name}.c').read_text()
        pcs=re.search(r'source_boundaries\[\]=\{([^}]+)\}',text).group(1)
        assert set(re.findall(r'0x([A-F0-9]{6})u',pcs))==owned,name+' ownership differs'
    return result
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); a=p.parse_args(); result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('render leaf helper source changed')
    print(f"render leaf helpers: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared source boundaries; original incoming calls and child sites sealed")
if __name__=='__main__': main()
