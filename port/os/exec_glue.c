/* CPU bridge for Kickstart 1.3 Exec Disable, Enable, and GetMsg ROM leaves. */
#include "exec_glue.h"

#include <string.h>

#include "exec.h"
#include "m68kcpu.h"
#include "m68kops.h"
#include "bus.h"
#include "machine.h"
#include "service_phase.h"

int fa18_os_exec_interrupt_signature_matches(const uint8_t *rom) {
    static const uint8_t source[] = {
        0x33, 0xFC, 0x40, 0x00, 0x00, 0xDF, 0xF0, 0x9A,
        0x52, 0x2E, 0x01, 0x26, 0x4E, 0x75,
        0x53, 0x2E, 0x01, 0x26, 0x6C, 0x08,
        0x33, 0xFC, 0xC0, 0x00, 0x00, 0xDF, 0xF0, 0x9A,
        0x4E, 0x75
    };
    return rom && memcmp(rom + 0x1428, source, sizeof source) == 0;
}

int fa18_os_exec_get_msg_signature_matches(const uint8_t *rom) {
    static const uint8_t source[] = {
        0x41, 0xE8, 0x00, 0x14,
        0x33, 0xFC, 0x40, 0x00, 0x00, 0xDF, 0xF0, 0x9A,
        0x52, 0x2E, 0x01, 0x26,
        0x22, 0x50, 0x20, 0x11, 0x67, 0x08,
        0x20, 0x80, 0xC1, 0x89, 0x23, 0x48, 0x00, 0x04,
        0x53, 0x2E, 0x01, 0x26, 0x6C, 0x08,
        0x33, 0xFC, 0xC0, 0x00, 0x00, 0xDF, 0xF0, 0x9A,
        0x4E, 0x75
    };
    return rom && memcmp(rom + 0x1BEA, source, sizeof source) == 0;
}

static void move_interrupt_word(int enable) {
    uint16_t value=enable?0xC000u:0x4000u;
    fa18_service_extension_words(3);
    m68k_write_memory_16(0xDFF09Au, value);
    FLAG_N = NFLAG_16(value);
    FLAG_Z = value;
    FLAG_V = VFLAG_CLEAR;
    FLAG_C = CFLAG_CLEAR;
}

static void adjust_interrupt_depth(int enable) {
    uint32_t address = REG_A[6] + 0x126u;
    uint8_t old_depth, new_depth;
    uint32_t result;
    fa18_service_extension_words(1);
    old_depth = (uint8_t)m68k_read_memory_8(address);
    new_depth = enable ? fa18_os_enable_depth(old_depth) : fa18_os_disable_depth(old_depth);
    result = enable ? (uint32_t)old_depth - 1u : (uint32_t)old_depth + 1u;
    FLAG_N = NFLAG_8(result);
    FLAG_Z = result & 0xFFu;
    FLAG_V = enable ? VFLAG_SUB_8(1u, old_depth, result) : VFLAG_ADD_8(1u, old_depth, result);
    FLAG_X = FLAG_C = CFLAG_8(result);
    m68k_write_memory_8(address, new_depth);
}

int fa18_os_exec_interrupt_step(void) {
    uint32_t pc = REG_PC, op;
    if (pc != 0xFC1428u && pc != 0xFC1430u && pc != 0xFC1434u &&
        pc != 0xFC1436u && pc != 0xFC143Au && pc != 0xFC143Cu && pc != 0xFC1444u) return 0;
    switch (pc) {
    case 0xFC1428u: case 0xFC143Cu: op=0x33FC; break;
    case 0xFC1430u: op=0x522E; break;
    case 0xFC1436u: op=0x532E; break;
    case 0xFC143Au: op=0x6C08; break;
    default: op=0x4E75; break;
    }
    fa18_service_begin(pc,(uint16_t)op);
    switch (pc) {
    case 0xFC1428u:
    case 0xFC143Cu:
        move_interrupt_word(pc==0xFC143Cu);
        break;
    case 0xFC1430u:
    case 0xFC1436u:
        adjust_interrupt_depth(pc == 0xFC1436u);
        break;
    case 0xFC143Au:
        if (COND_GE()) REG_PC = 0xFC1444u;
        else USE_CYCLES(CYC_BCC_NOTAKE_B);
        break;
    case 0xFC1434u:
    case 0xFC1444u:
        REG_PC = m68k_read_memory_32(REG_A[7]);
        REG_A[7] += 4;
        break;
    }
    USE_CYCLES(CYC_INSTRUCTION[op]);
    return 1;
}

