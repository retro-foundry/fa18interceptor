"""Compare all six complete original steering owners with whole native C."""
import argparse,json,subprocess
from audit_record_steering_source import ROOT,ENTRIES,MANIFEST
from hud_stream_machine_variant import prepare
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--cases',type=int,default=16384);a=p.parse_args()
 if a.cases<=0:p.error('case count must be positive')
 prepare();subprocess.run(['python','tools/recomp/audit_record_steering_source.py'],cwd=ROOT,check=True)
 exe=ROOT/'build/recomp/record_steering_oracle.exe'
 subprocess.run(['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),
  '--main','tools/recomp/record_steering_oracle.c',
  '--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c',
  '--replace-source','port/machine/bus.c=tools/recomp/render_entry_helpers_bus_budget.c',
  '--replace-source','port/machine/machine.c=tools/recomp/record_steering_hardware_log.c',
  '--replace-source','port/machine/blitter.c=tools/recomp/hud_stream_blitter_state.c'],cwd=ROOT,check=True)
 source=json.loads(MANIFEST.read_text());covered=set();owned={r['pc'] for r in source['instructions']}
 for e in ENTRIES:
  r=subprocess.run([str(exe),str(a.cases),e],cwd=ROOT,capture_output=True,text=True,timeout=600)
  (ROOT/'build/recomp'/f'record_steering_whole_{e}.log').write_text(r.stdout+r.stderr)
  if r.returncode:raise RuntimeError(e+': '+r.stderr+r.stdout)
  pcs=set(source['owners'][e]['source_pcs']);observed=set(r.stdout.split('visited:')[1].splitlines()[0].split())&pcs;covered|=observed
  print(r.stdout.splitlines()[0],flush=True);print('owner coverage',len(observed),'/',len(pcs),'missing',sorted(pcs-observed),flush=True)
 if a.cases>=16384 and covered!=owned:raise RuntimeError('Whole-call union missing '+str(sorted(owned-covered)))
 print('complete steering CPU/PC/full-SR/all-RAM and hardware comparisons pass',flush=True)
if __name__=='__main__':main()
