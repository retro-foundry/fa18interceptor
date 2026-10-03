#ifndef FA18_GLUE_CACHE_STEP_OPERANDS_H
#define FA18_GLUE_CACHE_STEP_OPERANDS_H
#include "glue_step.h"
#include <stdlib.h>

/* Only the operand forms used by this source family are accepted. */
static uint32_t cache_step_address(unsigned mode, unsigned reg, unsigned width) {
    uint32_t address;
    switch (mode) {
    case 2: return A(reg);
    case 3: address = A(reg); A(reg) += width; return address;
    case 4: A(reg) -= width; return A(reg);
    case 5: return step_displacement(A(reg));
    case 6: return step_indexed(A(reg));
    case 7:
        if (reg == 1) return m68ki_read_imm_32();
        if (reg == 2) return step_displacement(REG_PC);
        if (reg == 3) return step_indexed(REG_PC);
        break;
    }
    abort();
}

static uint32_t cache_step_read_memory(uint32_t address, unsigned width) {
    if (width == 1) return m68k_read_memory_8(address);
    if (width == 2) return m68k_read_memory_16(address);
    return m68k_read_memory_32(address);
}
static void cache_step_write_memory(uint32_t address, uint32_t value,
                                   unsigned width, int predecrement) {
    if (width == 1) m68k_write_memory_8(address, value);
    else if (width == 2) step_write_word(address, value);
    else if (predecrement) {
        m68k_write_memory_16(address + 2, value);
        m68k_write_memory_16(address, value >> 16);
    } else step_write_long(address, value);
}
static uint32_t cache_step_read(unsigned mode, unsigned reg, unsigned width) {
    if (mode == 0) return D(reg);
    if (mode == 1) return A(reg);
    if (mode == 7 && reg == 4)
        return width == 4 ? m68ki_read_imm_32() : m68ki_read_imm_16();
    return cache_step_read_memory(cache_step_address(mode, reg, width), width);
}
static void cache_step_logic(uint32_t value, unsigned width) {
    if (width == 1) flags_logic_b(value);
    else if (width == 2) flags_logic_w(value);
    else flags_logic_l(value);
}
static void cache_step_write(unsigned mode, unsigned reg, unsigned width, uint32_t value) {
    if (mode == 0) {
        if (width == 1) SET_B(D(reg), value);
        else if (width == 2) SET_W(D(reg), value);
        else D(reg) = value;
    } else if (mode == 1) {
        A(reg) = width == 2 ? (uint32_t)(int32_t)(int16_t)value : value;
    } else cache_step_write_memory(cache_step_address(mode, reg, width), value, width, mode == 4);
}

#endif
