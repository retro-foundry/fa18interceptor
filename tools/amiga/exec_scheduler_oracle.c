/* Complete calls use controlled guest task/callback code, original kernel
 * services on the reference side and cleared ROM/rtarea on the C side. */
#define main structural_phase_main
#include "service_phase_oracle.c"
#undef main
#include "../../port/amiga/abi_13.h"
#include "../../port/os/service_dispatch_adapter.h"
enum { EXEC=0xC63000, OLD=0xC64000, NEW=0xC64100, CALLBACK=0xC64600,
       LAUNCH=0xC64640, EXCEPT=0xC64680, WAKE=0xC64700, PORT=0xC64800,
       MESSAGE=0xC64900, RETURN=0xC10000, USER_SP=0xC7F800,
       NEW_SP=0xC7F000, SUPER_SP=0xC7FC00 };
static void vector(unsigned offset,uint32_t target) {
    wr_u16(EXEC-offset,0x4EF9); wr_u32(EXEC-offset+2,target);
}
static void ready(uint32_t node) {
    uint32_t list=EXEC+AMIGA_EXEC_TASK_READY;
    wr_u32(list,node?node:list+4); wr_u32(list+4,0); wr_u32(list+8,node?node:list);
    if (node) { wr_u32(node,list+4); wr_u32(node+4,list); }
}
static void saved_task(uint32_t task,uint32_t sp,uint32_t pc,unsigned n) {
    wr_u32(task+AMIGA_TASK_SAVED_SP,sp-66); wr_u32(sp-66,pc); wr_u16(sp-62,n&31);
    for (unsigned i=0;i<15;++i) wr_u32(sp-60+i*4,REG_DA[i]);
    wr_u32(sp-60+14*4,EXEC); wr_u16(task+AMIGA_TASK_NEST_COUNTS,0xFFFF);
    wr_u8(task+AMIGA_TASK_STATE,3); wr_u8(task+AMIGA_NODE_PRIORITY,0);
    wr_u32(task+AMIGA_TASK_LAUNCH,LAUNCH); wr_u32(task+AMIGA_TASK_SWITCH,CALLBACK);
    wr_u32(task+AMIGA_TASK_EXCEPT_CODE,EXCEPT); wr_u32(task+AMIGA_TASK_EXCEPT_DATA,EXCEPT+0x40);
    wr_u32(task+AMIGA_TASK_SIGNALS_RECEIVED,0x80000005); wr_u32(task+AMIGA_TASK_SIGNALS_EXCEPT,0x80000001);
}
static void callback(uint32_t code,uint32_t marker) {
    /* ADDQ.L #1,marker; RTS. Preserves the callback's required A3/A4/A6. */
    wr_u16(code,0x52B9); wr_u32(code+2,marker); wr_u16(code+6,0x4E75);
}
static void scheduler_fixture(unsigned kind,unsigned n) {
    fa18_write_log_active=0; CPU_STOPPED=0;
    fa18_machine->intreq=fa18_machine->intena=0; m68k_set_irq(0);
    memset(fa18_machine->slow+EXEC-FA18_SLOW_BASE-0x200,0,0x1C00);
    for (unsigned i=0;i<16;++i) REG_DA[i]=random_word();
    REG_A[6]=EXEC;
    m68k_set_reg(M68K_REG_SR,0x2700); m68k_set_reg(M68K_REG_USP,USER_SP);
    m68k_set_reg(M68K_REG_ISP,SUPER_SP);
    wr_u32(4,EXEC); wr_u32(0x20,0xFC090E);
    vector(30,0xFC08E6); vector(42,0xFC0EC2); vector(48,0xFC1F74);
    vector(54,0xFC0F1C); vector(324,0xFC1E84); vector(318,0xFC1F0C);
    vector(366,0xFC1B76);
    wr_u32(EXEC+AMIGA_EXEC_THIS_TASK,OLD); wr_u16(EXEC+AMIGA_EXEC_ID_NEST_CNT,0xFFFF);
    uint32_t waiting=EXEC+AMIGA_EXEC_TASK_WAIT;
    wr_u32(waiting,waiting+4); wr_u32(waiting+4,0); wr_u32(waiting+8,waiting);
    wr_u16(EXEC+AMIGA_EXEC_QUANTUM,0x1234); wr_u32(EXEC+AMIGA_EXEC_IDLE_COUNT,0x7FFFFFFF);
    wr_u32(EXEC+AMIGA_EXEC_DISPATCH_COUNT,0xFFFFFFFF);
    callback(CALLBACK,CALLBACK+0x200); callback(LAUNCH,CALLBACK+0x204);
    callback(EXCEPT,CALLBACK+0x208);
    /* Returned exception mask is a subset of the delivered mask. */
    wr_u16(EXCEPT+6,0x0280); wr_u32(EXCEPT+8,0x80000000); wr_u16(EXCEPT+12,0x4E75);
    saved_task(NEW,NEW_SP,RETURN,n); saved_task(OLD,USER_SP,RETURN,n);
    wr_u8(OLD+AMIGA_TASK_STATE,2); wr_u8(OLD+AMIGA_NODE_PRIORITY,10);
    wr_u32(USER_SP,RETURN); wr_u32(NEW_SP,RETURN);
    unsigned flags=kind==2?0x40:kind==8?0x80:kind>=9&&kind<=11?0x20:kind==12?0xA0:0;
    wr_u8(OLD+AMIGA_TASK_FLAGS,(uint8_t)flags); wr_u8(NEW+AMIGA_TASK_FLAGS,(uint8_t)flags);
    if (kind==10) { wr_u16(NEW+AMIGA_TASK_NEST_COUNTS,0x0001); }
    if (kind==11) { wr_u32(NEW+AMIGA_TASK_SIGNALS_RECEIVED,0); }
    ready(kind==0||kind==2?OLD:NEW);
    if (kind==3 || kind==4 || kind==6 || kind==7) ready(0);
    if (kind==4) ready(NEW); /* Lower priority, no quantum request. */
    if (kind==5) wr_u8(EXEC+AMIGA_EXEC_RESCHEDULE_FLAGS,0x40);
    if (kind==13 || (kind>=8 && kind<=12)) wr_u8(NEW+AMIGA_NODE_PRIORITY,20);
    if (kind<=2) {
        /* Switch enters with a supervisor exception frame and an untouched USP. */
        wr_u16(SUPER_SP,n&31); wr_u32(SUPER_SP+2,RETURN); REG_PC=0xFC0F1C;
    } else if (kind==6 || kind==7) {
        REG_PC=0xFC0E9C;
        for (unsigned i=0;i<6;++i) wr_u32(SUPER_SP+4*i,REG_DA[i]);
        wr_u16(SUPER_SP+24,(kind==6?0x2000:0)|(n&31)); wr_u32(SUPER_SP+26,RETURN);
    } else if (kind==14 || kind==15) {
        ready(NEW); saved_task(NEW,NEW_SP,WAKE,n);
        m68ki_set_sr_noint(n&31); wr_u32(OLD+AMIGA_TASK_SIGNALS_RECEIVED,0);
        wr_u32(OLD+AMIGA_TASK_SIGNALS_EXCEPT,0); wr_u32(NEW+AMIGA_TASK_SIGNALS_EXCEPT,0);
        uint32_t w=WAKE;
        wr_u16(w,0x2C78); wr_u16(w+2,4); w+=4;
        if (kind==14) {
            wr_u16(w,0x227C); wr_u32(w+2,OLD); w+=6;
            wr_u16(w,0x7001); w+=2; wr_u16(w,0x4EAE); wr_u16(w+2,(uint16_t)-324);
            REG_D[0]=1; REG_PC=0xFC1F0C;
        } else {
            uint32_t list=PORT+AMIGA_PORT_MESSAGES;
            wr_u32(list,list+4); wr_u32(list+4,0); wr_u32(list+8,list);
            wr_u8(PORT+AMIGA_PORT_SIGNAL_BIT,0); wr_u32(PORT+AMIGA_PORT_SIGNAL_TASK,OLD);
            wr_u16(w,0x207C); wr_u32(w+2,PORT); w+=6;
            wr_u16(w,0x227C); wr_u32(w+2,MESSAGE); w+=6;
            wr_u16(w,0x4EAE); wr_u16(w+2,(uint16_t)-366);
            REG_A[0]=PORT; REG_PC=0xFC1C32;
        }
        /* PutMsg protects its nested Signal; deliver the deferred reschedule
         * through its actual public service after PutMsg releases protection. */
        if (kind==15) { wr_u16(w+4,0x4EAE); wr_u16(w+6,(uint16_t)-48); w+=4; }
        wr_u16(w+4,0x60FE);
    } else {
        wr_u16(SUPER_SP,n&31); wr_u32(SUPER_SP+2,RETURN); REG_PC=0xFC0EC2;
    }
    if (kind<14) m68k_set_reg(M68K_REG_SR,0x2700|(n&31));
    SET_CYCLES(100000000); fa18_cycle_origin=100000000; fa18_next_event=INT64_MAX;
    dma_offset=40+(n%32)*4; reset_bus(); fa18_write_log_active=1;
}
static unsigned active_kind,active_fixture;
static unsigned execute_scheduler(int candidate) {
    unsigned count=0;
    while (REG_PC!=RETURN) {
        if (++count>500 || CPU_STOPPED) {
            fprintf(stderr,"scheduler did not return kind=%u n=%u candidate=%d PC=%06X SP=%06X stopped=%u\n",active_kind,active_fixture,candidate,REG_PC,REG_A[7],CPU_STOPPED); exit(1);
        }
        if (candidate && (fa18_os_exec_scheduler_step() || fa18_os_exec_supervisor_step() ||
            fa18_os_exec_lists_step() || fa18_os_exec_messages_step() || fa18_os_exec_signals_step() ||
            fa18_os_exec_task_protection_step())) continue;
        fa18_machine_require_supported_target(REG_PPC,REG_PC);
        uint32_t pc=REG_PC; uint16_t op=fa18_bus_read16(pc);
        fa18_bus_begin(pc); fa18_bus_fetch(pc); REG_PPC=pc; REG_IR=op; REG_PC=pc+2;
        m68ki_instruction_jump_table[op](); USE_CYCLES(CYC_INSTRUCTION[op]);
    }
    fa18_bus_finish(REG_PC); return count;
}
static int contract(unsigned kind) {
    if (kind==14) return REG_D[0]==1 && rd_u32(EXEC+AMIGA_EXEC_THIS_TASK)==OLD &&
        rd_u8(OLD+AMIGA_TASK_STATE)==2 && rd_u32(OLD+AMIGA_TASK_SIGNALS_RECEIVED)==0;
    if (kind==15) return REG_D[0]==MESSAGE && rd_u32(EXEC+AMIGA_EXEC_THIS_TASK)==OLD;
    if (kind==0 || kind==2) return rd_u32(EXEC+AMIGA_EXEC_THIS_TASK)==OLD &&
        REG_A[7]==USER_SP && rd_u32(OLD+AMIGA_TASK_SAVED_SP)==USER_SP-66 &&
        rd_u32(CALLBACK+0x200)==(kind==2);
    if (kind==3 || kind==4 || kind==5 || kind==6 || kind==7) return rd_u32(EXEC+AMIGA_EXEC_THIS_TASK)==OLD;
    return rd_u32(EXEC+AMIGA_EXEC_THIS_TASK)==NEW && REG_A[7]==NEW_SP &&
        rd_u32(EXEC+AMIGA_EXEC_DISPATCH_COUNT)==0 &&
        rd_u16(EXEC+AMIGA_EXEC_QUANTUM_REMAINING)==0x1234 && rd_u8(NEW+AMIGA_TASK_STATE)==2 &&
        rd_u32(CALLBACK+0x204)==(kind==8||kind==12) &&
        rd_u32(CALLBACK+0x208)==(kind>=9&&kind<=12);
}
#ifndef AMIGA_SCHEDULER_NO_MAIN
#include "../../build/amiga/scheduler_captures.h"
static int captured_contracts(FA18Machine *m,const uint8_t *rom,size_t nr) {
    char path[256],error[256]; size_t size;
    FA18Machine *before=malloc(sizeof *before);
    uint8_t *cpu_state=malloc(m68k_context_size());
    if (!before || !cpu_state) return 0;
    for (unsigned k=0;k<sizeof scheduler_captures/sizeof scheduler_captures[0];++k) {
        const char *directory=scheduler_captures[k].directory;
        snprintf(path,sizeof path,"%s/entry_state.bin",directory);
        uint8_t *state=read_file(path,&size);
        if (!state || !fa18_machine_load_state(m,state,size,rom,nr,error,sizeof error)) return 0;
        free(state); fa18_bus_timing=0;
        SET_CYCLES(100000000); fa18_cycle_origin=100000000; fa18_next_event=INT64_MAX;
        memcpy(before,m,sizeof *m); m68k_get_context(cpu_state);
        int reference_cycles=0; uint32_t reference_usp=0,reference_isp=0;
        for (unsigned side=0;side<2;++side) {
            memcpy(m,before,sizeof *m); m68k_set_context(cpu_state); SET_CYCLES(100000000);
            if (side) {
                memset(m->rom,0,sizeof m->rom); memset(m->rtarea,0,sizeof m->rtarea); fa18_machine_require_romfree(m);
            }
            fa18_bus_reset(); amiga_phase_observe_begin(); unsigned count=0;
            while (REG_PC!=scheduler_captures[k].pc) {
                if (++count>100) return 0;
                if (side && fa18_os_exec_scheduler_step()) continue;
                fa18_machine_require_supported_target(REG_PPC,REG_PC);
                uint32_t pc=REG_PC; uint16_t op=fa18_bus_read16(pc);
                fa18_bus_begin(pc); fa18_bus_fetch(pc); REG_PPC=pc; REG_IR=op; REG_PC=pc+2;
                m68ki_instruction_jump_table[op](); USE_CYCLES(CYC_INSTRUCTION[op]);
            }
            fa18_bus_finish(REG_PC);
            if (count!=scheduler_captures[k].count || memcmp(REG_DA,scheduler_captures[k].regs,sizeof scheduler_captures[k].regs) ||
                m68ki_get_sr()!=scheduler_captures[k].sr) {
                fprintf(stderr,"captured scheduler checkpoint differs %s side=%u count=%u SR=%04X\n",directory,side,count,m68ki_get_sr()); return 0;
            }
            for (unsigned bank=0;bank<2;++bank) {
                snprintf(path,sizeof path,"%s/checkpoint_%s.bin",directory,bank?"slow":"chip");
                uint8_t *expected=read_file(path,&size);
                if (!expected || size!=FA18_CHIP_SIZE || memcmp(expected,bank?m->slow:m->chip,size)) {
                    fprintf(stderr,"captured scheduler RAM differs %s side=%u bank=%u\n",directory,side,bank); return 0;
                }
                free(expected);
            }
            if (!side) {
                reference_cycles=GET_CYCLES(); reference_usp=m68k_get_reg(NULL,M68K_REG_USP);
                reference_isp=m68k_get_reg(NULL,M68K_REG_ISP); amiga_phase_observe_reference();
            }
            else if (reference_cycles!=GET_CYCLES() || reference_usp!=m68k_get_reg(NULL,M68K_REG_USP) ||
                reference_isp!=m68k_get_reg(NULL,M68K_REG_ISP) || !amiga_phase_observe_compare() || m->runtime_guard.rom_reads ||
                m->runtime_guard.rom_instruction_fetches || m->runtime_guard.unsupported_services) return 0;
            printf("captured %s side=%u: %u instructions, %u CPU cycles /%u native OCS clocks (Engine9000 %u); registers/full SR/RAM match\n",
                directory,side,count,100000000-GET_CYCLES(),(100000000-GET_CYCLES())/2,scheduler_captures[k].cycles);
        }
    }
    free(cpu_state); free(before); return 1;
}
int main(void) {
    size_t ns,nr; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&ns),*rom=read_file("local/system/kick13.rom",&nr);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *ram=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE),*cpu_state=malloc(m68k_context_size());
    if (!state || !rom || !m || !base || !before || !ram || !cpu_state) return 1;
    if (!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error) ||
        !fa18_os_exec_scheduler_signature_matches(m->rom)) return 1;
    free(state); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL);
    unsigned matched=0;
    for (unsigned bus=0;bus<2;++bus) {
        fa18_bus_timing=(int)bus;
        for (unsigned kind=0;kind<16;++kind) for (unsigned n=0;n<256;++n) {
            active_kind=kind; active_fixture=n;
            memcpy(m,base,sizeof *m); scheduler_fixture(kind,n);
            memcpy(before,m,sizeof *m); m68k_get_context(cpu_state); amiga_phase_observe_begin();
            unsigned want_count=execute_scheduler(0);
            if (!contract(kind)) { fprintf(stderr,"original scheduler contract kind=%u n=%u PC=%06X SP=%06X task=%06X\n",kind,n,REG_PC,REG_A[7],rd_u32(EXEC+AMIGA_EXEC_THIS_TASK)); return 1; }
            uint32_t regs[16]; memcpy(regs,REG_DA,sizeof regs);
            uint32_t sr=m68ki_get_sr(),usp=m68k_get_reg(NULL,M68K_REG_USP),isp=m68k_get_reg(NULL,M68K_REG_ISP);
            int cycles=GET_CYCLES();
            memcpy(ram,m->chip,FA18_CHIP_SIZE); memcpy(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
            amiga_phase_observe_reference();
            memcpy(m,before,sizeof *m); m68k_set_context(cpu_state); SET_CYCLES(100000000);
            memset(m->rom,0,sizeof m->rom); memset(m->rtarea,0,sizeof m->rtarea);
            fa18_machine_require_romfree(m); reset_bus(); amiga_phase_observe_begin();
            unsigned got_count=execute_scheduler(1);
            if (got_count!=want_count || !contract(kind) || memcmp(regs,REG_DA,sizeof regs) ||
                sr!=m68ki_get_sr() || usp!=m68k_get_reg(NULL,M68K_REG_USP) || isp!=m68k_get_reg(NULL,M68K_REG_ISP) ||
                cycles!=GET_CYCLES() || memcmp(ram,m->chip,FA18_CHIP_SIZE) ||
                memcmp(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE) || !amiga_phase_observe_compare() ||
                m->runtime_guard.rom_reads || m->runtime_guard.rom_instruction_fetches || m->runtime_guard.unsupported_services) {
                fprintf(stderr,"scheduler differs bus=%u kind=%u fixture=%u cycles=%d/%d\n",bus,kind,n,cycles,GET_CYCLES()); return 1;
            }
            ++matched;
        }
    }
    printf("Exec scheduler: %u complete CPU/DMA calls; switches, dispatch, interrupt exit, switch/launch/user exception callbacks, blocking Wait/WaitPort; registers/SR/stack banks/RAM/accesses/cycles match with ROM removed\n",matched);
    if (!captured_contracts(m,rom,nr)) return 1;
    free(rom);
    free(cpu_state); free(ram); free(before); free(base); free(m); return 0;
}

#endif
