"""Compare the C guest loader with disk-derived bytes and original DOS placement."""
from pathlib import Path
import argparse
import hashlib
import json
import struct
import subprocess

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable",type=Path,default=ROOT/"local/extracted/f18_interceptor")
    parser.add_argument("--adf",type=Path,default=ROOT/"local/media/fa18.adf")
    parser.add_argument("--layout",type=Path,default=ROOT/"analysis/data/romfree_hunk_layout.json")
    parser.add_argument("--inventory",type=Path,default=ROOT/"analysis/hunk_inventory.json")
    args=parser.parse_args()
    layout=json.loads(args.layout.read_text()); inventory=json.loads(args.inventory.read_text())
    disk=args.executable.read_bytes()
    assert hashlib.sha256(disk).hexdigest()==layout["evidence"]["executable_sha256"]
    assert hashlib.sha256(args.inventory.read_bytes()).hexdigest()==layout["evidence"]["inventory_sha256"]
    out=ROOT/"build/amiga"; out.mkdir(parents=True,exist_ok=True)
    packed=struct.pack(">I",len(layout["segments"]))+b"".join(struct.pack(">II",s["payload_base"],s["allocation_size"]) for s in layout["segments"])
    (out/"layout.bin").write_bytes(packed)
    exe=out/"hunk_loader_test.exe"
    subprocess.run(["gcc","-std=c11","-Wall","-Wextra","-Werror","-UNDEBUG","-Iport/amiga",
        "tools/amiga/hunk_loader_test.c","port/game/hunk.c","port/game/disk.c","port/amiga/hunk_loader.c","port/amiga/guest_memory.c",
        "-o",str(exe)],cwd=ROOT,check=True)
    # The C loading path receives only the ADF and caller-owned placements.
    # Extracted disk bytes are an independent comparison oracle below.
    subprocess.run([str(exe),"--adf",str(args.adf),"F-18 Interceptor",str(out/"layout.bin"),str(out/"loaded_ram.bin")],cwd=ROOT,check=True)
    ram=(out/"loaded_ram.bin").read_bytes(); matched=0
    for row,segment in zip(layout["segments"],inventory["segments"],strict=True):
        size=row["header_size"]
        expected=bytearray(size)
        if segment["kind"]!="BSS":
            start=segment["payload_file_offset"]
            expected[:segment["size_bytes"]]=disk[start:start+segment["size_bytes"]]
        for group in segment["reloc32"]:
            for offset in group["offsets"]:
                value=struct.unpack_from(">I",expected,offset)[0]
                struct.pack_into(">I",expected,offset,(value+layout["segments"][group["target_segment"]]["payload_base"])&0xFFFFFFFF)
                matched+=1
        base=row["payload_base"]
        at=base if base<0x80000 else 0x80000+base-0xC00000
        assert ram[at:at+size]==expected,row["index"]
        assert struct.unpack_from(">II",ram,at-8)==(row["allocation_size"],row["next_segment_bptr"])
    assert matched==layout["verified_relocations"]
    print(f"All {len(layout['segments'])} segment headers and disk-derived payloads match; {matched} original relocations match")


if __name__=="__main__": main()
