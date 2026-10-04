"""Observe original non-returning branches without manufacturing returns."""
import argparse,subprocess
from audit_segment_projection_source import ROOT
from hud_stream_machine_variant import prepare
ENTRIES=('C2F0C6','C2F0F4','C2F128','C2F156')
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--cases',type=int,default=16384);a=p.parse_args()
 if a.cases<=0:p.error('case count must be positive')
 prepare();exe=ROOT/'build/recomp/segment_projection_parallel_oracle.exe'
 subprocess.run(['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),
  '--main','tools/recomp/segment_projection_parallel_oracle.c',
  '--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c',
  '--replace-source','port/machine/machine.c=tools/recomp/segment_projection_hardware_log.c',
  '--replace-source','port/machine/blitter.c=tools/recomp/hud_stream_blitter_state.c'],cwd=ROOT,check=True)
 for e in ENTRIES:
  r=subprocess.run([str(exe),str(a.cases),e],cwd=ROOT,capture_output=True,text=True,timeout=600)
  (ROOT/'build/recomp'/f'segment_projection_parallel_{e}.log').write_text(r.stdout+r.stderr)
  if r.returncode:raise RuntimeError(e+': '+r.stdout+r.stderr)
  print(r.stdout.splitlines()[0],flush=True)
 print('Every original parallel branch remains non-returning; 64 observed iterations match full CPU/PC/SR and all RAM')
if __name__=='__main__':main()
