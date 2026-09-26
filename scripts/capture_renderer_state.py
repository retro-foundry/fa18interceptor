"""Capture renderer pointer tables at a deterministic Engine9000 breakpoint."""
from pathlib import Path
import argparse, json
from engine9000_bridge import Engine, ROOT
from profile_window import read_events

def main():
    p = argparse.ArgumentParser()
    p.add_argument('--restore', type=Path, required=True); p.add_argument('--playback', type=Path, required=True)
    p.add_argument('--frame', type=int, required=True); p.add_argument('--breakpoint', type=lambda x:int(x,0), default=0xC304F4)
    p.add_argument('--output', type=Path, required=True); a=p.parse_args()
    e=Engine((ROOT/'local/fa18.uae').resolve(), a.output.parent/'saves')
    try:
        e.core.retro_run(); state=a.restore.read_bytes()
        if not e.core.retro_unserialize(state,len(state)): raise RuntimeError('restore failed')
        events=read_events(a.playback); e.core.e9k_debug_add_breakpoint(a.breakpoint)
        for f in range(1,a.frame+1):
            for k,v in events.get(f,[]): e.event(k,v)
            e.core.retro_run()
        if not e.core.e9k_debug_is_paused(): raise RuntimeError('breakpoint not reached')
        def words(addr,count):
            raw=e.memory(addr,count*2); return [int.from_bytes(raw[i:i+2],'big') for i in range(0,len(raw),2)]
        def longs(addr,count):
            raw=e.memory(addr,count*4); return [int.from_bytes(raw[i:i+4],'big') for i in range(0,len(raw),4)]
        report={'frame':f,'pc':f'${e.regs()["pc"]:06X}',
                'renderer_pointer_tables':{
                    '$C4566E': [f'${x:06X}' for x in longs(0xC4566E,4)],
                    '$C4567E': [f'${x:06X}' for x in longs(0xC4567E,4)],
                    '$C456B6': [f'${x:06X}' for x in longs(0xC456B6,4)]},
                'renderer_words':{'$C45960':f'${longs(0xC45960,1)[0]:06X}', '$C4596E':f'${words(0xC4596E,1)[0]:04X}',
                                  '$C456E7':f'${words(0xC456E7,1)[0]:04X}', '$C456E8':f'${words(0xC456E8,1)[0]:04X}'}}
        a.output.parent.mkdir(parents=True,exist_ok=True); a.output.write_text(json.dumps(report,indent=2)+'\n')
        print(json.dumps(report))
    finally: e.core.retro_unload_game(); e.core.retro_deinit()
if __name__=='__main__': main()
