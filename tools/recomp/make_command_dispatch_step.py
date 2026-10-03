"""Reproduce command or postflight timing bridges from audited owner scopes.

Native behavior lives in the named domain modules. These adapters own
only source CPU/bus/event boundaries and invokes no opcode handlers.
"""
from pathlib import Path
from collections import defaultdict
import argparse
import json

ROOT=Path(__file__).resolve().parents[2]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument("--family",choices=("command_dispatch","postflight_scheduler"),default="command_dispatch")
family=parser.parse_args().family
manifest=json.loads((ROOT/f"analysis/data/{family}_source_scope.json").read_text())
groups=defaultdict(list)
for row in manifest["instructions"]: groups[row["instruction"].split()[0]].append(row["pc"])
body={
 "rts":"REG_PC=m68ki_pull_32(); break;",
 "jsr":"address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;",
 "jmp":"REG_PC=cache_step_address(mode,reg,4); break;",
 "bsr":"value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;",
 "link":"m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;",
 "unlk":"A(7)=A(reg); A(reg)=m68ki_pull_32(); break;",
 "moveq":"D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;",
 "lea":"A(destination)=cache_step_address(mode,reg,4); break;",
 "adda.w":"A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;",
 "ext.w":"SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;",
 "swap":"step_swap(&D(reg)); break;",
 "asl.l":"step_asl_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;",
 "movem.w":"mask=m68ki_read_imm_16(); address=cache_step_address(mode,reg,2); renderer_load(address,mask,2,mode==3?(int)reg:-1); break;",
 "movem.l":"mask=m68ki_read_imm_16(); address=cache_step_address(mode,reg,4); renderer_load(address,mask,4,mode==3?(int)reg:-1); break;",
 "exg":"value=A(destination); A(destination)=A(reg); A(reg)=value; break;",
 "neg.l":"renderer_negate(&D(reg),4); break;",
}
for mnemonic,condition in {"bra":"1","beq":"COND_EQ()","bne":"COND_NE()","blt":"COND_LT()","ble":"COND_LE()","bge":"COND_GE()","bgt":"COND_GT()"}.items():
 body[mnemonic]=f"step_branch(pc,opcode,{condition}); break;"
for suffix,width,name in (("b",1,"byte"),("w",2,"word"),("l",4,"long")):
 imm=f"m68ki_read_imm_{32 if width==4 else 16}()"
 for mnemonic in ("move","movea"): body[mnemonic+"."+suffix]=f"width={width}; goto move;"
 body["tst."+suffix]=f"cache_step_logic(cache_step_read(mode,reg,{width}),{width}); break;"
 body["clr."+suffix]=f"cache_step_write(mode,reg,{width},0); cache_step_logic(0,{width}); break;"
 body["cmpi."+suffix]=f"value={imm}; step_compare_{name}(value,cache_step_read(mode,reg,{width})); break;"
 body["cmp."+suffix]=f"step_compare_{name}(cache_step_read(mode,reg,{width}),D(destination)); break;"
 for mnemonic,operation in (("andi","&"),("ori","|"),("eori","^")):
  body[mnemonic+"."+suffix]=f"width={width}; value={imm}; operation='{operation}'; goto immediate_logic;"
 for mnemonic,operation in (("addi","+"),("subi","-")):
  body[mnemonic+"."+suffix]=f"width={width}; value={imm}; operation='{operation}'; goto arithmetic;"
 for mnemonic,operation in (("addq","+"),("subq","-")):
  body[mnemonic+"."+suffix]=f"width={width}; value=destination?destination:8; operation='{operation}'; if(mode==1) {{ A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; }} goto arithmetic;"
 body["add."+suffix]=f"width={width}; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;"
 body["sub."+suffix]=f"width={width}; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;"
 body["or."+suffix]=f"width={width}; if(opcode&0x100u) {{ value=D(destination); operation='|'; goto immediate_logic; }} value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;"
for mnemonic,operation in (("btst","?"),("bset","|"),("bclr","&"),("bchg","^")):
 body[mnemonic]=f"value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='{operation}'; goto bit_value;"
for mnemonic in ("asl","asr"):
 body[mnemonic+".w"]=f"if((opcode&0xc0u)!=0xc0u) {{ renderer_{mnemonic}_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; }} address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_{mnemonic}_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;"
