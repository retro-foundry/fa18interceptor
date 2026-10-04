"""Verify explicit machine startup without any ROM, savestate or game assets."""
from pathlib import Path
import subprocess
ROOT=Path(__file__).resolve().parents[2]
def main():
    subprocess.run(["python","scripts/build_recomp.py","--main","tools/amiga/machine_startup_test.c",
                    "--output","build/recomp/machine_startup_test.exe"],cwd=ROOT,check=True)
    subprocess.run([str(ROOT/"build/recomp/machine_startup_test.exe")],cwd=ROOT,check=True)
if __name__=="__main__":main()
