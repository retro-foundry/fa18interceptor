/* Complete 1.3 FindTask and FindName contracts. List/string reads, interrupt
 * nesting, nested vector calls and flags retain their original phase order. */
#include "exec_task_lookup.h"
#include "exec.h"
#include "../amiga/abi_13.h"
#include "service_phase.h"
#include "m68kops.h"
#include <string.h>

int fa18_os_exec_task_lookup_signature_matches(const uint8_t *rom) {
    static const uint8_t find_name[]={
        0x2F,0x0A,0x24,0x48,0x22,0x09,0x20,0x12,0x67,0x18,0x24,0x40,
        0x20,0x12,0x67,0x12,0x20,0x6A,0x00,0x0A,0x22,0x41,0xB3,0x08,
        0x66,0xF0,0x4A,0x28,0xFF,0xFF,0x66,0xF6,0x20,0x0A,0x22,0x41,
        0x24,0x5F,0x4E,0x75
    };
    static const uint8_t find_task[]={
        0x20,0x09,0x66,0x06,0x20,0x2E,0x01,0x14,0x60,0x48,0x41,0xEE,0x01,0x96,
        0x33,0xFC,0x40,0x00,0x00,0xDF,0xF0,0x9A,0x52,0x2E,0x01,0x26,
        0x4E,0xAE,0xFE,0xEC,0x4A,0x80,0x66,0x22,0x41,0xEE,0x01,0xA4,
        0x4E,0xAE,0xFE,0xEC,0x4A,0x80,0x66,0x16,0x20,0x6E,0x01,0x14,
        0x20,0x68,0x00,0x0A,0xB3,0x08,0x66,0x0A,0x4A,0x28,0xFF,0xFF,
        0x66,0xF6,0x20,0x2E,0x01,0x14,0x53,0x2E,0x01,0x26,0x6C,0x08,
        0x33,0xFC,0xC0,0x00,0x00,0xDF,0xF0,0x9A,0x4E,0x75
    };
    return rom && !memcmp(rom+0x1696,find_name,sizeof find_name) &&
                  !memcmp(rom+0x1DB0,find_task,sizeof find_task);
}
static void long_flags(uint32_t value) {
    FLAG_N=NFLAG_32(value); FLAG_Z=value; FLAG_V=VFLAG_CLEAR; FLAG_C=CFLAG_CLEAR;
}
static void byte_flags(uint8_t value) {
    FLAG_N=NFLAG_8(value); FLAG_Z=value; FLAG_V=VFLAG_CLEAR; FLAG_C=CFLAG_CLEAR;
}
static void compare_name_byte(void) {
    uint8_t source=m68k_read_memory_8(REG_A[0]++);
    uint8_t dest=m68k_read_memory_8(REG_A[1]++);
    uint32_t result=(uint32_t)dest-source;
    FLAG_N=NFLAG_8(result); FLAG_Z=result&0xFFu;
    FLAG_V=VFLAG_SUB_8(source,dest,result); FLAG_C=CFLAG_8(result);
}
static uint32_t displaced(uint32_t base,int16_t offset) {
    fa18_service_extension_words(1); return base+offset;
}
static void return_to_caller(void) {
    REG_PC=m68k_read_memory_32(REG_A[7]); REG_A[7]+=4;
}
static void interrupt_word(uint16_t value) {
    fa18_service_extension_words(3);
    m68k_write_memory_16(0xDFF09Au,value);
    FLAG_N=NFLAG_16(value); FLAG_Z=value; FLAG_V=VFLAG_CLEAR; FLAG_C=CFLAG_CLEAR;
}
static void interrupt_depth(int enable) {
    uint32_t address=displaced(REG_A[6],AMIGA_EXEC_ID_NEST_CNT);
    uint8_t old=(uint8_t)m68k_read_memory_8(address);
    uint32_t result=enable?(uint32_t)old-1u:(uint32_t)old+1u;
    FLAG_N=NFLAG_8(result); FLAG_Z=result&0xFFu;
    FLAG_V=enable?VFLAG_SUB_8(1u,old,result):VFLAG_ADD_8(1u,old,result);
    FLAG_X=FLAG_C=CFLAG_8(result);
    m68k_write_memory_8(address,enable?fa18_os_enable_depth(old):fa18_os_disable_depth(old));
}
int fa18_os_exec_find_name_step(void) {
    uint32_t pc=REG_PC; uint16_t op;
    switch (pc) {
    case 0xFC1696u: op=0x2F0A; break;
    case 0xFC1698u: op=0x2448; break;
    case 0xFC169Au: op=0x2209; break;
    case 0xFC169Cu: case 0xFC16A2u: op=0x2012; break;
    case 0xFC169Eu: op=0x6718; break;
    case 0xFC16A0u: op=0x2440; break;
    case 0xFC16A4u: op=0x6712; break;
    case 0xFC16A6u: op=0x206A; break;
    case 0xFC16AAu: case 0xFC16B8u: op=0x2241; break;
    case 0xFC16ACu: op=0xB308; break;
    case 0xFC16AEu: op=0x66F0; break;
    case 0xFC16B0u: op=0x4A28; break;
    case 0xFC16B4u: op=0x66F6; break;
    case 0xFC16B6u: op=0x200A; break;
    case 0xFC16BAu: op=0x245F; break;
    case 0xFC16BCu: op=0x4E75; break;
    default: return 0;
    }
    fa18_service_begin(pc,op);
    switch (pc) {
    case 0xFC1696u:
        REG_A[7]-=4;
        m68k_write_memory_16(REG_A[7]+2,(uint16_t)REG_A[2]);
        m68k_write_memory_16(REG_A[7],(uint16_t)(REG_A[2]>>16));
        long_flags(REG_A[2]); break;
    case 0xFC1698u: REG_A[2]=REG_A[0]; break;
    case 0xFC169Au: REG_D[1]=REG_A[1]; long_flags(REG_D[1]); break;
    case 0xFC169Cu: case 0xFC16A2u:
        REG_D[0]=m68k_read_memory_32(REG_A[2]); long_flags(REG_D[0]); break;
    case 0xFC169Eu: case 0xFC16A4u:
        if (COND_EQ()) REG_PC=0xFC16B8u; else USE_CYCLES(CYC_BCC_NOTAKE_B); break;
    case 0xFC16A0u: REG_A[2]=REG_D[0]; break;
    case 0xFC16A6u: REG_A[0]=m68k_read_memory_32(displaced(REG_A[2],AMIGA_NODE_NAME)); break;
    case 0xFC16AAu: case 0xFC16B8u: REG_A[1]=REG_D[1]; break;
    case 0xFC16ACu: compare_name_byte(); break;
    case 0xFC16AEu:
        if (COND_NE()) REG_PC=0xFC16A0u; else USE_CYCLES(CYC_BCC_NOTAKE_B); break;
    case 0xFC16B0u: byte_flags((uint8_t)m68k_read_memory_8(displaced(REG_A[0],-1))); break;
    case 0xFC16B4u:
        if (COND_NE()) REG_PC=0xFC16ACu; else USE_CYCLES(CYC_BCC_NOTAKE_B); break;
    case 0xFC16B6u: REG_D[0]=REG_A[2]; long_flags(REG_D[0]); break;
    case 0xFC16BAu: REG_A[2]=m68k_read_memory_32(REG_A[7]); REG_A[7]+=4; break;
    case 0xFC16BCu: return_to_caller(); break;
    }
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
int fa18_os_exec_find_task_step(void) {
    uint32_t pc=REG_PC,address; uint16_t op;
    switch (pc) {
    case 0xFC1DB0u: op=0x2009; break;
    case 0xFC1DB2u: op=0x6606; break;
    case 0xFC1DB4u: case 0xFC1DF0u: op=0x202E; break;
    case 0xFC1DB8u: op=0x6048; break;
    case 0xFC1DBAu: case 0xFC1DD2u: op=0x41EE; break;
    case 0xFC1DBEu: case 0xFC1DFAu: op=0x33FC; break;
    case 0xFC1DC6u: op=0x522E; break;
    case 0xFC1DCAu: case 0xFC1DD6u: op=0x4EAE; break;
    case 0xFC1DCEu: case 0xFC1DDAu: op=0x4A80; break;
    case 0xFC1DD0u: op=0x6622; break;
    case 0xFC1DDCu: op=0x6616; break;
    case 0xFC1DDEu: op=0x206E; break;
    case 0xFC1DE2u: op=0x2068; break;
    case 0xFC1DE6u: op=0xB308; break;
    case 0xFC1DE8u: op=0x660A; break;
    case 0xFC1DEAu: op=0x4A28; break;
    case 0xFC1DEEu: op=0x66F6; break;
    case 0xFC1DF4u: op=0x532E; break;
    case 0xFC1DF8u: op=0x6C08; break;
    case 0xFC1E02u: op=0x4E75; break;
    default: return 0;
    }
    fa18_service_begin(pc,op);
    switch (pc) {
    case 0xFC1DB0u: REG_D[0]=REG_A[1]; long_flags(REG_D[0]); break;
    case 0xFC1DB2u:
        if (COND_NE()) REG_PC=0xFC1DBAu; else USE_CYCLES(CYC_BCC_NOTAKE_B); break;
    case 0xFC1DB4u: case 0xFC1DF0u:
        REG_D[0]=m68k_read_memory_32(displaced(REG_A[6],AMIGA_EXEC_THIS_TASK)); long_flags(REG_D[0]); break;
    case 0xFC1DB8u: REG_PC=0xFC1E02u; break;
    case 0xFC1DBAu: case 0xFC1DD2u:
        REG_A[0]=displaced(REG_A[6],pc==0xFC1DBAu?AMIGA_EXEC_TASK_READY:AMIGA_EXEC_TASK_WAIT); break;
    case 0xFC1DBEu: case 0xFC1DFAu: interrupt_word(pc==0xFC1DBEu?0x4000u:0xC000u); break;
    case 0xFC1DC6u: case 0xFC1DF4u: interrupt_depth(pc==0xFC1DF4u); break;
    case 0xFC1DCAu: case 0xFC1DD6u:
        address=displaced(REG_A[6],AMIGA_EXEC_FIND_NAME_LVO); fa18_service_call(address); break;
    case 0xFC1DCEu: case 0xFC1DDAu: long_flags(REG_D[0]); break;
    case 0xFC1DD0u: case 0xFC1DDCu: case 0xFC1DE8u:
        if (COND_NE()) REG_PC=0xFC1DF4u; else USE_CYCLES(CYC_BCC_NOTAKE_B); break;
    case 0xFC1DDEu: REG_A[0]=m68k_read_memory_32(displaced(REG_A[6],AMIGA_EXEC_THIS_TASK)); break;
    case 0xFC1DE2u: REG_A[0]=m68k_read_memory_32(displaced(REG_A[0],AMIGA_NODE_NAME)); break;
    case 0xFC1DE6u: compare_name_byte(); break;
    case 0xFC1DEAu: byte_flags((uint8_t)m68k_read_memory_8(displaced(REG_A[0],-1))); break;
    case 0xFC1DEEu:
        if (COND_NE()) REG_PC=0xFC1DE6u; else USE_CYCLES(CYC_BCC_NOTAKE_B); break;
    case 0xFC1DF8u:
        if (COND_GE()) REG_PC=0xFC1E02u; else USE_CYCLES(CYC_BCC_NOTAKE_B); break;
    case 0xFC1E02u: return_to_caller(); break;
    }
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
