/* Complete nonblocking guest calls. Blocking task switches and Supervisor
 * remain separate, explicitly unproved dependencies. RAM callbacks test the
 * original message action ABI, not the substantive Cause implementation. */
#define main structural_phase_main
#include "service_phase_oracle.c"
#undef main
#include "../../port/amiga/abi_13.h"
typedef struct { uint32_t entry; const char *name; } Call;
static const Call calls[]={
    {0xFC1B76,"PutMsg"},{0xFC1C18,"ReplyMsg"},{0xFC1C32,"WaitPort/nonempty"},
    {0xFC1E54,"SetExcept"},{0xFC1E5E,"SetSignal"},{0xFC1E84,"Signal"},
    {0xFC1F0C,"Wait/pending"},{0xFC1F74,"Reschedule/deferred"},
    {0xFC1F96,"Forbid"},{0xFC1F9C,"Permit/deferred"},
    {0xFC1FCA,"AllocTrap"},{0xFC1FF0,"FreeTrap"},
    {0xFC2000,"AllocSignal"},{0xFC2038,"FreeSignal"}
};
enum { EXEC=0xC63000, PORT=0xC64100, MSG=0xC64200, OLD_MSG=0xC64300,
       TASK=0xC64400, READY=0xC64500, CALLBACK=0xC64600, RETURN=0xC10000 };
