/* CPU bridge for Kickstart 1.3 graphics.library OwnBlitter/DisownBlitter.
 * The internal JSR targets remain on the ordinary ROM path. */
#include "graphics_blitter_ownership.h"

#include <string.h>

#include "graphics.h"
#include "m68kcpu.h"
#include "m68kops.h"
#include "bus.h"
#include "machine.h"

int fa18_os_blitter_ownership_signature_matches(const uint8_t *rom) {
    static const uint8_t source[] = {
        0x52, 0x6E, 0x00, 0xAA, 0x66, 0x02, 0x4E, 0x75,
        0x48, 0xE7, 0xC0, 0xC0, 0x4E, 0xB9, 0x00, 0xFC, 0xF3, 0x24,
        0x4C, 0xDF, 0x03, 0x03, 0x4E, 0x75,
        0x53, 0x6E, 0x00, 0xAA, 0x6D, 0x60,
        0x48, 0xE7, 0xC0, 0xC0, 0x4E, 0xB9, 0x00, 0xFD, 0x3B, 0xC4,
        0x4C, 0xDF, 0x03, 0x03, 0x4A, 0xAE, 0x00, 0x3A, 0x67, 0x2A,
        0x08, 0x39, 0x00, 0x06, 0x00, 0xDF, 0xF0, 0x02,
        0x08, 0x39, 0x00, 0x06, 0x00, 0xDF, 0xF0, 0x02,
        0x66, 0x08, 0x33, 0xFC, 0x80, 0x40, 0x00, 0xDF, 0xF0, 0x9C,
        0x33, 0xFC, 0x80, 0x40, 0x00, 0xDF, 0xF0, 0x9A,
        0x00, 0x6E, 0x00, 0x02, 0x00, 0xA8, 0x60, 0x14,
        0x4A, 0xAE, 0x00, 0x42, 0x66, 0xD0,
        0x48, 0xE7, 0xC0, 0xC0, 0x4E, 0xB9, 0x00, 0xFC, 0xF3, 0x84,
        0x4C, 0xDF, 0x03, 0x03,
        0x48, 0xE7, 0xC0, 0xC0, 0x4E, 0xB9, 0x00, 0xFD, 0x3B, 0xD4,
        0x4C, 0xDF, 0x03, 0x03, 0x4E, 0x75
    };
    return rom && memcmp(rom + 0x64BC, source, sizeof source) == 0;
}

static uint32_t displacement_address(uint32_t base) {
    int16_t displacement = (int16_t)m68k_read_immediate_16(REG_PC);
    REG_PC += 2;
    return base + displacement;
}

static void word_flags(uint16_t value) {
    FLAG_N = NFLAG_16(value);
    FLAG_Z = value;
    FLAG_V = VFLAG_CLEAR;
    FLAG_C = CFLAG_CLEAR;
}

static void long_flags(uint32_t value) {
    FLAG_N = NFLAG_32(value);
    FLAG_Z = value;
    FLAG_V = VFLAG_CLEAR;
    FLAG_C = CFLAG_CLEAR;
}

static void adjust_depth(int disown) {
    uint32_t address = displacement_address(REG_A[6]);
    uint16_t old = m68k_read_memory_16(address);
    uint32_t result = disown ? (uint32_t)old - 1u : (uint32_t)old + 1u;
    uint16_t next = disown ? fa18_os_disown_blitter_depth(old) : fa18_os_own_blitter_depth(old);
    FLAG_N = NFLAG_16(result);
    FLAG_Z = result & 0xFFFFu;
    FLAG_V = disown ? VFLAG_SUB_16(1u, old, result) : VFLAG_ADD_16(1u, old, result);
    FLAG_X = FLAG_C = CFLAG_16(result);
    m68k_write_memory_16(address, next);
}

static void write_predecrement_long(uint32_t value) {
    REG_A[7] -= 4;
    m68k_write_memory_16(REG_A[7] + 2, (uint16_t)value);
    m68k_write_memory_16(REG_A[7], (uint16_t)(value >> 16));
}

static void save_registers(void) {
    (void)m68k_read_immediate_16(REG_PC);
    REG_PC += 2;
    write_predecrement_long(REG_A[1]);
    write_predecrement_long(REG_A[0]);
    write_predecrement_long(REG_D[1]);
    write_predecrement_long(REG_D[0]);
    USE_CYCLES(4 << CYC_MOVEM_L);
}

