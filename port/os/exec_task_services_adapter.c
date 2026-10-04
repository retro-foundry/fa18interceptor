/* 1.3 library addresses and 68000 bus phases. Task/message behavior is in
 * the SDK-free reusable Exec layer; nested calls stay on the guest timeline. */
#include "exec_task_services_adapter.h"
#include "../amiga/exec_task_services.h"
#include "../amiga/abi_13.h"
#include "service_phase.h"
#include "m68kops.h"
#include "machine.h"
#include "service_dispatch_adapter.h"
#include <string.h>

/* Full source intervals are checked by the differential phase registry.
 * These entry signatures restrict activation to the pinned reference ABI. */
int fa18_os_exec_messages_signature_matches(const uint8_t *r) {
    static const uint8_t put[]={0x22,0x08,0x41,0xE8,0x00,0x14,0x33,0xFC,0x40,0x00};
    static const uint8_t reply[]={0x20,0x29,0x00,0x0E,0x66,0x08,0x13,0x7C,0x00,0x06,0x00,0x08};
    static const uint8_t wait[]={0x22,0x68,0x00,0x14,0x4A,0x91,0x66,0x1C,0x12,0x28,0x00,0x0F};
    return r && !memcmp(r+0x1B76,put,sizeof put) && !memcmp(r+0x1C18,reply,sizeof reply) && !memcmp(r+0x1C32,wait,sizeof wait);
}
int fa18_os_exec_signals_signature_matches(const uint8_t *r) {
    static const uint8_t sets[]={0x22,0x6E,0x01,0x14,0x41,0xE9,0x00,0x1E,0x60,0x08,0x22,0x6E,0x01,0x14,0x41,0xE9,0x00,0x1A};
    static const uint8_t signal[]={0x41,0xE9,0x00,0x1A,0x33,0xFC,0x40,0x00,0x00,0xDF,0xF0,0x9A};
    static const uint8_t wait[]={0x22,0x6E,0x01,0x14,0x23,0x40,0x00,0x16,0x33,0xFC,0x40,0x00};
    static const uint8_t alloc[]={0x22,0x6E,0x01,0x14,0x22,0x29,0x00,0x12,0x0C,0x00,0x00,0xFF};
    return r && !memcmp(r+0x1E54,sets,sizeof sets) && !memcmp(r+0x1E84,signal,sizeof signal) &&
        !memcmp(r+0x1F0C,wait,sizeof wait) && !memcmp(r+0x2000,alloc,sizeof alloc);
}
int fa18_os_exec_task_protection_signature_matches(const uint8_t *r) {
    static const uint8_t reschedule[]={0x08,0xEE,0x00,0x07,0x01,0x24,0x56,0xC0,0x4A,0x2E,0x01,0x27};
    static const uint8_t protect[]={0x52,0x2E,0x01,0x27,0x4E,0x75,0x53,0x2E,0x01,0x27,0x6C,0x1A};
    return r && !memcmp(r+0x1F74,reschedule,sizeof reschedule) && !memcmp(r+0x1F96,protect,sizeof protect);
}
int fa18_os_exec_task_services_enable_reference(void) {
    const uint8_t *rom=fa18_machine->rom;
    int messages=fa18_os_exec_messages_signature_matches(rom);
    int signals=fa18_os_exec_signals_signature_matches(rom);
    int protection=fa18_os_exec_task_protection_signature_matches(rom);
    fa18_service_enable(FA18_SERVICE_EXEC_PUT_MSG,messages);
    fa18_service_enable(FA18_SERVICE_EXEC_REPLY_WAIT_PORT,messages);
    fa18_service_enable(FA18_SERVICE_EXEC_SIGNALS,signals);
    fa18_service_enable(FA18_SERVICE_EXEC_SIGNAL_TRAPS,signals);
    fa18_service_enable(FA18_SERVICE_EXEC_TASK_PROTECTION,protection);
    return messages && signals && protection;
}
static uint8_t read8(void *c,uint32_t a) { (void)c; return (uint8_t)m68k_read_memory_8(a); }
static uint16_t read16(void *c,uint32_t a) { (void)c; return (uint16_t)m68k_read_memory_16(a); }
static uint32_t read32(void *c,uint32_t a) { (void)c; return m68k_read_memory_32(a); }
static void write8(void *c,uint32_t a,uint8_t v) { (void)c; m68k_write_memory_8(a,v); }
static void write16(void *c,uint32_t a,uint16_t v) { (void)c; m68k_write_memory_16(a,v); }
static void write32(void *c,uint32_t a,uint32_t v) { (void)c; m68k_write_memory_32(a,v); }
enum { NEXT,BRANCH,TRANSFER,CALL,SET_A5,DBRA };
static int phase_step(uint32_t pc,uint16_t op,unsigned ext,AmigaExecTaskPhase phase,unsigned arg,int flow,uint32_t target) {
    AmigaExecTaskState state;
    AmigaExecTaskBus bus={NULL,read8,read16,read32,write8,write16,write32};
    AmigaExecTaskEffect effect;
    memcpy(state.d,REG_D,sizeof state.d); memcpy(state.a,REG_A,sizeof state.a);
    state.ccr=(uint8_t)m68k_get_reg(NULL,M68K_REG_SR);
    fa18_service_begin(pc,op);
    if (flow!=DBRA) fa18_service_extension_words(ext);
    if (!amiga_exec_task_step(phase,arg,&state,&bus,&effect)) return 0;
    memcpy(REG_D,state.d,sizeof state.d); memcpy(REG_A,state.a,sizeof state.a);
    FLAG_X=state.ccr&AMIGA_CCR_X?XFLAG_SET:XFLAG_CLEAR;
    FLAG_N=state.ccr&AMIGA_CCR_N?NFLAG_SET:NFLAG_CLEAR;
    FLAG_Z=state.ccr&AMIGA_CCR_Z?ZFLAG_SET:ZFLAG_CLEAR;
    FLAG_V=state.ccr&AMIGA_CCR_V?VFLAG_SET:VFLAG_CLEAR;
    FLAG_C=state.ccr&AMIGA_CCR_C?CFLAG_SET:CFLAG_CLEAR;
    if (flow==BRANCH) {
        if (effect.branch_taken) REG_PC=target;
        else USE_CYCLES(ext?CYC_BCC_NOTAKE_W:CYC_BCC_NOTAKE_B);
    } else if (flow==TRANSFER) { m68ki_trace_t0(); m68ki_jump(target); }
    else if (flow==CALL) fa18_service_call(target);
    else if (flow==SET_A5) REG_A[5]=target;
    else if (flow==DBRA) {
        if (effect.dbra_continues) {
            fa18_service_extension_words(1); m68ki_trace_t0(); REG_PC=target;
            USE_CYCLES(CYC_DBCC_F_NOEXP);
        } else { REG_PC+=2; USE_CYCLES(CYC_DBCC_F_EXP); }
    }
    if (effect.returned) REG_PC=effect.return_pc;
    if (op==0x56C0 && effect.scc_true) USE_CYCLES(CYC_SCC_R_TRUE);
    if (effect.bit_below16) USE_CYCLES(-2);
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
#define STEP(op,ext,phase,arg) return phase_step(pc,op,ext,AMIGA_EXEC_##phase,arg,NEXT,0)
#define LIST(op,ext,phase) STEP(op,ext,LIST_PHASE,AMIGA_LIST_##phase)
#define BR(op,phase,target) return phase_step(pc,op,0,AMIGA_EXEC_##phase,0,BRANCH,target)
#define GO(op,ext,target) return phase_step(pc,op,ext,AMIGA_EXEC_FLOW,0,TRANSFER,target)
#define JSR(op,ext,target) return phase_step(pc,op,ext,AMIGA_EXEC_FLOW,0,CALL,target)
int fa18_os_exec_messages_step(void) {
    uint32_t pc=REG_PC;
    switch (pc) {
    case 0xFC1B76u: STEP(0x2208,0,D1_FROM_A0,0);
    case 0xFC1B78u: STEP(0x41E8,1,PORT_LIST,0);
    case 0xFC1B7Cu: STEP(0x33FC,3,INTERRUPT_WORD,0x4000);
    case 0xFC1B84u: STEP(0x522E,1,DEPTH_UP,AMIGA_EXEC_ID_NEST_CNT);
    case 0xFC1B88u: LIST(0x41E8,1,A0_TO_TAIL);
    case 0xFC1B8Cu: LIST(0x2028,1,A0_PRED_TO_D0);
    case 0xFC1B90u: LIST(0x2149,1,NODE_TO_A0_PRED);
    case 0xFC1B94u: LIST(0x2288,0,A0_TO_NODE);
    case 0xFC1B96u: LIST(0x2340,1,D0_TO_NODE_PRED);
    case 0xFC1B9Au: LIST(0x2040,0,A0_FROM_D0);
    case 0xFC1B9Cu: LIST(0x2089,0,NODE_TO_A0);
    case 0xFC1B9Eu: STEP(0x2241,0,A1_FROM_D1,0);
    case 0xFC1BA0u: STEP(0x2229,1,PORT_OWNER,0);
    case 0xFC1BA4u: BR(0x6734,EQ,0xFC1BDA);
    case 0xFC1BA6u: STEP(0x1029,1,PORT_FLAGS,0);
    case 0xFC1BAAu: STEP(0x0240,1,PORT_ACTION_MASK,0);
    case 0xFC1BAEu: BR(0x671A,EQ,0xFC1BCA);
    case 0xFC1BB0u: STEP(0x0C00,1,PORT_ACTION_COMPARE,1);
    case 0xFC1BB4u: BR(0x6608,NE,0xFC1BBE);
    case 0xFC1BB6u: STEP(0x2241,0,A1_FROM_D1,0);
    case 0xFC1BB8u: JSR(0x4EAE,1,REG_A[6]-180);
    case 0xFC1BBCu: GO(0x601C,0,0xFC1BDA);
    case 0xFC1BBEu: STEP(0x0C00,1,PORT_ACTION_COMPARE,2);
    case 0xFC1BC2u: BR(0x6716,EQ,0xFC1BDA);
    case 0xFC1BC4u: STEP(0x2041,0,A0_FROM_D1,0);
    case 0xFC1BC6u: JSR(0x4E90,0,REG_A[0]);
    case 0xFC1BC8u: GO(0x6010,0,0xFC1BDA);
    case 0xFC1BCAu: STEP(0x1029,1,PORT_BIT_D0,0);
    case 0xFC1BCEu: STEP(0x2241,0,A1_FROM_D1,0);
    case 0xFC1BD0u: STEP(0x7200,0,D1_ZERO,0);
    case 0xFC1BD2u: STEP(0x01C1,0,MASK_FROM_D0,0);
    case 0xFC1BD4u: STEP(0x2001,0,D0_FROM_D1,0);
    case 0xFC1BD6u: JSR(0x4EAE,1,REG_A[6]-324);
    case 0xFC1BDAu: STEP(0x532E,1,DEPTH_DOWN,AMIGA_EXEC_ID_NEST_CNT);
    case 0xFC1BDEu: BR(0x6C08,GE,0xFC1BE8);
    case 0xFC1BE0u: STEP(0x33FC,3,INTERRUPT_WORD,0xC000);
    case 0xFC1BE8u: STEP(0x4E75,0,RETURN,0);
    case 0xFC1C18u: STEP(0x2029,1,REPLY_PORT,0);
    case 0xFC1C1Cu: BR(0x6608,NE,0xFC1C26);
    case 0xFC1C1Eu: STEP(0x137C,2,MESSAGE_TYPE,6);
    case 0xFC1C24u: STEP(0x4E75,0,RETURN,0);
    case 0xFC1C26u: STEP(0x137C,2,MESSAGE_TYPE,7);
    case 0xFC1C2Cu: LIST(0x2040,0,A0_FROM_D0);
    case 0xFC1C2Eu: GO(0x6000,1,0xFC1B76);
    case 0xFC1C32u: STEP(0x2268,1,PORT_HEAD,0);
    case 0xFC1C36u: STEP(0x4A91,0,TEST_NODE,0);
    case 0xFC1C38u: BR(0x661C,NE,0xFC1C56);
    case 0xFC1C3Au: STEP(0x1228,1,PORT_BIT_D1,0);
    case 0xFC1C3Eu: STEP(0x41E8,1,PORT_LIST,0);
    case 0xFC1C42u: STEP(0x7000,0,D0_ZERO,0);
    case 0xFC1C44u: STEP(0x03C0,0,MASK_FROM_D1,0);
    case 0xFC1C46u: STEP(0x2F0A,0,PUSH_A,2);
    case 0xFC1C48u: STEP(0x2448,0,A2_FROM_A0,0);
    case 0xFC1C4Au: JSR(0x4EAE,1,REG_A[6]-318);
    case 0xFC1C4Eu: STEP(0x2252,0,LIST_HEAD_A1,0);
    case 0xFC1C50u: STEP(0x4A91,0,TEST_NODE,0);
    case 0xFC1C52u: BR(0x67F6,EQ,0xFC1C4A);
    case 0xFC1C54u: STEP(0x245F,0,POP_A,2);
    case 0xFC1C56u: STEP(0x2009,0,D0_FROM_A1,0);
    case 0xFC1C58u: STEP(0x4E75,0,RETURN,0);
    default: return 0;
    }
}
int fa18_os_exec_signals_step(void) {
    uint32_t pc=REG_PC;
    switch (pc) {
    case 0xFC1E54u: case 0xFC1E5Eu: case 0xFC1F0Cu: case 0xFC1F4Eu:
    case 0xFC1FCAu: case 0xFC1FF0u: case 0xFC2000u: case 0xFC2038u: STEP(0x226E,1,CURRENT_TASK,0);
    case 0xFC1E58u: STEP(0x41E9,1,TASK_FIELD,AMIGA_TASK_SIGNALS_EXCEPT);
    case 0xFC1E5Cu: GO(0x6008,0,0xFC1E66);
    case 0xFC1E62u: case 0xFC1E84u: STEP(0x41E9,1,TASK_FIELD,AMIGA_TASK_SIGNALS_RECEIVED);
    case 0xFC1E66u: STEP(0xC081,0,SIGNALS_MASK_INPUT,0);
    case 0xFC1E68u: case 0xFC1E88u: case 0xFC1F14u: STEP(0x33FC,3,INTERRUPT_WORD,0x4000);
    case 0xFC1E70u: case 0xFC1E90u: case 0xFC1F1Cu: STEP(0x522E,1,DEPTH_UP,AMIGA_EXEC_ID_NEST_CNT);
    case 0xFC1E74u: case 0xFC1E94u: STEP(0x2F10,0,PUSH_FIELD,0);
    case 0xFC1E76u: STEP(0x4681,0,INVERT_D1,0);
    case 0xFC1E78u: STEP(0xC290,0,MASK_OLD_FIELD,0);
    case 0xFC1E7Au: STEP(0x8280,0,MERGE_SIGNALS,0);
    case 0xFC1E7Cu: STEP(0x2081,0,STORE_SIGNALS,0);
    case 0xFC1E7Eu: STEP(0x2029,1,LOAD_RECEIVED_D0,0);
    case 0xFC1E82u: GO(0x6014,0,0xFC1E98);
    case 0xFC1E96u: STEP(0x8190,0,OR_SIGNALS,0);
    case 0xFC1E98u: STEP(0x2229,1,LOAD_EXCEPT_D1,0);
    case 0xFC1E9Cu: case 0xFC1F5Au: STEP(0xC280,0,INTERSECT_D1,0);
    case 0xFC1E9Eu: BR(0x664A,NE,0xFC1EEA);
    case 0xFC1EA0u: case 0xFC1EF0u: STEP(0x0C29,2,COMPARE_TASK_STATE,4);
    case 0xFC1EA6u: BR(0x6652,NE,0xFC1EFA);
    case 0xFC1EA8u: STEP(0xC0A9,1,INTERSECT_WAIT_D0,0);
    case 0xFC1EACu: BR(0x674C,EQ,0xFC1EFA);
    case 0xFC1EAEu: case 0xFC1F28u: STEP(0x41EE,1,EXEC_LIST,AMIGA_EXEC_TASK_WAIT);
    case 0xFC1EB2u: STEP(0x2009,0,D0_FROM_A1,0);
    case 0xFC1EB4u: LIST(0x2051,0,NODE_NEXT_TO_A0);
    case 0xFC1EB6u: LIST(0x2269,1,NODE_PRED_TO_A1);
    case 0xFC1EBAu: case 0xFC1F38u: LIST(0x2288,0,A0_TO_NODE);
    case 0xFC1EBCu: case 0xFC1F34u: LIST(0x2149,1,NODE_TO_A0_PRED);
    case 0xFC1EC0u: STEP(0x2240,0,A1_FROM_D0,0);
    case 0xFC1EC2u: STEP(0x137C,2,TASK_STATE,3);
    case 0xFC1EC8u: STEP(0x41EE,1,EXEC_LIST,AMIGA_EXEC_TASK_READY);
    case 0xFC1ECCu: JSR(0x6100,1,0xFC1670);
    case 0xFC1ED0u: STEP(0xB3EE,1,COMPARE_READY_HEAD,0);
    case 0xFC1ED4u: BR(0x6624,NE,0xFC1EFA);
    case 0xFC1ED6u: case 0xFC1EFAu: case 0xFC1F62u: STEP(0x532E,1,DEPTH_DOWN,AMIGA_EXEC_ID_NEST_CNT);
    case 0xFC1EDAu: BR(0x6C08,GE,0xFC1EE4);
    case 0xFC1EDCu: case 0xFC1F00u: case 0xFC1F68u: STEP(0x33FC,3,INTERRUPT_WORD,0xC000);
    case 0xFC1EE4u: case 0xFC1F08u: STEP(0x201F,0,POP_D0,0);
    case 0xFC1EE6u: GO(0x4EEE,1,REG_A[6]-48);
    case 0xFC1EEAu: STEP(0x08E9,2,EXCEPTION_PENDING,0);
    case 0xFC1EF6u: BR(0x67B6,EQ,0xFC1EAE);
    case 0xFC1EF8u: GO(0x60DC,0,0xFC1ED6);
    case 0xFC1EFEu: BR(0x6C08,GE,0xFC1F08);
    case 0xFC1F0Au: case 0xFC1F72u: case 0xFC1FEEu: case 0xFC1FFEu:
    case 0xFC2036u: case 0xFC2046u: STEP(0x4E75,0,RETURN,0);
    case 0xFC1F10u: STEP(0x2340,1,STORE_WAIT,0);
    case 0xFC1F20u: GO(0x6034,0,0xFC1F56);
    case 0xFC1F22u: STEP(0x137C,2,TASK_STATE,4);
    case 0xFC1F2Cu: LIST(0x41E8,1,A0_TO_TAIL);
    case 0xFC1F30u: LIST(0x2028,1,A0_PRED_TO_D0);
    case 0xFC1F3Au: LIST(0x2340,1,D0_TO_NODE_PRED);
    case 0xFC1F3Eu: LIST(0x2040,0,A0_FROM_D0);
    case 0xFC1F40u: LIST(0x2089,0,NODE_TO_A0);
    case 0xFC1F42u: STEP(0x204D,0,A0_FROM_A5,0);
    case 0xFC1F44u: return phase_step(pc,0x4BEE,1,AMIGA_EXEC_FLOW,0,SET_A5,REG_A[6]-54);
    case 0xFC1F48u: JSR(0x4EAE,1,REG_A[6]-30);
    case 0xFC1F4Cu: STEP(0x2A48,0,A5_FROM_A0,0);
    case 0xFC1F52u: STEP(0x2029,1,LOAD_WAIT,0);
    case 0xFC1F56u: STEP(0x2229,1,LOAD_RECEIVED_D1,0);
    case 0xFC1F5Cu: BR(0x67C4,EQ,0xFC1F22);
    case 0xFC1F5Eu: STEP(0xB3A9,1,CLEAR_RECEIVED,0);
    case 0xFC1F66u: BR(0x6C08,GE,0xFC1F70);
    case 0xFC1F70u: STEP(0x2001,0,D0_FROM_D1,0);
    case 0xFC1FCEu: case 0xFC1FF4u: STEP(0x3229,1,LOAD_ALLOCATED_TRAPS,0);
    case 0xFC1FD2u: case 0xFC2008u: STEP(0x0C00,1,COMPARE_ANY_BIT,0);
    case 0xFC1FD6u: BR(0x6706,EQ,0xFC1FDE);
    case 0xFC1FD8u: case 0xFC1FE0u: case 0xFC200Eu: case 0xFC2016u: STEP(0x01C1,0,ALLOCATE_BIT,0);
    case 0xFC1FDAu: BR(0x670E,EQ,0xFC1FEA);
    case 0xFC1FDCu: GO(0x600A,0,0xFC1FE8);
    case 0xFC1FDEu: STEP(0x700F,0,FIRST_FREE_BIT,15);
    case 0xFC1FE2u: BR(0x6706,EQ,0xFC1FEA);
    case 0xFC1FE4u: return phase_step(pc,0x51C8,1,AMIGA_EXEC_NEXT_FREE_BIT,0,DBRA,0xFC1FE0);
    case 0xFC1FE8u: case 0xFC201Eu: STEP(0x70FF,0,NO_FREE_BIT,0);
    case 0xFC1FEAu: case 0xFC1FFAu: STEP(0x3341,1,STORE_ALLOCATED_TRAPS,0);
    case 0xFC1FF8u: case 0xFC2028u: case 0xFC2040u: STEP(0x0181,0,FREE_BIT,0);
    case 0xFC2004u: case 0xFC203Cu: STEP(0x2229,1,LOAD_ALLOCATED_SIGNALS,0);
    case 0xFC200Cu: BR(0x6706,EQ,0xFC2014);
    case 0xFC2010u: BR(0x6710,EQ,0xFC2022);
    case 0xFC2012u: GO(0x600A,0,0xFC201E);
    case 0xFC2014u: STEP(0x701F,0,FIRST_FREE_BIT,31);
    case 0xFC2018u: BR(0x6708,EQ,0xFC2022);
    case 0xFC201Au: return phase_step(pc,0x51C8,1,AMIGA_EXEC_NEXT_FREE_BIT,0,DBRA,0xFC2016);
    case 0xFC2020u: GO(0x6014,0,0xFC2036);
    case 0xFC2022u: case 0xFC2042u: STEP(0x2341,1,STORE_ALLOCATED_SIGNALS,0);
    case 0xFC2026u: STEP(0x72FF,0,CLEAR_MASK,0);
    case 0xFC202Au: STEP(0xC3A9,1,CLEAR_TASK_MASK,AMIGA_TASK_SIGNALS_RECEIVED);
    case 0xFC202Eu: STEP(0xC3A9,1,CLEAR_TASK_MASK,AMIGA_TASK_SIGNALS_EXCEPT);
    case 0xFC2032u: STEP(0xC3A9,1,CLEAR_TASK_MASK,AMIGA_TASK_SIGNALS_WAIT);
    default: return 0;
    }
}
int fa18_os_exec_task_protection_step(void) {
    uint32_t pc=REG_PC;
    switch (pc) {
    case 0xFC1F74u: STEP(0x08EE,2,RESCHEDULE_PENDING,0);
    case 0xFC1F7Au: STEP(0x56C0,0,PENDING_TO_D0,0);
    case 0xFC1F7Cu: case 0xFC1F82u: case 0xFC1FA2u:
        STEP(0x4A2E,1,TEST_DEPTH,pc==0xFC1F7C?AMIGA_EXEC_TD_NEST_CNT:AMIGA_EXEC_ID_NEST_CNT);
    case 0xFC1F80u: BR(0x6C12,GE,0xFC1F94);
    case 0xFC1F86u: BR(0x6D28,LT,0xFC1FB0);
    case 0xFC1F88u: STEP(0x4A00,0,TEST_D0_BYTE,0);
    case 0xFC1F8Au: BR(0x6608,NE,0xFC1F94);
    case 0xFC1F8Cu: STEP(0x33FC,3,REQUEST_RESCHEDULE_INTERRUPT,0);
    case 0xFC1F94u: case 0xFC1F9Au: case 0xFC1FBCu: STEP(0x4E75,0,RETURN,0);
    case 0xFC1F96u: STEP(0x522E,1,DEPTH_UP,AMIGA_EXEC_TD_NEST_CNT);
    case 0xFC1F9Cu: STEP(0x532E,1,DEPTH_DOWN,AMIGA_EXEC_TD_NEST_CNT);
    case 0xFC1FA0u: BR(0x6C1A,GE,0xFC1FBC);
    case 0xFC1FA6u: BR(0x6C14,GE,0xFC1FBC);
    case 0xFC1FA8u: STEP(0x082E,2,TEST_RESCHEDULE,0);
    case 0xFC1FAEu: BR(0x670C,EQ,0xFC1FBC);
    case 0xFC1FB0u: STEP(0x2F0D,0,PUSH_A,5);
    case 0xFC1FB2u: return phase_step(pc,0x4BFA,1,AMIGA_EXEC_FLOW,0,SET_A5,0xFC1FBE);
    case 0xFC1FB6u: JSR(0x4EAE,1,REG_A[6]-30);
    case 0xFC1FBAu: STEP(0x2A5F,0,POP_A,5);
    default: return 0;
    }
}