static void link_one(uint32_t list,uint32_t node) {
    wr_u32(list,node?node:list+4); wr_u32(list+4,0); wr_u32(list+8,node?node:list);
    if (node) { wr_u32(node,list+4); wr_u32(node+4,list); }
}
static void vector(unsigned offset,uint32_t target) {
    wr_u16(EXEC-offset,0x4EF9); wr_u32(EXEC-offset+2,target);
}
static void task_fixture(const Call *call,unsigned mode,unsigned n) {
    static const uint32_t patterns[]={0,1,0x80000000,0xFFFFFFFF,0x7FFFFFFF,0xFFFEFFFF,0xFFFF,0xAAAAAAAA};
    static const uint32_t requests[]={0xFFFFFFFF,0,15,31,16,32,7,0x80000001};
    unsigned variant=n/32; uint32_t pattern=patterns[variant];
    fa18_write_log_active=0;
    for (unsigned i=0;i<16;++i) REG_DA[i]=random_word();
    memset(fa18_machine->slow+(EXEC-FA18_SLOW_BASE)-0x200,0,0x1A00);
    REG_A[0]=PORT; REG_A[1]=TASK; REG_A[6]=EXEC; REG_A[7]=0xC7FC00;
    wr_u32(REG_A[7],RETURN); wr_u32(EXEC+AMIGA_EXEC_THIS_TASK,TASK);
    wr_u8(EXEC+AMIGA_EXEC_ID_NEST_CNT,(uint8_t)(mode%3-1));
    wr_u8(EXEC+AMIGA_EXEC_TD_NEST_CNT,(uint8_t)(mode/3));
    wr_u8(EXEC+AMIGA_EXEC_RESCHEDULE_FLAGS,mode&1?0x80:0);
    wr_u8(TASK+AMIGA_TASK_STATE,mode&1?4:2);
    wr_u8(TASK+AMIGA_NODE_PRIORITY,mode&2?10:(uint8_t)-5);
    wr_u32(TASK+AMIGA_TASK_SIGNALS_RECEIVED,pattern);
    wr_u32(TASK+AMIGA_TASK_SIGNALS_EXCEPT,mode>=4?0x80000001:0);
    wr_u32(TASK+AMIGA_TASK_SIGNALS_WAIT,0x80000001);
    wr_u32(TASK+AMIGA_TASK_SIGNALS_ALLOCATED,pattern);
    wr_u16(TASK+AMIGA_TASK_TRAPS_ALLOCATED,(uint16_t)pattern);
    link_one(EXEC+AMIGA_EXEC_TASK_WAIT,mode&1?TASK:0);
    link_one(EXEC+AMIGA_EXEC_TASK_READY,mode&2?READY:0);
    wr_u8(READY+AMIGA_NODE_PRIORITY,0);
    vector(324,0xFC1E84); vector(48,0xFC1F74); vector(318,0xFC1F0C);
    /* Isolated controlled callback: write a marker and return. */
    wr_u16(CALLBACK,0x33FC); wr_u16(CALLBACK+2,0x1234);
    wr_u32(CALLBACK+4,CALLBACK+0x80); wr_u16(CALLBACK+8,0x4E75);
    vector(180,CALLBACK);
    REG_D[0]=patterns[(variant+1)%8]; REG_D[1]=pattern;
    if (call->entry==0xFC1B76 || call->entry==0xFC1C18 || call->entry==0xFC1C32) {
        REG_A[1]=MSG;
        link_one(PORT+AMIGA_PORT_MESSAGES,mode&1?OLD_MSG:0);
        wr_u8(PORT+AMIGA_PORT_FLAGS,(uint8_t)(mode%4));
        wr_u8(PORT+AMIGA_PORT_SIGNAL_BIT,(uint8_t)(variant*5));
        wr_u32(PORT+AMIGA_PORT_SIGNAL_TASK,mode==5?0:mode%4==0?TASK:CALLBACK);
        wr_u32(MSG+AMIGA_MESSAGE_REPLY_PORT,mode==5?0:PORT);
        if (call->entry==0xFC1C32) link_one(PORT+AMIGA_PORT_MESSAGES,OLD_MSG);
    } else if (call->entry>=0xFC1FCA) {
        REG_D[0]=requests[variant];
    } else if (call->entry==0xFC1F0C) {
        /* Pending Wait consumes these bits without entering Switch/Supervisor. */
        REG_D[0]=0x80000001; wr_u32(TASK+AMIGA_TASK_SIGNALS_RECEIVED,pattern|1);
    }
    if (call->entry==0xFC1F9C) wr_u8(EXEC+AMIGA_EXEC_TD_NEST_CNT,mode%3+1);
    REG_PC=call->entry; m68k_set_reg(M68K_REG_SR,0x2700|(n&31));
    SET_CYCLES(100000000); fa18_next_event=INT64_MAX; fa18_cycle_origin=100000000;
    dma_offset=40+(n%32)*4; reset_bus(); fa18_write_log_active=1;
}
static unsigned execute_task(int candidate) {
    unsigned count=0;
    while (REG_PC!=RETURN || REG_A[7]!=0xC7FC04) {
        if (++count>1000) { fprintf(stderr,"task service did not return at %06X\n",REG_PC); exit(1); }
        if (candidate && (fa18_os_exec_messages_step() || fa18_os_exec_signals_step() ||
            fa18_os_exec_task_protection_step() || fa18_os_exec_lists_step())) continue;
        fa18_machine_require_supported_target(REG_PPC,REG_PC);
        uint32_t pc=REG_PC; uint16_t op=fa18_bus_read16(pc);
        fa18_bus_begin(pc); fa18_bus_fetch(pc); REG_PPC=pc; REG_IR=op; REG_PC=pc+2;
        m68ki_instruction_jump_table[op](); USE_CYCLES(CYC_INSTRUCTION[op]);
    }
    fa18_bus_finish(REG_PC); return count;
}
static int contract_matches(const Call *call,unsigned mode,uint32_t old_received) {
    if (call->entry==0xFC1C32) return REG_D[0]==OLD_MSG;
    if (call->entry==0xFC1F0C)
        return REG_D[0]==(old_received&0x80000001) &&
               rd_u32(TASK+AMIGA_TASK_SIGNALS_RECEIVED)==(old_received&~0x80000001u);
    if (call->entry==0xFC1B76 || (call->entry==0xFC1C18 && mode!=5)) {
        uint32_t list=PORT+AMIGA_PORT_MESSAGES,first=mode&1?OLD_MSG:MSG;
        return rd_u32(list)==first && rd_u32(MSG)==list+4 && rd_u32(list+8)==MSG &&
            rd_u32(MSG+4)==(mode&1?OLD_MSG:list) &&
            (call->entry!=0xFC1C18 || rd_u8(MSG+AMIGA_NODE_TYPE)==7);
    }
    if (call->entry==0xFC1C18) return rd_u8(MSG+AMIGA_NODE_TYPE)==6;
    return 1;
}
int main(void) {
    size_t ns,nr; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&ns),*rom=read_file("local/system/kick13.rom",&nr);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *ram=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE),*cpu=malloc(m68k_context_size());
    if (!state || !rom || !m || !base || !before || !ram || !cpu) return 1;
    if (!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error) ||
        !fa18_os_exec_messages_signature_matches(m->rom) || !fa18_os_exec_signals_signature_matches(m->rom) ||
        !fa18_os_exec_task_protection_signature_matches(m->rom)) return 1;
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL);
    unsigned matched=0;
    for (unsigned bus=0;bus<2;++bus) {
        fa18_bus_timing=(int)bus;
        for (unsigned k=0;k<sizeof calls/sizeof calls[0];++k)
        for (unsigned mode=0;mode<6;++mode) for (unsigned n=0;n<256;++n) {
            memcpy(m,base,sizeof *m); task_fixture(&calls[k],mode,n);
            uint32_t old_received=rd_u32(TASK+AMIGA_TASK_SIGNALS_RECEIVED);
            memcpy(before,m,sizeof *m); m68k_get_context(cpu); amiga_phase_observe_begin();
            unsigned want_count=execute_task(0);
            uint32_t regs[16]; memcpy(regs,REG_DA,sizeof regs);
            uint32_t sr=m68k_get_reg(NULL,M68K_REG_SR); int cycles=GET_CYCLES();
            if (!contract_matches(&calls[k],mode,old_received)) { fprintf(stderr,"original contract differs for %s mode=%u\n",calls[k].name,mode); return 1; }
            memcpy(ram,m->chip,FA18_CHIP_SIZE); memcpy(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
            amiga_phase_observe_reference();
            memcpy(m,before,sizeof *m); m68k_set_context(cpu); SET_CYCLES(100000000);
            memset(m->rom,0,sizeof m->rom); memset(m->rtarea,0,sizeof m->rtarea);
            fa18_machine_require_romfree(m); reset_bus(); amiga_phase_observe_begin();
            unsigned got_count=execute_task(1);
            if (got_count!=want_count || memcmp(regs,REG_DA,sizeof regs) ||
                sr!=m68k_get_reg(NULL,M68K_REG_SR) || cycles!=GET_CYCLES() ||
                memcmp(ram,m->chip,FA18_CHIP_SIZE) || memcmp(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE) ||
                !amiga_phase_observe_compare() || !contract_matches(&calls[k],mode,old_received) ||
                m->runtime_guard.rom_reads || m->runtime_guard.rom_instruction_fetches || m->runtime_guard.unsupported_services) {
                fprintf(stderr,"%s differs: bus=%u mode=%u fixture=%u cycles=%d/%d\n",calls[k].name,bus,mode,n,cycles,GET_CYCLES()); return 1;
            }
            ++matched;
        }
    }
    printf("Exec task services: %u complete CPU/DMA calls, message queues/callbacks, pending waits, signal/trap exhaustion and nesting; registers/SR/RAM/accesses/cycles match with ROM removed\n",matched);
    free(cpu); free(ram); free(before); free(base); free(m); return 0;
}
