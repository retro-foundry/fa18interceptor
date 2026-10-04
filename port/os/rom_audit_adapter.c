#include "rom_audit_adapter.h"
#include <stdio.h>
#include <stdlib.h>
#include "machine.h"
#include "m68kcpu.h"
extern int fa18_write_log_active;
static AmigaRomAudit *audit;
int fa18_rom_audit_open(void) {
    const AmigaAuditRange ranges[]={{0xF80000u,0x1000000u},{0xF00000u,0xF10000u}};
    amiga_rom_audit_destroy(audit);
    audit=amiga_rom_audit_create(ranges,sizeof ranges/sizeof ranges[0],262144);
    return audit!=NULL;
}
static void failed(void) {
    fprintf(stderr,"ROM audit exhausted its inventory at pc=%06X cycle=%llu frame=%llu; no complete report is accepted\n",
        REG_PC,(unsigned long long)fa18_machine_now(),(unsigned long long)fa18_machine->frame);
    abort();
}
void fa18_rom_audit_instruction(uint32_t pc) {
    if (!audit || fa18_write_log_active) return;
    AmigaAuditCpu cpu;
    for (unsigned i=0; i<8; ++i) { cpu.d[i]=REG_D[i]; cpu.a[i]=REG_A[i]; }
    cpu.pc=pc; cpu.sr=(uint16_t)m68k_get_reg(NULL,M68K_REG_SR);
    cpu.cycle=(uint64_t)fa18_machine_now(); cpu.frame=fa18_machine->frame;
    if (!amiga_rom_audit_instruction(audit,&cpu)) failed();
}
void fa18_rom_audit_access(AmigaAuditKind kind,uint32_t address,unsigned size,uint32_t value) {
    if (!audit || fa18_write_log_active) return;
    address&=0xFFFFFFu;
    if (kind==AMIGA_AUDIT_DATA_READ && address<0x400u) kind=AMIGA_AUDIT_VECTOR_READ;
    if (kind==AMIGA_AUDIT_DATA_WRITE && address<0x400u) kind=AMIGA_AUDIT_VECTOR_WRITE;
    if (!amiga_rom_audit_access(audit,kind,REG_PPC,address,size,value,
        (uint64_t)fa18_machine_now(),fa18_machine->frame)) failed();
}
int fa18_rom_audit_finish(const char *path) {
    int result=amiga_rom_audit_write(audit,path);
    amiga_rom_audit_destroy(audit); audit=NULL;
    return result;
}
