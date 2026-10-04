/* Reuse the shared machine fixture and access observer, not source semantics. */
#define main structural_phase_main
#include "service_phase_oracle.c"
#undef main
#include "../../port/amiga/abi_13.h"
static void put_name(uint32_t address,const char *name) {
    for (size_t i=0;i<=strlen(name);++i) wr_u8(address+(uint32_t)i,(uint8_t)name[i]);
}
static void make_list(uint32_t list,uint32_t first,uint32_t second) {
    uint32_t tail=list+4;
    wr_u32(list,first?first:tail); wr_u32(tail,0); wr_u32(list+8,second?second:first?first:list);
    if (first) { wr_u32(first,second?second:tail); wr_u32(first+4,list); }
    if (second) { wr_u32(second,tail); wr_u32(second+4,first); }
}
static void lookup_fixture(unsigned scenario,unsigned query,unsigned empty) {
    static const uint8_t depths[]={0xFF,0,1,2,0x7F,0x80,0xFE,0xFF};
    static const char *names[]={NULL,"ReadyAlpha","ReadyBeta","Waiting","Current","Missing","ReadyAlph","current",""};
    fa18_write_log_active=0;
    for (unsigned i=0;i<16;++i) REG_DA[i]=random_word();
    REG_A[6]=0xC63000; REG_A[7]=0xC7FC00;
    REG_A[1]=query?0xC65400:0;
    if (query) put_name(REG_A[1],names[query]);
    for (unsigned i=0;i<4;++i) wr_u32(0xC64000+0x80*i+AMIGA_NODE_NAME,0xC65000+0x80*i);
    put_name(0xC65000,"ReadyAlpha"); put_name(0xC65080,"ReadyBeta");
    put_name(0xC65100,"Waiting"); put_name(0xC65180,"Current");
    make_list(REG_A[6]+AMIGA_EXEC_TASK_READY,empty?0:0xC64000,empty?0:0xC64080);
    make_list(REG_A[6]+AMIGA_EXEC_TASK_WAIT,empty?0:0xC64100,0);
    wr_u32(REG_A[6]+AMIGA_EXEC_THIS_TASK,0xC64180);
    wr_u8(REG_A[6]+AMIGA_EXEC_ID_NEST_CNT,depths[scenario/32%8]);
    wr_u16(REG_A[6]+AMIGA_EXEC_FIND_NAME_LVO,0x4EF9);
    wr_u32(REG_A[6]+AMIGA_EXEC_FIND_NAME_LVO+2,0xFC1696);
    wr_u32(REG_A[7],0xC10000);
    REG_PC=0xFC1DB0;
    m68k_set_reg(M68K_REG_SR,0x2700|(scenario&31));
    SET_CYCLES(100000000); fa18_next_event=INT64_MAX; fa18_cycle_origin=100000000;
    dma_offset=40+(scenario%32)*4; reset_bus(); fa18_write_log_active=1;
}
static unsigned execute_lookup(int candidate,uint32_t ret,uint32_t sp) {
    unsigned instructions=0;
    while (REG_PC!=ret || REG_A[7]!=sp) {
        if (++instructions>10000) { fputs("lookup did not return\n",stderr); exit(1); }
        if (candidate && (fa18_os_exec_find_task_step() || fa18_os_exec_find_name_step())) continue;
        uint32_t pc=REG_PC;
        fa18_machine_require_supported_target(REG_PPC,pc);
        uint16_t op=fa18_bus_read16(pc);
        fa18_bus_begin(pc); fa18_bus_fetch(pc); REG_PPC=pc; REG_IR=op; REG_PC=pc+2;
        m68ki_instruction_jump_table[op](); USE_CYCLES(CYC_INSTRUCTION[op]);
    }
    fa18_bus_finish(REG_PC); return instructions;
}
int main(void) {
    size_t ns,nr; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&ns),*rom=read_file("local/system/kick13.rom",&nr);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *ram=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE),*cpu=malloc(m68k_context_size());
    if (!state || !rom || !m || !base || !before || !ram || !cpu) return 1;
    if (!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) return 1;
    if (!fa18_os_exec_task_lookup_signature_matches(m->rom)) return 1;
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL);
    unsigned calls=0;
    for (unsigned bus=0;bus<2;++bus) {
        fa18_bus_timing=(int)bus;
        for (unsigned empty=0;empty<2;++empty) for (unsigned query=0;query<9;++query)
        for (unsigned n=0;n<256;++n) {
            memcpy(m,base,sizeof *m); lookup_fixture(n,query,empty);
            memcpy(before,m,sizeof *m); m68k_get_context(cpu);
            amiga_phase_observe_begin();
            unsigned want_instructions=execute_lookup(0,0xC10000,0xC7FC04);
            uint32_t regs[16]; memcpy(regs,REG_DA,sizeof regs);
            uint32_t sr=m68k_get_reg(NULL,M68K_REG_SR); int cycles=GET_CYCLES();
            uint32_t expected=query==0 || query==4?0xC64180:
                empty?0:query==1?0xC64000:query==2?0xC64080:query==3?0xC64100:0;
            if (REG_D[0]!=expected) { fputs("fixture disagrees with original task contract\n",stderr); return 1; }
            memcpy(ram,m->chip,FA18_CHIP_SIZE); memcpy(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
            amiga_phase_observe_reference();
            memcpy(m,before,sizeof *m); m68k_set_context(cpu); SET_CYCLES(100000000);
            memset(m->rom,0,sizeof m->rom); memset(m->rtarea,0,sizeof m->rtarea);
            fa18_machine_require_romfree(m); reset_bus(); amiga_phase_observe_begin();
            unsigned got_instructions=execute_lookup(1,0xC10000,0xC7FC04);
            if (got_instructions!=want_instructions || memcmp(regs,REG_DA,sizeof regs) ||
                sr!=m68k_get_reg(NULL,M68K_REG_SR) || cycles!=GET_CYCLES() ||
                memcmp(ram,m->chip,FA18_CHIP_SIZE) || memcmp(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE) ||
                !amiga_phase_observe_compare()) {
                fprintf(stderr,"FindTask call differs: bus=%u empty=%u query=%u case=%u cycles=%d/%d\n",bus,empty,query,n,cycles,GET_CYCLES());
                return 1;
            }
            ++calls;
        }
    }
    printf("FindTask/FindName: %u complete CPU/DMA calls match registers, SR, all RAM, ordered accesses and cycles with ROM removed\n",calls);
    free(cpu); free(ram); free(before); free(base); free(m); return 0;
}
