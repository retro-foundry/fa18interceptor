#ifndef FA18_GLUE_STEP_H
#define FA18_GLUE_STEP_H
/* Source instruction plumbing shared by timing bridges. CPU state stays in
 * glue; the game operations remain in their readable domain modules. */
#include "glue.h"
#include "hardware.h"
#include "bus.h"
#include "m68kops.h"

static inline uint16_t step_begin(uint32_t pc) {
    uint16_t opcode = fa18_bus_read16(pc);
    fa18_bus_finish(pc); fa18_bus_begin(pc); fa18_bus_fetch(pc);
    REG_PPC = pc; REG_IR = opcode; REG_PC = pc + 2;
    return opcode;
}
static inline uint32_t step_displacement(uint32_t base) {
    return base + (int16_t)m68ki_read_imm_16();
}
static inline uint32_t step_indexed(uint32_t base) {
    uint16_t extension = m68ki_read_imm_16();
    uint32_t index = REG_DA[extension >> 12];
    if (!(extension & 0x0800u)) index = (uint32_t)(int32_t)(int16_t)index;
    return base + index + (int8_t)extension;
}
static inline void step_write_word(uint32_t address, uint16_t value) {
    if (address >= 0xDFF000u && address < 0xDFF200u) {
        fa18_bus_access(address); custom_write(address - 0xDFF000u, value);
    } else m68k_write_memory_16(address, value);
}
static inline void step_write_long(uint32_t address, uint32_t value) {
    if (address >= 0xDFF000u && address < 0xDFF1FEu) {
        fa18_bus_access(address); fa18_bus_access(address + 2);
        custom_write_ptr(address - 0xDFF000u, value);
    } else m68k_write_memory_32(address, value);
}
static inline void step_predecrement_long(uint32_t value) {
    A(7) -= 4;
    m68k_write_memory_16(A(7) + 2, value);
    m68k_write_memory_16(A(7), value >> 16);
}
static inline void step_compare_word(uint16_t source, uint16_t destination) {
    uint32_t result = (uint32_t)destination - source;
    FLAG_N = NFLAG_16(result); FLAG_Z = result & 0xFFFFu;
    FLAG_V = VFLAG_SUB_16(source, destination, result); FLAG_C = CFLAG_16(result);
}
static inline void step_compare_long(uint32_t source, uint32_t destination) {
    uint32_t result = destination - source;
    FLAG_N = NFLAG_32(result); FLAG_Z = result;
    FLAG_V = VFLAG_SUB_32(source, destination, result);
    FLAG_C = CFLAG_SUB_32(source, destination, result);
}
static inline void step_add_word(uint32_t *reg, uint16_t value) {
    uint32_t old = *reg & 0xFFFFu, sum = old + value;
    SET_W(*reg, sum);
    FLAG_N = NFLAG_16(sum); FLAG_Z = sum & 0xFFFFu;
    FLAG_V = VFLAG_ADD_16(value, old, sum); FLAG_X = FLAG_C = CFLAG_16(sum);
}
static inline void step_add_long(uint32_t *reg, uint32_t value) {
    uint32_t old = *reg, sum = old + value;
    *reg = sum;
    FLAG_N = NFLAG_32(sum); FLAG_Z = sum;
    FLAG_V = VFLAG_ADD_32(value, old, sum);
    FLAG_X = FLAG_C = CFLAG_ADD_32(value, old, sum);
}
static inline void step_subtract_long(uint32_t *reg, uint32_t value) {
    uint32_t old = *reg, result = old - value;
    *reg = result;
    step_compare_long(value, old); FLAG_X = FLAG_C;
}
static inline void step_branch(uint32_t pc, uint16_t opcode, int take) {
    int displacement = (int8_t)opcode;
    if (!(opcode & 0xFFu)) displacement = (int16_t)m68ki_read_imm_16();
    if (take) REG_PC = pc + 2 + displacement;
    else USE_CYCLES((opcode & 0xFFu) ? CYC_BCC_NOTAKE_B : CYC_BCC_NOTAKE_W);
}
#endif
