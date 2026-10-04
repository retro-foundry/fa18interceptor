/* Full guest calls and list topology against the original service family. */
#define main structural_phase_main
#include "service_phase_oracle.c"
#undef main
#include "../../port/amiga/abi_13.h"
typedef struct { uint32_t entry; const char *name; unsigned kind; } Call;
static const Call calls[]={
    {0xFC15E8,"Insert/head",0},{0xFC15E8,"Insert/first",1},
    {0xFC15E8,"Insert/last",2},{0xFC15E8,"Insert/tail",3},
    {0xFC1614,"AddHead",4},{0xFC1624,"AddTail",5},
    {0xFC163C,"Remove/first",6},{0xFC163C,"Remove/last",7},
    {0xFC164A,"RemHead",8},{0xFC165A,"RemTail",9},{0xFC1670,"Enqueue",10}
};
static uint32_t expected_nodes[4];
static unsigned expected_length;
static unsigned covered[0xAE/2];
static void list_fixture(const Call *call,unsigned length,unsigned n) {
    static const int priorities[]={127,0,-128};
    static const int new_priorities[]={127,126,1,0,-1,-127,-128,127};
    uint32_t list=0xC64000,tail=list+4,node=0xC64200;
    unsigned insertion=0,removal=0;
    fa18_write_log_active=0;
    for (unsigned i=0;i<16;++i) REG_DA[i]=random_word();
    for (unsigned i=0;i<length;++i) {
        uint32_t at=0xC64100+0x40*i;
        expected_nodes[i]=at;
        wr_u32(at,i+1<length?at+0x40:tail);
        wr_u32(at+4,i?at-0x40:list);
        wr_u8(at+AMIGA_NODE_PRIORITY,(uint8_t)priorities[i]);
    }
    wr_u32(list,length?0xC64100:tail); wr_u32(tail,0);
    wr_u32(list+8,length?0xC64100+0x40*(length-1):list);
    REG_A[0]=list; REG_A[1]=node; REG_A[2]=0;
    REG_A[6]=0xC63000; REG_A[7]=0xC7FC00;
    wr_u32(REG_A[7],0xC10000);
    wr_u32(node,0xAAAAAAAA); wr_u32(node+4,0xBBBBBBBB);
    int priority=new_priorities[n/32%8]; wr_u8(node+AMIGA_NODE_PRIORITY,(uint8_t)priority);
    if (call->kind==1 && length) { REG_A[2]=0xC64100; insertion=1; }
    if (call->kind==2) { REG_A[2]=length?0xC64100+0x40*(length-1):0; insertion=length; }
    if (call->kind==3) { REG_A[2]=tail; insertion=length; }
    if (call->kind==5) insertion=length;
    if (call->kind==10) {
        while (insertion<length && priority<=priorities[insertion]) ++insertion;
    }
    expected_length=length;
    if (call->kind<=5 || call->kind==10) {
        memmove(expected_nodes+insertion+1,expected_nodes+insertion,(length-insertion)*sizeof(uint32_t));
        expected_nodes[insertion]=node; ++expected_length;
    } else if (length) {
        removal=(call->kind==7 || call->kind==9)?length-1:0;
        if (call->kind==6 || call->kind==7) REG_A[1]=expected_nodes[removal];
        memmove(expected_nodes+removal,expected_nodes+removal+1,(length-removal-1)*sizeof(uint32_t));
        --expected_length;
    }
    REG_PC=call->entry; m68k_set_reg(M68K_REG_SR,0x2700|(n&31));
    SET_CYCLES(100000000); fa18_next_event=INT64_MAX; fa18_cycle_origin=100000000;
    dma_offset=40+(n%32)*4; reset_bus(); fa18_write_log_active=1;
}
static int topology_matches(void) {
    uint32_t list=0xC64000,tail=list+4,previous=list,current=rd_u32(list);
    for (unsigned i=0;i<expected_length;++i) {
        if (current!=expected_nodes[i] || rd_u32(current+4)!=previous) return 0;
        previous=current; current=rd_u32(current);
    }
    return current==tail && rd_u32(tail)==0 && rd_u32(list+8)==previous;
}
static unsigned execute_list(int candidate) {
    unsigned count=0;
    while (REG_PC!=0xC10000 || REG_A[7]!=0xC7FC04) {
        if (++count>100) { fputs("list service did not return\n",stderr); exit(1); }
        if (REG_PC>=0xFC15E8 && REG_PC<0xFC1696) covered[(REG_PC-0xFC15E8)/2]|=candidate?2:1;
        if (candidate && fa18_os_exec_lists_step()) continue;
        fa18_machine_require_supported_target(REG_PPC,REG_PC);
        uint32_t pc=REG_PC; uint16_t op=fa18_bus_read16(pc);
        fa18_bus_begin(pc); fa18_bus_fetch(pc); REG_PPC=pc; REG_IR=op; REG_PC=pc+2;
        m68ki_instruction_jump_table[op](); USE_CYCLES(CYC_INSTRUCTION[op]);
    }
    fa18_bus_finish(REG_PC); return count;
}
int main(void) {
    size_t ns,nr; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&ns),*rom=read_file("local/system/kick13.rom",&nr);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *ram=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE),*cpu=malloc(m68k_context_size());
    if (!state || !rom || !m || !base || !before || !ram || !cpu) return 1;
    if (!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error) ||
        !fa18_os_exec_lists_signature_matches(m->rom)) return 1;
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL);
    unsigned matched=0;
    for (unsigned bus=0;bus<2;++bus) {
        fa18_bus_timing=(int)bus;
        for (unsigned k=0;k<sizeof calls/sizeof calls[0];++k)
        for (unsigned length=0;length<4;++length) for (unsigned n=0;n<256;++n) {
            if (!length && (calls[k].kind==6 || calls[k].kind==7)) continue;
            memcpy(m,base,sizeof *m); list_fixture(&calls[k],length,n);
            memcpy(before,m,sizeof *m); m68k_get_context(cpu); amiga_phase_observe_begin();
            unsigned want_count=execute_list(0);
            uint32_t regs[16]; memcpy(regs,REG_DA,sizeof regs);
            uint32_t sr=m68k_get_reg(NULL,M68K_REG_SR); int cycles=GET_CYCLES();
            if (!topology_matches()) { fprintf(stderr,"original topology differs for %s length=%u priority=%u\n",calls[k].name,length,n/32); return 1; }
            memcpy(ram,m->chip,FA18_CHIP_SIZE); memcpy(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
            amiga_phase_observe_reference();
            memcpy(m,before,sizeof *m); m68k_set_context(cpu); SET_CYCLES(100000000);
            memset(m->rom,0,sizeof m->rom); memset(m->rtarea,0,sizeof m->rtarea);
            fa18_machine_require_romfree(m); reset_bus(); amiga_phase_observe_begin();
            unsigned got_count=execute_list(1);
            if (got_count!=want_count || memcmp(regs,REG_DA,sizeof regs) ||
                sr!=m68k_get_reg(NULL,M68K_REG_SR) || cycles!=GET_CYCLES() ||
                memcmp(ram,m->chip,FA18_CHIP_SIZE) || memcmp(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE) ||
                !amiga_phase_observe_compare() || !topology_matches() ||
                m->runtime_guard.rom_reads || m->runtime_guard.rom_instruction_fetches || m->runtime_guard.unsupported_services) {
                fprintf(stderr,"%s differs: bus=%u length=%u fixture=%u cycles=%d/%d\n",calls[k].name,bus,length,n,cycles,GET_CYCLES()); return 1;
            }
            ++matched;
        }
    }
    unsigned pcs=0;
    for (unsigned k=0;k<sizeof service_phase_cases/sizeof service_phase_cases[0];++k) {
        uint32_t pc=service_phase_cases[k].pc;
        if (pc>=0xFC15E8 && pc<0xFC1696) {
            if (covered[(pc-0xFC15E8)/2]!=3) { fprintf(stderr,"missing whole-call list phase %06X\n",pc); return 1; }
            ++pcs;
        }
    }
    printf("Exec lists: %u complete CPU/DMA calls, %u/%u phases, original topology and stable priority order; registers/SR/RAM/accesses/cycles match with ROM removed\n",matched,pcs,pcs);
    free(cpu); free(ram); free(before); free(base); free(m); return 0;
}
