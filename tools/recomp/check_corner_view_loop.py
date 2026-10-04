"""Observe original non-returning branches without manufacturing returns."""
import argparse,subprocess
from audit_corner_view_source import ROOT
from hud_stream_machine_variant import prepare
ENTRIES=('C2E758',)
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--cases',type=int,default=16384);a=p.parse_args()
 if a.cases<=0:p.error('case count must be positive')
 prepare();exe=ROOT/'build/recomp/corner_view_loop_oracle.exe'
 subprocess.run(['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),
  '--main','tools/recomp/corner_view_loop_oracle.c',
  '--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c',
  '--replace-source','port/machine/machine.c=tools/recomp/corner_view_hardware_log.c',
  '--replace-source','port/machine/blitter.c=tools/recomp/hud_stream_blitter_state.c',
  '--replace-source','port/game/glue/glue_child_call.c=tools/recomp/corner_view_contract_children.c'],cwd=ROOT,check=True)
 for e in ENTRIES:
  r=subprocess.run([str(exe),str(a.cases),e],cwd=ROOT,capture_output=True,text=True,timeout=600)
  (ROOT/'build/recomp'/f'corner_view_loop_{e}.log').write_text(r.stdout+r.stderr)
  if r.returncode:raise RuntimeError(e+': '+r.stdout+r.stderr)
  print(r.stdout.splitlines()[0],flush=True)
 print('Original corner depth branch remains non-returning; 64 observed iterations match full CPU/PC/SR and all RAM')
if __name__=='__main__':main()
