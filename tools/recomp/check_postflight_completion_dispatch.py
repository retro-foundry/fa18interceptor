"""Prove real postflight completion dispatch, reference modes, selection and code-write guards."""
import argparse
import subprocess
import json
from audit_postflight_completion_source import ROOT,ENTRIES,MANIFEST
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cases',type=int,default=256); args=parser.parse_args()
    if args.cases<=0: parser.error('cases must be positive')
    source=json.loads(MANIFEST.read_text())
    executable=ROOT/'build/recomp/postflight_completion_dispatch_oracle.exe'
    subprocess.run(['python','scripts/build_recomp.py','--output',str(executable.relative_to(ROOT)),
        '--main','tools/recomp/postflight_completion_dispatch_oracle.c','--replace-source',
        'port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c'],cwd=ROOT,check=True)
    for mode,name in [(1,'on'),(2,'shadow'),(3,'sandbox')]:
        for entry in ENTRIES:
            result=subprocess.run([str(executable),str(args.cases),entry,str(mode)],cwd=ROOT,
                capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'postflight_completion_dispatch_{entry}_{name}.log').write_text(result.stdout+result.stderr)
            if result.returncode: raise RuntimeError(result.stderr or result.stdout)
            print(name+': '+result.stdout.splitlines()[0],flush=True)
            if args.cases>=256:
                visited=set(result.stdout.split('visited:')[1].split())
                missing=set(source['owners'][entry]['source_pcs'])-visited
                if missing: raise RuntimeError(f'{name} {entry}: uncovered owner boundaries {sorted(missing)}')
    print(f'postflight completion dispatch: {len(ENTRIES)*3*args.cases} complete CPU/RAM fixtures and entry/mode/selection/write guards passed')
if __name__=='__main__': main()
