"""Check ADF-only construction and the current explicit startup boundary."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import re
import shutil
import struct
import subprocess
import tempfile

ROOT=Path(__file__).resolve().parents[2]

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runner",type=Path,default=ROOT/"build/recomp/fa18_romfree.exe")
    parser.add_argument("--expected-service",default="exec.OpenLibrary.wrapper")
    args=parser.parse_args()
    layout=json.loads((ROOT/"analysis/data/romfree_hunk_layout.json").read_text())
    inventory=json.loads((ROOT/"analysis/hunk_inventory.json").read_text())
    disk=(ROOT/"local/extracted/f18_interceptor").read_bytes()
    assert hashlib.sha256(disk).hexdigest()==layout["evidence"]["executable_sha256"]
    out=ROOT/"build/amiga"; out.mkdir(parents=True,exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="romfree-isolated-",dir=out) as temporary:
        work=Path(temporary); runner=work/"fa18_romfree.exe"
        shutil.copy2(args.runner,runner); shutil.copy2(ROOT/"local/media/fa18.adf",work/"original.adf")
        env=os.environ.copy(); env.pop("FA18_TRACE",None)
        base=[str(runner),"--adf","original.adf","--ports","off"]
        result=subprocess.run(base+["--frames","0","--ram-out","constructed.ram"],cwd=work,env=env,capture_output=True,text=True)
        assert result.returncode==0,result.stderr
        stats=[json.loads(line) for line in result.stdout.splitlines()]
        assert stats[-1]==dict(rom_reads=0,rom_instruction_fetches=0,unsupported_services=0)
        assert stats[0]["disabled_functions"]==42 and stats[0]["cpu_cycles"]==0
        ram=(work/"constructed.ram").read_bytes()
        at=lambda a:a if a<0x80000 else a-0xC00000+0x80000
        for row,segment in zip(layout["segments"],inventory["segments"],strict=True):
            expected=bytearray(row["header_size"])
            if segment["kind"]!="BSS":
                start=segment["payload_file_offset"]
                expected[:segment["size_bytes"]]=disk[start:start+segment["size_bytes"]]
            for group in segment["reloc32"]:
                for offset in group["offsets"]:
                    value=struct.unpack_from(">I",expected,offset)[0]
                    struct.pack_into(">I",expected,offset,(value+layout["segments"][group["target_segment"]]["payload_base"])&0xFFFFFFFF)
            address=at(row["payload_base"])
            assert ram[address:address+len(expected)]==expected,row["index"]
            assert struct.unpack_from(">II",ram,address-8)==(row["allocation_size"],row["next_segment_bptr"])
        regs=struct.unpack_from(">18I",ram,0x100000)
        assert regs[:15]==(0,)*15 and regs[15:]==(0xC550DC,0,0xC0DEB0)
        faults=[]
        for mode in ([],["--no-recomp"]):
            result=subprocess.run(base+["--frames","1"]+mode,cwd=work,env=env,capture_output=True,text=True)
            assert result.returncode==1,(mode,result.stdout,result.stderr)
            assert "service="+args.expected_service in result.stderr,result.stderr
            assert "rom_reads=0 rom_instruction_fetches=0 unsupported_services=1" in result.stderr,result.stderr
            faults.append(result.stderr.strip())
        assert faults[0]==faults[1],faults
        for options,code in (([],2),(["--rom","missing.rom"],2),(["--state","missing.state"],2),
                             (["--adf","missing.adf"],1),(["--help"],0)):
            result=subprocess.run([str(runner)]+options,cwd=work,env=env,capture_output=True,text=True)
            assert result.returncode==code,(options,result.returncode)
        (work/"bad.adf").write_bytes(b"invalid image")
        result=subprocess.run([str(runner),"--adf","bad.adf"],cwd=work,env=env,capture_output=True,text=True)
        assert result.returncode==1 and "ROM-free launch:" in result.stderr
    print(f"ADF-only construction: all 185 hunks/8441 relocations and CPU handoff match; 42 OS translations excluded; both execution modes stop explicitly: {faults[0]}")

if __name__=="__main__":main()
