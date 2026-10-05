"""Check every game-owned ADF resource against its independent extraction seal."""
from pathlib import Path
import hashlib
import json
import subprocess
ROOT=Path(__file__).resolve().parents[2]
def main():
    manifest=json.loads((ROOT/"analysis/disk_game_resources.json").read_text())
    disk=ROOT/"local/media/fa18.adf"
    assert hashlib.sha256(disk.read_bytes()).hexdigest()==manifest["authority_disk_sha256"]
    out=ROOT/"build/amiga";out.mkdir(parents=True,exist_ok=True)
    exe=out/"ofs_reader_test.exe"
    subprocess.run(["gcc","-std=c11","-Wall","-Wextra","-Werror","-UNDEBUG","-Iport/amiga",
        "tools/amiga/ofs_reader_test.c","port/game/disk.c","port/amiga/guest_memory.c","-o",str(exe)],cwd=ROOT,check=True)
    for row in manifest["resources"]:
        # Deliberately alter ASCII case to test the original OFS name hash.
        result=subprocess.run([str(exe),str(disk),row["adf_path"].swapcase(),str(out/"ofs_resource.bin")],
                              cwd=ROOT,check=True,capture_output=True,text=True)
        data=(out/"ofs_resource.bin").read_bytes(); info=json.loads(result.stdout)
        assert len(data)==row["size_bytes"] and hashlib.sha256(data).hexdigest()==row["sha256"],row["adf_path"]
        # Metadata is checked independently against the actual header block.
        header=disk.read_bytes()[info["block"]*512:(info["block"]+1)*512]
        for key,at in [("size",324),("protection",320),("days",420),("minutes",424),("ticks",428)]:
            assert info[key]==int.from_bytes(header[at:at+4],"big"),key
    print(f"All {len(manifest['resources'])} game-owned ADF resources match extraction hashes and metadata")
if __name__=="__main__":main()
