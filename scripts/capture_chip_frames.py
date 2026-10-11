"""Capture Chip RAM at selected ordinary replay frames."""
from pathlib import Path
import argparse, json
from engine9000_bridge import Engine, ROOT, PROJECT
from profile_window import read_events

def main():
    p=argparse.ArgumentParser(); p.add_argument('--restore',type=Path,required=True); p.add_argument('--playback',type=Path,required=True); p.add_argument('--frames',type=int,nargs='+',required=True); p.add_argument('--output',type=Path,required=True); a=p.parse_args()
    chip = next((region for region in PROJECT.memory_regions if region[0] == 'chip'), None)
    if chip is None: raise ValueError('Selected project has no chip memory region')
    targets=set(a.frames); e=Engine((PROJECT.path('config')).resolve(),a.output.parent/'saves')
    try:
        e.core.retro_run(); s=a.restore.read_bytes()
        if not e.core.retro_unserialize(s,len(s)): raise RuntimeError('restore failed')
        events=read_events(a.playback); a.output.mkdir(parents=True,exist_ok=True)
        for f in range(1,max(targets)+1):
            for k,v in events.get(f,[]): e.event(k,v)
            e.core.retro_run()
            if f in targets: (a.output/f'frame{f}.chip').write_bytes(e.memory(chip[1],chip[2]))
        (a.output/'report.json').write_text(json.dumps({'frames':sorted(targets)})+'\n')
    finally: e.core.retro_unload_game(); e.core.retro_deinit()
if __name__=='__main__': main()