missing=set(groups)-set(body)
if missing: raise ValueError(f"unsupported owned source operations: {sorted(missing)}")
out="""/* Complete C1AC28/C1AD74 source CPU/bus/event boundaries.
 * Readable behavior lives in command_dispatch.c and its action modules.
 * Reproduce with tools/recomp/make_command_dispatch_step.py. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_command_dispatch.h"

static int command_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
"""
if family=="postflight_scheduler":
 out=out.replace("Complete C1AC28/C1AD74", "Complete postflight scheduler family").replace(
     "command_dispatch.c and its action modules", "postflight_scheduler.c").replace(
     "make_command_dispatch_step.py.", "make_command_dispatch_step.py --family postflight_scheduler.").replace(
     '"glue_command_dispatch.h"','"glue_postflight_scheduler.h"').replace("command_step", "schedule_step")
for key,pcs in groups.items():
 for j in range(0,len(pcs),4): out+="    "+" ".join("case 0x"+pc+":" for pc in pcs[j:j+4])+"\n"
 out+="        "+body[key]+"\n"
out+="""    default: return 0;
    }
    goto finish;
move:
    value=cache_step_read(mode,reg,width); mode=(opcode>>6)&7u;
    cache_step_write(mode,destination,width,value); if(mode!=1) cache_step_logic(value,width);
    goto finish;
immediate_logic:
    if(mode==0) old=D(reg);
    else { address=cache_step_address(mode,reg,width); old=cache_step_read_memory(address,width); }
    value=operation=='&'?old&value:operation=='|'?old|value:old^value;
    if(mode==0) cache_step_write(0,reg,width,value); else cache_step_write_memory(address,value,width,0);
    cache_step_logic(value,width); goto finish;
arithmetic:
    if(mode==0) old=D(reg);
    else { address=cache_step_address(mode,reg,width); old=cache_step_read_memory(address,width); }
    if(operation=='+') {
        if(width==1) renderer_add_byte(&old,value); else if(width==2) step_add_word(&old,value); else step_add_long(&old,value);
    } else {
        if(width==1) step_subtract_byte(&old,value); else if(width==2) step_subtract_word(&old,value); else step_subtract_long(&old,value);
    }
    if(mode==0) cache_step_write(0,reg,width,old); else cache_step_write_memory(address,old,width,0);
    goto finish;
bit_value:
    mask=(uint16_t)(value&(mode==0?31u:7u)); value=1u<<mask;
    if(mode==0) old=D(reg);
    else { address=cache_step_address(mode,reg,1); old=cache_step_read_memory(address,1); }
    FLAG_Z=old&value;
    if(operation!='?') {
        value=operation=='|'?old|value:operation=='&'?old&~value:old^value;
        if(mode==0) { D(reg)=value; if(mask<16) USE_CYCLES(-2); }
        else cache_step_write_memory(address,value,1,0);
    }
finish:
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
static int owns_pc(const uint32_t *pcs,unsigned count,uint32_t pc) {
    unsigned lo=0,hi=count;
    while(lo<hi) { unsigned mid=lo+(hi-lo)/2; if(pcs[mid]<pc) lo=mid+1; else hi=mid; }
    return lo<count && pcs[lo]==pc;
}
"""
for entry,owner in manifest["owners"].items():
 pcs=owner["source_pcs"]
 out+=f"static const uint32_t owned_{entry}[]={{\n"
 for j in range(0,len(pcs),8): out+="    "+",".join("0x"+pc for pc in pcs[j:j+8])+",\n"
 out+="};\n"
 out+=f"int glue_{entry}_owns(uint32_t pc) {{ return owns_pc(owned_{entry},sizeof owned_{entry}/sizeof owned_{entry}[0],pc); }}\n"
 out+=f"int glue_{entry}_step(void) {{ if(!glue_{entry}_owns(REG_PC)) return 0; return {'schedule_step' if family=='postflight_scheduler' else 'command_step'}(); }}\n"
path=ROOT/f"port/game/glue/glue_{family}_step.c"
path.write_text(out)
print(f"{family} timing bridge: {len(manifest['instructions'])} unique original boundaries")
