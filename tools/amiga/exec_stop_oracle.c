/* Exercise m68k_execute and its real service hook, not a direct STOP call.
 * An IRQ accepted by STOP may clear the stopped flag immediately; neither
 * that IRQ's first instruction nor the stopped continuation may be fetched. */
#define AMIGA_SCHEDULER_NO_MAIN
#include "exec_scheduler_oracle.c"
#undef AMIGA_SCHEDULER_NO_MAIN
int main(void) {
    size_t ns,nr; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&ns),*rom=read_file("local/system/kick13.rom",&nr);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *ram=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE),*cpu_state=malloc(m68k_context_size());
    if (!state || !rom || !m || !base || !before || !ram || !cpu_state) return 1;
    if (!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error) ||
        !fa18_os_exec_scheduler_signature_matches(m->rom)) return 1;
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(0); fa18_ports_init(FA18_PORTS_OFF,NULL);
    unsigned matched=0;
    /* Preserve the routine-scoped resume boundary for all context returns.
     * The outer instruction hook must own the eventual C RTE/STOP execution. */
    const uint32_t boundaries[]={0xFC0EC0,0xFC0FF0,0xFC1074,0xFC0F90,0xFC0C8C,0xFC0E9A};
    for (unsigned k=0;k<sizeof boundaries/sizeof boundaries[0];++k) {
        memcpy(m,base,sizeof *m); scheduler_fixture(3,0);
        REG_PC=boundaries[k]; REG_IR=0;
        memset(m->rom,0,sizeof m->rom); memset(m->rtarea,0,sizeof m->rtarea);
        fa18_machine_require_romfree(m); fa18_services_reset();
        fa18_service_enable(k<4?FA18_SERVICE_EXEC_SCHEDULER:FA18_SERVICE_EXEC_IRQ_ROOTS,1);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu_state);
        int cycles=GET_CYCLES();
        if (fa18_recomp_resume(RETURN,USER_SP)!=FA18_EXIT_INTERP || REG_PC!=boundaries[k] ||
            cycles!=GET_CYCLES() || memcmp(REG_DA,((m68ki_cpu_core *)cpu_state)->dar,sizeof REG_DA) ||
            memcmp(m->chip,before->chip,FA18_CHIP_SIZE) || memcmp(m->slow,before->slow,FA18_SLOW_SIZE) ||
            m->runtime_guard.rom_reads || m->runtime_guard.rom_instruction_fetches || m->runtime_guard.unsupported_services) {
            fprintf(stderr,"routine resume crossed service context boundary %06X\n",boundaries[k]); return 1;
        }
    }
    for (unsigned bus=0;bus<2;++bus) {
        fa18_bus_timing=(int)bus;
        for (unsigned irq=0;irq<2;++irq) for (unsigned n=0;n<256;++n) {
            memcpy(m,base,sizeof *m); scheduler_fixture(3,n);
            REG_PC=irq?0xFC0F90:0xFC0F66; REG_IR=0;
            wr_u32(0x6C,CALLBACK); wr_u16(CALLBACK,0x4AFC);
            /* Poison the IRQ instruction: this slice must stop before it. */
            m68k_set_irq(irq?3:0); SET_CYCLES(1024);
            fa18_cycle_origin=1024; fa18_next_event=INT64_MAX;
            memcpy(before,m,sizeof *m); m68k_get_context(cpu_state);
            fa18_services_reset(); reset_bus(); amiga_phase_observe_begin();
            int elapsed=m68k_execute(1024); fa18_bus_finish(REG_PC);
            if (irq ? CPU_STOPPED || REG_PC!=CALLBACK : !CPU_STOPPED || REG_PC!=0xFC0F94) {
                fprintf(stderr,"original STOP contract bus=%u irq=%u n=%u PC=%06X stopped=%u\n",bus,irq,n,REG_PC,CPU_STOPPED); return 1;
            }
            uint32_t regs[16]; memcpy(regs,REG_DA,sizeof regs);
            uint32_t pc=REG_PC,ppc=REG_PPC,sr=m68ki_get_sr(),stopped=CPU_STOPPED;
            uint32_t usp=m68k_get_reg(NULL,M68K_REG_USP),isp=m68k_get_reg(NULL,M68K_REG_ISP);
            int cycles=GET_CYCLES();
            memcpy(ram,m->chip,FA18_CHIP_SIZE); memcpy(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
            amiga_phase_observe_reference();
            memcpy(m,before,sizeof *m); m68k_set_context(cpu_state); SET_CYCLES(1024);
            memset(m->rom,0,sizeof m->rom); memset(m->rtarea,0,sizeof m->rtarea);
            fa18_machine_require_romfree(m); fa18_service_enable(FA18_SERVICE_EXEC_SCHEDULER,1);
            reset_bus(); amiga_phase_observe_begin();
            int got=m68k_execute(1024); fa18_bus_finish(REG_PC);
            if (got!=elapsed || pc!=REG_PC || ppc!=REG_PPC || stopped!=CPU_STOPPED || memcmp(regs,REG_DA,sizeof regs) ||
                sr!=m68ki_get_sr() || usp!=m68k_get_reg(NULL,M68K_REG_USP) || isp!=m68k_get_reg(NULL,M68K_REG_ISP) ||
                cycles!=GET_CYCLES() || memcmp(ram,m->chip,FA18_CHIP_SIZE) ||
                memcmp(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE) || !amiga_phase_observe_compare() ||
                m->runtime_guard.rom_reads || m->runtime_guard.rom_instruction_fetches || m->runtime_guard.unsupported_services) {
                fprintf(stderr,"STOP hook differs bus=%u irq=%u n=%u PC=%06X/%06X stopped=%u/%u cycles=%d/%d\n",
                    bus,irq,n,pc,REG_PC,stopped,CPU_STOPPED,cycles,GET_CYCLES()); return 1;
            }
            ++matched;
        }
    }
    printf("Exec STOP: %u real instruction-hook CPU/DMA calls and six routine-resume context boundaries, idle queues and immediately accepted IRQs; exact CPU/RAM/access/cycle parity, zero ROM accesses and no continuation/IRQ opcode fetched\n",matched);
    free(cpu_state); free(ram); free(before); free(base); free(m); return 0;
}
