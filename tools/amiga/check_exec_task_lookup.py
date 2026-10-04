"""Check complete FindTask calls, including both lists and current-task names."""
from pathlib import Path
import subprocess
ROOT = Path(__file__).resolve().parents[2]
def main():
    subprocess.run([
        "python", "scripts/build_recomp.py", "--main", "tools/amiga/exec_task_lookup_oracle.c",
        "--replace-source", "port/machine/machine.c=tools/amiga/service_phase_machine.c",
        "--output", "build/recomp/exec_task_lookup_oracle.exe",
    ], cwd=ROOT, check=True)
    subprocess.run([str(ROOT / "build/recomp/exec_task_lookup_oracle.exe")], cwd=ROOT, check=True)
if __name__ == "__main__": main()
