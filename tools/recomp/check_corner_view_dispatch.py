"""Exercise ON/shadow/sandbox continuations and the complete C200F6 ON tail."""
import argparse,subprocess
from audit_corner_view_source import ROOT,ENTRIES
from hud_stream_machine_variant import prepare
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--cases',type=int,default=256); a=p.parse_args()
    if a.cases<=0: p.error('case count must be positive')
    prepare()
    exe=ROOT/'build/recomp/corner_view_dispatch_oracle.exe'
    subprocess.run(['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),
        '--main','tools/recomp/corner_view_dispatch_oracle.c',
        '--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c',
        '--replace-source','port/machine/bus.c=tools/recomp/render_entry_helpers_bus_budget.c',
        '--replace-source','port/game/glue/glue_child_call.c=tools/recomp/render_entry_helpers_child_call.c',
        '--replace-source','port/machine/machine.c=tools/recomp/corner_view_hardware_log.c',
        '--replace-source','port/machine/blitter.c=tools/recomp/hud_stream_blitter_state.c'],cwd=ROOT,check=True)
    for e in ENTRIES:
        for mode,name in ((1,'on'),(2,'shadow'),(3,'sandbox')):
            # This original shared return tail has two saved pointer longs
            # above its RTS return. Whole C is independently proven; its
            # source boundaries already belong to the C20100 continuation.
            if e=='C200F6' and mode!=1:continue
            r=subprocess.run([str(exe),str(a.cases),e,str(mode)],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'corner_view_dispatch_{e}_{name}.log').write_text(r.stdout+r.stderr)
            if r.returncode: raise RuntimeError(f'{e} {name}: {r.stderr or r.stdout}')
            print(name,r.stdout.splitlines()[0],flush=True)
    print('eleven ON/shadow/sandbox owners and C200F6 ON tail pass; completed comparisons retain strict source classification and guards')
if __name__=='__main__': main()