static void restore_registers(void) {
    (void)m68k_read_immediate_16(REG_PC);
    REG_PC += 2;
    REG_D[0] = m68k_read_memory_32(REG_A[7]);
    REG_A[7] += 4;
    REG_D[1] = m68k_read_memory_32(REG_A[7]);
    REG_A[7] += 4;
    REG_A[0] = m68k_read_memory_32(REG_A[7]);
    REG_A[7] += 4;
    REG_A[1] = m68k_read_memory_32(REG_A[7]);
    REG_A[7] += 4;
    USE_CYCLES(4 << CYC_MOVEM_L);
}

int fa18_os_blitter_ownership_step(void) {
    uint32_t pc = REG_PC, op, address;
    uint16_t value;
    if (pc != 0xFC64BCu && pc != 0xFC64C0u && pc != 0xFC64C2u &&
        pc != 0xFC64C4u && pc != 0xFC64CEu && pc != 0xFC64D2u &&
        pc != 0xFC64D4u && pc != 0xFC64D8u && pc != 0xFC64DAu &&
        pc != 0xFC64E4u && pc != 0xFC64E8u && pc != 0xFC64ECu &&
        pc != 0xFC64EEu && pc != 0xFC64F6u && pc != 0xFC64FEu &&
        pc != 0xFC6500u && pc != 0xFC6508u && pc != 0xFC6510u &&
        pc != 0xFC6516u && pc != 0xFC6518u && pc != 0xFC651Cu &&
        pc != 0xFC651Eu && pc != 0xFC6528u && pc != 0xFC652Cu &&
        pc != 0xFC6536u && pc != 0xFC653Au) return 0;
    op = fa18_bus_read16(pc);
    fa18_bus_begin(pc);
    fa18_bus_fetch(pc);
    REG_PPC = pc;
    REG_IR = op;
    REG_PC = pc + 2;
    switch (pc) {
    case 0xFC64BCu:
    case 0xFC64D4u:
        adjust_depth(pc == 0xFC64D4u);
        break;
    case 0xFC64C0u:
        if (COND_NE()) REG_PC = 0xFC64C4u;
        else USE_CYCLES(CYC_BCC_NOTAKE_B);
        break;
    case 0xFC64D8u:
        if (COND_LT()) REG_PC = 0xFC653Au;
        else USE_CYCLES(CYC_BCC_NOTAKE_B);
        break;
    case 0xFC64C4u:
    case 0xFC64DAu:
    case 0xFC651Eu:
    case 0xFC652Cu:
        save_registers();
        break;
    case 0xFC64CEu:
    case 0xFC64E4u:
    case 0xFC6528u:
    case 0xFC6536u:
        restore_registers();
        break;
    case 0xFC64E8u:
    case 0xFC6518u:
        long_flags(m68k_read_memory_32(displacement_address(REG_A[6])));
        break;
    case 0xFC64ECu:
        if (COND_EQ()) REG_PC = 0xFC6518u;
        else USE_CYCLES(CYC_BCC_NOTAKE_B);
        break;
    case 0xFC64EEu:
    case 0xFC64F6u:
        (void)m68k_read_immediate_16(REG_PC);
        REG_PC += 2;
        address = m68k_read_immediate_32(REG_PC);
        REG_PC += 4;
        FLAG_Z = fa18_os_blitter_busy((uint8_t)m68k_read_memory_8(address));
        break;
    case 0xFC64FEu:
        if (COND_NE()) REG_PC = 0xFC6508u;
        else USE_CYCLES(CYC_BCC_NOTAKE_B);
        break;
    case 0xFC6500u:
    case 0xFC6508u:
        value = (uint16_t)m68k_read_immediate_16(REG_PC);
        REG_PC += 2;
        address = m68k_read_immediate_32(REG_PC);
        REG_PC += 4;
        m68k_write_memory_16(address, value);
        word_flags(value);
        break;
    case 0xFC6510u:
        value = (uint16_t)m68k_read_immediate_16(REG_PC);
        REG_PC += 2;
        address = displacement_address(REG_A[6]);
        value |= m68k_read_memory_16(address);
        m68k_write_memory_16(address, value);
        word_flags(value);
        break;
    case 0xFC6516u:
        REG_PC = 0xFC652Cu;
        break;
    case 0xFC651Cu:
        if (COND_NE()) REG_PC = 0xFC64EEu;
        else USE_CYCLES(CYC_BCC_NOTAKE_B);
        break;
    case 0xFC64C2u:
    case 0xFC64D2u:
    case 0xFC653Au:
        REG_PC = m68k_read_memory_32(REG_A[7]);
        REG_A[7] += 4;
        break;
    }
    USE_CYCLES(CYC_INSTRUCTION[op]);
    return 1;
}
