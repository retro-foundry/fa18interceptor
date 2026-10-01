#ifndef FA18_GLUE_RENDERER_STEP_MATH_H
#define FA18_GLUE_RENDERER_STEP_MATH_H
/* CPU arithmetic shared by record-view and the four-plane polygon clip
 * timing bridges. Source-width operations retain every register high half. */
#include "glue_step.h"

static void renderer_add_byte(uint32_t *reg, uint8_t source) {
    uint8_t old = (uint8_t)*reg;
    uint32_t result = (uint32_t)old + source;
    SET_B(*reg, result); FLAG_N = NFLAG_8(result); FLAG_Z = result & 0xffu;
    FLAG_V = VFLAG_ADD_8(source, old, result); FLAG_X = FLAG_C = CFLAG_8(result);
}
static void renderer_asr_word(uint32_t *reg, unsigned count) {
    uint16_t old = (uint16_t)*reg, result;
    count &= 63u;
    result = count < 16 ? (uint16_t)((int16_t)old >> count) : (uint16_t)((int16_t)old >> 15);
    SET_W(*reg, result); flags_logic_w(result); FLAG_C = 0;
    if (count) FLAG_X = FLAG_C = count <= 16 ? ((old >> (count - 1)) & 1u) << 8 : (old >> 15) << 8;
    USE_CYCLES(count << CYC_SHIFT);
}
static void renderer_asl_word(uint32_t *reg, unsigned count) {
    uint16_t old = (uint16_t)*reg, result;
    uint32_t mask;
    count &= 63u; result = count < 16 ? (uint16_t)(old << count) : 0;
    SET_W(*reg, result); flags_logic_w(result); FLAG_C = FLAG_V = 0;
    if (count) {
        FLAG_X = FLAG_C = count <= 16 ? ((old >> (16 - count)) & 1u) << 8 : 0;
        if (count < 16) {
            mask = (0xffffu << (15 - count)) & 0xffffu;
            old &= mask; FLAG_V = (old != 0 && old != mask) << 7;
        } else if (count == 16) FLAG_V = (old != 0 && old != 0xffffu) << 7;
        else FLAG_V = (old != 0) << 7;
    }
    USE_CYCLES(count << CYC_SHIFT);
}
static void renderer_negate(uint32_t *reg, unsigned width) {
    uint32_t old = *reg;
    if (width == 1) { SET_B(*reg, 0); step_subtract_byte(reg, old); }
    else if (width == 2) { SET_W(*reg, 0); step_subtract_word(reg, old); }
    else { *reg = 0; step_subtract_long(reg, old); }
}
static void renderer_load(uint32_t address, uint16_t mask, unsigned width, int postincrement) {
    unsigned i, count = 0;
    for (i = 0; i < 16; ++i) if (mask & (1u << i)) {
        REG_DA[i] = width == 2 ? (uint32_t)(int32_t)(int16_t)m68k_read_memory_16(address) : m68k_read_memory_32(address);
        address += width; ++count;
    }
    if (postincrement >= 0) A(postincrement) = address;
    USE_CYCLES(count << (width == 2 ? CYC_MOVEM_W : CYC_MOVEM_L));
}
static void renderer_store(uint32_t address, uint16_t mask, unsigned width, int predecrement) {
    unsigned i, count = 0;
    for (i = 0; i < 16; ++i) if (mask & (1u << i)) {
        uint32_t value = REG_DA[predecrement >= 0 ? 15 - i : i];
        if (predecrement >= 0) address -= width;
        if (width == 2) m68k_write_memory_16(address, value);
        else if (predecrement >= 0) {
            m68k_write_memory_16(address + 2, value); m68k_write_memory_16(address, value >> 16);
        } else m68k_write_memory_32(address, value);
        if (predecrement < 0) address += width;
        ++count;
    }
    if (predecrement >= 0) A(predecrement) = address;
    USE_CYCLES(count << (width == 2 ? CYC_MOVEM_W : CYC_MOVEM_L));
}
static void renderer_multiply(uint32_t *reg, uint16_t source) {
    uint32_t bits = (uint32_t)source << 1;
    unsigned i, transitions = 0;
    for (i = 0; i < 16; ++i, bits >>= 1)
        if ((bits & 3u) == 1u || (bits & 3u) == 2u) ++transitions;
    USE_CYCLES(2 * transitions);
    *reg = (uint32_t)((int32_t)(int16_t)*reg * (int32_t)(int16_t)source);
    flags_logic_l(*reg);
}
static void renderer_divide(uint32_t *reg, int16_t divisor) {
    int32_t dividend = (int32_t)*reg, quotient, remainder;
    uint32_t absolute_dividend, absolute_divisor, magnitude;
    int cycles = 6, i;
    if (!divisor) { m68ki_exception_trap(EXCEPTION_ZERO_DIVIDE); return; }
    absolute_dividend = dividend < 0 ? 0u - (uint32_t)dividend : (uint32_t)dividend;
    absolute_divisor = divisor < 0 ? (uint32_t)-divisor : (uint32_t)divisor;
    if (dividend < 0) ++cycles;
    if ((absolute_dividend >> 16) >= absolute_divisor) cycles = (cycles + 2) * 2;
    else {
        magnitude = absolute_dividend / absolute_divisor; cycles += 55;
        if (divisor >= 0) cycles += dividend >= 0 ? -1 : 1;
        for (i = 0; i < 15; ++i, magnitude <<= 1)
            if ((int16_t)magnitude >= 0) ++cycles;
        cycles *= 2;
    }
    USE_CYCLES(cycles - 158);
    if ((uint32_t)dividend == 0x80000000u && divisor == -1) {
        FLAG_Z = FLAG_N = FLAG_V = FLAG_C = 0; *reg = 0; return;
    }
    quotient = dividend / divisor; remainder = dividend % divisor;
    if (quotient == (int16_t)quotient) {
        FLAG_Z = (uint16_t)quotient; FLAG_N = NFLAG_16(quotient); FLAG_V = FLAG_C = 0;
        *reg = ((uint32_t)(uint16_t)remainder << 16) | (uint16_t)quotient;
    } else FLAG_V = VFLAG_SET;
}

#endif