int fa18_os_exec_get_msg_step(void) {
    uint32_t pc = REG_PC, op, value;
    if (pc != 0xFC1BEAu && pc != 0xFC1BEEu && pc != 0xFC1BF6u &&
        pc != 0xFC1BFAu && pc != 0xFC1BFCu && pc != 0xFC1BFEu &&
        pc != 0xFC1C00u && pc != 0xFC1C02u && pc != 0xFC1C04u &&
        pc != 0xFC1C08u && pc != 0xFC1C0Cu && pc != 0xFC1C0Eu &&
        pc != 0xFC1C16u) return 0;
    switch (pc) {
    case 0xFC1BEAu: op=0x41E8; break;
    case 0xFC1BEEu: case 0xFC1C0Eu: op=0x33FC; break;
    case 0xFC1BF6u: op=0x522E; break;
    case 0xFC1BFAu: op=0x2250; break;
    case 0xFC1BFCu: op=0x2011; break;
    case 0xFC1BFEu: op=0x6708; break;
    case 0xFC1C00u: op=0x2080; break;
    case 0xFC1C02u: op=0xC189; break;
    case 0xFC1C04u: op=0x2348; break;
    case 0xFC1C08u: op=0x532E; break;
    case 0xFC1C0Cu: op=0x6C08; break;
    default: op=0x4E75; break;
    }
    fa18_service_begin(pc,(uint16_t)op);
    switch (pc) {
    case 0xFC1BEAu: /* The MsgPort message list begins at offset $14. */
        fa18_service_extension_words(1);
        REG_A[0] += 0x14u;
        break;
    case 0xFC1BEEu:
    case 0xFC1C0Eu:
        move_interrupt_word(pc==0xFC1C0Eu);
        break;
    case 0xFC1BF6u:
    case 0xFC1C08u:
        adjust_interrupt_depth(pc == 0xFC1C08u);
        break;
    case 0xFC1BFAu:
        REG_A[1] = m68k_read_memory_32(REG_A[0]);
        break;
    case 0xFC1BFCu:
        REG_D[0] = m68k_read_memory_32(REG_A[1]);
        FLAG_N = NFLAG_32(REG_D[0]);
        FLAG_Z = REG_D[0];
        FLAG_V = VFLAG_CLEAR;
        FLAG_C = CFLAG_CLEAR;
        break;
    case 0xFC1BFEu:
        if (COND_EQ()) REG_PC = 0xFC1C08u;
        else USE_CYCLES(CYC_BCC_NOTAKE_B);
        break;
    case 0xFC1C00u:
        m68k_write_memory_32(REG_A[0], REG_D[0]);
        FLAG_N = NFLAG_32(REG_D[0]);
        FLAG_Z = REG_D[0];
        FLAG_V = VFLAG_CLEAR;
        FLAG_C = CFLAG_CLEAR;
        break;
    case 0xFC1C02u:
        value = REG_D[0];
        REG_D[0] = REG_A[1];
        REG_A[1] = value;
        break;
    case 0xFC1C04u:
        fa18_service_extension_words(1);
        value=4;
        m68k_write_memory_32(REG_A[1] + value, REG_A[0]);
        FLAG_N = NFLAG_32(REG_A[0]);
        FLAG_Z = REG_A[0];
        FLAG_V = VFLAG_CLEAR;
        FLAG_C = CFLAG_CLEAR;
        break;
    case 0xFC1C0Cu:
        if (COND_GE()) REG_PC = 0xFC1C16u;
        else USE_CYCLES(CYC_BCC_NOTAKE_B);
        break;
    case 0xFC1C16u:
        REG_PC = m68k_read_memory_32(REG_A[7]);
        REG_A[7] += 4;
        break;
    }
    USE_CYCLES(CYC_INSTRUCTION[op]);
    return 1;
}
