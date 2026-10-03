"""Prove actual input-device dispatch and strict reference classifications."""
import argparse,json,subprocess
from audit_input_device_callbacks_source import ROOT,ENTRIES,MANIFEST
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--cases',type=int,default=256); args=parser.parse_args()
    if args.cases<=0: parser.error('cases must be positive')
    source=json.loads(MANIFEST.read_text()); exe=ROOT/'build/recomp/input_device_callbacks_dispatch_oracle.exe'
    subprocess.run(['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),
        '--main','tools/recomp/input_device_callbacks_dispatch_oracle.c','--replace-source',
        'port/recomp/recomp_ports.c=tools/recomp/input_device_callbacks_write_log.c'],cwd=ROOT,check=True)
    for mode,name in [(1,'on'),(2,'shadow'),(3,'sandbox')]:
        for e in ENTRIES:
            result=subprocess.run([str(exe),str(args.cases),e,str(mode)],cwd=ROOT,capture_output=True,text=True,timeout=600)
            (ROOT/'build/recomp'/f'input_device_callbacks_dispatch_{e}_{name}.log').write_text(result.stdout+result.stderr)
            if result.returncode: raise RuntimeError(result.stderr or result.stdout)
            print(name+': '+result.stdout.splitlines()[0],flush=True)
            if args.cases>=256:
                missing=set(source['owners'][e]['source_pcs'])-set(result.stdout.split('visited:')[1].split())
                if missing: raise RuntimeError(f'{name} {e}: missing boundaries {sorted(missing)}')
    print(f'input device dispatch: {len(ENTRIES)*3*args.cases} complete CPU/RAM fixtures and entry/mode/selection/write guards passed')
if __name__=='__main__': main()
