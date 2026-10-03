"""Check actual formatter dispatch; file-service stops are tested separately."""
import argparse,json,subprocess
from audit_postflight_file_callers_source import ROOT,MANIFEST
def main():
    parser=argparse.ArgumentParser(description=__doc__); parser.add_argument('--cases',type=int,default=256); args=parser.parse_args()
    if args.cases<=0: parser.error('cases must be positive')
    source=json.loads(MANIFEST.read_text()); exe=ROOT/'build/recomp/postflight_file_callers_dispatch_oracle.exe'
    subprocess.run(['python','scripts/build_recomp.py','--output',str(exe.relative_to(ROOT)),
        '--main','tools/recomp/postflight_file_callers_dispatch_oracle.c','--replace-source',
        'port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c'],cwd=ROOT,check=True)
    for mode,name in [(1,'on'),(2,'shadow'),(3,'sandbox')]:
        result=subprocess.run([str(exe),str(args.cases),'C0F56A',str(mode)],cwd=ROOT,capture_output=True,text=True,timeout=600)
        (ROOT/'build/recomp'/f'postflight_file_callers_dispatch_C0F56A_{name}.log').write_text(result.stdout+result.stderr)
        if result.returncode: raise RuntimeError(result.stderr or result.stdout)
        print(name+': '+result.stdout.splitlines()[0],flush=True)
        if args.cases>=256:
            missing=set(source['owners']['C0F56A']['source_pcs'])-set(result.stdout.split('visited:')[1].split())
            if missing: raise RuntimeError(f'{name}: missing source boundaries {sorted(missing)}')
    print(f'postflight file callers dispatch: {args.cases*3} complete formatter CPU/RAM fixtures and entry/mode/selection/write guards passed; file owners retain original service-stop classification')
if __name__=='__main__': main()
