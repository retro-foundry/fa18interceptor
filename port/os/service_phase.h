#ifndef FA18_SERVICE_PHASE_H
#define FA18_SERVICE_PHASE_H
#include "bus.h"
#include "m68kcpu.h"
/* The service phase's semantic C supplies its operands. Program bus phases
 * remain on the original timeline, but never fetch data from Kickstart. */
static inline void fa18_service_begin(uint32_t pc,uint16_t timing_opcode) {
    fa18_bus_begin_instruction(pc,timing_opcode);
    fa18_bus_fetch(pc);
    REG_PPC=pc; REG_IR=timing_opcode; REG_PC=pc+2;
}
static inline void fa18_service_extension_words(unsigned words) {
    while (words--) { fa18_bus_fetch(REG_PC); REG_PC+=2; }
}
/* Match the original 68000 JSR stack write and PC-change notification. */
static inline void fa18_service_call(uint32_t target) {
    m68ki_trace_t0();
    REG_A[7]-=4;
    m68k_write_memory_32(REG_A[7],REG_PC);
    m68ki_jump(target);
}
#endif
