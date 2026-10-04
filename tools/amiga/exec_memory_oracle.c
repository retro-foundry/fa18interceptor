/* Complete calls and independent free-list accounting against original Exec.
 * Invalid frees stop at the original Alert vector, never a fabricated return. */
#define main structural_phase_main
#include "service_phase_oracle.c"
#undef main
#include "../../port/amiga/abi_13.h"
enum { EXEC=0xC63000, HEADER=0xC64000, HEADER2=0xC64400,
       REGION=0x6000, REGION2=0xC65000, RETURN=0xC10000 };
typedef struct { uint32_t entry; const char *name; } Call;
static const Call calls[]={
    {0xFC16D8,"Allocate"},{0xFC1740,"Deallocate"},{0xFC17D0,"AllocMem"},
    {0xFC182C,"FreeMem"},{0xFC1856,"TypeOfMem"},{0xFC190C,"AvailMem"},
    {0xFC187C,"AllocAbs"}
};
enum { CALL_COUNT=sizeof calls/sizeof calls[0] };
static uint32_t expected_result,expected_total,expected_total2,old_total,request,requirements;
static int expected_self_link;
static uint32_t freed_address,rounded,first_sizes[3],first_addresses[3];
static unsigned first_count;
static void chunks(uint32_t header,const uint32_t *addresses,const uint32_t *sizes,unsigned count) {
    uint32_t total=0;
    wr_u32(header+AMIGA_MEMORY_FIRST,count?addresses[0]:0);
    for (unsigned i=0;i<count;++i) {
        wr_u32(addresses[i],i+1<count?addresses[i+1]:0);
        wr_u32(addresses[i]+4,sizes[i]); total+=sizes[i];
    }
    wr_u32(header+AMIGA_MEMORY_FREE,total);
}
static void memory_fixture(const Call *call,unsigned mode,unsigned n) {
    static const uint32_t sizes[]={0,1,7,8,9,16,32,0xFFFFFFFF};
    static const uint32_t flags[]={0,1,2,4,3,5,8,1};
    static const unsigned counts[]={0,1,2,3,1,2};
    static const uint32_t shape[][3]={{0},{8},{8,32},{8,16,24},{64},{32,32}};
    unsigned variant=n/32;
    fa18_write_log_active=0;
    for (unsigned i=0;i<16;++i) REG_DA[i]=random_word();
    memset(fa18_machine->slow+(EXEC-FA18_SLOW_BASE)-0x200,0,0x2700);
    memset(fa18_machine->chip+REGION,0xA5,0x800);
    memset(fa18_machine->slow+REGION2-FA18_SLOW_BASE,0xA5,0x800);
    REG_A[0]=HEADER; REG_A[1]=REGION+0x100; REG_A[6]=EXEC; REG_A[7]=0xC7FC00;
    wr_u32(REG_A[7],RETURN); wr_u32(4,EXEC);
    wr_u16(EXEC-138,0x4EF9); wr_u32(EXEC-136,0xFC1F9C);
    wr_u8(EXEC+AMIGA_EXEC_ID_NEST_CNT,0xFF);
    wr_u8(EXEC+AMIGA_EXEC_TD_NEST_CNT,(uint8_t)(variant-1));
    wr_u8(EXEC+AMIGA_EXEC_RESCHEDULE_FLAGS,0);
    uint32_t list=EXEC+AMIGA_EXEC_MEMORY_LIST;
    wr_u32(list,HEADER); wr_u32(list+4,0); wr_u32(list+8,HEADER2);
    wr_u32(HEADER,HEADER2); wr_u32(HEADER+4,list);
    wr_u32(HEADER2,list+4); wr_u32(HEADER2+4,HEADER);
    wr_u16(HEADER+AMIGA_MEMORY_ATTRIBUTES,3); wr_u16(HEADER2+AMIGA_MEMORY_ATTRIBUTES,5);
    wr_u32(HEADER+AMIGA_MEMORY_LOWER,REGION); wr_u32(HEADER+AMIGA_MEMORY_UPPER,REGION+0x800);
    wr_u32(HEADER2+AMIGA_MEMORY_LOWER,REGION2); wr_u32(HEADER2+AMIGA_MEMORY_UPPER,REGION2+0x800);
    first_count=counts[mode];
    for (unsigned i=0;i<3;++i) { first_addresses[i]=REGION+i*0x80; first_sizes[i]=shape[mode][i]; }
    chunks(HEADER,first_addresses,first_sizes,first_count);
    const uint32_t second_addresses[]={REGION2,REGION2+0x100},second_sizes[]={16,64};
    chunks(HEADER2,second_addresses,second_sizes,2);
    request=sizes[variant]; requirements=flags[variant]|(mode&1?0x10000:0);
    REG_D[0]=request; REG_D[1]=requirements;
    rounded=(request+7)&~7u; expected_result=0;
    expected_self_link=0;
    if (call->entry==0xFC16D8 || call->entry==0xFC17D0) {
        if (request && (call->entry==0xFC16D8 || (3&(uint16_t)requirements)==(uint16_t)requirements))
            for (unsigned i=0;i<first_count;++i) if (first_sizes[i]>=rounded) { expected_result=first_addresses[i]; break; }
        if (call->entry==0xFC17D0 && !expected_result && request && (5&(uint16_t)requirements)==(uint16_t)requirements)
            for (unsigned i=0;i<2;++i) if (second_sizes[i]>=rounded) { expected_result=second_addresses[i]; break; }
    } else if (call->entry==0xFC187C) {
        const uint32_t locations[]={REGION,REGION+1,REGION+7,REGION+8,REGION+0x80,REGION2+1,REGION+0x800,REGION+0x60};
        REG_A[1]=locations[variant]; uint32_t at=REG_A[1]&~7u;
        rounded=(request+(REG_A[1]&7)+7)&~7u;
        for (unsigned i=0;i<first_count;++i) if (at>=first_addresses[i] && at+rounded<=first_addresses[i]+first_sizes[i]) {
            expected_result=at; expected_self_link=!rounded && at==first_addresses[i]; break;
        }
        for (unsigned i=0;i<2;++i) if (at>=second_addresses[i] && at+rounded<=second_addresses[i]+second_sizes[i]) {
            expected_result=at; expected_self_link=!rounded && at==second_addresses[i]; break;
        }
    } else if (call->entry==0xFC1740 || call->entry==0xFC182C) {
        unsigned offset=variant&7; rounded=(request+offset+7)&~7u;
        uint32_t addresses[2],bytes[2]; unsigned count=0;
        freed_address=REGION+0x100;
        if (mode==1) freed_address=REGION+0xA0;
        if (mode==1 || mode==3 || mode==4 || mode==5) {
            addresses[count]=REGION+0x80; bytes[count++]=mode==3?0x80:0x20;
        }
        if (mode==2) freed_address=REGION+0x200-rounded;
        if (mode==2 || mode==3 || mode==4) {
            addresses[count]=mode==2?REGION+0x200:mode==3?freed_address+rounded:REGION+0x200;
            bytes[count++]=0x20;
        }
        chunks(HEADER,addresses,bytes,count); REG_A[1]=freed_address+offset;
    } else if (call->entry==0xFC1856) {
        const uint32_t addresses[]={REGION-1,REGION,REGION+7,REGION+0x7FF,REGION+0x800,REGION2,REGION2+0x7FF,REGION2+0x800};
        REG_A[1]=addresses[variant]; expected_result=variant==1 || variant==2 || variant==3?3:variant==5 || variant==6?5:0;
    } else {
        requirements=flags[variant]|(mode&1?0x20000:0); REG_D[1]=requirements;
        for (unsigned i=0;i<first_count;++i) if ((3&(uint16_t)requirements)==(uint16_t)requirements) {
            if (requirements&0x20000) { if (first_sizes[i]>expected_result) expected_result=first_sizes[i]; }
            else expected_result+=first_sizes[i];
        }
        if ((5&(uint16_t)requirements)==(uint16_t)requirements) {
            if (requirements&0x20000) { if (expected_result<64) expected_result=64; }
            else expected_result+=80;
        }
    }
    old_total=rd_u32(HEADER+AMIGA_MEMORY_FREE);
    expected_total=old_total;
    expected_total2=80;
    if (call->entry==0xFC1740 || call->entry==0xFC182C) expected_total+=request?rounded:0;
    else if (expected_result>=REGION && expected_result<REGION+0x800) expected_total-=rounded;
    if (expected_result>=REGION2 && expected_result<REGION2+0x800) expected_total2-=rounded;
    REG_PC=call->entry; m68k_set_reg(M68K_REG_SR,0x2700|(n&31));
    SET_CYCLES(100000000); fa18_cycle_origin=100000000; fa18_next_event=INT64_MAX;
    dma_offset=40+(n%32)*4; reset_bus(); fa18_write_log_active=1;
}
static int topology(uint32_t header) {
    uint32_t a=rd_u32(header+AMIGA_MEMORY_FIRST),sum=0,end=rd_u32(header+AMIGA_MEMORY_LOWER);
    unsigned count=0;
    while (a) {
        if (++count>8 || (a&7) || a<end || a+8>rd_u32(header+AMIGA_MEMORY_UPPER)) return 0;
        uint32_t size=rd_u32(a+4);
        if (!size || (size&7) || size>rd_u32(header+AMIGA_MEMORY_UPPER)-a) return 0;
        end=a+size; sum+=size; a=rd_u32(a);
    }
    return sum==rd_u32(header+AMIGA_MEMORY_FREE);
}
static int contract_matches(const Call *call) {
    /* The original zero-size AllocAbs at a chunk start self-links that chunk.
     * Preserve and explicitly assert that observed behavior, rather than invent
     * a successful empty allocation or a safe return value. */
    if (expected_self_link) { if (rd_u32(expected_result)!=expected_result) return 0; }
    else if (!topology(HEADER) || !topology(HEADER2)) return 0;
    if (rd_u32(HEADER+AMIGA_MEMORY_FREE)!=expected_total || rd_u32(HEADER2+AMIGA_MEMORY_FREE)!=expected_total2) return 0;
    if (call->entry!=0xFC1740 && call->entry!=0xFC182C && REG_D[0]!=expected_result) return 0;
    if (call->entry==0xFC17D0 && expected_result && (requirements&0x10000)) {
        unsigned clear_bytes=((uint32_t)(request+3)>>2)*4;
        for (unsigned i=0;i<clear_bytes;++i) if (rd_u8(expected_result+i)) return 0;
    }
    return 1;
}
static unsigned execute_memory(int candidate,int alert) {
    unsigned count=0;
    while (alert?REG_PC!=EXEC-108:REG_PC!=RETURN || REG_A[7]!=0xC7FC04) {
        if (++count>1000000) { fprintf(stderr,"memory service did not reach endpoint: %06X\n",REG_PC); exit(1); }
        if (candidate && (fa18_os_exec_memory_step() || fa18_os_exec_task_protection_step())) continue;
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
    if (!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error) || !fa18_os_exec_memory_signature_matches(m->rom)) return 1;
    free(state); free(rom); memcpy(base,m,sizeof *m); fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL);
    unsigned matched=0,alerts=0;
    for (unsigned bus=0;bus<2;++bus) {
        fa18_bus_timing=(int)bus;
        for (unsigned k=0;k<CALL_COUNT+5;++k)
        for (unsigned mode=0;mode<(k<CALL_COUNT?6:1);++mode) for (unsigned n=0;n<(k==CALL_COUNT+4?32:256);++n) {
            int alert=k>=CALL_COUNT && k<CALL_COUNT+4;
            const Call *call=&calls[k==CALL_COUNT+4?2:alert?(k==CALL_COUNT?3:1):k];
            memcpy(m,base,sizeof *m); memory_fixture(call,mode,n);
            if (alert) {
                REG_D[0]=16;
                const uint32_t addresses[]={0x400000,REGION,REGION+8,REGION-8};
                REG_A[1]=addresses[k-CALL_COUNT];
                const uint32_t a[]={REGION},s[]={32}; chunks(HEADER,a,s,1);
            } else if (k==CALL_COUNT+4) {
                /* 65,537 longword stores require the high-word clear loop. */
                memset(m->chip+REGION,0xA5,0x48000);
                wr_u32(HEADER+AMIGA_MEMORY_UPPER,REGION+0x48000);
                const uint32_t a[]={REGION},s[]={0x48000}; chunks(HEADER,a,s,1);
                request=REG_D[0]=0x40004; requirements=REG_D[1]=0x10003;
                expected_result=REGION; expected_total=0x48000-0x40008;
            }
            memcpy(before,m,sizeof *m); m68k_get_context(cpu); amiga_phase_observe_begin();
            unsigned want_count=execute_memory(0,alert);
            uint32_t regs[16]; memcpy(regs,REG_DA,sizeof regs);
            uint32_t sr=m68k_get_reg(NULL,M68K_REG_SR); int cycles=GET_CYCLES();
            if (alert?REG_D[7]!=(k==CALL_COUNT?0x81000005u:0x81000009u):!contract_matches(call)) {
                fprintf(stderr,"original memory contract differs %s kind=%u mode=%u fixture=%u\n",call->name,k,mode,n); return 1;
            }
            memcpy(ram,m->chip,FA18_CHIP_SIZE); memcpy(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE); amiga_phase_observe_reference();
            memcpy(m,before,sizeof *m); m68k_set_context(cpu); SET_CYCLES(100000000);
            memset(m->rom,0,sizeof m->rom); memset(m->rtarea,0,sizeof m->rtarea); fa18_machine_require_romfree(m);
            reset_bus(); amiga_phase_observe_begin(); unsigned got_count=execute_memory(1,alert);
            if (got_count!=want_count || memcmp(regs,REG_DA,sizeof regs) || sr!=m68k_get_reg(NULL,M68K_REG_SR) ||
                cycles!=GET_CYCLES() || memcmp(ram,m->chip,FA18_CHIP_SIZE) || memcmp(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE) ||
                !amiga_phase_observe_compare() || (!alert && !contract_matches(call)) ||
                m->runtime_guard.rom_reads || m->runtime_guard.rom_instruction_fetches || m->runtime_guard.unsupported_services) {
                fprintf(stderr,"%s differs bus=%u kind=%u mode=%u fixture=%u cycles=%d/%d\n",call->name,bus,k,mode,n,cycles,GET_CYCLES()); return 1;
            }
            if (alert) ++alerts; else ++matched;
        }
    }
    printf("Exec memory: %u complete CPU/DMA calls and %u original Alert-vector paths; free-list topology/accounting, flags, clearing, zero/wrapping sizes and coalescing match registers/SR/RAM/accesses/cycles with ROM removed\n",matched,alerts);
    free(cpu); free(ram); free(before); free(base); free(m); return 0;
}
