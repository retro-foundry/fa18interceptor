"""Sample live wide M-map depth branch choices from a sealed state."""
from __future__ import annotations
import argparse, json, tempfile
from pathlib import Path
from engine9000_bridge import Engine, ROOT

ENTRY, LOW, MID, HIGH = 0xC2AC1A, 0xC2AC3E, 0xC2AC2E, 0xC2AC36

def main() -> None:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--state',type=Path,required=True); p.add_argument('--output',type=Path,required=True)
    p.add_argument('--max-instructions',type=int,default=180000); p.add_argument('--config',type=Path,default=ROOT/'local/fa18.uae'); p.add_argument('--replace',action='store_true')
    a=p.parse_args(); md=a.output.with_suffix('.md')
    if not a.replace and (a.output.exists() or md.exists()): raise FileExistsError(a.output)
    state=a.state.read_bytes(); samples=[]
    with tempfile.TemporaryDirectory(prefix='fa18-wide-depth-') as temp:
        e=Engine(a.config.resolve(),Path(temp)); e.core.retro_run()
        if not e.core.retro_unserialize(state,len(state)): raise RuntimeError('core rejected state')
        try:
            for i in range(a.max_instructions):
                r=e.regs(); pc=r['pc']
                if pc in (ENTRY,LOW,MID,HIGH):
                    metric=int.from_bytes(e.memory(r['a6']-0x28,4),'big',signed=True)
                    samples.append({'instruction':i,'pc':f'${pc:06X}','metric':metric,'a0':f"${r['a0']&0xffffff:06X}"})
                e.core.e9k_debug_step_instr(); e.core.retro_run()
        finally: e.core.retro_unload_game(); e.core.retro_deinit()
    names={f'${ENTRY:06X}':'threshold_test',f'${LOW:06X}':'low_filter',f'${MID:06X}':'mid_base_C2A072',f'${HIGH:06X}':'high_base_C2A0C2'}
    report={'scope':'live C2AC1A wide-pass depth branch samples from sealed no-input state','state':str(a.state),'samples':samples,'branch_names':names,'qualification':'Samples prove only the reached branch and live metric in this state; they do not establish physical-world distance or a mesh LOD.'}
    a.output.parent.mkdir(parents=True,exist_ok=True); a.output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    lines=['# Live wide M-map depth branches','',report['qualification'],'','| instruction | branch | metric | A0 before branch |','| ---: | --- | ---: | --- |']
    for x in samples: lines.append(f"| {x['instruction']} | {x['pc']} ({names[x['pc']]}) | {x['metric']} | `{x['a0']}` |")
    md.write_text('\n'.join(lines)+'\n',encoding='utf8'); print(json.dumps({'samples':len(samples)}))
if __name__=='__main__': main()
