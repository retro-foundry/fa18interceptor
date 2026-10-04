#include "host_compat_adapter.h"
#include "service_dispatch_adapter.h"
#include "service_phase.h"
#include "../amiga/abi_13.h"
#include "../amiga/hunk.h"
#include "../amiga/host_graphics.h"
#include "m68kcpu.h"
#include "m68kops.h"
#include "machine.h"
#include "recomp_runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static AmigaHostCompat *active;
static const AmigaLibraryVector *original_vectors;
static size_t vector_count;
static unsigned trace_left;
static uint8_t *guest(AmigaHostCompat *c,uint32_t a,uint32_t n) {
    uint8_t *p=amiga_guest_range(&c->memory,a,n);
    if (!p) {
        fprintf(stderr,"compat: invalid guest range %06X+%u caller=%06X service=%06X\n",a,n,REG_PPC,REG_PC);
        amiga_runtime_guard_unsupported(&fa18_machine->runtime_guard,REG_PPC,REG_PC,(uint64_t)fa18_machine_now());
        fa18_machine_runtime_fault();
    }
    return p;
}
static uint32_t read32(AmigaHostCompat *c,uint32_t a) { return amiga_be32(guest(c,a,4)); }
static void write32(AmigaHostCompat *c,uint32_t a,uint32_t v) {
    amiga_store_be32(guest(c,a,4),v); fa18_recomp_note_write(a,4);
}
static void logic(uint32_t v) { FLAG_N=NFLAG_32(v); FLAG_Z=v; FLAG_V=VFLAG_CLEAR; FLAG_C=CFLAG_CLEAR; }
static int returned(uint32_t pc) {
    fa18_service_begin(pc,0x4E75);
    REG_PC=m68k_read_memory_32(REG_A[7]); REG_A[7]+=4;
    /* Coarse compatibility charge. The chipset still owns waits. */
    USE_CYCLES(16); return 1;
}
static int unsupported(unsigned id,unsigned offset) {
    fprintf(stderr,"compat: unsupported %s LVO=-%u caller=%06X time=%lld\n",
            amiga_host_library_name(id),offset,REG_PPC,(long long)fa18_machine_now());
    amiga_runtime_guard_unsupported(&fa18_machine->runtime_guard,REG_PPC,REG_PC,(uint64_t)fa18_machine_now());
    fa18_machine_runtime_fault(); return 0;
}
static int forward_exec(unsigned offset) {
    switch (offset) {
    case 30: case 36: case 48: case 54: case 60: case 66:
    case 120: case 126: case 132: case 138:
    case 162: case 168: case 174: case 180:
    case 234: case 240: case 246: case 252: case 258: case 264: case 270: case 276:
    case 294: case 306: case 312: case 318: case 324: case 330: case 336: case 342: case 348:
    case 372: case 378: case 384:
        for (size_t i=0;i<vector_count;++i) if (original_vectors[i].offset==offset) {
            REG_PC=original_vectors[i].target; USE_CYCLES(4); return 1;
        }
        break;
    }
    return 0;
}
static int io_complete(AmigaHostCompat *c,uint32_t request,int reply) {
    uint8_t *io=guest(c,request,32); io[31]=0; io[8]=7;
    if (reply && !(io[30]&1)) {
        uint32_t port=amiga_be32(io+14);
        if (port) {
            if (!amiga_host_add_tail(c,port+AMIGA_PORT_MESSAGES,request)) return 0;
            uint8_t *p=guest(c,port,32);
            if (!(p[14]&3)) {
                uint32_t task=amiga_be32(p+16),mask=1u<<(p[15]&31);
                write32(c,task+AMIGA_TASK_SIGNALS_RECEIVED,read32(c,task+AMIGA_TASK_SIGNALS_RECEIVED)|mask);
            }
        }
    }
    return 1;
}
static int queue_io(AmigaHostCompat *c,uint32_t request,unsigned device,uint64_t deadline) {
    for (size_t i=0;i<c->pending_count;++i) if (c->pending[i].request==request) return 0;
    if (c->pending_count==64) return 0;
    size_t i=c->pending_count++;
    c->pending[i].request=request; c->pending[i].device=device; c->pending[i].deadline=deadline;
    uint8_t *io=guest(c,request,32); io[8]=5; io[30]&=(uint8_t)~1; io[31]=0;
    return 1;
}
int fa18_os_host_key(unsigned rawkey,int down) {
    if (!active) return 0;
    if (!amiga_host_queue_key(active,rawkey,down)) {
        fprintf(stderr,"compat: keyboard event queue is full or key is invalid\n");
        fa18_machine_runtime_fault();
    }
    return 1;
}
void fa18_os_host_tick(uint64_t cycle) {
    AmigaHostCompat *c=active; if (!c) return;
    for (size_t i=0;i<c->pending_count;) {
        unsigned device=c->pending[i].device; uint32_t request=c->pending[i].request;
        if (device==AMIGA_HOST_KEYBOARD) {
            if (c->key_head==c->key_tail) { ++i; continue; }
            uint8_t *io=guest(c,request,48);
            if (amiga_be32(io+36)<22) { fprintf(stderr,"compat: keyboard event buffer is too short\n"); fa18_machine_runtime_fault(); }
            uint8_t key=c->keys[c->key_head]; c->key_head=(c->key_head+1)%sizeof c->keys;
            uint8_t *event=guest(c,amiga_be32(io+40),22); memset(event,0,22); event[4]=1; event[7]=key;
            amiga_store_be32(event+14,(uint32_t)(cycle/7093790));
            amiga_store_be32(event+18,(uint32_t)((cycle%7093790)*1000000/7093790));
            write32(c,request+32,22);
        } else if (device==AMIGA_HOST_TIMER) {
            if (cycle<c->pending[i].deadline) { ++i; continue; }
        } else { ++i; continue; }
        if (!io_complete(c,request,1)) { fprintf(stderr,"compat: cannot complete device request %06X\n",request); fa18_machine_runtime_fault(); }
        memmove(c->pending+i,c->pending+i+1,(c->pending_count-i-1)*sizeof *c->pending); --c->pending_count;
    }
    if (c->wait_kind) {
        int ready=0;
        if (c->wait_kind==1) {
            uint32_t task=read32(c,c->libraries[0]+AMIGA_EXEC_THIS_TASK);
            ready=(read32(c,task+AMIGA_TASK_SIGNALS_RECEIVED)&c->wait_value)!=0;
        } else if (c->wait_kind==2) ready=read32(c,read32(c,c->wait_value+AMIGA_PORT_MESSAGES))!=0;
        else if (c->wait_kind==3) ready=guest(c,c->wait_value+8,1)[0]==7;
        if (ready) { CPU_STOPPED&=~STOP_LEVEL_STOP; c->wait_kind=0; }
    }
}
static int wait_retry(AmigaHostCompat *c,unsigned kind,uint32_t value) {
    /* A single guest process waits while chipset interrupts and host device
     * completions continue. No invented idle task or captured OS context. */
    c->wait_kind=kind; c->wait_value=value;
    USE_CYCLES(16); CPU_STOPPED|=STOP_LEVEL_STOP; SET_CYCLES(0);
    return 2;
}
static int device_command(AmigaHostCompat *c,uint32_t request,int reply) {
    uint8_t *io=guest(c,request,48); uint32_t device=amiga_be32(io+20);
    unsigned id;
    for (id=AMIGA_HOST_TIMER;id<AMIGA_HOST_LIBRARY_COUNT;++id) if (device==c->libraries[id] && device) break;
    if (id==AMIGA_HOST_LIBRARY_COUNT) return 0;
    unsigned command=amiga_be16(io+28); uint32_t data=amiga_be32(io+40),length=amiga_be32(io+36);
    int handled=0;
    if ((id==AMIGA_HOST_KEYBOARD || id==AMIGA_HOST_GAMEPORT) && command==9)
        return queue_io(c,request,id,UINT64_MAX);
    if (id==AMIGA_HOST_TIMER && command==9) {
        uint32_t seconds=amiga_be32(io+32),microseconds=amiga_be32(io+36);
        return queue_io(c,request,id,(uint64_t)fa18_machine_now()+(uint64_t)seconds*7093790+(uint64_t)microseconds*7093790/1000000);
    }
    if (id==AMIGA_HOST_TIMER && command==10) {
        uint64_t now=(uint64_t)fa18_machine_now();
        write32(c,request+32,(uint32_t)(now/7093790));
        write32(c,request+36,(uint32_t)((now%7093790)*1000000/7093790));
        return io_complete(c,request,reply);
    }
    if (id==AMIGA_HOST_INPUT && (command==9 || command==10)) {
        guest(c,data,22);
        if (command==9) {
            for (size_t i=0;i<c->input_handler_count;++i) if (c->input_handlers[i]==data) { handled=1; break; }
            if (!handled && c->input_handler_count<16) { c->input_handlers[c->input_handler_count++]=data; handled=1; }
        } else {
            for (size_t i=0;i<c->input_handler_count;++i) if (c->input_handlers[i]==data) {
                memmove(c->input_handlers+i,c->input_handlers+i+1,(c->input_handler_count-i-1)*sizeof *c->input_handlers);
                --c->input_handler_count; handled=1; break;
            }
        }
    } else if (id==AMIGA_HOST_KEYBOARD && command==10) {
        if (length>16) length=16;
        if (length) memcpy(guest(c,data,length),c->keyboard_matrix,length);
        write32(c,request+32,length); handled=1;
    } else if (id==AMIGA_HOST_GAMEPORT && command>=10 && command<=13) {
        unsigned unit=amiga_be32(io+24);
        if (unit>=2) return 0;
        if (command==10) *guest(c,data,1)=c->gameport_type[unit];
        if (command==11) c->gameport_type[unit]=*guest(c,data,1);
        if (command==12) memcpy(guest(c,data,8),c->gameport_trigger[unit],8);
        if (command==13) memcpy(c->gameport_trigger[unit],guest(c,data,8),8);
        handled=1;
    }
    if (!handled) {
        fprintf(stderr,"compat: unsupported %s command=%u request=%06X\n",amiga_host_library_name(id),command,request);
        return 0;
    }
    return io_complete(c,request,reply);
}
static int exec_call(AmigaHostCompat *c,unsigned offset) {
    char name[256];
    switch (offset) {
    case 300: {
        uint32_t task=REG_A[1]?REG_A[1]:read32(c,c->libraries[0]+AMIGA_EXEC_THIS_TASK);
        uint8_t *priority=guest(c,task+AMIGA_NODE_PRIORITY,1);
        int8_t previous=(int8_t)*priority; *priority=(uint8_t)REG_D[0];
        REG_D[0]=(uint32_t)(int32_t)previous; logic(REG_D[0]); break;
    }
    case 318: {
        uint32_t task=read32(c,c->libraries[0]+AMIGA_EXEC_THIS_TASK);
        uint32_t received=read32(c,task+AMIGA_TASK_SIGNALS_RECEIVED)&REG_D[0];
        if (!received) return wait_retry(c,1,REG_D[0]);
        write32(c,task+AMIGA_TASK_SIGNALS_RECEIVED,read32(c,task+AMIGA_TASK_SIGNALS_RECEIVED)&~received);
        REG_D[0]=received; logic(received); break;
    }
    case 384: {
        uint32_t head=read32(c,REG_A[0]+AMIGA_PORT_MESSAGES);
        if (!read32(c,head)) return wait_retry(c,2,REG_A[0]);
        REG_D[0]=head; logic(head); break;
    }
    case 198: REG_D[0]=amiga_host_alloc(c,REG_D[0],REG_D[1]); logic(REG_D[0]); break;
    case 210:
        if (!amiga_host_free(c,REG_A[1],REG_D[0])) return unsupported(0,offset);
        break;
    case 216: REG_D[0]=amiga_host_available(c,REG_D[1]); logic(REG_D[0]); break;
    case 354:
        guest(c,REG_A[1]+8,1)[0]=4;
        if (!amiga_host_add_tail(c,c->libraries[0]+AMIGA_EXEC_PORTS,REG_A[1])) return 0;
        break;
    case 360: if (!amiga_host_remove(c,REG_A[1])) return 0; break;
    case 366:
        guest(c,REG_A[1]+8,1)[0]=5; REG_PC=0xFC1B76; USE_CYCLES(4); return 2;
    case 408: case 552: case 498:
        if (!amiga_host_string(c,REG_A[1],name,sizeof name)) return unsupported(0,offset);
        REG_D[0]=amiga_host_library(c,name,offset==552?REG_D[0]:0);
        if (REG_D[0] && !strcmp(name,"graphics.library"))
            fa18_custom_write(fa18_machine,0x096,0x8240); /* CPU-controlled graphics owns master/blitter DMA. */
        logic(REG_D[0]); break;
    case 414: {
        uint8_t *p=guest(c,REG_A[1]+32,2); unsigned n=amiga_be16(p);
        if (n) { p[0]=(uint8_t)((n-1)>>8); p[1]=(uint8_t)(n-1); }
        break;
    }
    case 444: {
        if (!amiga_host_string(c,REG_A[0],name,sizeof name)) return 0;
        uint32_t base=amiga_host_library(c,name,0); unsigned id;
        for (id=AMIGA_HOST_TIMER;id<AMIGA_HOST_LIBRARY_COUNT;++id) if (base && base==c->libraries[id]) break;
        uint8_t *io=guest(c,REG_A[1],32);
        if (id==AMIGA_HOST_LIBRARY_COUNT || (id==AMIGA_HOST_GAMEPORT && REG_D[0]>1)) {
            io[31]=0xFF; REG_D[0]=0xFFFFFFFFu; break;
        }
        amiga_store_be32(io+20,base); amiga_store_be32(io+24,REG_D[0]); io[31]=0;
        REG_D[0]=0; logic(0); break;
    }
    case 450: {
        uint8_t *io=guest(c,REG_A[1],32); uint32_t base=amiga_be32(io+20);
        if (base) { uint8_t *p=guest(c,base+32,2); unsigned count=amiga_be16(p); if (count) { p[0]=(uint8_t)((count-1)>>8); p[1]=(uint8_t)(count-1); } }
        amiga_store_be32(io+20,0); amiga_store_be32(io+24,0); break;
    }
    case 456: case 462:
        if (offset==456 && c->synchronous_request==REG_A[1]) {
            if (guest(c,REG_A[1]+8,1)[0]!=7) return wait_retry(c,3,REG_A[1]);
            c->synchronous_request=0;
            if (read32(c,REG_A[1]+14) && !(guest(c,REG_A[1]+30,1)[0]&1)) amiga_host_remove(c,REG_A[1]);
            REG_D[0]=(uint32_t)(int32_t)(int8_t)*guest(c,REG_A[1]+31,1); logic(REG_D[0]); break;
        }
        if (offset==456) guest(c,REG_A[1]+30,1)[0]|=1;
        else guest(c,REG_A[1]+30,1)[0]&=(uint8_t)~1;
        if (!device_command(c,REG_A[1],offset==462)) return 0;
        if (offset==456 && guest(c,REG_A[1]+8,1)[0]!=7) {
            if (c->synchronous_request) return 0;
            c->synchronous_request=REG_A[1]; return wait_retry(c,3,REG_A[1]);
        }
        REG_D[0]=(uint32_t)(int32_t)(int8_t)*guest(c,REG_A[1]+31,1); logic(REG_D[0]); break;
    case 468: REG_D[0]=guest(c,REG_A[1]+8,1)[0]==7?REG_A[1]:0; logic(REG_D[0]); break;
    case 474:
        if (guest(c,REG_A[1]+8,1)[0]!=7) return wait_retry(c,3,REG_A[1]);
        if (read32(c,REG_A[1]+14) && !(guest(c,REG_A[1]+30,1)[0]&1)) amiga_host_remove(c,REG_A[1]);
        REG_D[0]=(uint32_t)(int32_t)(int8_t)*guest(c,REG_A[1]+31,1); logic(REG_D[0]); break;
    case 480:
        for (size_t i=0;i<c->pending_count;++i) if (c->pending[i].request==REG_A[1]) {
            memmove(c->pending+i,c->pending+i+1,(c->pending_count-i-1)*sizeof *c->pending); --c->pending_count;
            if (!io_complete(c,REG_A[1],1)) return 0;
            guest(c,REG_A[1]+31,1)[0]=(uint8_t)-2; break;
        }
        REG_D[0]=0; logic(0); break;
    case 534: {
        REG_D[0]=0;
        for (size_t i=0;i<c->memory.count;++i) {
            const AmigaGuestBank *b=&c->banks[i];
            if (REG_A[1]>=b->base && REG_A[1]-b->base<b->size)
                REG_D[0]=1|((b->attributes&AMIGA_MEMORY_CHIP)?2:4);
        }
        logic(REG_D[0]); break;
    }
    case 624: case 630:
        if (REG_D[0]) { memmove(guest(c,REG_A[1],REG_D[0]),guest(c,REG_A[0],REG_D[0]),REG_D[0]); fa18_recomp_note_write(REG_A[1],(int)REG_D[0]); }
        break;
    default: return forward_exec(offset)?2:0;
    }
    return 1;
}
static int dos_call(AmigaHostCompat *c,unsigned offset) {
    char name[256]; uint8_t *p; uint32_t handle;
    switch (offset) {
    case 30:
        if (!amiga_host_string(c,REG_D[1],name,sizeof name)) return 0;
        REG_D[0]=amiga_host_open(c,name,(int32_t)REG_D[2]); break;
    case 36: REG_D[0]=amiga_host_file_close(c,REG_D[1])?0xFFFFFFFFu:0; break;
    case 42: case 48:
        if ((int32_t)REG_D[3]<0) { c->error=115; REG_D[0]=0xFFFFFFFFu; break; }
        if (!REG_D[3]) { REG_D[0]=0; break; }
        p=guest(c,REG_D[2],REG_D[3]);
        if (offset==42) { REG_D[0]=(uint32_t)amiga_host_read(c,REG_D[1],p,(int32_t)REG_D[3]); if ((int32_t)REG_D[0]>0) fa18_recomp_note_write(REG_D[2],(int)REG_D[0]); }
        else REG_D[0]=(uint32_t)amiga_host_write(c,REG_D[1],p,(int32_t)REG_D[3]);
        break;
    case 66: REG_D[0]=(uint32_t)amiga_host_seek(c,REG_D[1],(int32_t)REG_D[2],(int32_t)REG_D[3]); break;
    case 84:
        if (!amiga_host_string(c,REG_D[1],name,sizeof name)) return 0;
        REG_D[0]=amiga_host_lock(c,name); break;
    case 90: if (!amiga_host_unlock(c,REG_D[1])) return 0; break;
    case 102: REG_D[0]=amiga_host_examine(c,REG_D[1],guest(c,REG_D[2],260),260)?0xFFFFFFFFu:0; fa18_recomp_note_write(REG_D[2],260); break;
    case 114: REG_D[0]=amiga_host_exnext(c,REG_D[1],guest(c,REG_D[2],260),260)?0xFFFFFFFFu:0; fa18_recomp_note_write(REG_D[2],260); break;
    case 126:
        handle=REG_D[1];
        if (handle && (handle>64 || !c->locks[handle-1].active)) { c->error=205; REG_D[0]=0; break; }
        REG_D[0]=c->current_directory; c->current_directory=handle;
        write32(c,read32(c,c->libraries[0]+AMIGA_EXEC_THIS_TASK)+AMIGA_PROCESS_CURRENT_DIR,handle); break;
    case 132: REG_D[0]=(uint32_t)c->error; break;
    default: return 0;
    }
    logic(REG_D[0]); return 1;
}
static void word(uint8_t *p,uint16_t value) { p[0]=(uint8_t)(value>>8); p[1]=(uint8_t)value; }
static int graphics_call(AmigaHostCompat *c,unsigned offset) {
    uint8_t *p; uint32_t size,address;
    switch (offset) {
    case 198: memset(guest(c,REG_A[1],100),0,100); break;
    case 204: memset(guest(c,REG_A[0],40),0,40); break;
    case 360:
        p=guest(c,REG_A[1],18); memset(p,0,18); word(p+12,16); word(p+14,129); break;
    case 390:
        p=guest(c,REG_A[0],40); memset(p,0,40);
        word(p,(uint16_t)((((uint16_t)REG_D[1]+15u)/16)*2)); word(p+2,(uint16_t)REG_D[2]); p[5]=(uint8_t)REG_D[0]; break;
    case 492:
        size=(((REG_D[0]&0xFFFF)+15)/16)*2*(REG_D[1]&0xFFFF);
        REG_D[0]=amiga_host_alloc(c,size,0x10002); logic(REG_D[0]); break;
    case 498:
        size=(((REG_D[0]&0xFFFF)+15)/16)*2*(REG_D[1]&0xFFFF);
        if (!amiga_host_free(c,REG_A[0],size)) return 0;
        break;
    case 570:
        if (REG_D[0]>256) { REG_D[0]=0; break; }
        size=8+REG_D[0]*2; address=amiga_host_alloc(c,size,0x10001);
        if (address) { p=guest(c,address,size); word(p+2,(uint16_t)REG_D[0]); amiga_store_be32(p+4,address+8); }
        REG_D[0]=address; logic(address); break;
    case 576:
        p=guest(c,REG_A[0],8); if (!amiga_host_free(c,REG_A[0],8+amiga_be16(p+2)*2)) return 0; break;
    case 582:
        p=guest(c,REG_A[0],8); if (REG_D[0]>=amiga_be16(p+2)) { REG_D[0]=0xFFFFFFFFu; break; }
        REG_D[0]=amiga_be16(guest(c,amiga_be32(p+4)+REG_D[0]*2,2)); logic(REG_D[0]); break;
    case 192:
        if (!amiga_host_load_rgb4(c,REG_A[0],REG_A[1],(uint16_t)REG_D[0])) return 0;
        break;
    case 216: REG_D[0]=amiga_host_make_viewport(c,REG_A[0],REG_A[1])?0:1; logic(REG_D[0]); break;
    case 210: REG_D[0]=amiga_host_merge_view(c,REG_A[1])?0:1; logic(REG_D[0]); break;
    case 222:
        write32(c,REG_A[6]+34,REG_A[1]);
        if (REG_A[1]) {
            uint32_t header=read32(c,REG_A[1]+4); if (!header) return 0;
            address=read32(c,header+4); write32(c,REG_A[6]+50,address); write32(c,REG_A[6]+54,address);
            fa18_custom_write(fa18_machine,0x080,(uint16_t)(address>>16)); fa18_custom_write(fa18_machine,0x082,(uint16_t)address);
            /* Publish for the next vertical blank. Rewinding the live Copper
             * during each game loop repeatedly draws its first text line. */
            fa18_custom_write(fa18_machine,0x096,0x83C0);
        } else { fa18_custom_write(fa18_machine,0x096,0x0180); }
        break;
    case 540:
        p=guest(c,REG_A[0],40);
        for (unsigned i=8;i<=16;i+=4) { address=amiga_be32(p+i); if (address && !amiga_host_free_copper(c,address)) return 0; amiga_store_be32(p+i,0); }
        break;
    case 546: case 564: if (!amiga_host_free_copper(c,REG_A[0])) return 0; break;
    case 228: REG_PC=0xFC5A58; USE_CYCLES(4); return 2;
    case 384: REG_PC=0xFC5ECE; USE_CYCLES(4); return 2;
    case 402: REG_PC=0xFC5E58; USE_CYCLES(4); return 2;
    case 408: {
        int index=(int16_t)REG_D[0];
        if (index<0) for (index=0;index<8 && (c->sprite_allocated&(1u<<index));++index) {}
        if (index>=8 || (c->sprite_allocated&(1u<<index))) REG_D[0]=0xFFFFFFFFu;
        else { c->sprite_allocated|=(uint8_t)(1u<<index); word(guest(c,REG_A[0]+10,2),(uint16_t)index); REG_D[0]=(uint32_t)index; }
        logic(REG_D[0]); break;
    }
    case 414:
        if (REG_D[0]>7) return 0;
        c->sprite_allocated&=(uint8_t)~(1u<<REG_D[0]);
        fa18_custom_write(fa18_machine,0x120+4*REG_D[0],0); fa18_custom_write(fa18_machine,0x122+4*REG_D[0],0); break;
    case 456: {
        p=guest(c,REG_A[6]+0xAA,2); word(p,(uint16_t)(amiga_be16(p)+1)); break;
    }
    case 462: {
        p=guest(c,REG_A[6]+0xAA,2); if (amiga_be16(p)) word(p,(uint16_t)(amiga_be16(p)-1)); break;
    }
    default: return 0;
    }
    return 1;
}
static int dispatch(void *context) {
    AmigaHostCompat *c=context; uint32_t pc=REG_PC;
    unsigned id=(pc-AMIGA_HOST_SERVICE_BASE)/0x400,slot=(pc-AMIGA_HOST_SERVICE_BASE)%0x400;
    if ((slot&1) || id>=AMIGA_HOST_LIBRARY_COUNT || !slot) return unsupported(id<AMIGA_HOST_LIBRARY_COUNT?id:0,slot);
    unsigned offset=(slot/2)*6;
    fa18_machine->runtime_guard.service.name=amiga_host_library_name(id);
    int traced=trace_left && !(id==AMIGA_HOST_DOS && offset==42) && !(id==AMIGA_HOST_EXEC && offset==36) &&
        !(id==AMIGA_HOST_GRAPHICS && (offset==384 || offset==228));
    if (traced) { --trace_left; fprintf(stderr,"HOST %s -%u pc=%06X d0=%08X d1=%08X d2=%08X d3=%08X a0=%06X a1=%06X a6=%06X\n",
        amiga_host_library_name(id),offset,REG_PPC,REG_D[0],REG_D[1],REG_D[2],REG_D[3],REG_A[0],REG_A[1],REG_A[6]); }
    int result=id==AMIGA_HOST_EXEC?exec_call(c,offset):id==AMIGA_HOST_DOS?dos_call(c,offset):
        id==AMIGA_HOST_GRAPHICS?graphics_call(c,offset):0;
    if (id==AMIGA_HOST_INTUITION && offset==78) {
        /* The host owns the window; there is no emulated desktop to reclaim. */
        c->desktop_hidden=1; REG_D[0]=1; logic(1); result=1;
    }
    if (!result) return unsupported(id,offset);
    if (traced && id==AMIGA_HOST_GRAPHICS && (offset==210 || offset==216))
        fprintf(stderr,"HOST result=%u\n",REG_D[0]);
    return result==2?1:returned(pc);
}
static int exited(void *context) {
    AmigaHostCompat *c=context; c->exited=1; c->exit_code=(int32_t)REG_D[0];
    CPU_STOPPED|=STOP_LEVEL_STOP; SET_CYCLES(0); m68k_yield_from_instruction_hook(); return 1;
}
static int exception(void *context) {
    (void)context;
    fprintf(stderr,"compat: unhandled CPU exception vector=%u pc=%06X saved_pc=%06X sr=%04X\n",
        (REG_PC-0xEF4000)/2,REG_PPC,fa18_bus_read32(REG_A[7]+2),fa18_bus_read16(REG_A[7]));
    return unsupported(0,(REG_PC-0xEF4000)/2);
}
int fa18_os_host_compat_install(AmigaHostCompat *c,const AmigaLibraryVector *vectors,size_t count) {
    if (!c || !vectors || !count || !c->libraries[0]) return 0;
    AmigaService entries[]={
        {AMIGA_HOST_SERVICE_BASE,AMIGA_HOST_SERVICE_BASE+AMIGA_HOST_LIBRARY_COUNT*0x400,AMIGA_HOST_SERVICE_BASE,"host.compatibility",1,dispatch,c},
        {0xFF446E,0xFF4470,0xFF446E,"host.process_exit",1,exited,c},
        {0xEF4000,0xEF4100,0xEF4000,"host.cpu_exception",1,exception,c}};
    if (!fa18_services_install_extra(entries,3)) return 0;
    for (size_t i=0;i<count;++i) {
        uint8_t *p=amiga_guest_range(&c->memory,c->libraries[0]-vectors[i].offset,6);
        if (!p) return 0;
        amiga_store_be32(p+2,AMIGA_HOST_SERVICE_BASE+(vectors[i].offset/6)*2);
    }
    original_vectors=vectors; vector_count=count; active=c;
    const char *trace=getenv("FA18_HOST_TRACE"); trace_left=trace?(unsigned)strtoul(trace,NULL,10):0;
    return 1;
}
void fa18_os_host_compat_detach(void) { active=NULL; original_vectors=NULL; vector_count=0; }
int fa18_os_host_exited(void) { return active && active->exited; }
