"""Prove service STOP yields through the real interpreter instruction hook."""
from pathlib import Path
import subprocess
from check_service_phases import write_cases
ROOT=Path(__file__).resolve().parents[2]
def main():
    write_cases()
    subprocess.run([
        "python","scripts/build_recomp.py","--main","tools/amiga/exec_stop_oracle.c",
        "--replace-source","port/machine/machine.c=tools/amiga/service_phase_machine.c",
        "--output","build/recomp/exec_stop_oracle.exe",
    ],cwd=ROOT,check=True)
    subprocess.run([str(ROOT/"build/recomp/exec_stop_oracle.exe")],cwd=ROOT,check=True)
if __name__=="__main__":main()
