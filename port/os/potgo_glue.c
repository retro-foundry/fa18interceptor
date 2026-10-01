/* CPU bridge for Kickstart 1.3 potgo.resource WritePotgo. The nested Exec
 * calls at $FE44FC and $FE451C remain on the ordinary execution path. */
#include "potgo_glue.h"

#include <string.h>

#include "potgo.h"
#include "m68kcpu.h"
#include "m68kops.h"
#include "bus.h"
#include "machine.h"

int fa18_os_potgo_signature_matches(const uint8_t *rom) {
    static const uint8_t source[] = {
        0xC0, 0x41, 0x46, 0x41, 0x2F, 0x0E,
        0x2C, 0x6E, 0x00, 0x22, 0x4E, 0xAE, 0xFF, 0x88,
        0x2C, 0x5F, 0xC3, 0x6E, 0x00, 0x28,
        0x80, 0x6E, 0x00, 0x28, 0x33, 0xC0, 0x00, 0xDF, 0xF0, 0x34,
        0x42, 0x00, 0x3D, 0x40, 0x00, 0x28,
        0x2F, 0x0E, 0x2C, 0x6E, 0x00, 0x22, 0x4E, 0xAE, 0xFF, 0x82,
        0x2C, 0x5F, 0x4E, 0x75
    };
    return rom && memcmp(rom + 0x244F2, source, sizeof source) == 0;
}

static void set_word_flags(uint16_t value) {
    FLAG_N = NFLAG_16(value);
    FLAG_Z = value;
    FLAG_V = VFLAG_CLEAR;
    FLAG_C = CFLAG_CLEAR;
}

static void set_long_flags(uint32_t value) {
    FLAG_N = NFLAG_32(value);
    FLAG_Z = value;
    FLAG_V = VFLAG_CLEAR;
    FLAG_C = CFLAG_CLEAR;
}

int fa18_os_potgo_step(void) {
    uint32_t pc = REG_PC, op, address, value;
    uint16_t result;
    if (pc != 0xFE44F2u && pc != 0xFE44F4u && pc != 0xFE44F6u &&
        pc != 0xFE44F8u && pc != 0xFE4500u && pc != 0xFE4502u &&
        pc != 0xFE4506u && pc != 0xFE450Au && pc != 0xFE4510u &&
        pc != 0xFE4512u && pc != 0xFE4516u && pc != 0xFE4518u &&
        pc != 0xFE4520u && pc != 0xFE4522u) return 0;
    op = fa18_bus_read16(pc);
    fa18_bus_begin(pc);
    fa18_bus_fetch(pc);
    REG_PPC = pc;
    REG_IR = op;
    REG_PC = pc + 2;
    switch (pc) {
    case 0xFE44F2u:
        result = fa18_os_potgo_requested((uint16_t)REG_D[0], (uint16_t)REG_D[1]);
        REG_D[0] = (REG_D[0] & 0xFFFF0000u) | result;
        set_word_flags(result);
        break;
    case 0xFE44F4u:
        result = (uint16_t)~REG_D[1];
        REG_D[1] = (REG_D[1] & 0xFFFF0000u) | result;
        set_word_flags(result);
        break;
    case 0xFE44F6u:
    case 0xFE4516u:
        REG_A[7] -= 4;
        m68k_write_memory_32(REG_A[7], REG_A[6]);
        set_long_flags(REG_A[6]);
        break;
    case 0xFE44F8u:
    case 0xFE4518u:
        address = REG_A[6] + (int16_t)m68k_read_immediate_16(REG_PC);
        REG_PC += 2;
        REG_A[6] = m68k_read_memory_32(address);
        break;
    case 0xFE4500u:
    case 0xFE4520u:
        REG_A[6] = m68k_read_memory_32(REG_A[7]);
        REG_A[7] += 4;
        break;
    case 0xFE4502u:
        address = REG_A[6] + (int16_t)m68k_read_immediate_16(REG_PC);
        REG_PC += 2;
        result = fa18_os_potgo_retained(m68k_read_memory_16(address), (uint16_t)REG_D[1]);
        m68k_write_memory_16(address, result);
        set_word_flags(result);
        break;
    case 0xFE4506u:
        address = REG_A[6] + (int16_t)m68k_read_immediate_16(REG_PC);
        REG_PC += 2;
        result = fa18_os_potgo_merge((uint16_t)REG_D[0], m68k_read_memory_16(address));
        REG_D[0] = (REG_D[0] & 0xFFFF0000u) | result;
        set_word_flags(result);
        break;
    case 0xFE450Au:
        address = m68k_read_immediate_32(REG_PC);
        REG_PC += 4;
        m68k_write_memory_16(address, (uint16_t)REG_D[0]);
        set_word_flags((uint16_t)REG_D[0]);
        break;
    case 0xFE4510u:
        REG_D[0] &= 0xFFFFFF00u;
        FLAG_N = NFLAG_8(0);
        FLAG_Z = 0;
        FLAG_V = VFLAG_CLEAR;
        FLAG_C = CFLAG_CLEAR;
        break;
    case 0xFE4512u:
        address = REG_A[6] + (int16_t)m68k_read_immediate_16(REG_PC);
        REG_PC += 2;
        result = fa18_os_potgo_cached((uint16_t)REG_D[0]);
        m68k_write_memory_16(address, result);
        set_word_flags(result);
        break;
    case 0xFE4522u:
        REG_PC = m68k_read_memory_32(REG_A[7]);
        REG_A[7] += 4;
        break;
    }
    USE_CYCLES(CYC_INSTRUCTION[op]);
    return 1;
}
