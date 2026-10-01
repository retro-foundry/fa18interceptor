/* CPU bridge for Kickstart 1.3 Exec Disable/Enable ROM leaves. */
#include "exec_glue.h"

#include <string.h>

#include "exec.h"
#include "m68kcpu.h"
#include "m68kops.h"
#include "bus.h"
#include "machine.h"

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

static void move_interrupt_word(void) {
    uint16_t value = (uint16_t)m68k_read_immediate_16(REG_PC);
    uint32_t address;
    REG_PC += 2;
    address = m68k_read_immediate_32(REG_PC);
    REG_PC += 4;
    m68k_write_memory_16(address, value);
    FLAG_N = NFLAG_16(value);
    FLAG_Z = value;
    FLAG_V = VFLAG_CLEAR;
    FLAG_C = CFLAG_CLEAR;
}

static void adjust_interrupt_depth(int enable) {
    int16_t displacement = (int16_t)m68k_read_immediate_16(REG_PC);
    uint32_t address = REG_A[6] + displacement;
    uint8_t old_depth, new_depth;
    uint32_t result;
    REG_PC += 2;
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
    op = fa18_bus_read16(pc);
    fa18_bus_begin(pc);
    fa18_bus_fetch(pc);
    REG_PPC = pc;
    REG_IR = op;
    REG_PC = pc + 2;
    switch (pc) {
    case 0xFC1428u:
    case 0xFC143Cu:
        move_interrupt_word();
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
