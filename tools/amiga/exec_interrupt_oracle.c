/* Whole original calls and real guest callbacks. The candidate has its ROM
 * and expansion ROM cleared. Fixtures build explicit guest structures; they
 * never use captured RAM to initialize a runtime. */
#define main structural_phase_main
#include "service_phase_oracle.c"
#undef main
#include "../../port/amiga/abi_13.h"
#include "../../port/os/service_dispatch_adapter.h"
#include "../../build/amiga/interrupt_captures.h"
enum { EXEC=0xC63000,TASK_NODE=0xC63800,SERVER_LIST=0xC64000,NODES=0xC65000,
       START=0xC68000,JOURNAL=0xC69000,COUNTS=0xC69400,JOURNAL_PTR=0xC69500,
       RETURN=0xC10000,USER_SP=0xC7F800,SUPER_SP=0xC7FC00 };
static uint32_t node(unsigned i) { return NODES+0x100*i; }
static uint32_t code(unsigned i) { return node(i)+0x40; }
static void empty(uint32_t list) {
    wr_u32(list,list+4); wr_u32(list+4,0); wr_u32(list+8,list);
}
static void append(uint32_t list,uint32_t item) {
    uint32_t last=rd_u32(list+8);
    wr_u32(item,list+4); wr_u32(item+4,last); wr_u32(last,item); wr_u32(list+8,item);
}
static uint32_t emit_word(uint32_t p,uint16_t w) { wr_u16(p,w); return p+2; }
static uint32_t emit_long(uint32_t p,uint32_t v) { wr_u32(p,v); return p+4; }
static uint32_t call(uint32_t p,uint32_t target,uint32_t item) {
    p=emit_word(p,0x227C); p=emit_long(p,item);
    p=emit_word(p,0x4EB9); return emit_long(p,target);
}
static void callback(unsigned id,int claimed,unsigned mask,int requeue) {
    uint32_t p=code(id);
    p=emit_word(p,0x52B9); p=emit_long(p,COUNTS+id*4);
    p=emit_word(p,0x2079); p=emit_long(p,JOURNAL_PTR);
    p=emit_word(p,0x20BC); p=emit_long(p,id);
    p=emit_word(p,0x58B9); p=emit_long(p,JOURNAL_PTR);
    if (mask) { p=emit_word(p,0x33FC); p=emit_word(p,(uint16_t)mask); p=emit_long(p,0xDFF09C); }
    if (requeue>=0) {
        /* Only the first callback invocation requeues. Cause itself changes
         * node type, queues it and requests the actual software interrupt. */
        p=emit_word(p,0x0CB9); p=emit_long(p,1); p=emit_long(p,COUNTS+id*4);
        uint32_t branch=p; p=emit_word(p,0x6600);
        uint32_t operand=p; p=emit_word(p,0);
        p=call(p,0xFC135C,node((unsigned)requeue));
        wr_u16(branch+2,(uint16_t)(p-operand));
    }
    p=emit_word(p,(uint16_t)(0x7000|!!claimed)); emit_word(p,0x4E75);
    wr_u32(node(id)+AMIGA_INTERRUPT_DATA,id); wr_u32(node(id)+AMIGA_INTERRUPT_CODE,code(id));
    wr_u8(node(id)+AMIGA_NODE_TYPE,2);
}
static unsigned kind_now,fixture_now,nested_delivered;
static unsigned irq_level(unsigned kind) { return 1+(kind-17)%7; }
static uint16_t level_bits(unsigned level) {
    static const uint16_t masks[]={0,7,8,0x70,0x780,0x1800,0x6000,0};
    return masks[level];
}
static void interrupt_fixture(unsigned kind,unsigned n) {
    static const uint8_t depths[]={0xFF,0,1,0x7F,0x80,0xFE,3,0xFF};
    fa18_write_log_active=0; CPU_STOPPED=0; nested_delivered=0;
    fa18_machine->intena=fa18_machine->intreq=0; m68k_set_irq(0);
    memset(fa18_machine->slow+EXEC-FA18_SLOW_BASE-0x200,0,0x7000);
    for (unsigned i=0;i<16;++i) REG_DA[i]=random_word();
    m68ki_set_sr_noint(0x2700|(n&31)); m68k_set_reg(M68K_REG_USP,USER_SP); m68k_set_reg(M68K_REG_ISP,SUPER_SP);
    REG_A[6]=EXEC; wr_u32(4,EXEC); wr_u32(SUPER_SP,RETURN);
    wr_u16(EXEC-36,0x4EF9); wr_u32(EXEC-34,0xFC0E9C);
    wr_u32(EXEC+AMIGA_EXEC_THIS_TASK,TASK_NODE); wr_u16(EXEC+AMIGA_EXEC_ID_NEST_CNT,0xFFFF);
    wr_u8(TASK_NODE+AMIGA_TASK_STATE,2); empty(EXEC+AMIGA_EXEC_TASK_READY); empty(EXEC+AMIGA_EXEC_TASK_WAIT);
    wr_u32(JOURNAL_PTR,JOURNAL); empty(SERVER_LIST); wr_u16(SERVER_LIST+18,0x20);
    for (unsigned i=0;i<16;++i) {
        callback(i,0,0,-1);
        uint32_t vector=EXEC+AMIGA_EXEC_INT_VECTORS+AMIGA_INT_VECTOR_BYTES*i;
        wr_u32(vector,i); wr_u32(vector+4,code(i)); wr_u32(vector+8,node(i));
    }
    for (unsigned i=0;i<5;++i) empty(EXEC+AMIGA_EXEC_SOFT_INTS+16*i);
    unsigned number=(n/16)%16;
    if (kind<6) {
        wr_u8(EXEC+AMIGA_EXEC_ID_NEST_CNT,depths[n/32]);
        REG_D[0]=(random_word()&0xFFFF0000u)|number;
        wr_u32(EXEC+AMIGA_EXEC_INT_VECTORS+12*number,SERVER_LIST);
        if (kind>=3) { append(SERVER_LIST,node(1)); wr_u8(node(1)+AMIGA_NODE_PRIORITY,0); }
        if (kind==4) REG_A[1]=node(1);
        else if (kind==5) { append(SERVER_LIST,node(2)); append(SERVER_LIST,node(3)); REG_A[1]=node(2); }
        else REG_A[1]=kind==1?0:node(0);
        if (kind==3) wr_u8(node(0)+AMIGA_NODE_PRIORITY,(n/32)&1?0:0x80);
        fa18_machine->intena=0x7FFF;
        REG_PC=kind<2?0xFC11CA:kind<4?0xFC1210:0xFC1250;
    } else if (kind<10) {
        unsigned count=kind==6?0:3;
        for (unsigned i=0;i<count;++i) {
            callback(i,(kind==8&&i==0)||(kind==9&&i==1),0,-1); append(SERVER_LIST,node(i));
        }
        REG_A[1]=SERVER_LIST; REG_PC=0xFC1338; fa18_machine->intreq=0x20;
    } else if (kind<12) {
        int priority=((int)(n/32%5)-2)*16+(int)(n&15);
        wr_u8(node(0)+AMIGA_NODE_PRIORITY,(uint8_t)priority);
        wr_u8(EXEC+AMIGA_EXEC_ID_NEST_CNT,depths[n/32]);
        if (kind==11) {
            wr_u8(node(0)+AMIGA_NODE_TYPE,11);
            append(EXEC+AMIGA_EXEC_SOFT_INTS+(priority+32)/16*16,node(0));
        }
        REG_A[1]=node(0); REG_PC=0xFC135C; fa18_machine->intena=0x7FFF;
    } else if (kind<17 || kind==38) {
        fa18_machine->intena=0x7FFF;
        if (kind!=12) { wr_u8(EXEC+AMIGA_EXEC_RESCHEDULE_FLAGS,32); fa18_machine->intreq=4; }
        if (kind==14) {
            for (unsigned i=0;i<10;++i) {
                wr_u8(node(i)+AMIGA_NODE_TYPE,11); append(EXEC+AMIGA_EXEC_SOFT_INTS+16*(i/2),node(i));
            }
        } else if (kind==15 || kind==16 || kind==38) {
            wr_u8(node(0)+AMIGA_NODE_PRIORITY,0xE0); wr_u8(node(1)+AMIGA_NODE_PRIORITY,0xE0);
            wr_u8(node(0)+AMIGA_NODE_TYPE,11); append(EXEC+AMIGA_EXEC_SOFT_INTS,node(0));
            wr_u8(node(1)+AMIGA_NODE_TYPE,11); append(EXEC+AMIGA_EXEC_SOFT_INTS,node(1));
            wr_u8(node(2)+AMIGA_NODE_PRIORITY,32);
            callback(0,0,0,kind==15?2:kind==16?0:-1);
            if (kind==38) {
                callback(5,0,0x20,-1);
                wr_u32(0x6C,0xFC0D14);
            }
        }
        REG_PC=0xFC13BC;
    } else {
        unsigned level=irq_level(kind); uint16_t mask=level_bits(level);
        for (unsigned i=0;i<15;++i) if (mask&(1u<<i)) callback(i,0,1u<<i,-1);
        /* CPU interrupt entry already supplied this six-byte frame. Both
         * user and supervisor returns, all CCRs and all levels are exercised. */
        wr_u16(SUPER_SP,((n&32)?0x2000:0)|(n&31)); wr_u32(SUPER_SP+2,RETURN);
        m68ki_set_sr_noint(0x2000|(level<<8)|(n&31));
        fa18_machine->intena=(uint16_t)((mask&0x3FFF)|(kind>=24&&kind<31?0:0x4000));
        fa18_machine->intreq=kind>=31?mask:mask?(uint16_t)(mask&-(int16_t)mask):0;
        static const uint32_t entries[]={0,0xFC0C8E,0xFC0CE2,0xFC0D14,0xFC0D6C,0xFC0DFA,0xFC0E40,0xFC0E86};
        for (unsigned i=1;i<=7;++i) wr_u32(0x60+i*4,entries[i]);
        REG_PC=entries[level];
    }
    SET_CYCLES(100000000); fa18_cycle_origin=100000000; fa18_next_event=INT64_MAX;
    dma_offset=40+(n%32)*4; reset_bus(); fa18_write_log_active=0;
}
static int candidate_step(void) {
    return fa18_os_exec_irq_roots_step() || fa18_os_exec_int_servers_step() ||
        fa18_os_exec_soft_interrupts_step() || fa18_os_exec_scheduler_step() || fa18_os_exec_lists_step();
}
static unsigned execute_interrupt(int candidate,uint32_t stop) {
    unsigned count=0;
    while (REG_PC!=stop) {
        if (++count>3000 || CPU_STOPPED) {
            fprintf(stderr,"interrupt did not return kind=%u n=%u candidate=%d PC=%06X SP=%06X\n",kind_now,fixture_now,candidate,REG_PC,REG_A[7]); exit(1);
        }
        if (kind_now==38 && REG_PC==code(0) && !nested_delivered) {
            /* Deliver a level-three hardware interrupt after soft dispatch
             * lowered SR. Use the CPU's real interrupt frame and nesting. */
            fa18_machine->intreq|=0x20; m68k_set_irq(3); m68ki_check_interrupts(); m68k_set_irq(0); nested_delivered=1;
        }
        if (candidate && candidate_step()) continue;
        fa18_machine_require_supported_target(REG_PPC,REG_PC);
        uint32_t pc=REG_PC; uint16_t op=fa18_bus_read16(pc);
        fa18_bus_begin(pc); fa18_bus_fetch(pc); REG_PPC=pc; REG_IR=op; REG_PC=pc+2;
        m68ki_instruction_jump_table[op](); USE_CYCLES(CYC_INSTRUCTION[op]);
    }
    fa18_bus_finish(REG_PC); return count;
}
static int order(const unsigned *ids,unsigned count) {
    if (rd_u32(JOURNAL_PTR)!=JOURNAL+count*4) return 0;
    for (unsigned i=0;i<count;++i) if (rd_u32(JOURNAL+i*4)!=ids[i]) return 0;
    return 1;
}
static int contract(unsigned kind,unsigned n) {
    unsigned number=(n/16)%16; uint32_t v=EXEC+AMIGA_EXEC_INT_VECTORS+12*number;
    if (kind<2) return REG_D[0]==node(number) && rd_u32(v+8)==(kind?0:node(0)) &&
        rd_u32(v)==(kind?UINT32_MAX:0) && rd_u32(v+4)==(kind?UINT32_MAX:code(0));
    if (kind==2) return rd_u32(SERVER_LIST)==node(0) && rd_u32(SERVER_LIST+8)==node(0);
    if (kind==3) return rd_u32(SERVER_LIST)==node(1) && rd_u32(node(1))==node(0) && rd_u32(SERVER_LIST+8)==node(0);
    if (kind==4) {
        /* Pinned 1.3 preserves D2 after multiplication: the empty-chain
         * disable writes bit (interrupt number *12)&31, not the number. */
        uint16_t cleared=(uint16_t)(1u<<((number*12)&31));
        return rd_u32(SERVER_LIST)==SERVER_LIST+4 && !(fa18_machine->intena&cleared);
    }
    if (kind==5) return rd_u32(node(1))==node(3) && rd_u32(node(3)+4)==node(1);
    if (kind>=6&&kind<10) {
        static const unsigned seq[]={0,1,2}; return order(seq,kind==6?0:kind==7?3:kind==8?1:2) && !(fa18_machine->intreq&0x20);
    }
    if (kind==10 || kind==11) {
        int priority=((int)(n/32%5)-2)*16+(int)(n&15);
        uint32_t q=EXEC+AMIGA_EXEC_SOFT_INTS+(priority+32)/16*16;
        return rd_u32(q)==node(0) && rd_u32(node(0))==q+4 && rd_u8(node(0)+AMIGA_NODE_TYPE)==11 &&
            (kind==11 || ((fa18_machine->intreq&4) && (rd_u8(EXEC+AMIGA_EXEC_RESCHEDULE_FLAGS)&32)));
    }
    if (kind==12 || kind==13) return rd_u32(JOURNAL_PTR)==JOURNAL && !(fa18_machine->intreq&4);
    if (kind==14) { static const unsigned seq[]={8,9,6,7,4,5,2,3,0,1}; return order(seq,10); }
    if (kind==15) { static const unsigned seq[]={0,2,1}; return order(seq,3); }
    if (kind==16) { static const unsigned seq[]={0,1,0}; return order(seq,3); }
    if (kind==38) { static const unsigned seq[]={5,0,1}; return nested_delivered && order(seq,3) && !(fa18_machine->intreq&0x24); }
    if (kind>=24 && kind<31 && irq_level(kind)!=7) return rd_u32(JOURNAL_PTR)==JOURNAL;
    unsigned level=irq_level(kind);
    if (kind>=31) {
        static const unsigned seq[8][4]={{0},{0,1,2},{3},{6,5,4},{8,10,7,9},{12,11},{14,13},{15}};
        static const unsigned lengths[]={0,3,1,3,4,2,2,1};
        return order(seq[level],lengths[level]) && !(fa18_machine->intreq&level_bits(level));
    }
    unsigned expected=level==1?0:level==2?3:level==3?4:level==4?7:level==5?11:level==6?13:15;
    return order(&expected,1);
}
static int captured_contracts(FA18Machine *m,const uint8_t *rom,size_t nr) {
    char path[256],error[256]; size_t size;
    FA18Machine *before=malloc(sizeof *before); uint8_t *cpu=malloc(m68k_context_size());
    if (!before || !cpu) return 0;
    kind_now=99;
    for (unsigned k=0;k<sizeof interrupt_captures/sizeof interrupt_captures[0];++k) {
        const char *directory=interrupt_captures[k].directory,*boundary=interrupt_captures[k].boundary;
        snprintf(path,sizeof path,"%s/entry_state.bin",directory); uint8_t *state=read_file(path,&size);
        if (!state || !fa18_machine_load_state(m,state,size,rom,nr,error,sizeof error)) return 0;
        free(state); fa18_bus_timing=0;
        SET_CYCLES(100000000); fa18_cycle_origin=100000000; fa18_next_event=INT64_MAX;
        memcpy(before,m,sizeof *m); m68k_get_context(cpu);
        int cycles=0; uint32_t usp=0,isp=0;
        for (unsigned side=0;side<2;++side) {
            memcpy(m,before,sizeof *m); m68k_set_context(cpu); SET_CYCLES(100000000);
            if (side) { memset(m->rom,0,sizeof m->rom); memset(m->rtarea,0,sizeof m->rtarea); fa18_machine_require_romfree(m); }
            fa18_bus_reset(); amiga_phase_observe_begin();
            unsigned count=execute_interrupt((int)side,interrupt_captures[k].pc);
            if (count!=interrupt_captures[k].count || memcmp(REG_DA,interrupt_captures[k].regs,sizeof interrupt_captures[k].regs) ||
                m68ki_get_sr()!=interrupt_captures[k].sr) {
                fprintf(stderr,"captured interrupt differs %s side=%u count=%u SR=%04X\n",directory,side,count,m68ki_get_sr()); return 0;
            }
            for (unsigned bank=0;bank<2;++bank) {
                snprintf(path,sizeof path,"%s/%s_%s.bin",directory,boundary,bank?"slow":"chip");
                uint8_t *expected=read_file(path,&size);
                if (!expected || size!=FA18_CHIP_SIZE || memcmp(expected,bank?m->slow:m->chip,size)) {
                    fprintf(stderr,"captured interrupt RAM differs %s side=%u bank=%u\n",directory,side,bank); return 0;
                }
                free(expected);
            }
            if (!side) { cycles=GET_CYCLES(); usp=m68k_get_reg(NULL,M68K_REG_USP); isp=m68k_get_reg(NULL,M68K_REG_ISP); amiga_phase_observe_reference(); }
            else if (cycles!=GET_CYCLES() || usp!=m68k_get_reg(NULL,M68K_REG_USP) || isp!=m68k_get_reg(NULL,M68K_REG_ISP) ||
                !amiga_phase_observe_compare() || m->runtime_guard.rom_reads || m->runtime_guard.rom_instruction_fetches ||
                m->runtime_guard.unsupported_services) return 0;
            printf("captured %s side=%u: %u instructions, %u CPU cycles /%u native OCS clocks (Engine9000 %u); registers/full SR/RAM match\n",
                directory,side,count,100000000-GET_CYCLES(),(100000000-GET_CYCLES())/2,interrupt_captures[k].cycles);
        }
    }
    free(cpu); free(before); return 1;
}
int main(void) {
    size_t ns,nr; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&ns),*rom=read_file("local/system/kick13.rom",&nr);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *ram=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE),*cpu=malloc(m68k_context_size());
    if (!state || !rom || !m || !base || !before || !ram || !cpu) return 1;
    if (!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error) || !fa18_os_exec_interrupt_services_signature_matches(m->rom)) return 1;
    free(state); memcpy(base,m,sizeof *m); fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL);
    unsigned matched=0;
    for (unsigned bus=0;bus<2;++bus) {
        fa18_bus_timing=(int)bus;
        for (unsigned kind=0;kind<39;++kind) for (unsigned n=0;n<256;++n) {
            kind_now=kind; fixture_now=n;
            memcpy(m,base,sizeof *m); interrupt_fixture(kind,n);
            memcpy(before,m,sizeof *m); m68k_get_context(cpu); amiga_phase_observe_begin();
            unsigned want_count=execute_interrupt(0,RETURN);
            if (!contract(kind,n)) { fprintf(stderr,"original interrupt contract fails bus=%u kind=%u n=%u SR=%04X head=%06X pred=%06X INTENA=%04X D0=%08X D1=%08X D2=%08X\n",bus,kind,n,m68ki_get_sr(),rd_u32(SERVER_LIST),rd_u32(SERVER_LIST+8),m->intena,REG_D[0],REG_D[1],REG_D[2]); return 1; }
            uint32_t regs[16]; memcpy(regs,REG_DA,sizeof regs);
            uint32_t sr=m68ki_get_sr(),usp=m68k_get_reg(NULL,M68K_REG_USP),isp=m68k_get_reg(NULL,M68K_REG_ISP);
            int cycles=GET_CYCLES(); memcpy(ram,m->chip,FA18_CHIP_SIZE); memcpy(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
            amiga_phase_observe_reference();
            memcpy(m,before,sizeof *m); m68k_set_context(cpu); nested_delivered=0; SET_CYCLES(100000000);
            memset(m->rom,0,sizeof m->rom); memset(m->rtarea,0,sizeof m->rtarea); fa18_machine_require_romfree(m);
            reset_bus(); amiga_phase_observe_begin(); unsigned got_count=execute_interrupt(1,RETURN);
            if (got_count!=want_count || !contract(kind,n) || memcmp(regs,REG_DA,sizeof regs) || sr!=m68ki_get_sr() ||
                usp!=m68k_get_reg(NULL,M68K_REG_USP) || isp!=m68k_get_reg(NULL,M68K_REG_ISP) || cycles!=GET_CYCLES() ||
                memcmp(ram,m->chip,FA18_CHIP_SIZE) || memcmp(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE) ||
                !amiga_phase_observe_compare() || m->runtime_guard.rom_reads || m->runtime_guard.rom_instruction_fetches || m->runtime_guard.unsupported_services) {
                fprintf(stderr,"interrupt differs bus=%u kind=%u fixture=%u cycles=%d/%d count=%u/%u\n",bus,kind,n,cycles,GET_CYCLES(),want_count,got_count); return 1;
            }
            ++matched;
        }
    }
    printf("Exec interrupts: %u complete CPU/DMA calls; vectors, server lifecycle/claim order, Cause coalescing, soft priority/FIFO/requeue, seven IRQ levels, simultaneous audio and nested hardware IRQ; registers/full SR/stack banks/RAM/accesses/cycles match with ROM removed\n",matched);
    if (!captured_contracts(m,rom,nr)) return 1;
    free(cpu); free(ram); free(before); free(base); free(m); free(rom); return 0;
}
