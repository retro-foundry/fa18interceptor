"""Seal complete renderer entries and original table/call contracts."""
import argparse,json,re
from audit_render_entry_helpers_scope import ROOT,inventory as scope_inventory
from audit_command_dispatch import source_decoder
MANIFEST=ROOT/'analysis/data/render_entry_helpers_source_scope.json'
ENTRIES=tuple(json.loads((ROOT/'analysis/data/render_entry_helpers_scope_inventory.json').read_text())['owners'])

def inventory():
 result=scope_inventory();classification=result.pop('classification')
 # Registration is a planning snapshot; preserve the committed source
 # inventory's original before/after classification when ports are installed.
 planning=json.loads((ROOT/'analysis/data/render_entry_helpers_scope_inventory.json').read_text())['classification']
 result['new_registered_owners']=planning['new_seeded_entries']+planning['new_source_only_entries']
 result['upgraded_registered_owners']=planning['upgraded_registered_entries']
 _,decoder=source_decoder();rows=[]
 for pc in (0xc2f5e0,0xc2f600,0xc2f632,0xc2f65e,0xc2f682):
  length,opcode,handler,assembly=decoder.decode(pc)
  assert opcode==0x49f9
  target=decoder.word(pc+2)<<16|decoder.word(pc+4)
  assert target in (0xc2f786,0xc2f7e6)
  rows.append({'pc':f'{pc:06X}','instruction':assembly,'bytes':''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2)),'writer_table':f'{target:06X}','rows':2 if target==0xc2f7e6 else 1})
 result['shared_pixel_writer_table_invariant']={'original_incoming_setups':rows,'tables':['C2F786','C2F7E6'],'runtime_selector_guard_added':False,'scope':'All original incoming shared-pixel setup paths select one of these two immutable writer tables.'}
 writers=re.findall(r'LANE_HANDLER\((C[0-9A-F]{5}),\s*(\d+),\s*(\d+)\)',(ROOT/'port/game/glue/glue_planar_lane_masks.c').read_text())
 expected={row['entry']:(row['colour'],row['rows']-1) for row in result['writer_semantics']}
 assert len(writers)==len(expected)==32
 assert {entry:(int(colour),int(rows)) for entry,colour,rows in writers}==expected,'Writer wrappers differ from original tables'
 child_sites={(int(c['target'],16),int(c['return_pc'],16)) for owner in result['owners'].values() for c in owner['child_call_sites']}
 contracts=(ROOT/'tools/recomp/render_entry_helpers_contract_children.c').read_text()
 assert {(int(e,16),int(r,16)) for e,r in re.findall(r'\{0x([a-f0-9]{6}),0x([a-f0-9]{6})\}',contracts)}==child_sites
 leaf_glue=(ROOT/'port/game/glue/glue_render_leaf_helpers.c').read_text()
 production={(int(e,16),int(r,16)) for e,r in re.findall(r'\{0x([a-f0-9]{6}),0x([a-f0-9]{6})\}',leaf_glue)}
 entry_glue=(ROOT/'port/game/glue/glue_render_entry_helpers.c').read_text()
 production|={(int(e,16),int(r,16)) for e,r in re.findall(r'glue_complete_child\(0x([a-f0-9]{6})u,0x([a-f0-9]{6})u\)',entry_glue)}
 assert child_sites<=production,'Production child contracts differ from source'
 owned={row['pc'] for row in result['instructions']}
 for name in ('contract_oracle','oracle','dispatch_oracle'):
  text=(ROOT/'tools/recomp'/f'render_entry_helpers_{name}.c').read_text()
  pcs=re.search(r'source_boundaries\[\]=\{([^}]+)\}',text).group(1)
  assert set(re.findall(r'0x([A-F0-9]{6})u',pcs))==owned,name+' ownership differs'
 return result

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--write',action='store_true');a=p.parse_args();result=inventory()
 if a.write:MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
 elif json.loads(MANIFEST.read_text())!=result:raise ValueError('Renderer entry source changed')
 print(f"Renderer entries: {len(result['owners'])} owners, {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries; all incoming table setups sealed")
if __name__=='__main__':main()
