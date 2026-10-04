/* Original 1.3 interrupt ABI and bus timing. Guest vectors, server chains and
 * deferred queues live in the SDK/CPU/ROM-independent amiga_compat layer. */
#include "exec_interrupt_adapter.h"
#include "../amiga/exec_interrupt_services.h"
#include "../amiga/abi_13.h"
#include "exec_service_state.h"
#include "service_dispatch_adapter.h"
#include "machine.h"
#include "m68kops.h"
#include <stdlib.h>
static uint32_t signature(const uint8_t *r,unsigned lo,unsigned hi) {
    uint32_t h=2166136261u;
    for (unsigned i=lo;i<hi;++i) h=(h^r[i])*16777619u;
    return h;
}
int fa18_os_exec_interrupt_services_signature_matches(const uint8_t *r) {
    return r && signature(r,0xC88,0xE9C)==0xF1EBA146u &&
        signature(r,0x11CA,0x1298)==0x34E74845u && signature(r,0x1338,0x1428)==0x8FED1E73u;
}
int fa18_os_exec_interrupt_services_enable_reference(void) {
    int enabled=fa18_os_exec_interrupt_services_signature_matches(fa18_machine->rom);
    fa18_service_enable(FA18_SERVICE_EXEC_IRQ_ROOTS,enabled);
    fa18_service_enable(FA18_SERVICE_EXEC_INT_SERVERS,enabled);
    fa18_service_enable(FA18_SERVICE_EXEC_SOFT_INTERRUPTS,enabled);
    return enabled;
}
static int semantic(uint32_t pc,uint16_t op,unsigned ext,AmigaExecInterruptPhase p,unsigned arg) {
    AmigaExecTaskState s; AmigaExecInterruptEffect e; fa18_exec_service_load(&s);
    unsigned late=p==AMIGA_INT_COPY_DATA || p==AMIGA_INT_COPY_CODE?1:p==AMIGA_INT_ACK_SAVED_MASK?2:0;
    AmigaExecTaskBus b=fa18_exec_service_bus(&late);
    fa18_service_begin(pc,op);
    if (p!=AMIGA_INT_NEXT_PRIORITY) fa18_service_extension_words(ext-late);
    if (!amiga_exec_interrupt_step(p,arg,&s,&b,&e) || late) abort();
    fa18_exec_service_store(&s);
    if (p==AMIGA_INT_NEXT_PRIORITY) {
        if (e.dbra_continues) {
            fa18_service_extension_words(1); m68ki_trace_t0(); REG_PC=0xFC140C; USE_CYCLES(CYC_DBCC_F_NOEXP);
        } else { REG_PC+=2; USE_CYCLES(CYC_DBCC_F_EXP); }
    }
    if (p==AMIGA_INT_VECTOR_INDEX) USE_CYCLES(4); /* MULU immediate 12: two set bits. */
    if (e.bit_below16) USE_CYCLES(-2);
    USE_CYCLES(e.transferred_longs<<CYC_MOVEM_L); USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
static int task(uint32_t pc,uint16_t op,unsigned ext,AmigaExecTaskPhase p,unsigned arg) {
    AmigaExecTaskState s; AmigaExecTaskEffect e; AmigaExecTaskBus b=fa18_exec_service_bus(NULL);
    fa18_exec_service_load(&s); fa18_service_begin(pc,op); fa18_service_extension_words(ext);
    if (!amiga_exec_task_step(p,arg,&s,&b,&e)) abort(); fa18_exec_service_store(&s);
    if (e.returned) REG_PC=e.return_pc;
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
static int registers(uint32_t pc,uint16_t op,int save) {
    AmigaExecTaskState s; unsigned count; AmigaExecTaskBus b=fa18_exec_service_bus(NULL);
    fa18_exec_service_load(&s); fa18_service_begin(pc,op); fa18_service_extension_words(1);
    int ok=save?amiga_exec_context_save(&s,&b,7,0x6303,&count):amiga_exec_context_restore(&s,&b,7,0x6303,1,&count);
    if (!ok) abort(); fa18_exec_service_store(&s);
    USE_CYCLES(count<<CYC_MOVEM_L); USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
static int branch(uint32_t pc,uint16_t op,unsigned ext,int taken,uint32_t target) {
    fa18_service_begin(pc,op);
    if (taken) { fa18_service_extension_words(ext); m68ki_trace_t0(); REG_PC=target; }
    else { REG_PC+=ext*2; USE_CYCLES(ext?CYC_BCC_NOTAKE_W:CYC_BCC_NOTAKE_B); }
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
static int transfer(uint32_t pc,uint16_t op,unsigned ext,uint32_t target,int call) {
    fa18_service_begin(pc,op); fa18_service_extension_words(ext);
    if (call) fa18_service_call(target); else { m68ki_trace_t0(); m68ki_jump(target); }
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
static int exec_base(uint32_t pc) {
    fa18_service_begin(pc,0x2C78); fa18_service_extension_words(1); REG_A[6]=m68k_read_memory_32(4);
    USE_CYCLES(CYC_INSTRUCTION[0x2C78]); return 1;
}
static int cpu_status(uint32_t pc,uint16_t op,uint16_t sr) {
    fa18_service_begin(pc,op);
    if (op==0x4E73) m68ki_instruction_jump_table[op]();
    else if (!FLAG_S) m68ki_exception_privilege_violation();
    else { fa18_service_extension_words(1); m68ki_trace_t0(); m68ki_set_sr(sr); }
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
#define STEP(op,ext,phase,arg) return semantic(pc,op,ext,AMIGA_INT_##phase,arg)
#define TASK(op,ext,phase,arg) return task(pc,op,ext,AMIGA_EXEC_##phase,arg)
#define LIST(op,ext,phase) TASK(op,ext,LIST_PHASE,AMIGA_LIST_##phase)
#define BR(op,ext,cond,target) return branch(pc,op,ext,cond,target)
#define GO(op,ext,target) return transfer(pc,op,ext,target,0)
#define CALL(op,ext,target) return transfer(pc,op,ext,target,1)
int fa18_os_exec_irq_requires_outer_dispatch(uint32_t pc) { return pc==0xFC0C8C || pc==0xFC0E9A; }
int fa18_os_exec_irq_roots_step(void) {
    uint32_t pc=REG_PC;
    switch (pc) {
    case 0xFC0C88u: return registers(pc,0x4CDF,0);
    case 0xFC0C8Cu: return cpu_status(pc,0x4E73,0);
    case 0xFC0C8Eu: return registers(pc,0x48E7,1);
    case 0xFC0C92u: STEP(0x41F9,2,CUSTOM_BASE,0);
    case 0xFC0C98u: return exec_base(pc);
    case 0xFC0C9Cu: STEP(0x3228,1,ENABLED_WORD,0);
    case 0xFC0CA0u: STEP(0x0801,1,TEST_PENDING,14);
    case 0xFC0CA4u: BR(0x67E2,0,COND_EQ(),0xFC0C88);
    case 0xFC0CA6u: STEP(0xC268,1,PENDING_WORD,0);
    case 0xFC0CAAu: STEP(0x0801,1,TEST_PENDING,0);
    case 0xFC0CAEu: BR(0x670C,0,COND_EQ(),0xFC0CBC);
    case 0xFC0CB0u: STEP(0x4CEE,2,VECTOR_CALLBACK,0x54);
    case 0xFC0CB6u: TASK(0x486E,1,PUSH_RETURN_PC,REG_A[6]-36);
    case 0xFC0CBAu: GO(0x4ED5,0,REG_A[5]);
    case 0xFC0CBCu: STEP(0x0801,1,TEST_PENDING,1);
    case 0xFC0CC0u: BR(0x670C,0,COND_EQ(),0xFC0CCE);
    case 0xFC0CC2u: STEP(0x4CEE,2,VECTOR_CALLBACK,0x60);
    case 0xFC0CC8u: TASK(0x486E,1,PUSH_RETURN_PC,REG_A[6]-36);
    case 0xFC0CCCu: GO(0x4ED5,0,REG_A[5]);
    case 0xFC0CCEu: STEP(0x0801,1,TEST_PENDING,2);
    case 0xFC0CD2u: BR(0x670C,0,COND_EQ(),0xFC0CE0);
    case 0xFC0CD4u: STEP(0x4CEE,2,VECTOR_CALLBACK,0x6C);
    case 0xFC0CDAu: TASK(0x486E,1,PUSH_RETURN_PC,REG_A[6]-36);
    case 0xFC0CDEu: GO(0x4ED5,0,REG_A[5]);
    case 0xFC0CE0u: GO(0x60A6,0,0xFC0C88);
    case 0xFC0CE2u: return registers(pc,0x48E7,1);
    case 0xFC0CE6u: STEP(0x41F9,2,CUSTOM_BASE,0);
    case 0xFC0CECu: return exec_base(pc);
    case 0xFC0CF0u: STEP(0x3228,1,ENABLED_WORD,0);
    case 0xFC0CF4u: STEP(0x0801,1,TEST_PENDING,14);
    case 0xFC0CF8u: BR(0x678E,0,COND_EQ(),0xFC0C88);
    case 0xFC0CFAu: STEP(0xC268,1,PENDING_WORD,0);
    case 0xFC0CFEu: STEP(0x0801,1,TEST_PENDING,3);
    case 0xFC0D02u: BR(0x670C,0,COND_EQ(),0xFC0D10);
    case 0xFC0D04u: STEP(0x4CEE,2,VECTOR_CALLBACK,0x78);
    case 0xFC0D0Au: TASK(0x486E,1,PUSH_RETURN_PC,REG_A[6]-36);
    case 0xFC0D0Eu: GO(0x4ED5,0,REG_A[5]);
    case 0xFC0D10u: GO(0x6000,1,0xFC0C88);
    case 0xFC0D14u: return registers(pc,0x48E7,1);
    case 0xFC0D18u: STEP(0x41F9,2,CUSTOM_BASE,0);
    case 0xFC0D1Eu: return exec_base(pc);
    case 0xFC0D22u: STEP(0x3228,1,ENABLED_WORD,0);
    case 0xFC0D26u: STEP(0x0801,1,TEST_PENDING,14);
    case 0xFC0D2Au: BR(0x6700,1,COND_EQ(),0xFC0C88);
    case 0xFC0D2Eu: STEP(0xC268,1,PENDING_WORD,0);
    case 0xFC0D32u: STEP(0x0801,1,TEST_PENDING,6);
    case 0xFC0D36u: BR(0x670C,0,COND_EQ(),0xFC0D44);
    case 0xFC0D38u: STEP(0x4CEE,2,VECTOR_CALLBACK,0x9C);
    case 0xFC0D3Eu: TASK(0x486E,1,PUSH_RETURN_PC,REG_A[6]-36);
    case 0xFC0D42u: GO(0x4ED5,0,REG_A[5]);
    case 0xFC0D44u: STEP(0x0801,1,TEST_PENDING,5);
    case 0xFC0D48u: BR(0x670C,0,COND_EQ(),0xFC0D56);
    case 0xFC0D4Au: STEP(0x4CEE,2,VECTOR_CALLBACK,0x90);
    case 0xFC0D50u: TASK(0x486E,1,PUSH_RETURN_PC,REG_A[6]-36);
    case 0xFC0D54u: GO(0x4ED5,0,REG_A[5]);
    case 0xFC0D56u: STEP(0x0801,1,TEST_PENDING,4);
    case 0xFC0D5Au: BR(0x670C,0,COND_EQ(),0xFC0D68);
    case 0xFC0D5Cu: STEP(0x4CEE,2,VECTOR_CALLBACK,0x84);
    case 0xFC0D62u: TASK(0x486E,1,PUSH_RETURN_PC,REG_A[6]-36);
    case 0xFC0D66u: GO(0x4ED5,0,REG_A[5]);
    case 0xFC0D68u: GO(0x6000,1,0xFC0C88);
    case 0xFC0D6Cu: return registers(pc,0x48E7,1);
    case 0xFC0D70u: STEP(0x41F9,2,CUSTOM_BASE,0);
    case 0xFC0D76u: return exec_base(pc);
    case 0xFC0D7Au: STEP(0x3228,1,ENABLED_WORD,0);
    case 0xFC0D7Eu: STEP(0x0801,1,TEST_PENDING,14);
    case 0xFC0D82u: BR(0x6700,1,COND_EQ(),0xFC0C88);
    case 0xFC0D86u: STEP(0xC268,1,PENDING_WORD,0);
    case 0xFC0D8Au: STEP(0x0801,1,TEST_PENDING,8);
    case 0xFC0D8Eu: BR(0x670E,0,COND_EQ(),0xFC0D9E);
    case 0xFC0D90u: STEP(0x4CEE,2,VECTOR_CALLBACK,0xB4);
    case 0xFC0D96u: TASK(0x4879,2,PUSH_RETURN_PC,0xFC0DDE);
    case 0xFC0D9Cu: GO(0x4ED5,0,REG_A[5]);
    case 0xFC0D9Eu: STEP(0x0801,1,TEST_PENDING,10);
    case 0xFC0DA2u: BR(0x670E,0,COND_EQ(),0xFC0DB2);
    case 0xFC0DA4u: STEP(0x4CEE,2,VECTOR_CALLBACK,0xCC);
    case 0xFC0DAAu: TASK(0x4879,2,PUSH_RETURN_PC,0xFC0DDE);
    case 0xFC0DB0u: GO(0x4ED5,0,REG_A[5]);
    case 0xFC0DB2u: STEP(0x0801,1,TEST_PENDING,7);
    case 0xFC0DB6u: BR(0x670E,0,COND_EQ(),0xFC0DC6);
    case 0xFC0DB8u: STEP(0x4CEE,2,VECTOR_CALLBACK,0xA8);
    case 0xFC0DBEu: TASK(0x4879,2,PUSH_RETURN_PC,0xFC0DDE);
    case 0xFC0DC4u: GO(0x4ED5,0,REG_A[5]);
    case 0xFC0DC6u: STEP(0x0801,1,TEST_PENDING,9);
    case 0xFC0DCAu: BR(0x670E,0,COND_EQ(),0xFC0DDA);
    case 0xFC0DCCu: STEP(0x4CEE,2,VECTOR_CALLBACK,0xC0);
    case 0xFC0DD2u: TASK(0x4879,2,PUSH_RETURN_PC,0xFC0DDE);
    case 0xFC0DD8u: GO(0x4ED5,0,REG_A[5]);
    case 0xFC0DDAu: GO(0x6000,1,0xFC0C88);
    case 0xFC0DDEu: STEP(0x41F9,2,CUSTOM_BASE,0);
    case 0xFC0DE4u: return exec_base(pc);
    case 0xFC0DE8u: STEP(0x323C,1,AUDIO_MASK,0);
    case 0xFC0DECu: STEP(0xC268,1,MASK_ENABLED_WORD,0);
    case 0xFC0DF0u: STEP(0xC268,1,PENDING_WORD,0);
    case 0xFC0DF4u: BR(0x6694,0,COND_NE(),0xFC0D8A);
    case 0xFC0DF6u: GO(0x4EEE,1,REG_A[6]-36);
    case 0xFC0DFAu: return registers(pc,0x48E7,1);
    case 0xFC0DFEu: STEP(0x41F9,2,CUSTOM_BASE,0);
    case 0xFC0E04u: return exec_base(pc);
    case 0xFC0E08u: STEP(0x3228,1,ENABLED_WORD,0);
    case 0xFC0E0Cu: STEP(0x0801,1,TEST_PENDING,14);
    case 0xFC0E10u: BR(0x6700,1,COND_EQ(),0xFC0C88);
    case 0xFC0E14u: STEP(0xC268,1,PENDING_WORD,0);
    case 0xFC0E18u: STEP(0x0801,1,TEST_PENDING,12);
    case 0xFC0E1Cu: BR(0x670C,0,COND_EQ(),0xFC0E2A);
    case 0xFC0E1Eu: STEP(0x4CEE,2,VECTOR_CALLBACK,0xE4);
    case 0xFC0E24u: TASK(0x486E,1,PUSH_RETURN_PC,REG_A[6]-36);
    case 0xFC0E28u: GO(0x4ED5,0,REG_A[5]);
    case 0xFC0E2Au: STEP(0x0801,1,TEST_PENDING,11);
    case 0xFC0E2Eu: BR(0x670C,0,COND_EQ(),0xFC0E3C);
    case 0xFC0E30u: STEP(0x4CEE,2,VECTOR_CALLBACK,0xD8);
    case 0xFC0E36u: TASK(0x486E,1,PUSH_RETURN_PC,REG_A[6]-36);
    case 0xFC0E3Au: GO(0x4ED5,0,REG_A[5]);
    case 0xFC0E3Cu: GO(0x6000,1,0xFC0C88);
    case 0xFC0E40u: return registers(pc,0x48E7,1);
    case 0xFC0E44u: STEP(0x41F9,2,CUSTOM_BASE,0);
    case 0xFC0E4Au: return exec_base(pc);
    case 0xFC0E4Eu: STEP(0x3228,1,ENABLED_WORD,0);
    case 0xFC0E52u: STEP(0x0801,1,TEST_PENDING,14);
    case 0xFC0E56u: BR(0x6700,1,COND_EQ(),0xFC0C88);
    case 0xFC0E5Au: STEP(0xC268,1,PENDING_WORD,0);
    case 0xFC0E5Eu: STEP(0x0801,1,TEST_PENDING,14);
    case 0xFC0E62u: BR(0x670C,0,COND_EQ(),0xFC0E70);
    case 0xFC0E64u: STEP(0x4CEE,2,VECTOR_CALLBACK,0xFC);
    case 0xFC0E6Au: TASK(0x486E,1,PUSH_RETURN_PC,REG_A[6]-36);
    case 0xFC0E6Eu: GO(0x4ED5,0,REG_A[5]);
    case 0xFC0E70u: STEP(0x0801,1,TEST_PENDING,13);
    case 0xFC0E74u: BR(0x670C,0,COND_EQ(),0xFC0E82);
    case 0xFC0E76u: STEP(0x4CEE,2,VECTOR_CALLBACK,0xF0);
    case 0xFC0E7Cu: TASK(0x486E,1,PUSH_RETURN_PC,REG_A[6]-36);
    case 0xFC0E80u: GO(0x4ED5,0,REG_A[5]);
    case 0xFC0E82u: GO(0x6000,1,0xFC0C88);
    case 0xFC0E86u: return registers(pc,0x48E7,1);
    case 0xFC0E8Au: return exec_base(pc);
    case 0xFC0E8Eu: STEP(0x4CEE,2,VECTOR_CALLBACK,0x108);
    case 0xFC0E94u: CALL(0x4E95,0,REG_A[5]);
    case 0xFC0E96u: return registers(pc,0x4CDF,0);
    case 0xFC0E9Au: return cpu_status(pc,0x4E73,0);
    default: return 0;
    }
}
int fa18_os_exec_int_servers_step(void) {
    uint32_t pc=REG_PC;
    switch (pc) {
    case 0xFC11CAu: case 0xFC1216u: case 0xFC1252u: STEP(0xC0FC,1,VECTOR_INDEX,0);
    case 0xFC11CEu: case 0xFC121Au: case 0xFC1256u: STEP(0x41F6,1,VECTOR_ADDRESS,0);
    case 0xFC11D2u: case 0xFC1222u: case 0xFC1262u: TASK(0x33FC,3,INTERRUPT_WORD,0x4000);
    case 0xFC11DAu: case 0xFC122Au: case 0xFC126Au: TASK(0x522E,1,DEPTH_UP,AMIGA_EXEC_ID_NEST_CNT);
    case 0xFC11DEu: STEP(0x2028,1,OLD_NODE,0);
    case 0xFC11E2u: STEP(0x2149,1,INSTALL_NODE,0);
    case 0xFC11E6u: BR(0x670E,0,COND_EQ(),0xFC11F6);
    case 0xFC11E8u: STEP(0x2169,2,COPY_DATA,0);
    case 0xFC11EEu: STEP(0x2169,2,COPY_CODE,0);
    case 0xFC11F4u: GO(0x600A,0,0xFC1200);
    case 0xFC11F6u: STEP(0x72FF,0,NO_HANDLER,0);
    case 0xFC11F8u: STEP(0x2141,1,CLEAR_VECTOR,AMIGA_INT_VECTOR_DATA);
    case 0xFC11FCu: STEP(0x2141,1,CLEAR_VECTOR,AMIGA_INT_VECTOR_CODE);
    case 0xFC1200u: case 0xFC123Eu: case 0xFC1286u: TASK(0x532E,1,DEPTH_DOWN,AMIGA_EXEC_ID_NEST_CNT);
    case 0xFC1204u: BR(0x6C08,0,COND_GE(),0xFC120E);
    case 0xFC1242u: BR(0x6C08,0,COND_GE(),0xFC124C);
    case 0xFC128Au: BR(0x6C08,0,COND_GE(),0xFC1294);
    case 0xFC1206u: case 0xFC1244u: case 0xFC128Cu: TASK(0x33FC,3,INTERRUPT_WORD,0xC000);
    case 0xFC120Eu: case 0xFC124Eu: case 0xFC1296u: TASK(0x4E75,0,RETURN,0);
    case 0xFC1210u: case 0xFC1250u: STEP(0x2F02,0,SAVE_D2,0);
    case 0xFC1212u: case 0xFC125Eu: STEP(0x2400,0,SAVE_NUMBER,0);
    case 0xFC1214u: STEP(0x2200,0,NUMBER_D1,0);
    case 0xFC121Eu: case 0xFC125Au: STEP(0x2068,1,SERVER_LIST,0);
    case 0xFC122Eu: CALL(0x6100,1,0xFC1670);
    case 0xFC1232u: STEP(0x303C,1,SET_ENABLE_BIT,0);
    case 0xFC1236u: STEP(0x05C0,0,ENABLE_WORD,0);
    case 0xFC1238u: TASK(0x33C0,2,INTERRUPT_WORD,(uint16_t)REG_D[0]);
    case 0xFC124Cu: case 0xFC1294u: STEP(0x241F,0,RESTORE_D2,0);
    case 0xFC1260u: TASK(0x2208,0,D1_FROM_A0,0);
    case 0xFC126Eu: CALL(0x6100,1,0xFC163C);
    case 0xFC1272u: STEP(0x2041,0,LIST_FROM_D1,0);
    case 0xFC1274u: STEP(0xB1E8,1,LIST_EMPTY,0);
    case 0xFC1278u: BR(0x6600,1,COND_NE(),0xFC1286);
    case 0xFC127Cu: STEP(0x7200,0,CLEAR_ENABLE_BIT,0);
    case 0xFC127Eu: STEP(0x05C1,0,DISABLE_WORD,0);
    case 0xFC1280u: TASK(0x33C1,2,INTERRUPT_WORD,(uint16_t)REG_D[1]);
    default: return 0;
    }
}
int fa18_os_exec_soft_interrupts_step(void) {
    uint32_t pc=REG_PC;
    switch (pc) {
    case 0xFC1338u: STEP(0x3F29,1,PUSH_CLEAR_MASK,0);
    case 0xFC133Cu: STEP(0x2F0A,0,SAVE_A2,0);
    case 0xFC133Eu: STEP(0x2451,0,FIRST_SERVER,0);
    case 0xFC1340u: STEP(0x2012,0,SERVER_SUCCESSOR,0);
    case 0xFC1342u: BR(0x670E,0,COND_EQ(),0xFC1352);
    case 0xFC1344u: STEP(0x4CEA,2,NODE_CALLBACK,2);
    case 0xFC134Au: CALL(0x4E95,0,REG_A[5]);
    case 0xFC134Cu: BR(0x6604,0,COND_NE(),0xFC1352);
    case 0xFC134Eu: STEP(0x2452,0,NEXT_SERVER,0);
    case 0xFC1350u: GO(0x60EE,0,0xFC1340);
    case 0xFC1352u: STEP(0x245F,0,RESTORE_A2,0);
    case 0xFC1354u: STEP(0x33DF,2,ACK_SAVED_MASK,0);
    case 0xFC135Au: case 0xFC13BAu: case 0xFC13CCu: case 0xFC1426u: TASK(0x4E75,0,RETURN,0);
    case 0xFC135Cu: TASK(0x33FC,3,INTERRUPT_WORD,0x4000);
    case 0xFC1364u: TASK(0x522E,1,DEPTH_UP,AMIGA_EXEC_ID_NEST_CNT);
    case 0xFC1368u: STEP(0x0C29,2,COMPARE_QUEUED,0);
    case 0xFC136Eu: BR(0x673C,0,COND_EQ(),0xFC13AC);
    case 0xFC1370u: STEP(0x137C,2,QUEUE_NODE,0);
    case 0xFC1376u: TASK(0x7000,0,D0_ZERO,0);
    case 0xFC1378u: STEP(0x1029,1,PRIORITY_BYTE,0);
    case 0xFC137Cu: STEP(0x0240,1,PRIORITY_GROUP,0);
    case 0xFC1380u: STEP(0x4880,0,SIGN_PRIORITY,0);
    case 0xFC1382u: TASK(0x41EE,1,EXEC_LIST,0x1D2);
    case 0xFC1386u: STEP(0xD0C0,0,SOFT_QUEUE,0);
    case 0xFC1388u: LIST(0x41E8,1,A0_TO_TAIL);
    case 0xFC138Cu: LIST(0x2028,1,A0_PRED_TO_D0);
    case 0xFC1390u: LIST(0x2149,1,NODE_TO_A0_PRED);
    case 0xFC1394u: LIST(0x2288,0,A0_TO_NODE);
    case 0xFC1396u: LIST(0x2340,1,D0_TO_NODE_PRED);
    case 0xFC139Au: LIST(0x2040,0,A0_FROM_D0);
    case 0xFC139Cu: LIST(0x2089,0,NODE_TO_A0);
    case 0xFC139Eu: STEP(0x08EE,2,SOFT_PENDING,0);
    case 0xFC13A4u: TASK(0x33FC,3,REQUEST_RESCHEDULE_INTERRUPT,0);
    case 0xFC13ACu: TASK(0x532E,1,DEPTH_DOWN,AMIGA_EXEC_ID_NEST_CNT);
    case 0xFC13B0u: BR(0x6C08,0,COND_GE(),0xFC13BA);
    case 0xFC13B2u: TASK(0x33FC,3,INTERRUPT_WORD,0xC000);
    case 0xFC13BCu: case 0xFC1404u: STEP(0x33FC,3,REQUEST_WORD,4);
    case 0xFC13C4u: STEP(0x08AE,2,CLEAR_SOFT_PENDING,0);
    case 0xFC13CAu: BR(0x6602,0,COND_NE(),0xFC13CE);
    case 0xFC13CEu: TASK(0x33FC,3,INTERRUPT_WORD,4);
    case 0xFC13D6u: GO(0x6026,0,0xFC13FE);
    case 0xFC13D8u: return cpu_status(pc,0x46FC,0x2700);
    case 0xFC13DCu: STEP(0x2250,0,SOFT_HEAD,0);
    case 0xFC13DEu: STEP(0x2011,0,SOFT_SUCCESSOR,0);
    case 0xFC13E0u: BR(0x6708,0,COND_EQ(),0xFC13EA);
    case 0xFC13E2u: STEP(0x2080,0,UNLINK_SOFT,0);
    case 0xFC13E4u: STEP(0xC189,0,EXCHANGE_NODE,0);
    case 0xFC13E6u: STEP(0x2348,1,LINK_SOFT_SUCCESSOR,0);
    case 0xFC13EAu: STEP(0x2240,0,NODE_FROM_D0,0);
    case 0xFC13ECu: STEP(0x137C,2,CALLBACK_NODE_TYPE,0);
    case 0xFC13F2u: return cpu_status(pc,0x46FC,0x2000);
    case 0xFC13F6u: STEP(0x4CE9,2,NODE_CALLBACK,1);
    case 0xFC13FCu: CALL(0x4E95,0,REG_A[5]);
    case 0xFC13FEu: STEP(0x7004,0,SOFT_SCAN,0);
    case 0xFC1400u: TASK(0x41EE,1,EXEC_LIST,0x1F2);
    case 0xFC140Cu: STEP(0xB1E8,1,LIST_EMPTY,0);
    case 0xFC1410u: BR(0x66C6,0,COND_NE(),0xFC13D8);
    case 0xFC1412u: STEP(0x41E8,1,SOFT_NEXT_QUEUE,0);
    case 0xFC1416u: STEP(0x51C8,1,NEXT_PRIORITY,0);
    case 0xFC141Au: return cpu_status(pc,0x46FC,0x2100);
    case 0xFC141Eu: TASK(0x33FC,3,INTERRUPT_WORD,0x8004);
    default: return 0;
    }
}
