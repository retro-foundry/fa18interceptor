"""Seal the complete corner, view-record and rejection-return owner scopes."""
import argparse
import json
import re
from audit_command_dispatch import ROOT, audit, source_decoder
from recomp import classify, static_target

ENTRIES=('C2E758','C2CE82','C2CCA0','C2CD28','C2CD94','C2D082','C2D3A4',
         'C200F6','C203CC','C2058E','C20826','C22C70')
MANIFEST=ROOT/'analysis/data/corner_view_source_scope.json'

def inventory():
    result=audit(ENTRIES,additional_cold_entries=ENTRIES)
    _,decoder=source_decoder()
    incoming={entry:[] for entry in ENTRIES}
    candidates=set()
    for path in (ROOT/'port/recomp/generated').glob('recomp_*.c'):
        candidates.update(int(pc,16) for pc in re.findall(r'/\* ([0-9A-F]{6}): (?:jsr|bsr|bra|jmp|b\w+)\s',path.read_text()))
    for pc in sorted(candidates):
        length,opcode,handler,assembly=decoder.decode(pc)
        kind=classify(handler)
        if kind not in ('jsr','bsr','jmp','bra','bcc'):continue
        target=static_target(decoder,pc,opcode,handler,kind)
        entry=f'{target:06X}' if target is not None else ''
        if entry not in incoming:continue
        incoming[entry].append({'pc':f'{pc:06X}','kind':kind,
            'return_pc':f'{pc+length:06X}' if kind in ('jsr','bsr') else None,
            'instruction':assembly,'bytes':''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2))})
    result['original_static_incoming']=incoming
    result['upgraded_registered_owners']=['C2E758','C2CE82']
    result['new_registered_owners']=list(ENTRIES[2:])
    result['non_returning_loop']={'pc':'C2EA02','instruction':decoder.decode(0xc2ea02)[3],
                                  'bytes':f'{decoder.word(0xc2ea02):04x}'}
    assert result['non_returning_loop']['bytes']=='6ffe'
    result['record_pair_template']={'start':'C2CE5E','end':'C2CE82',
        'bytes':''.join(f'{decoder.word(pc):04x}' for pc in range(0xc2ce5e,0xc2ce82,2))}
    child_sites={(int(c['target'],16),int(c['return_pc'],16)) for owner in result['owners'].values() for c in owner['child_call_sites']}
    for path in ('port/game/glue/glue_corner_view.c','tools/recomp/corner_view_contract_children.c'):
        contents=(ROOT/path).read_text()
        pairs={(int(e,16),int(r,16)) for e,r in re.findall(r'\{0x([a-f0-9]{6}),0x([a-f0-9]{6})\}',contents)}
        assert pairs==child_sites,path+' child contracts differ'
    owned={row['pc'] for row in result['instructions']}
    for name in ('contract_oracle','oracle','dispatch_oracle','loop_oracle'):
        contents=(ROOT/'tools/recomp'/f'corner_view_{name}.c').read_text()
        pcs=re.search(r'source_boundaries\[\]=\{([^}]+)\}',contents).group(1)
        assert set(re.findall(r'0x([A-F0-9]{6})u',pcs))==owned,name+' ownership differs'
    fixtures=(ROOT/'tools/recomp/corner_view_fixture.h').read_text()
    for entry in ENTRIES:
        case=re.search(r'case 0x'+entry.lower()+r'u:REG_PPC=0x([a-f0-9]{6})u;',fixtures)
        assert case and any(row['pc']==case[1].upper() for row in incoming[entry]),entry+' fixture caller differs'
    result['sealed_native_child_contracts']=len(child_sites)
    result['sealed_oracle_ownership_arrays']=4
    return result

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--write',action='store_true');args=p.parse_args();result=inventory()
    if args.write:MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result:raise ValueError('Corner/view source changed')
    print(f"Corner/view: {len(ENTRIES)} owners, {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared source PCs")
    for entry,owner in result['owners'].items():
        print(entry,owner['instruction_count'],'PCs;',len(owner['child_call_sites']),'child sites;',len(result['original_static_incoming'][entry]),'static incoming')
if __name__=='__main__':main()
