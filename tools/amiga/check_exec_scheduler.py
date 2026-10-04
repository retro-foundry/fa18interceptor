"""Prove complete 68000 task dispatch, callbacks and blocking waits."""
from pathlib import Path
import hashlib
import json
import subprocess
from check_service_phases import write_cases
ROOT=Path(__file__).resolve().parents[2]
def main():
    write_cases()
    rows=[]
    inventory=json.loads((ROOT/"analysis/data/romfree_exec_scheduler_contracts.json").read_text())
    for c in inventory["original_engine_contracts"]:
        directory=ROOT/c["evidence_directory"]
        for boundary in ("entry","checkpoint"):
            for name,want in c[boundary]["files"].items():
                got=hashlib.sha256((directory/name).read_bytes()).hexdigest()
                if got!=want: raise AssertionError(f"altered scheduler evidence: {directory/name}")
        r=c["checkpoint"]["registers"]
        registers=",".join(str(r[f"{reg}{i}"])+"u" for reg in ("d","a") for i in range(8))
        elapsed=c["checkpoint"]["cycle"]-c["entry"]["cycle"]
        rows.append('{"'+directory.relative_to(ROOT).as_posix()+'",'+str(c["stop_pc"])+
                    'u,'+str(c["instructions"])+'u,'+str(elapsed)+'u,{'+registers+'},'+str(r["sr"])+"u},")
    (ROOT/"build/amiga/scheduler_captures.h").write_text(
        "static const struct { const char *directory; uint32_t pc,count,cycles,regs[16],sr; } scheduler_captures[]={\n"+
        "\n".join(rows)+"\n};\n")
    subprocess.run([
        "python","scripts/build_recomp.py","--main","tools/amiga/exec_scheduler_oracle.c",
        "--replace-source","port/machine/machine.c=tools/amiga/service_phase_machine.c",
        "--output","build/recomp/exec_scheduler_oracle.exe",
    ],cwd=ROOT,check=True)
    subprocess.run([str(ROOT/"build/recomp/exec_scheduler_oracle.exe")],cwd=ROOT,check=True)
if __name__=="__main__":main()
