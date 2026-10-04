/* Exec 1.3 Supervisor and its 68000 privilege-frame continuation. The retained
 * CPU owns stack-bank changes, privilege exceptions and RTE; no ROM opcode or
 * operand is fetched. Service-owned frame semantics live in amiga_compat. */
#include "exec_supervisor.h"
#include "../amiga/exec_task_services.h"
#include "service_dispatch_adapter.h"
#include "service_phase.h"
#include "machine.h"
#include "m68kops.h"
#include <string.h>
#include <stdlib.h>
int fa18_os_exec_supervisor_signature_matches(const uint8_t *r) {
    static const uint8_t entry[]={0x00,0x7C,0x20,0x00,0x48,0x79,0x00,0xFC,0x08,0xF4,0x40,0xE7,0x4E,0xD5,0x4E,0x75};
    static const uint8_t handler[]={0x0C,0xAF,0x00,0xFC,0x08,0xE6,0x00,0x02,0x67,0x0A,
        0x0C,0xAF,0x00,0xFC,0x08,0xF6,0x00,0x02,0x66,0x0A,
        0x2F,0x7C,0x00,0xFC,0x08,0xF4,0x00,0x02,0x4E,0xD5};
    static const uint8_t callback[]={0x08,0x17,0x00,0x05,0x67,0x02,0x4E,0x73,0x4E,0xEE,0xFF,0xD6};
    return r && !memcmp(r+0x8E6,entry,sizeof entry) && !memcmp(r+0x90E,handler,sizeof handler) &&
        !memcmp(r+0x1FBE,callback,sizeof callback);
}
int fa18_os_exec_supervisor_enable_reference(void) {
    int enabled=fa18_os_exec_supervisor_signature_matches(fa18_machine->rom);
    fa18_service_enable(FA18_SERVICE_EXEC_SUPERVISOR,enabled);
    fa18_service_enable(FA18_SERVICE_EXEC_SUPERVISOR_EXCEPTION,enabled);
    fa18_service_enable(FA18_SERVICE_EXEC_PERMIT_CALLBACK,enabled);
    return enabled;
}
static uint8_t read8(void *c,uint32_t a) { (void)c; return (uint8_t)m68k_read_memory_8(a); }
static uint16_t read16(void *c,uint32_t a) { (void)c; return (uint16_t)m68k_read_memory_16(a); }
static uint32_t read32(void *c,uint32_t a) { (void)c; return m68k_read_memory_32(a); }
static void write8(void *c,uint32_t a,uint8_t v) { (void)c; m68k_write_memory_8(a,v); }
static void write16(void *c,uint32_t a,uint16_t v) { (void)c; m68k_write_memory_16(a,v); }
static void write32(void *c,uint32_t a,uint32_t v) { (void)c; m68k_write_memory_32(a,v); }
static void semantic(AmigaExecTaskPhase phase,unsigned arg) {
    AmigaExecTaskState s; AmigaExecTaskEffect e;
    AmigaExecTaskBus b={NULL,read8,read16,read32,write8,write16,write32};
    memcpy(s.d,REG_D,sizeof s.d); memcpy(s.a,REG_A,sizeof s.a);
    s.ccr=(uint8_t)m68ki_get_ccr();
    if (!amiga_exec_task_step(phase,arg,&s,&b,&e)) abort();
    memcpy(REG_D,s.d,sizeof s.d); memcpy(REG_A,s.a,sizeof s.a);
    m68ki_set_ccr(s.ccr);
    if (e.returned) REG_PC=e.return_pc;
}
int fa18_os_exec_supervisor_step(void) {
    uint32_t pc=REG_PC; uint16_t op;
    switch (pc) {
    case 0xFC08E6u: op=0x007C; break;
    case 0xFC08EAu: op=0x4879; break;
    case 0xFC08F0u: op=0x40E7; break;
    case 0xFC08F2u: case 0xFC092Au: op=0x4ED5; break;
    case 0xFC08F4u: op=0x4E75; break;
    case 0xFC090Eu: case 0xFC0918u: op=0x0CAF; break;
    case 0xFC0916u: op=0x670A; break;
    case 0xFC0920u: op=0x660A; break;
    case 0xFC0922u: op=0x2F7C; break;
    case 0xFC1FBEu: op=0x0817; break;
    case 0xFC1FC2u: op=0x6702; break;
    case 0xFC1FC4u: op=0x4E73; break;
    case 0xFC1FC6u: op=0x4EEE; break;
    default: return 0;
    }
    fa18_service_begin(pc,op);
    switch (pc) {
    case 0xFC08E6u:
        if (FLAG_S) {
            fa18_service_extension_words(1); m68ki_trace_t0();
            m68ki_set_sr(m68ki_get_sr()|0x2000);
        } else m68ki_exception_privilege_violation();
        break;
    case 0xFC08EAu:
        fa18_service_extension_words(2); semantic(AMIGA_EXEC_PUSH_RETURN_PC,0xFC08F4); break;
    case 0xFC08F0u: semantic(AMIGA_EXEC_PUSH_SR,m68ki_get_sr()); break;
    case 0xFC08F2u: case 0xFC092Au:
        m68ki_trace_t0(); m68ki_jump(REG_A[5]); break;
    case 0xFC08F4u: semantic(AMIGA_EXEC_RETURN,0); break;
    case 0xFC090Eu: case 0xFC0918u:
        fa18_service_extension_words(3);
        semantic(AMIGA_EXEC_COMPARE_EXCEPTION_CALL,pc==0xFC090E?0xFC08E6:0xFC08F6); break;
    case 0xFC0916u: case 0xFC0920u: case 0xFC1FC2u:
        if (pc==0xFC0920?COND_NE():COND_EQ()) REG_PC=pc==0xFC0920?0xFC092C:pc==0xFC0916?0xFC0922:0xFC1FC6;
        else USE_CYCLES(CYC_BCC_NOTAKE_B);
        break;
    case 0xFC0922u:
        fa18_service_extension_words(3); semantic(AMIGA_EXEC_SET_EXCEPTION_RETURN,0xFC08F4); break;
    case 0xFC1FBEu:
        fa18_service_extension_words(1); semantic(AMIGA_EXEC_TEST_SAVED_SUPERVISOR,0); break;
    case 0xFC1FC4u:
        /* CPU exception-frame operation; the fixed instruction has no operands.
         * Preserves CPU callback/state rules without executing any ROM bytes. */
        m68ki_instruction_jump_table[0x4E73](); break;
    case 0xFC1FC6u:
        fa18_service_extension_words(1); m68ki_trace_t0(); m68ki_jump(REG_A[6]-42); break;
    }
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
