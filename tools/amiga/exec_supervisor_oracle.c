/* Original Supervisor calls, privilege frames, nested callbacks and Permit
 * callbacks from supervisor mode. No controlled substitute for Dispatch. */
#define main structural_phase_main
#include "service_phase_oracle.c"
#undef main
#include "../../port/amiga/abi_13.h"
enum { EXEC=0xC63000, CALLBACK=0xC64600, SECOND=0xC64680,
       RETURN=0xC10000, USER_SP=0xC7F800, SUPER_SP=0xC7FC00 };
static void supervisor_fixture(unsigned kind,unsigned n) {
    fa18_write_log_active=0;
    for (unsigned i=0;i<16;++i) REG_DA[i]=random_word();
    REG_A[6]=EXEC; REG_A[5]=CALLBACK;
    /* Establish both stack banks before choosing the caller's mode. */
    m68k_set_reg(M68K_REG_SR,0x2700); m68k_set_reg(M68K_REG_ISP,SUPER_SP);
    m68k_set_reg(M68K_REG_USP,USER_SP);
    int user=kind<4 && (kind&1);
    m68k_set_reg(M68K_REG_SR,(user?0x0700:0x2700)|(n&31));
    wr_u32(REG_A[7],RETURN); wr_u32(0x20,0xFC090E);
    wr_u16(EXEC-30,0x4EF9); wr_u32(EXEC-28,0xFC08E6);
    wr_u16(CALLBACK,0x5280); wr_u16(CALLBACK+2,0x4E73);
    if (kind==2 || kind==3) {
        wr_u16(CALLBACK,0x2F0D); /* save outer A5 */
        wr_u16(CALLBACK+2,0x4BF9); wr_u32(CALLBACK+4,SECOND);
        wr_u16(CALLBACK+8,0x4EAE); wr_u16(CALLBACK+10,0xFFE2);
        wr_u16(CALLBACK+12,0x2A5F); wr_u16(CALLBACK+14,0x4E73);
        wr_u16(SECOND,0x5280); wr_u16(SECOND+2,0x4E73);
    }
    wr_u8(EXEC+AMIGA_EXEC_ID_NEST_CNT,0xFF);
    wr_u8(EXEC+AMIGA_EXEC_TD_NEST_CNT,kind==4?0:0xFF);
    wr_u8(EXEC+AMIGA_EXEC_RESCHEDULE_FLAGS,kind==4?0x80:0);
    REG_PC=kind==4?0xFC1F9C:kind==5?0xFC1F74:0xFC08E6;
    SET_CYCLES(100000000); fa18_cycle_origin=100000000; fa18_next_event=INT64_MAX;
    dma_offset=40+(n%32)*4; reset_bus(); fa18_write_log_active=1;
}
static unsigned execute_supervisor(int candidate,uint32_t returned_sp) {
    unsigned count=0;
    while (REG_PC!=RETURN || REG_A[7]!=returned_sp) {
        if (++count>100) { fprintf(stderr,"Supervisor did not return: PC=%06X SP=%06X\n",REG_PC,REG_A[7]); exit(1); }
        if (candidate && (fa18_os_exec_supervisor_step() || fa18_os_exec_task_protection_step())) continue;
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
        !fa18_os_exec_supervisor_signature_matches(m->rom)) return 1;
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL);
    unsigned matched=0;
    for (unsigned bus=0;bus<2;++bus) {
        fa18_bus_timing=(int)bus;
        for (unsigned kind=0;kind<6;++kind) for (unsigned n=0;n<256;++n) {
            memcpy(m,base,sizeof *m); supervisor_fixture(kind,n);
            uint32_t returned_sp=REG_A[7]+4,old_d0=REG_D[0],old_a5=REG_A[5];
            uint32_t old_sr=m68k_get_reg(NULL,M68K_REG_SR);
            memcpy(before,m,sizeof *m); m68k_get_context(cpu); amiga_phase_observe_begin();
            unsigned want_count=execute_supervisor(0,returned_sp);
            uint32_t regs[16]; memcpy(regs,REG_DA,sizeof regs);
            uint32_t sr=m68k_get_reg(NULL,M68K_REG_SR),usp=m68k_get_reg(NULL,M68K_REG_USP),isp=m68k_get_reg(NULL,M68K_REG_ISP);
            int cycles=GET_CYCLES();
            if (kind<4 && (REG_D[0]!=old_d0+1 || sr!=old_sr || REG_A[5]!=old_a5)) {
                fprintf(stderr,"original Supervisor contract differs kind=%u\n",kind); return 1;
            }
            memcpy(ram,m->chip,FA18_CHIP_SIZE); memcpy(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
            amiga_phase_observe_reference();
            memcpy(m,before,sizeof *m); m68k_set_context(cpu); SET_CYCLES(100000000);
            memset(m->rom,0,sizeof m->rom); memset(m->rtarea,0,sizeof m->rtarea);
            fa18_machine_require_romfree(m); reset_bus(); amiga_phase_observe_begin();
            unsigned got_count=execute_supervisor(1,returned_sp);
            if (got_count!=want_count || memcmp(regs,REG_DA,sizeof regs) ||
                sr!=m68k_get_reg(NULL,M68K_REG_SR) || usp!=m68k_get_reg(NULL,M68K_REG_USP) ||
                isp!=m68k_get_reg(NULL,M68K_REG_ISP) || cycles!=GET_CYCLES() ||
                memcmp(ram,m->chip,FA18_CHIP_SIZE) || memcmp(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE) ||
                !amiga_phase_observe_compare() || m->runtime_guard.rom_reads ||
                m->runtime_guard.rom_instruction_fetches || m->runtime_guard.unsupported_services) {
                fprintf(stderr,"Supervisor differs: bus=%u kind=%u fixture=%u cycles=%d/%d\n",bus,kind,n,cycles,GET_CYCLES()); return 1;
            }
            ++matched;
        }
    }
    printf("Exec Supervisor: %u complete CPU/DMA calls, user/supervisor and nested callbacks, both stack banks, Permit/Reschedule supervisor callbacks; registers/SR/RAM/accesses/cycles match with ROM removed\n",matched);
    free(cpu); free(ram); free(before); free(base); free(m); return 0;
}
