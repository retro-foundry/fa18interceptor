/* Original Exec memory-list ABI and bus timeline; guest allocation semantics
 * live in the reusable SDK/CPU/ROM-independent amiga_compat layer. */
#include "exec_memory_adapter.h"
#include "../amiga/exec_memory.h"
#include "service_phase.h"
#include "m68kops.h"
#include "machine.h"
#include "service_dispatch_adapter.h"
#include <string.h>
static uint32_t signature(const uint8_t *rom,unsigned start,unsigned end) {
    uint32_t hash=2166136261u;
    for (unsigned i=start;i<end;++i) hash=(hash^rom[i])*16777619u;
    return hash;
}
int fa18_os_exec_memory_signature_matches(const uint8_t *rom) {
    return rom && signature(rom,0x16D8,0x190C)==0x4E783CB9u &&
        signature(rom,0x190C,0x195A)==0x3CAF2787u;
}
int fa18_os_exec_memory_enable_reference(void) {
    int enabled=fa18_os_exec_memory_signature_matches(fa18_machine->rom);
    fa18_service_enable(FA18_SERVICE_EXEC_MEMORY,enabled);
    return enabled;
}
static uint8_t read8(void *c,uint32_t a) { (void)c; return (uint8_t)m68k_read_memory_8(a); }
static uint16_t read16(void *c,uint32_t a) { (void)c; return (uint16_t)m68k_read_memory_16(a); }
static uint32_t read32(void *c,uint32_t a) { (void)c; return m68k_read_memory_32(a); }
static void write8(void *c,uint32_t a,uint8_t v) { (void)c; m68k_write_memory_8(a,v); }
static void write16(void *c,uint32_t a,uint16_t v) { (void)c; m68k_write_memory_16(a,v); }
static void write32(void *c,uint32_t a,uint32_t v) { (void)c; m68k_write_memory_32(a,v); }
enum { NEXT,BRANCH,TRANSFER,CALL,DBRA };
static int phase_step(uint32_t pc,uint16_t op,unsigned ext,AmigaExecMemoryPhase phase,unsigned arg,int flow,uint32_t target) {
    AmigaExecTaskState s; AmigaExecMemoryEffect e;
    AmigaExecTaskBus b={NULL,read8,read16,read32,write8,write16,write32};
    memcpy(s.d,REG_D,sizeof s.d); memcpy(s.a,REG_A,sizeof s.a); s.ccr=(uint8_t)m68ki_get_ccr();
    fa18_service_begin(pc,op);
    if (flow!=DBRA) fa18_service_extension_words(ext);
    if (!amiga_exec_memory_step(phase,arg,&s,&b,&e)) return 0;
    memcpy(REG_D,s.d,sizeof s.d); memcpy(REG_A,s.a,sizeof s.a); m68ki_set_ccr(s.ccr);
    if (flow==BRANCH) {
        if (e.branch_taken) REG_PC=target;
        else USE_CYCLES(ext?CYC_BCC_NOTAKE_W:CYC_BCC_NOTAKE_B);
    } else if (flow==TRANSFER) { m68ki_trace_t0(); m68ki_jump(target); }
    else if (flow==CALL) fa18_service_call(target);
    else if (flow==DBRA) {
        if (e.dbra_continues) {
            fa18_service_extension_words(1); m68ki_trace_t0(); REG_PC=target; USE_CYCLES(CYC_DBCC_F_NOEXP);
        } else { REG_PC+=2; USE_CYCLES(CYC_DBCC_F_EXP); }
    }
    if (e.returned) REG_PC=e.return_pc;
    if (op==0xE48B) USE_CYCLES(2<<CYC_SHIFT);
    USE_CYCLES(e.transferred_longs<<CYC_MOVEM_L);
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
#define STEP(op,ext,phase,arg) return phase_step(pc,op,ext,AMIGA_MEM_##phase,arg,NEXT,0)
#define BR(op,ext,phase,target) return phase_step(pc,op,ext,AMIGA_MEM_##phase,0,BRANCH,target)
#define GO(op,ext,target) return phase_step(pc,op,ext,AMIGA_MEM_FLOW,0,TRANSFER,target)
#define JSR(op,ext,target) return phase_step(pc,op,ext,AMIGA_MEM_FLOW,0,CALL,target)
int fa18_os_exec_memory_step(void) {
    uint32_t pc=REG_PC;
    switch (pc) {
    case 0xFC16D8u: case 0xFC1740u: case 0xFC17FCu: case 0xFC1830u: STEP(0x4A80,0,TEST_SIZE,0);
    case 0xFC16DAu: BR(0x674C,0,EQ,0xFC1728);
    case 0xFC16DCu: STEP(0x48E7,1,SAVE_REGISTERS,0x0C08);
    case 0xFC16E0u: case 0xFC1754u: STEP(0x5E80,0,ROUND_ADD,0);
    case 0xFC16E2u: STEP(0x0200,1,ROUND_MASK_BYTE,0);
    case 0xFC16E6u: case 0xFC1910u: STEP(0x7600,0,RESULT_ZERO,0);
    case 0xFC16E8u: STEP(0xB0A8,1,COMPARE_TOTAL,0);
    case 0xFC16ECu: BR(0x6200,1,HI,0xFC1722);
    case 0xFC16F0u: case 0xFC175Cu: STEP(0x45E8,1,BEGIN_FREE_LINK,0);
    case 0xFC16F4u: case 0xFC1760u: case 0xFC176Cu: STEP(0x2612,0,NEXT_FREE_CHUNK,0);
    case 0xFC16F6u: BR(0x672A,0,EQ,0xFC1722);
    case 0xFC16F8u: STEP(0x2243,0,CHUNK_FROM_RESULT,0);
    case 0xFC16FAu: STEP(0xB0A9,1,COMPARE_CHUNK_SIZE,0);
    case 0xFC16FEu: BR(0x6304,0,LS,0xFC1704);
    case 0xFC1700u: STEP(0x2449,0,FOLLOW_CHUNK,0);
    case 0xFC1702u: GO(0x60F0,0,0xFC16F4);
    case 0xFC1704u: BR(0x6714,0,EQ,0xFC171A);
    case 0xFC1706u: STEP(0x47F1,1,SPLIT_ADDRESS,0);
    case 0xFC170Au: STEP(0x2691,0,COPY_SPLIT_LINK,0);
    case 0xFC170Cu: case 0xFC1798u: STEP(0x2629,1,LOAD_CHUNK_SIZE,0);
    case 0xFC1710u: STEP(0x9680,0,SUBTRACT_SIZE,0);
    case 0xFC1712u: STEP(0x2743,1,STORE_SPLIT_SIZE,0);
    case 0xFC1716u: STEP(0x248B,0,LINK_SPLIT,0);
    case 0xFC1718u: GO(0x6002,0,0xFC171C);
    case 0xFC171Au: STEP(0x2491,0,UNLINK_CHUNK,0);
    case 0xFC171Cu: STEP(0x91A8,1,SUBTRACT_TOTAL,0);
    case 0xFC1720u: STEP(0x2609,0,RESULT_ADDRESS,0);
    case 0xFC1722u: case 0xFC1952u: STEP(0x2003,0,RETURN_RESULT,0);
    case 0xFC1724u: STEP(0x4CDF,1,RESTORE_REGISTERS,0x0C08);
    case 0xFC1728u: case 0xFC17B8u: case 0xFC182Au: case 0xFC1854u:
    case 0xFC187Au: case 0xFC1958u: STEP(0x4E75,0,RETURN,0);
    case 0xFC172Au: case 0xFC17BAu: STEP(0x48E7,1,SAVE_REGISTERS,0x6080);
    case 0xFC172Eu: case 0xFC17BEu: STEP(0x2E3C,2,ALERT_NUMBER,pc==0xFC172E?0x81000005u:0x81000009u);
    case 0xFC1734u: case 0xFC17C4u: STEP(0x2C78,1,EXEC_BASE,0);
    case 0xFC1738u: case 0xFC17C8u: JSR(0x4EAE,1,REG_A[6]-108);
    case 0xFC173Cu: case 0xFC17CCu: STEP(0x4CDF,1,RESTORE_REGISTERS,0x6080);
    case 0xFC1742u: BR(0x6774,0,EQ,0xFC17B8);
    case 0xFC1744u: case 0xFC190Cu: STEP(0x48E7,1,SAVE_REGISTERS,0x0408);
    case 0xFC1748u: STEP(0x2209,0,ADDRESS_OFFSET,0);
    case 0xFC174Au: STEP(0x76F8,0,ALIGNMENT_MASK,0);
    case 0xFC174Cu: STEP(0xC283,0,ALIGN_ADDRESS,0);
    case 0xFC174Eu: STEP(0xC389,0,EXCHANGE_ADDRESS,0);
    case 0xFC1750u: STEP(0x9289,0,ADDRESS_REMAINDER,0);
    case 0xFC1752u: STEP(0xD081,0,ADD_REMAINDER,0);
    case 0xFC1756u: STEP(0xC083,0,ALIGN_SIZE,0);
    case 0xFC1758u: BR(0x6700,1,EQ,0xFC17B4);
    case 0xFC1762u: BR(0x6720,0,EQ,0xFC1784);
    case 0xFC1764u: STEP(0xB3C3,0,COMPARE_FREE_ADDRESS,0);
    case 0xFC1766u: BR(0x6508,0,CS,0xFC1770);
    case 0xFC1768u: BR(0x6750,0,EQ,0xFC17BA);
    case 0xFC176Au: STEP(0x2443,0,FOLLOW_RESULT,0);
    case 0xFC176Eu: BR(0x66F4,0,NE,0xFC1764);
    case 0xFC1770u: STEP(0x7210,0,HEADER_LINK_OFFSET,0);
    case 0xFC1772u: STEP(0xD288,0,ADD_HEADER_ADDRESS,0);
    case 0xFC1774u: STEP(0xB28A,0,COMPARE_LINK_ADDRESS,0);
    case 0xFC1776u: BR(0x670C,0,EQ,0xFC1784);
    case 0xFC1778u: case 0xFC17A8u: STEP(0x262A,1,LOAD_LEFT_SIZE,0);
    case 0xFC177Cu: STEP(0xD68A,0,LEFT_END,0);
    case 0xFC177Eu: STEP(0xB689,0,COMPARE_LEFT_END,0);
    case 0xFC1780u: BR(0x670C,0,EQ,0xFC178E);
    case 0xFC1782u: BR(0x6236,0,HI,0xFC17BA);
    case 0xFC1784u: STEP(0x2292,0,COPY_LEFT_LINK,0);
    case 0xFC1786u: STEP(0x2489,0,LINK_FREED_CHUNK,0);
    case 0xFC1788u: STEP(0x2340,1,STORE_FREED_SIZE,0);
    case 0xFC178Cu: GO(0x6006,0,0xFC1794);
    case 0xFC178Eu: STEP(0xD1AA,1,EXTEND_LEFT,0);
    case 0xFC1792u: STEP(0x224A,0,USE_LEFT_CHUNK,0);
    case 0xFC1794u: STEP(0x4A91,0,TEST_NEXT_CHUNK,0);
    case 0xFC1796u: BR(0x6718,0,EQ,0xFC17B0);
    case 0xFC179Cu: STEP(0xD689,0,CHUNK_END,0);
    case 0xFC179Eu: STEP(0xB691,0,COMPARE_RIGHT_ADDRESS,0);
    case 0xFC17A0u: BR(0x6218,0,HI,0xFC17BA);
    case 0xFC17A2u: BR(0x660C,0,NE,0xFC17B0);
    case 0xFC17A4u: STEP(0x2451,0,USE_RIGHT_CHUNK,0);
    case 0xFC17A6u: STEP(0x2292,0,COPY_RIGHT_LINK,0);
    case 0xFC17ACu: STEP(0xD7A9,1,EXTEND_RIGHT,0);
    case 0xFC17B0u: STEP(0xD1A8,1,ADD_TOTAL,0);
    case 0xFC17B4u: case 0xFC1954u: STEP(0x4CDF,1,RESTORE_REGISTERS,0x0408);
    case 0xFC17D0u: case 0xFC182Cu: case 0xFC1856u: case 0xFC1916u: STEP(0x522E,1,TASK_DEPTH_UP,0);
    case 0xFC17D4u: STEP(0x48E7,1,SAVE_REGISTERS,0x040C);
    case 0xFC17D8u: STEP(0x2600,0,SAVE_SIZE,0);
    case 0xFC17DAu: STEP(0x2401,0,SAVE_REQUIREMENTS,0);
    case 0xFC17DCu: STEP(0x45EE,1,BEGIN_HEADERS_A2,0);
    case 0xFC17E0u: STEP(0x2452,0,NEXT_HEADER_A2,0);
    case 0xFC17E2u: STEP(0x4A92,0,TEST_HEADER_A2,0);
    case 0xFC17E4u: BR(0x6604,0,NE,0xFC17EA);
    case 0xFC17E6u: case 0xFC185Eu: STEP(0x7000,0,NO_ALLOCATION,0);
    case 0xFC17E8u: GO(0x6038,0,0xFC1822);
    case 0xFC17EAu: STEP(0x302A,1,LOAD_ATTRIBUTES_A2,0);
    case 0xFC17EEu: STEP(0xC042,0,MASK_ATTRIBUTES_D2,0);
    case 0xFC17F0u: STEP(0xB042,0,COMPARE_ATTRIBUTES_D2,0);
    case 0xFC17F2u: BR(0x66EC,0,NE,0xFC17E0);
    case 0xFC17F4u: STEP(0x204A,0,USE_HEADER_A2,0);
    case 0xFC17F6u: STEP(0x2003,0,RESTORE_SIZE,0);
    case 0xFC17F8u: JSR(0x6100,1,0xFC16D8);
    case 0xFC17FEu: BR(0x67E0,0,EQ,0xFC17E0);
    case 0xFC1800u: STEP(0x0802,1,TEST_CLEAR,0);
    case 0xFC1804u: BR(0x671C,0,EQ,0xFC1822);
    case 0xFC1806u: STEP(0x7200,0,CLEAR_VALUE,0);
    case 0xFC1808u: STEP(0x5683,0,CLEAR_ROUND,0);
    case 0xFC180Au: STEP(0xE48B,0,CLEAR_WORD_COUNT,0);
    case 0xFC180Cu: STEP(0x2040,0,CLEAR_ADDRESS,0);
    case 0xFC180Eu: GO(0x6002,0,0xFC1812);
    case 0xFC1810u: STEP(0x20C1,0,CLEAR_WORD,0);
    case 0xFC1812u: return phase_step(pc,0x51CB,1,AMIGA_MEM_NEXT_CLEAR_WORD,0,DBRA,0xFC1810);
    case 0xFC1816u: case 0xFC181Eu: STEP(0x4843,0,SWAP_WORD_COUNT,0);
    case 0xFC1818u: STEP(0x4A43,0,TEST_WORD_COUNT,0);
    case 0xFC181Au: BR(0x6706,0,EQ,0xFC1822);
    case 0xFC181Cu: STEP(0x5343,0,DECREMENT_WORD_COUNT,0);
    case 0xFC1820u: GO(0x60EE,0,0xFC1810);
    case 0xFC1822u: case 0xFC1850u: case 0xFC1876u: case 0xFC194Eu: JSR(0x4EAE,1,REG_A[6]-138);
    case 0xFC1826u: STEP(0x4CDF,1,RESTORE_REGISTERS,0x040C);
    case 0xFC1832u: BR(0x671C,0,EQ,0xFC1850);
    case 0xFC1834u: case 0xFC185Au: STEP(0x41EE,1,BEGIN_HEADERS_A0,0);
    case 0xFC1838u: case 0xFC1860u: STEP(0x2050,0,NEXT_HEADER_A0,0);
    case 0xFC183Au: case 0xFC1862u: STEP(0x4A90,0,TEST_HEADER_A0,0);
    case 0xFC183Cu: BR(0x6700,1,EQ,0xFC172A);
    case 0xFC1840u: case 0xFC1866u: STEP(0xB3E8,1,COMPARE_LOWER,0);
    case 0xFC1844u: BR(0x65F2,0,CS,0xFC1838);
    case 0xFC1846u: case 0xFC186Cu: STEP(0xB3E8,1,COMPARE_UPPER,0);
    case 0xFC184Au: BR(0x64EC,0,CC,0xFC1838);
    case 0xFC184Cu: JSR(0x6100,1,0xFC1740);
    case 0xFC1864u: BR(0x6710,0,EQ,0xFC1876);
    case 0xFC186Au: BR(0x65F4,0,CS,0xFC1860);
    case 0xFC1870u: BR(0x64EE,0,CC,0xFC1860);
    case 0xFC1872u: STEP(0x3028,1,LOAD_ATTRIBUTES_A0,0);
    case 0xFC187Cu: STEP(0x522E,1,TASK_DEPTH_UP,0);
    case 0xFC1880u: STEP(0x48E7,1,SAVE_REGISTERS,0x0C1C);
    case 0xFC1884u: STEP(0x2409,0,ABSOLUTE_OFFSET,0);
    case 0xFC1886u: STEP(0x0282,2,MASK_ABSOLUTE_OFFSET,0);
    case 0xFC188Cu: STEP(0x93C2,0,ALIGN_ABSOLUTE_ADDRESS,0);
    case 0xFC188Eu: STEP(0xD082,0,ADD_ABSOLUTE_OFFSET,0);
    case 0xFC1890u: STEP(0x5E80,0,ROUND_ADD,0);
    case 0xFC1892u: STEP(0x0200,1,ROUND_MASK_BYTE,0);
    case 0xFC1896u: STEP(0x41EE,1,BEGIN_HEADERS_A0,0);
    case 0xFC189Au: STEP(0x2050,0,NEXT_HEADER_A0,0);
    case 0xFC189Cu: STEP(0x4A90,0,TEST_HEADER_A0,0);
    case 0xFC189Eu: BR(0x6768,0,EQ,0xFC1908);
    case 0xFC18A0u: STEP(0xB3E8,1,COMPARE_LOWER,0);
    case 0xFC18A4u: BR(0x65F4,0,CS,0xFC189A);
    case 0xFC18A6u: STEP(0xB3E8,1,COMPARE_UPPER,0);
    case 0xFC18AAu: BR(0x64EE,0,CC,0xFC189A);
    case 0xFC18ACu: STEP(0xB0A8,1,COMPARE_TOTAL,0);
    case 0xFC18B0u: BR(0x6256,0,HI,0xFC1908);
    case 0xFC18B2u: STEP(0x2649,0,REMEMBER_ABSOLUTE_ADDRESS,0);
    case 0xFC18B4u: STEP(0x2409,0,ABSOLUTE_END_START,0);
    case 0xFC18B6u: STEP(0xD480,0,ABSOLUTE_END_SIZE,0);
    case 0xFC18B8u: STEP(0x45E8,1,BEGIN_FREE_LINK,0);
    case 0xFC18BCu: STEP(0x2612,0,NEXT_FREE_CHUNK,0);
    case 0xFC18BEu: BR(0x6748,0,EQ,0xFC1908);
    case 0xFC18C0u: STEP(0x2243,0,CHUNK_FROM_RESULT,0);
    case 0xFC18C2u: STEP(0x2829,1,LOAD_ABSOLUTE_CHUNK_SIZE,0);
    case 0xFC18C6u: STEP(0xD883,0,ABSOLUTE_CHUNK_END,0);
    case 0xFC18C8u: STEP(0xB882,0,COMPARE_ABSOLUTE_END,0);
    case 0xFC18CAu: BR(0x6404,0,CC,0xFC18D0);
    case 0xFC18CCu: STEP(0x2449,0,FOLLOW_CHUNK,0);
    case 0xFC18CEu: GO(0x60EC,0,0xFC18BC);
    case 0xFC18D0u: case 0xFC18ECu: STEP(0xB68B,0,COMPARE_ABSOLUTE_START,0);
    case 0xFC18D2u: BR(0x6234,0,HI,0xFC1908);
    case 0xFC18D4u: STEP(0x91A8,1,SUBTRACT_TOTAL,0);
    case 0xFC18D8u: STEP(0x9882,0,ABSOLUTE_REMAINDER,0);
    case 0xFC18DAu: BR(0x6604,0,NE,0xFC18E0);
    case 0xFC18DCu: STEP(0x2051,0,ABSOLUTE_SUCCESSOR,0);
    case 0xFC18DEu: GO(0x600C,0,0xFC18EC);
    case 0xFC18E0u: STEP(0x41F3,1,ABSOLUTE_SPLIT_ADDRESS,0);
    case 0xFC18E4u: STEP(0x2091,0,COPY_ABSOLUTE_SPLIT_LINK,0);
    case 0xFC18E6u: STEP(0x2288,0,LINK_ABSOLUTE_SPLIT,0);
    case 0xFC18E8u: STEP(0x2144,1,STORE_ABSOLUTE_SPLIT_SIZE,0);
    case 0xFC18EEu: BR(0x670A,0,EQ,0xFC18FA);
    case 0xFC18F0u: STEP(0x968B,0,ABSOLUTE_LEFT_SIZE,0);
    case 0xFC18F2u: STEP(0x4483,0,NEGATE_LEFT_SIZE,0);
    case 0xFC18F4u: STEP(0x2343,1,STORE_ABSOLUTE_LEFT_SIZE,0);
    case 0xFC18F8u: GO(0x6002,0,0xFC18FC);
    case 0xFC18FAu: STEP(0x2488,0,LINK_ABSOLUTE_SUCCESSOR,0);
    case 0xFC18FCu: STEP(0x200B,0,ABSOLUTE_RESULT,0);
    case 0xFC18FEu: STEP(0x4CDF,1,RESTORE_REGISTERS,0x0C1C);
    case 0xFC1902u: JSR(0x4EAE,1,REG_A[6]-138);
    case 0xFC1906u: STEP(0x4E75,0,RETURN,0);
    case 0xFC1908u: STEP(0x7000,0,NO_ALLOCATION,0);
    case 0xFC190Au: GO(0x60F2,0,0xFC18FE);
    case 0xFC1912u: STEP(0x43EE,1,BEGIN_HEADERS_A1,0);
    case 0xFC191Au: STEP(0x2251,0,NEXT_HEADER_A1,0);
    case 0xFC191Cu: STEP(0x4A91,0,TEST_HEADER_A1,0);
    case 0xFC191Eu: BR(0x672E,0,EQ,0xFC194E);
    case 0xFC1920u: STEP(0x3029,1,LOAD_ATTRIBUTES_A1,0);
    case 0xFC1924u: STEP(0xC041,0,MASK_ATTRIBUTES_D1,0);
    case 0xFC1926u: STEP(0xB041,0,COMPARE_ATTRIBUTES_D1,0);
    case 0xFC1928u: BR(0x66F0,0,NE,0xFC191A);
    case 0xFC192Au: STEP(0x0801,1,TEST_LARGEST,0);
    case 0xFC192Eu: BR(0x6606,0,NE,0xFC1936);
    case 0xFC1930u: STEP(0xD6A9,1,ACCUMULATE_FREE,0);
    case 0xFC1934u: GO(0x60E4,0,0xFC191A);
    case 0xFC1936u: STEP(0x2029,1,FIRST_CHUNK_D0,0);
    case 0xFC193Au: BR(0x67DE,0,EQ,0xFC191A);
    case 0xFC193Cu: STEP(0x2040,0,CHUNK_FROM_D0,0);
    case 0xFC193Eu: STEP(0xB6A8,1,COMPARE_LARGEST,0);
    case 0xFC1942u: BR(0x6C06,0,GE,0xFC194A);
    case 0xFC1944u: STEP(0x2448,0,REMEMBER_LARGEST,0);
    case 0xFC1946u: STEP(0x2628,1,LOAD_LARGEST,0);
    case 0xFC194Au: STEP(0x2010,0,NEXT_CHUNK_D0,0);
    case 0xFC194Cu: GO(0x60EC,0,0xFC193A);
    default: return 0;
    }
}
