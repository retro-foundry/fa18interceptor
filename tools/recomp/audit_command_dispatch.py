"""Audit complete static command-owner flow against sealed original bytes.

Calls remain distinct child boundaries; jumps (including C06BF0) belong to
the owner. Default checks the committed manifest; --write refreshes it.
"""
import argparse
import hashlib
import json
from pathlib import Path
from port_info import ROOT, instructions
from recomp import Decoder, classify, static_target

MANIFEST=ROOT/"analysis/data/command_dispatch_source_scope.json"
STATE=ROOT/"captures/native/demo01/state.bin"
ENTRIES=("C1AC28", "C1AD74")

def source_decoder():
    state=STATE.read_bytes()
    regions=[]
    offset=0
    while offset+8<=len(state):
        tag=state[offset:offset+4]
        length=int.from_bytes(state[offset+4:offset+8],"big")
        if tag==b"\0\0\0\0": offset+=4; continue
        if tag==b"END ": break
        if length<12: raise ValueError(f"bad state chunk at {offset}")
        if tag in (b"CRAM",b"BRAM"):
            regions.append((0 if tag==b"CRAM" else 0xc00000,state[offset+12:offset+length]))
        offset+=(length+3)&~3
    return state,Decoder((ROOT/"build/recomp/dasm_helper.dll").resolve(),regions)

def audit(entries=ENTRIES, dynamic_targets=None, additional_cold_entries=()):
    state,decoder=source_decoder()
    dynamic_targets=dynamic_targets or {}
    rows={}
    owners={}
    for entry in entries:
        pending=[int(entry,16)]
        visited=set()
        calls=[]
        while pending:
            pc=pending.pop()
            if pc in visited: continue
            decoded=decoder.decode(pc)
            if decoded is None: raise ValueError(f"{entry}: undecodable source {pc:06X}")
            length,opcode,handler,assembly=decoded
            visited.add(pc)
            kind=classify(handler)
            target=static_target(decoder,pc,opcode,handler,kind)
            raw=b"".join(decoder.word(pc+i).to_bytes(2,"big") for i in range(0,length,2))
            rows[pc]={"pc":f"{pc:06X}","length":length,"bytes":raw.hex(),
                      "opcode":f"{opcode:04X}","instruction":assembly}
            if kind=="rts": continue
            if kind=="interp": raise ValueError(f"{entry}: exceptional source exit {pc:06X}")
            if kind=="jmp" and target is None and pc in dynamic_targets:
                pending.extend(dynamic_targets[pc]); continue
            if kind in ("bra","jmp","bcc","dbcc","bsr","jsr") and target is None:
                raise ValueError(f"{entry}: dynamic transfer needs further evidence at {pc:06X}")
            if kind in ("bra","jmp"): pending.append(target)
            elif kind in ("bcc","dbcc"): pending.extend((pc+length,target))
            else:
                if kind in ("bsr","jsr"):
                    calls.append({"pc":f"{pc:06X}","target":f"{target:06X}",
                                  "return_pc":f"{pc+length:06X}"})
                pending.append(pc+length)
        generated={int(line.split(":")[0],16) for line in instructions(entry)}
        if generated-visited or (generated!=visited and entry not in additional_cold_entries):
            raise ValueError(f"{entry}: generated/source difference {sorted(generated^visited)}")
        owners[entry]={"instruction_count":len(visited),"additional_cold_instructions":len(visited-generated),
                       "source_pcs":[f"{pc:06X}" for pc in sorted(visited)],
                       "child_call_sites":sorted(calls,key=lambda row:row["pc"])}
    sets=[set(owner["source_pcs"]) for owner in owners.values()]
    shared=sum(sum(pc in pcs for pcs in sets)>1 for pc in set.union(*sets))
    packed=b"".join(int(row["pc"],16).to_bytes(4,"big")+bytes.fromhex(row["bytes"])
                    for pc,row in sorted(rows.items()))
    return {"state":"captures/native/demo01/state.bin",
            "state_sha256":hashlib.sha256(state).hexdigest(),
            "scope":"static owner flow: follow jumps, stop at returns; calls are child boundaries",
            "owners":owners,"unique_instruction_count":len(rows),
            "shared_instruction_count":shared,
            "owned_pc_and_source_bytes_sha256":hashlib.sha256(packed).hexdigest(),
            "instructions":[rows[pc] for pc in sorted(rows)]}

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write",action="store_true")
    args=parser.parse_args()
    result=audit()
    if args.write: MANIFEST.write_text(json.dumps(result,indent=2)+"\n")
    elif json.loads(MANIFEST.read_text())!=result:
        raise ValueError("command-dispatch source audit changed")
    print(f"command source audit: {result['unique_instruction_count']} unique / "
          f"{result['shared_instruction_count']} shared instructions; no missing static paths")

if __name__=="__main__": main()
