"""Capture repeated line-packet preparation entries from one replay frame."""
from pathlib import Path
import argparse, json
from engine9000_bridge import Engine, ROOT
from profile_window import read_events

def main():
    p=argparse.ArgumentParser(); p.add_argument('--restore',type=Path,required=True); p.add_argument('--playback',type=Path,required=True); p.add_argument('--frame',type=int,required=True); p.add_argument('--address',type=lambda x:int(x,0),default=0xC30F5A); p.add_argument('--count',type=int,default=4); p.add_argument('--output',type=Path,required=True); a=p.parse_args()
    e=Engine((ROOT/'local/fa18.uae').resolve(),a.output.parent/'saves')
    try:
        e.core.retro_run(); state=a.restore.read_bytes()
        if not e.core.retro_unserialize(state,len(state)): raise RuntimeError('restore failed')
        events=read_events(a.playback); hits=[]; armed=False
        for f in range(1,a.frame+1):
            if f==a.frame: e.core.e9k_debug_add_breakpoint(a.address); armed=True
            for k,v in events.get(f,[]): e.event(k,v)
            e.core.retro_run()
            while armed and e.core.e9k_debug_is_paused() and len(hits)<a.count:
                regs=e.regs()
                a.output.parent.mkdir(parents=True, exist_ok=True)
                (a.output.parent / f'{a.output.stem}_hit{len(hits)}.chip').write_bytes(
                    e.memory(0, 0x80000))
                hits.append({'hit':len(hits),'frame':f,'registers':regs,
                             'record_words':e.memory(0xC4B390,0x100).hex(),
                             'a1_bytes':e.memory(regs['a1'],16).hex(),
                             'a2_bytes':e.memory(regs['a2'],16).hex(),
                             'a3_bytes':e.memory(regs['a3'],16).hex(),
                             'custom_bytes':e.memory(0xDFF040,0x2A).hex(),
                             'a_source_rows':e.memory(regs['d0'],12 * 37).hex(),
                             'b_source_rows':e.memory(regs['d3'],12 * 37).hex()})
                e.core.e9k_debug_step_instr(); e.core.e9k_debug_resume(); e.core.retro_run()
            if len(hits)>=a.count: break
        if len(hits)!=a.count: raise RuntimeError(f'only captured {len(hits)} of {a.count} hits')
        a.output.parent.mkdir(parents=True,exist_ok=True); a.output.write_text(json.dumps({'frame':a.frame,'address':f'${a.address:06X}','hits':hits},indent=2)+'\n')
        print(json.dumps({'hits':len(hits),'output':str(a.output)}))
    finally: e.core.retro_unload_game(); e.core.retro_deinit()
if __name__=='__main__': main()
