"""Check write-index semantics and deliberate byte mismatches in both reference modes."""
import json
import subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
def main():
    executable=ROOT/'build/recomp/write_index_oracle.exe'
    subprocess.run(['gcc','-O2','-std=c11','-Iport/recomp','tools/recomp/write_index_oracle.c','-o',str(executable)],cwd=ROOT,check=True)
    result=subprocess.run([str(executable)],capture_output=True,text=True,check=True)
    (ROOT/'build/recomp/write_index_oracle.log').write_text(result.stdout+result.stderr)
    print(result.stdout.strip(),flush=True)
    mutation=ROOT/'build/recomp/menu_cold_mutation_dispatch.exe'
    subprocess.run(['python','scripts/build_recomp.py','--output',str(mutation.relative_to(ROOT)),
        '--main','tools/recomp/menu_cold_dispatch_oracle.c','--replace-source',
        'port/game/glue/glue_menu_cold_step.c=tools/recomp/menu_cold_mutation_step.c',
        '--replace-source','port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c'],cwd=ROOT,check=True)
    for mode,name in [(2,'shadow'),(3,'sandbox')]:
        report=ROOT/'build/recomp/menu_cold_dispatch_failed_report.json'
        if report.exists(): report.unlink()
        result=subprocess.run([str(mutation),'1','C1017E',str(mode)],cwd=ROOT,capture_output=True,text=True,timeout=120)
        if not result.returncode or not report.exists(): raise RuntimeError('mutated queue byte was not rejected')
        rows=json.loads(report.read_text()); row=next(row for row in rows if row['entry']=='C1017E')
        if (row['calls'],row['matched'],row['mismatched'],row['hardware'],row['incomplete'])!=(1,0,1,0,0):
            raise RuntimeError(f'mutation was not a completed byte mismatch: {row}')
        if 'mismatch: byte' not in result.stderr: raise RuntimeError('mutation rejection did not identify the byte difference')
        (ROOT/'build/recomp'/f'write_index_mutation_{name}.json').write_text(report.read_text())
        (ROOT/'build/recomp'/f'write_index_mutation_{name}.log').write_text(result.stdout+result.stderr)
        print(f'{name}: deliberately changed queue byte rejected as a completed mismatch',flush=True)
if __name__=='__main__': main()
