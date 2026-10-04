"""Seal complete selected-segment, projection and eight crossing owners."""
import argparse,hashlib,json,re
from audit_command_dispatch import ROOT,audit,source_decoder
ENTRIES=('C1FF9C','C1FFA4','C2ED70','C2EE4A','C2F0C6','C2F0F4','C2F128','C2F156','C2EA5A','C2EAD0','C2EB4C','C2EBC2')
MANIFEST=ROOT/'analysis/data/segment_projection_source_scope.json'

def inventory():
    result=None
    rows={};owners={}
    for entry in ENTRIES:
        targets=[0xc2ed70 if entry=='C1FF9C' else 0xc2ee4a]
        part=audit((entry,),additional_cold_entries=ENTRIES,
                   dynamic_calls={0xc1fff6:targets} if entry in ENTRIES[:2] else {})
        if result is None:result={k:v for k,v in part.items() if k not in ('instructions','owners','unique_instruction_count','shared_instruction_count','owned_pc_and_source_bytes_sha256')}
        owners.update(part['owners'])
        for row in part['instructions']:
            if row['pc'] in rows:assert rows[row['pc']]==row
            rows[row['pc']]=row
    instructions=[rows[pc] for pc in sorted(rows)]
    sets=[set(owner['source_pcs']) for owner in owners.values()]
    packed=b''.join(int(row['pc'],16).to_bytes(4,'big')+bytes.fromhex(row['bytes']) for row in instructions)
    result.update(owners=owners,instructions=instructions,unique_instruction_count=len(rows),
                  shared_instruction_count=sum(sum(pc in pcs for pcs in sets)>1 for pc in rows),
                  owned_pc_and_source_bytes_sha256=hashlib.sha256(packed).hexdigest())
    _,decoder=source_decoder()
    authorities=[]
    for entry,pc,target in (('C1FF9C',0xc1ff9c,0xc2ed70),('C1FFA4',0xc1ffb4,0xc2ee4a)):
        length,opcode,handler,assembly=decoder.decode(pc)
        assert opcode==0x49f9 and decoder.word(pc+2)<<16|decoder.word(pc+4)==target
        length2,opcode2,_,assembly2=decoder.decode(0xc1fff6)
        assert length2==2 and opcode2==0x4e94
        authorities.append({'owner':entry,'target_setup_pc':f'{pc:06X}','setup_instruction':assembly,
            'setup_bytes':''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2)),
            'call_pc':'C1FFF6','call_instruction':assembly2,'call_bytes':f'{opcode2:04x}',
            'target':f'{target:06X}','return_pc':'C1FFF8','selector_guard_added':False})
    result['dynamic_child_authority']=authorities
    incoming={entry:[] for entry in ENTRIES};candidates=set()
    from recomp import classify,static_target
    for path in (ROOT/'port/recomp/generated').glob('recomp_*.c'):
        candidates.update(int(pc,16) for pc in re.findall(r'/\* ([0-9A-F]{6}): (?:jsr|bsr)\s',path.read_text()))
    for pc in sorted(candidates):
        length,opcode,handler,assembly=decoder.decode(pc)
        kind=classify(handler)
        if kind not in ('jsr','bsr'):continue
        target=static_target(decoder,pc,opcode,handler,kind)
        if target is None or f'{target:06X}' not in incoming:continue
        incoming[f'{target:06X}'].append({'pc':f'{pc:06X}','return_pc':f'{pc+length:06X}',
            'instruction':assembly,'bytes':''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2))})
    for row in authorities:incoming[row['target']].append({'kind':'original selected-segment indirect child',**row})
    callers=[]
    for pcs in ((0xc1f934,0xc1f938,0xc1f93e,0xc1f942),(0xc0983c,0xc09840,0xc09846,0xc0984a)):
        instructions=[]
        for pc in pcs:
            length,opcode,handler,assembly=decoder.decode(pc)
            instructions.append({'pc':f'{pc:06X}','instruction':assembly,
                'bytes':''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2))})
        assert [row['bytes'] for row in instructions]==['02403fff','41f900c1fce8','20700000','4e90']
        callers.append(instructions)
    edge_path=ROOT/'port/recomp/generated/recomp_edges.json';edges=json.loads(edge_path.read_text())
    # Exact slots suffice for these incoming contracts; no complete selector
    # extent is inferred from this existing bounded table evidence.
    for slot in range(0xc1fce8,0xc1fe20,4):
        target=decoder.word(slot)<<16|decoder.word(slot+2);entry=f'{target:06X}'
        if entry not in ENTRIES[:2]:continue
        for caller in callers:
            call=int(caller[-1]['pc'],16);observed=[edge for edge in edges if edge==[f'{call+2:06X}',entry]]
            if not observed:continue
            incoming[entry].append({'kind':'original table slot and observed indirect edge',
                'call_pc':f'{call:06X}','return_pc':f'{call+2:06X}','table':'C1FCE8',
                'slot':f'{slot:06X}','selector_offset':f'{slot-0xc1fce8:04X}',
                'target':entry,'bytes':f'{target:08x}','caller_instructions':caller,'observed_edges':observed})
    result['indirect_edge_authority']={'path':str(edge_path.relative_to(ROOT)).replace('\\','/'),
        'sha256':hashlib.sha256(edge_path.read_bytes()).hexdigest(),'whole_table_extent_claimed':False}
    assert all(incoming.values()),[entry for entry,rows in incoming.items() if not rows]
    result['actual_original_callability']=incoming
    result['new_registered_owners']=[];result['upgraded_registered_owners']=list(ENTRIES)
    result['unrounded_parallel_source_loops']=['C2F0DC','C2F10E','C2F13E','C2F170']
    for pc in result['unrounded_parallel_source_loops']:
        row=rows[pc]
        assert row['bytes']=='67fe' and row['instruction']==f'beq     ${pc.lower()}'
    child_sites={(int(c['target'],16),int(c['return_pc'],16)) for owner in owners.values() for c in owner['child_call_sites']}
    for path in ('port/game/glue/glue_segment_projection.c','tools/recomp/segment_projection_contract_children.c'):
        contents=(ROOT/path).read_text()
        pairs={(int(e,16),int(r,16)) for e,r in re.findall(r'\{0x([a-f0-9]{6}),0x([a-f0-9]{6})\}',contents)}
        assert pairs==child_sites,path+' child contracts differ'
    owned=set(rows)
    for name in ('contract_oracle','oracle','dispatch_oracle'):
        contents=(ROOT/'tools/recomp'/f'segment_projection_{name}.c').read_text()
        pcs=re.search(r'source_boundaries\[\]=\{([^}]+)\}',contents).group(1)
        assert set(re.findall(r'0x([A-F0-9]{6})u',pcs))==owned,name+' ownership differs'
    fixtures=(ROOT/'tools/recomp/segment_projection_fixture.h').read_text()
    for entry in ENTRIES:
        case=re.search(r'case 0x'+entry.lower()+r'u:REG_PPC=0x([a-f0-9]{6})u;',fixtures)
        if entry=='C2ED70':case=re.search(r'case 0xc2ed70u:case 0xc2ee4au:REG_PPC=0x([a-f0-9]{6})u;',fixtures)
        assert case,entry+' missing actual caller fixture'
        call=case[1].upper()
        assert any(row.get('pc',row.get('call_pc'))==call for row in incoming[entry]),entry+' fixture caller differs'
    result['sealed_native_child_contracts']=len(child_sites)
    result['sealed_oracle_ownership_arrays']=3
    return result

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--write',action='store_true');a=p.parse_args();result=inventory()
    if a.write:MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result:raise ValueError('Segment projection source changed')
    print(f"Segment projection: {len(ENTRIES)} owners, {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared source PCs; both actual indirect children sealed")
if __name__=='__main__':main()
