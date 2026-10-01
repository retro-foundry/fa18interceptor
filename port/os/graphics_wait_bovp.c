/* Kickstart 1.3 graphics.library WaitBOVP CPU bridge. The nested VBeamPos
 * call at $FC5E90 remains on the ordinary vector/interpreter path. */
#include "graphics_wait_bovp.h"

#include <string.h>

#include "m68kcpu.h"
#include "m68kops.h"
#include "bus.h"
#include "machine.h"

int fa18_os_wait_bovp_signature_matches(const uint8_t *rom) {
    static const uint8_t source[] = {
        0x30, 0x28, 0x00, 0x1A, 0x60, 0x08,
        0x20, 0x6F, 0x00, 0x04, 0x30, 0x2F, 0x00, 0x0A,
        0x48, 0xE7, 0x30, 0x00, 0x22, 0x6E, 0x00, 0x22,
        0x36, 0x2E, 0x00, 0xD4, 0x53, 0x43, 0x34, 0x00,
        0x08, 0x28, 0x00, 0x04, 0x00, 0x10, 0x67, 0x02,
        0xE2, 0x42, 0x32, 0x29, 0x00, 0x0C, 0xD2, 0x68,
        0x00, 0x1E, 0xD2, 0x42, 0xB2, 0x43, 0x6F, 0x02,
        0x32, 0x03, 0x4E, 0xAE, 0xFE, 0x80, 0xB2, 0x40,
        0x6E, 0xE8, 0x4C, 0xDF, 0x00, 0x0C, 0x4E, 0x75
    };
    return rom && memcmp(rom + 0x5E58, source, sizeof source) == 0;
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

static void add_word(uint32_t *reg, uint16_t source) {
    uint16_t old = (uint16_t)*reg;
    uint32_t result = (uint32_t)old + source;
    *reg = (*reg & 0xFFFF0000u) | (uint16_t)result;
    FLAG_N = NFLAG_16(result);
    FLAG_Z = result & 0xFFFFu;
    FLAG_V = VFLAG_ADD_16(source, old, result);
    FLAG_X = FLAG_C = CFLAG_16(result);
}

static void cmp_word(uint16_t source, uint16_t dest) {
    uint32_t result = (uint32_t)dest - source;
    FLAG_N = NFLAG_16(result);
    FLAG_Z = result & 0xFFFFu;
    FLAG_V = VFLAG_SUB_16(source, dest, result);
    FLAG_C = CFLAG_16(result);
}

static void push_saved_d2_d3(void) {
    (void)m68k_read_immediate_16(REG_PC);
    REG_PC += 2;
    REG_A[7] -= 4;
    m68k_write_memory_16(REG_A[7] + 2, (uint16_t)REG_D[3]);
    m68k_write_memory_16(REG_A[7], (uint16_t)(REG_D[3] >> 16));
    REG_A[7] -= 4;
    m68k_write_memory_16(REG_A[7] + 2, (uint16_t)REG_D[2]);
    m68k_write_memory_16(REG_A[7], (uint16_t)(REG_D[2] >> 16));
    USE_CYCLES(2 << CYC_MOVEM_L);
}

static void pop_saved_d2_d3(void) {
    (void)m68k_read_immediate_16(REG_PC);
    REG_PC += 2;
    REG_D[2] = m68k_read_memory_32(REG_A[7]);
    REG_A[7] += 4;
    REG_D[3] = m68k_read_memory_32(REG_A[7]);
    REG_A[7] += 4;
    USE_CYCLES(2 << CYC_MOVEM_L);
}

int fa18_os_wait_bovp_step(void) {
    uint32_t pc = REG_PC, op, address, result, old;
    if (pc != 0xFC5E58u && pc != 0xFC5E5Cu && pc != 0xFC5E5Eu &&
        pc != 0xFC5E62u && pc != 0xFC5E66u && pc != 0xFC5E6Au &&
        pc != 0xFC5E6Eu && pc != 0xFC5E72u && pc != 0xFC5E74u &&
        pc != 0xFC5E76u && pc != 0xFC5E7Cu && pc != 0xFC5E7Eu &&
        pc != 0xFC5E80u && pc != 0xFC5E84u && pc != 0xFC5E88u &&
        pc != 0xFC5E8Au && pc != 0xFC5E8Cu && pc != 0xFC5E8Eu &&
        pc != 0xFC5E94u && pc != 0xFC5E96u && pc != 0xFC5E98u &&
        pc != 0xFC5E9Cu) return 0;
    op = fa18_bus_read16(pc);
    fa18_bus_begin(pc);
    fa18_bus_fetch(pc);
    REG_PPC = pc;
    REG_IR = op;
    REG_PC = pc + 2;
    switch (pc) {
    case 0xFC5E58u:
        result = m68k_read_memory_16(displacement_address(REG_A[0]));
        REG_D[0] = (REG_D[0] & 0xFFFF0000u) | result;
        word_flags((uint16_t)result);
        break;
    case 0xFC5E5Cu:
        REG_PC = 0xFC5E66u;
        break;
    case 0xFC5E5Eu:
        REG_A[0] = m68k_read_memory_32(displacement_address(REG_A[7]));
        break;
    case 0xFC5E62u:
        result = m68k_read_memory_16(displacement_address(REG_A[7]));
        REG_D[0] = (REG_D[0] & 0xFFFF0000u) | result;
        word_flags((uint16_t)result);
        break;
    case 0xFC5E66u:
        push_saved_d2_d3();
        break;
    case 0xFC5E6Au:
        REG_A[1] = m68k_read_memory_32(displacement_address(REG_A[6]));
        break;
    case 0xFC5E6Eu:
        result = m68k_read_memory_16(displacement_address(REG_A[6]));
        REG_D[3] = (REG_D[3] & 0xFFFF0000u) | result;
        word_flags((uint16_t)result);
        break;
    case 0xFC5E72u:
        old = (uint16_t)REG_D[3];
        result = old - 1u;
        REG_D[3] = (REG_D[3] & 0xFFFF0000u) | (uint16_t)result;
        FLAG_N = NFLAG_16(result);
        FLAG_Z = result & 0xFFFFu;
        FLAG_V = VFLAG_SUB_16(1u, old, result);
        FLAG_X = FLAG_C = CFLAG_16(result);
        break;
    case 0xFC5E74u:
        result = (uint16_t)REG_D[0];
        REG_D[2] = (REG_D[2] & 0xFFFF0000u) | result;
        word_flags((uint16_t)result);
        break;
    case 0xFC5E76u:
        (void)m68k_read_immediate_16(REG_PC);
        REG_PC += 2;
        address = displacement_address(REG_A[0]);
        FLAG_Z = m68k_read_memory_8(address) & 0x10u;
        break;
    case 0xFC5E7Cu:
        if (COND_EQ()) REG_PC = 0xFC5E80u;
        else USE_CYCLES(CYC_BCC_NOTAKE_B);
        break;
    case 0xFC5E7Eu:
        old = (uint16_t)REG_D[2];
        result = (uint16_t)((int16_t)old >> 1);
        REG_D[2] = (REG_D[2] & 0xFFFF0000u) | result;
        word_flags((uint16_t)result);
        FLAG_X = FLAG_C = (old & 1u) ? XFLAG_SET : XFLAG_CLEAR;
        USE_CYCLES(2);
        break;
    case 0xFC5E80u:
        result = m68k_read_memory_16(displacement_address(REG_A[1]));
        REG_D[1] = (REG_D[1] & 0xFFFF0000u) | result;
        word_flags((uint16_t)result);
        break;
    case 0xFC5E84u:
        add_word(&REG_D[1], m68k_read_memory_16(displacement_address(REG_A[0])));
        break;
    case 0xFC5E88u:
        add_word(&REG_D[1], (uint16_t)REG_D[2]);
        break;
    case 0xFC5E8Au:
        cmp_word((uint16_t)REG_D[3], (uint16_t)REG_D[1]);
        break;
    case 0xFC5E8Cu:
        if (COND_LE()) REG_PC = 0xFC5E90u;
        else USE_CYCLES(CYC_BCC_NOTAKE_B);
        break;
    case 0xFC5E8Eu:
        result = (uint16_t)REG_D[3];
        REG_D[1] = (REG_D[1] & 0xFFFF0000u) | result;
        word_flags((uint16_t)result);
        break;
    case 0xFC5E94u:
        cmp_word((uint16_t)REG_D[0], (uint16_t)REG_D[1]);
        break;
    case 0xFC5E96u:
        if (COND_GT()) REG_PC = 0xFC5E80u;
        else USE_CYCLES(CYC_BCC_NOTAKE_B);
        break;
    case 0xFC5E98u:
        pop_saved_d2_d3();
        break;
    case 0xFC5E9Cu:
        REG_PC = m68k_read_memory_32(REG_A[7]);
        REG_A[7] += 4;
        break;
    }
    USE_CYCLES(CYC_INSTRUCTION[op]);
    return 1;
}
