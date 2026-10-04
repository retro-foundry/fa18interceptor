/* Pinned 1.3 68000 scheduler ABI/timing. Task/list/context behavior lives in
 * amiga_compat; guest callbacks, interrupts and STOP stay on the machine clock. */
#include "exec_scheduler_adapter.h"
#include "../amiga/exec_scheduler.h"
#include "../amiga/abi_13.h"
#include "exec_service_state.h"
#include "service_dispatch_adapter.h"
#include "machine.h"
#include "m68kops.h"
#include <string.h>
#include <stdlib.h>
int fa18_os_exec_scheduler_signature_matches(const uint8_t *rom) {
    uint32_t hash=2166136261u;
    if (!rom) return 0;
    for (unsigned i=0xE9C;i<0x10C6;++i) hash=(hash^rom[i])*16777619u;
    return hash==0x97882438u;
}
int fa18_os_exec_scheduler_enable_reference(void) {
    int enabled=fa18_os_exec_scheduler_signature_matches(fa18_machine->rom);
    fa18_service_enable(FA18_SERVICE_EXEC_SCHEDULER,enabled); return enabled;
}
static int semantic(uint32_t pc,uint16_t op,unsigned ext,AmigaExecSchedulerPhase p,unsigned arg) {
    AmigaExecTaskState s; fa18_exec_service_load(&s);
    unsigned late=p==AMIGA_SCHED_INSTALL_SWITCH_RETURN || p==AMIGA_SCHED_SAVE_CALLER_A5 ||
        p==AMIGA_SCHED_RESET_QUANTUM || p==AMIGA_SCHED_RESTORE_NEST_COUNTS || p==AMIGA_SCHED_RESTORE_TASK_HEADER;
    AmigaExecTaskBus ordered=fa18_exec_service_bus(&late);
    fa18_service_begin(pc,op); fa18_service_extension_words(ext-late);
    if (!amiga_exec_scheduler_step(p,arg,&s,&ordered) || late) abort(); fa18_exec_service_store(&s);
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
static int task(uint32_t pc,uint16_t op,unsigned ext,AmigaExecTaskPhase p,unsigned arg) {
    AmigaExecTaskBus bus=fa18_exec_service_bus(NULL);
    AmigaExecTaskState s; AmigaExecTaskEffect e; fa18_exec_service_load(&s);
    fa18_service_begin(pc,op); fa18_service_extension_words(ext);
    if (!amiga_exec_task_step(p,arg,&s,&bus,&e)) abort(); fa18_exec_service_store(&s);
    if (e.returned) REG_PC=e.return_pc;
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
static int context(uint32_t pc,uint16_t op,unsigned base,unsigned mask,int save,int increment) {
    AmigaExecTaskBus bus=fa18_exec_service_bus(NULL);
    AmigaExecTaskState s; unsigned count; fa18_exec_service_load(&s);
    fa18_service_begin(pc,op); fa18_service_extension_words(1);
    int ok=save?amiga_exec_context_save(&s,&bus,base,mask,&count):
        amiga_exec_context_restore(&s,&bus,base,mask,increment,&count);
    if (!ok) abort(); fa18_exec_service_store(&s);
    USE_CYCLES(count<<CYC_MOVEM_L); USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
static int branch(uint32_t pc,uint16_t op,int taken,uint32_t target) {
    fa18_service_begin(pc,op);
    if (taken) REG_PC=target; else USE_CYCLES(CYC_BCC_NOTAKE_B);
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
static int transfer(uint32_t pc,uint16_t op,unsigned ext,uint32_t target,int call) {
    fa18_service_begin(pc,op); fa18_service_extension_words(ext);
    if (call) fa18_service_call(target); else { m68ki_trace_t0(); m68ki_jump(target); }
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
static int address(uint32_t pc,uint16_t op,unsigned reg,uint32_t value) {
    fa18_service_begin(pc,op); fa18_service_extension_words(1); REG_A[reg]=value;
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
static int cpu(uint32_t pc,uint16_t op,unsigned value) {
    fa18_service_begin(pc,op);
    if (op==0x4E73) m68ki_instruction_jump_table[op]();
    else if (!FLAG_S) m68ki_exception_privilege_violation();
    else if (op==0x46FC || op==0x4E72) {
        fa18_service_extension_words(1); m68ki_trace_t0();
        if (op==0x4E72) CPU_STOPPED|=STOP_LEVEL_STOP;
        m68ki_set_sr(value);
        if (op==0x4E72) {
            if (GET_CYCLES()>=CYC_INSTRUCTION[op]) SET_CYCLES(CYC_INSTRUCTION[op]);
            else USE_ALL_CYCLES();
        }
    } else if ((op&0xFFF8)==0x4E68) REG_A[op&7]=REG_USP;
    else if ((op&0xFFF8)==0x4E60) { m68ki_trace_t0(); REG_USP=REG_A[op&7]; }
    else abort();
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
#define STEP(op,ext,phase,arg) return semantic(pc,op,ext,AMIGA_SCHED_##phase,arg)
#define TASK(op,ext,phase,arg) return task(pc,op,ext,AMIGA_EXEC_##phase,arg)
#define BR(op,cond,target) return branch(pc,op,cond,target)
#define GO(op,ext,target) return transfer(pc,op,ext,target,0)
#define CALL(op,ext,target) return transfer(pc,op,ext,target,1)
#define CPU(op,value) return cpu(pc,op,value)
int fa18_os_exec_scheduler_requires_outer_dispatch(uint32_t pc) {
    /* A routine-scoped resume cannot follow a task/exception frame return or
     * idle STOP into another execution context. The outer hook executes these
     * C phases, preserving the existing RTE/STOP dispatch boundary. */
    return pc==0xFC0EC0 || pc==0xFC0FF0 || pc==0xFC1074 || pc==0xFC0F90;
}
int fa18_os_exec_scheduler_step(void) {
    uint32_t pc=REG_PC;
    switch (pc) {
    case 0xFC0E9Cu: STEP(0x082F,2,SAVED_SUPERVISOR,0);
    case 0xFC0EA2u: BR(0x6618,COND_NE(),0xFC0EBC);
    case 0xFC0EA4u: case 0xFC0F28u: case 0xFC1076u: STEP(0x2C78,1,EXEC_BASE,0);
    case 0xFC0EA8u: TASK(0x4A2E,1,TEST_DEPTH,AMIGA_EXEC_TD_NEST_CNT);
    case 0xFC0EACu: BR(0x6C0E,COND_GE(),0xFC0EBC);
    case 0xFC0EAEu: STEP(0x082E,2,TEST_RESCHEDULE_BIT,7);
    case 0xFC0EB4u: BR(0x6706,COND_EQ(),0xFC0EBC);
    case 0xFC0EB6u: case 0xFC0F08u: case 0xFC0F1Cu: case 0xFC0FCCu: CPU(0x46FC,0x2000);
    case 0xFC0EBAu: GO(0x600A,0,0xFC0EC6);
    case 0xFC0EBCu: return context(pc,0x4CDF,7,0x6303,0,1);
    case 0xFC0EC0u: case 0xFC0FF0u: case 0xFC1074u: CPU(0x4E73,0);
    case 0xFC0EC2u: return context(pc,0x48E7,7,0x6303,1,0);
    case 0xFC0EC6u: case 0xFC0F7Cu: CPU(0x46FC,0x2700);
    case 0xFC0ECAu: STEP(0x08AE,2,CLEAR_RESCHEDULE_BIT,7);
    case 0xFC0ED0u: TASK(0x226E,1,CURRENT_TASK,0);
    case 0xFC0ED4u: STEP(0x0829,2,TEST_TASK_EXCEPTION,0);
    case 0xFC0EDAu: BR(0x661E,COND_NE(),0xFC0EFA);
    case 0xFC0EDCu: case 0xFC0EFAu: case 0xFC0F78u: TASK(0x41EE,1,EXEC_LIST,AMIGA_EXEC_TASK_READY);
    case 0xFC0EE0u: STEP(0xB1E8,1,READY_EMPTY,0);
    case 0xFC0EE4u: BR(0x67D6,COND_EQ(),0xFC0EBC);
    case 0xFC0EE6u: STEP(0x2050,0,READY_HEAD_A0,0);
    case 0xFC0EE8u: STEP(0x1228,1,READY_PRIORITY,0);
    case 0xFC0EECu: STEP(0xB229,1,COMPARE_PRIORITY,0);
    case 0xFC0EF0u: BR(0x6C08,COND_GE(),0xFC0EFA);
    case 0xFC0EF2u: STEP(0x082E,2,TEST_RESCHEDULE_BIT,6);
    case 0xFC0EF8u: BR(0x67C2,COND_EQ(),0xFC0EBC);
    case 0xFC0EFEu: CALL(0x6100,1,0xFC1670);
    case 0xFC0F02u: TASK(0x137C,2,TASK_STATE,3);
    case 0xFC0F0Cu: return context(pc,0x4CDF,7,0x2303,0,1);
    case 0xFC0F10u: STEP(0x2F17,0,PUSH_SAVED_A6,0);
    case 0xFC0F12u: STEP(0x2F6E,2,INSTALL_SWITCH_RETURN,0);
    case 0xFC0F18u: TASK(0x2C5F,0,POP_A,6);
    case 0xFC0F1Au: case 0xFC1008u: case 0xFC10C4u: TASK(0x4E75,0,RETURN,0);
    case 0xFC0F20u: TASK(0x2F0D,0,PUSH_A,5);
    case 0xFC0F22u: CPU(0x4E6D,0);
    case 0xFC0F24u: return context(pc,0x48E5,5,0x7FFF,1,0);
    case 0xFC0F2Cu: STEP(0x302E,1,LOAD_NEST_COUNTS,0);
    case 0xFC0F30u: case 0xFC0F6Au: STEP(0x3D7C,2,RESET_NEST_COUNTS,0);
    case 0xFC0F36u: case 0xFC0F70u: case 0xFC1032u: case 0xFC104Eu: TASK(0x33FC,3,INTERRUPT_WORD,0xC000);
    case 0xFC0F3Eu: STEP(0x2B5F,1,SAVE_CALLER_A5,0);
    case 0xFC0F42u: STEP(0x3B1F,0,SAVE_STATUS,0);
    case 0xFC0F44u: STEP(0x2B1F,0,SAVE_PC,0);
    case 0xFC0F46u: case 0xFC0F66u: case 0xFC1082u: return address(pc,0x49FA,4,0xFC0FE2);
    case 0xFC0F4Au: case 0xFC109Eu: STEP(0x266E,1,CURRENT_TASK_A3,0);
    case 0xFC0F4Eu: STEP(0x3740,1,SAVE_TASK_NEST_COUNTS,0);
    case 0xFC0F52u: STEP(0x274D,1,SAVE_TASK_SP,0);
    case 0xFC0F56u: STEP(0x082B,2,TEST_TASK_SWITCH,0);
    case 0xFC0F5Cu: BR(0x671A,COND_EQ(),0xFC0F78);
    case 0xFC0F5Eu: STEP(0x2A6B,1,LOAD_SWITCH_CALLBACK,0);
    case 0xFC0F62u: case 0xFC0FFEu: CALL(0x4E95,0,REG_A[5]);
    case 0xFC0F64u: GO(0x6012,0,0xFC0F78);
    case 0xFC0F80u: STEP(0x2650,0,READY_HEAD_A3,0);
    case 0xFC0F82u: STEP(0x2013,0,READY_SUCCESSOR,0);
    case 0xFC0F84u: BR(0x6610,COND_NE(),0xFC0F96);
    case 0xFC0F86u: STEP(0x52AE,1,INCREMENT_COUNTER,AMIGA_EXEC_IDLE_COUNT);
    case 0xFC0F8Au: STEP(0x08EE,2,SET_RESCHEDULE_BIT,7);
    case 0xFC0F90u: CPU(0x4E72,0x2000);
    case 0xFC0F94u: GO(0x60E6,0,0xFC0F7C);
    case 0xFC0F96u: STEP(0x2080,0,REMOVE_READY_HEAD,0);
    case 0xFC0F98u: STEP(0x2240,0,READY_SUCCESSOR_A1,0);
    case 0xFC0F9Au: STEP(0x2348,1,LINK_READY_SUCCESSOR,0);
    case 0xFC0F9Eu: STEP(0x52AE,1,INCREMENT_COUNTER,AMIGA_EXEC_DISPATCH_COUNT);
    case 0xFC0FA2u: STEP(0x2D4B,1,INSTALL_CURRENT_TASK,0);
    case 0xFC0FA6u: STEP(0x3D6E,2,RESET_QUANTUM,0);
    case 0xFC0FACu: STEP(0x08AE,2,CLEAR_RESCHEDULE_BIT,6);
    case 0xFC0FB2u: STEP(0x177C,2,SET_TASK_STATE,2);
    case 0xFC0FB8u: case 0xFC10B0u: STEP(0x3D6B,2,RESTORE_NEST_COUNTS,0);
    case 0xFC0FBEu: case 0xFC1042u: case 0xFC10B6u: TASK(0x4A2E,1,TEST_DEPTH,AMIGA_EXEC_ID_NEST_CNT);
    case 0xFC0FC2u: BR(0x6B08,COND_MI(),0xFC0FCC);
    case 0xFC0FC4u: case 0xFC1010u: case 0xFC10BCu: TASK(0x33FC,3,INTERRUPT_WORD,0x4000);
    case 0xFC0FD0u: STEP(0x102B,1,LOAD_TASK_FLAGS,0);
    case 0xFC0FD4u: STEP(0x0200,1,CALLBACK_FLAGS,0);
    case 0xFC0FD8u: BR(0x6702,COND_EQ(),0xFC0FDC);
    case 0xFC0FDAu: CALL(0x6116,0,0xFC0FF2);
    case 0xFC0FDCu: STEP(0x2A6B,1,LOAD_TASK_SP_A5,0);
    case 0xFC0FE0u: GO(0x4ED4,0,REG_A[4]);
    case 0xFC0FE2u: STEP(0x45ED,1,RESUMED_USP_A2,0);
    case 0xFC0FE6u: CPU(0x4E62,0);
    case 0xFC0FE8u: STEP(0x2F1D,0,PUSH_TASK_PC,0);
    case 0xFC0FEAu: STEP(0x3F1D,0,PUSH_TASK_STATUS,0);
    case 0xFC0FECu: return context(pc,0x4CD5,5,0x7FFF,0,0);
    case 0xFC0FF2u: STEP(0x0800,1,TEST_CALLBACK_BIT,7);
    case 0xFC0FF6u: BR(0x670A,COND_EQ(),0xFC1002);
    case 0xFC0FF8u: STEP(0x1400,0,SAVE_CALLBACK_FLAGS,0);
    case 0xFC0FFAu: STEP(0x2A6B,1,LOAD_LAUNCH_CALLBACK,0);
    case 0xFC1000u: STEP(0x1002,0,RESTORE_CALLBACK_FLAGS,0);
    case 0xFC1002u: STEP(0x0800,1,TEST_CALLBACK_BIT,5);
    case 0xFC1006u: BR(0x6602,COND_NE(),0xFC100A);
    case 0xFC100Au: STEP(0x08AB,2,CLEAR_TASK_EXCEPTION,0);
    case 0xFC1018u: TASK(0x522E,1,DEPTH_UP,AMIGA_EXEC_ID_NEST_CNT);
    case 0xFC101Cu: STEP(0x202B,1,LOAD_RECEIVED,0);
    case 0xFC1020u: STEP(0xC0AB,1,EXCEPTION_SIGNALS,0);
    case 0xFC1024u: STEP(0xB1AB,1,CLEAR_SIGNALS,AMIGA_TASK_SIGNALS_EXCEPT);
    case 0xFC1028u: STEP(0xB1AB,1,CLEAR_SIGNALS,AMIGA_TASK_SIGNALS_RECEIVED);
    case 0xFC102Cu: case 0xFC1048u: TASK(0x532E,1,DEPTH_DOWN,AMIGA_EXEC_ID_NEST_CNT);
    case 0xFC1030u: BR(0x6C08,COND_GE(),0xFC103A);
    case 0xFC103Au: STEP(0x226B,1,LOAD_TASK_SP_A1,0);
    case 0xFC103Eu: STEP(0x232B,1,PUSH_TASK_HEADER,0);
    case 0xFC1046u: BR(0x660E,COND_NE(),0xFC1056);
    case 0xFC104Cu: BR(0x6C08,COND_GE(),0xFC1056);
    case 0xFC1056u: STEP(0x233C,2,PUSH_EXCEPTION_RETURN,0xFC1076);
    case 0xFC105Cu: CPU(0x4E61,0);
    case 0xFC105Eu: case 0xFC1086u: STEP(0x082E,2,TEST_CPU_FLAG,0);
    case 0xFC1064u: BR(0x6704,COND_EQ(),0xFC106A);
    case 0xFC1066u: STEP(0x3F3C,1,PUSH_FRAME_FORMAT,0);
    case 0xFC106Au: STEP(0x2F2B,1,PUSH_EXCEPTION_CODE,0);
    case 0xFC106Eu: STEP(0x4267,0,PUSH_USER_STATUS,0);
    case 0xFC1070u: STEP(0x226B,1,EXCEPTION_DATA,0);
    case 0xFC107Au: return address(pc,0x4BFA,5,0xFC1082);
    case 0xFC107Eu: GO(0x4EEE,1,REG_A[6]-30);
    case 0xFC108Cu: BR(0x670E,COND_EQ(),0xFC109C);
    case 0xFC108Eu: fa18_service_begin(pc,0x548F); REG_A[7]+=2; USE_CYCLES(CYC_INSTRUCTION[0x548F]); return 1;
    case 0xFC1090u: STEP(0x082E,2,TEST_CPU_FLAG,4);
    case 0xFC1096u: BR(0x6704,COND_EQ(),0xFC109C);
    case 0xFC1098u: return address(pc,0x49FA,4,0xFC112C);
    case 0xFC109Cu: fa18_service_begin(pc,0x5C8F); REG_A[7]+=6; USE_CYCLES(CYC_INSTRUCTION[0x5C8F]); return 1;
    case 0xFC10A2u: STEP(0x81AB,1,ADD_RETURNED_SIGNALS,0);
    case 0xFC10A6u: CPU(0x4E69,0);
    case 0xFC10A8u: STEP(0x2759,1,RESTORE_TASK_HEADER,0);
    case 0xFC10ACu: STEP(0x2749,1,SAVE_EXCEPTION_SP,0);
    case 0xFC10BAu: BR(0x6B08,COND_MI(),0xFC10C4);
    default: return 0;
    }
}
