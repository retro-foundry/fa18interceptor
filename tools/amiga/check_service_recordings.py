"""Compare service C vs original ROM over every sealed native recording.

Both configurations use identical generated game code and machine timing.
RGB444 is compared byte for byte, then temporary multi-gigabyte frame files
are removed. This proves replacements, not whole-game ROM independence.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

ROOT=Path(__file__).resolve().parents[2]
ROM_FLAGS=["--no-os-"+name for name in
           ("vbeam","waitblit","waitbovp","blitter-owner","exec-interrupts","getmsg","potgo","task-lookup","lists","task-services","supervisor")]


def digest(path):
    h=hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda:source.read(1<<20),b""):h.update(block)
    return h.hexdigest()


def same_frames(a,b):
    with a.open("rb") as x,b.open("rb") as y:
        offset=0
        while True:
            first=x.read(1<<20);second=y.read(1<<20)
            if first!=second:
                where=next((i for i,(v,w) in enumerate(zip(first,second)) if v!=w),min(len(first),len(second)))
                raise AssertionError(f"RGB444 differs at byte {offset+where} (frame {(offset+where)//163840})")
            if not first:return offset
            offset+=len(first)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--runner",type=Path,default=ROOT/"build/recomp/fa18_recomp.exe")
    parser.add_argument("--frames",type=int,help="optional bounded smoke run instead of full recordings")
    parser.add_argument("--output",type=Path,default=ROOT/"build/amiga/service-recordings",
                        help="isolated directory for replay outputs and proof metadata")
    args=parser.parse_args()
    if args.frames is not None and args.frames<1:parser.error("positive frame bound required")
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
    rows=[]
    for capture in sorted((ROOT/"captures/native").iterdir()):
        if not (capture/"input.fa18in").is_file():continue
        processes=[];streams=[];paths={}
        try:
            for mode in ("C","ROM"):
                prefix=out/(capture.name+"_"+mode)
                paths[mode]={key:prefix.with_suffix(ext) for key,ext in
                             (("frames",".rgb444"),("ram",".ram"),("log",".log"))}
                log=paths[mode]["log"].open("w");streams.append(log)
                command=[str(args.runner),"--state",str(capture/"state.bin"),"--rom",str(ROOT/"local/system/kick13.rom"),
                         "--input",str(capture/"input.fa18in"),"--ports","off","--rgb444",str(paths[mode]["frames"]),
                         "--ram-out",str(paths[mode]["ram"])]
                command+= ["--frames",str(args.frames)] if args.frames else ["--to-end"]
                if mode=="ROM":command+=ROM_FLAGS
                processes.append(subprocess.Popen(command,cwd=ROOT,stdout=log,stderr=log))
            results=[p.wait() for p in processes]
            if any(results):raise RuntimeError(f"{capture.name}: runner exit codes {results}")
            for stream in streams:stream.close()
            bytes_checked=same_frames(paths["C"]["frames"],paths["ROM"]["frames"])
            ram=digest(paths["C"]["ram"])
            assert ram==digest(paths["ROM"]["ram"]),capture.name
            if not args.frames:
                assert ram==json.loads((capture/"run.json").read_text())["replay"]["final_ram_sha256"],capture.name
            stats=[json.loads(paths[mode]["log"].read_text().splitlines()[-1]) for mode in ("C","ROM")]
            for key in ("frames","cpu_cycles","pc","iterations","blits","line_blits"):
                assert stats[0][key]==stats[1][key],(capture.name,key)
            row={"recording":capture.name,"frames":stats[0]["frames"],"rgb444_bytes_compared":bytes_checked,
                 "rgb444_sha256":digest(paths["C"]["frames"]),"ram_sha256":ram,"cpu_cycles":stats[0]["cpu_cycles"],
                 "final_pc":stats[0]["pc"],"matches_seal":not bool(args.frames)}
            rows.append(row);print(json.dumps(row),flush=True)
        finally:
            for process in processes:
                if process.poll() is None:process.terminate();process.wait()
            for stream in streams:
                if not stream.closed:stream.close()
            for mode in paths:
                paths[mode]["frames"].unlink(missing_ok=True)
    report={"schema":"amiga.service_recordings.v1","runner_sha256":digest(args.runner),
            "rom_sha256":digest(ROOT/"local/system/kick13.rom"),"rom_flags":ROM_FLAGS,
            "bounded_frames":args.frames,"recordings":rows,"whole_game_rom_independent":False}
    (out/("smoke.json" if args.frames else "full.json")).write_text(json.dumps(report,indent=2)+"\n")


if __name__=="__main__":main()
