"""Prove interrupt vectors, server chains, Cause and hardware IRQ continuations."""
from pathlib import Path
import hashlib
import json
import subprocess
from check_service_phases import write_cases
ROOT=Path(__file__).resolve().parents[2]

def main():
    write_cases()
    rows=[]
    inventory=json.loads((ROOT/"analysis/data/romfree_exec_interrupt_contracts.json").read_text())
    for c in inventory["original_engine_contracts"]:
        directory=ROOT/c["evidence_directory"]
        boundary="return" if "return" in c else "checkpoint"
        for key in ("entry",boundary):
            for name,want in c[key]["files"].items():
                if hashlib.sha256((directory/name).read_bytes()).hexdigest()!=want:
                    raise AssertionError(f"altered interrupt evidence: {directory/name}")
        for name,key in (("instructions.jsonl","trace_sha256"),("hardware_window.jsonl","hardware_sha256")):
            if hashlib.sha256((directory/name).read_bytes()).hexdigest()!=c[key]:
                raise AssertionError(f"altered interrupt evidence: {directory/name}")
        # The two longer captures include other, still unreplaced OS handlers.
        # Keep their full evidence inventory; replay them when closure exists.
        if c["entry_pc"] not in (0xFC11CA,0xFC1210,0xFC13BC):continue
        r=c[boundary]["registers"]
        registers=",".join(str(r[f"{reg}{i}"])+"u" for reg in ("d","a") for i in range(8))
        elapsed=c[boundary]["cycle"]-c["entry"]["cycle"]
        pc=c.get("return_pc",c.get("stop_pc"))
        rows.append('{"'+directory.relative_to(ROOT).as_posix()+'","'+boundary+'",'+str(pc)+
                    'u,'+str(c["instructions"])+'u,'+str(elapsed)+'u,{'+registers+'},'+str(r["sr"])+"u},")
    (ROOT/"build/amiga/interrupt_captures.h").write_text(
        "static const struct { const char *directory,*boundary; uint32_t pc,count,cycles,regs[16],sr; } interrupt_captures[]={\n"+
        "\n".join(rows)+"\n};\n")
    subprocess.run([
        "python","scripts/build_recomp.py","--main","tools/amiga/exec_interrupt_oracle.c",
        "--replace-source","port/machine/machine.c=tools/amiga/service_phase_machine.c",
        "--output","build/recomp/exec_interrupt_oracle.exe",
    ],cwd=ROOT,check=True)
    subprocess.run([str(ROOT/"build/recomp/exec_interrupt_oracle.exe")],cwd=ROOT,check=True)
if __name__=="__main__":main()
