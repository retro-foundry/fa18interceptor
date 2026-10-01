/* Kickstart 1.3 graphics.library VBeamPos and WaitBlit. Each step completes
 * one original instruction so chipset service and interrupts stay at the
 * original boundaries. Their pure operations are in graphics.c. */
#include "graphics_glue.h"

#include <string.h>

#include "graphics.h"
#include "m68kcpu.h"
#include "m68kops.h"
#include "bus.h"
#include "machine.h"

int fa18_os_vbeam_signature_matches(const uint8_t *rom) {
    static const uint8_t source[] = {
        0x20, 0x39, 0x00, 0xDF, 0xF0, 0x04, /* MOVE.L $DFF004,D0 */
        0xE0, 0x80,                         /* ASR.L #8,D0 */
        0x02, 0x80, 0x00, 0x00, 0x01, 0xFF, /* ANDI.L #$1FF,D0 */
        0x4E, 0x75                          /* RTS */
    };
    return rom && memcmp(rom + 0x5ECE, source, sizeof source) == 0;
}

int fa18_os_vbeam_step(void) {
    uint32_t pc = REG_PC, op, value;
    if (pc != 0xFC5ECEu && pc != 0xFC5ED4u && pc != 0xFC5ED6u && pc != 0xFC5EDCu) return 0;
    op = fa18_bus_read16(pc);
    fa18_bus_begin(pc);
    fa18_bus_fetch(pc);
    REG_PPC = pc;
    REG_IR = op;
    REG_PC = pc + 2;
    switch (pc) {
    case 0xFC5ECEu:
        (void)m68k_read_immediate_32(REG_PC);
        REG_PC += 4;
        REG_D[0] = m68k_read_memory_32(0xDFF004u);
        FLAG_N = NFLAG_32(REG_D[0]);
        FLAG_Z = REG_D[0];
        FLAG_V = VFLAG_CLEAR;
        FLAG_C = CFLAG_CLEAR;
        break;
    case 0xFC5ED4u:
        value = REG_D[0];
        REG_D[0] = (uint32_t)((int32_t)value >> 8);
        FLAG_N = NFLAG_32(REG_D[0]);
        FLAG_Z = REG_D[0];
        FLAG_V = VFLAG_CLEAR;
        FLAG_X = FLAG_C = (value & 0x80u) ? XFLAG_SET : XFLAG_CLEAR;
        USE_CYCLES(16); /* 68000 ASR.L immediate: 2 cycles per shift. */
        break;
    case 0xFC5ED6u:
        (void)m68k_read_immediate_32(REG_PC);
        REG_PC += 4;
        REG_D[0] = fa18_os_vbeam_row(REG_D[0]);
        FLAG_N = NFLAG_32(REG_D[0]);
        FLAG_Z = REG_D[0];
        FLAG_V = VFLAG_CLEAR;
        FLAG_C = CFLAG_CLEAR;
        break;
    case 0xFC5EDCu:
        REG_PC = m68k_read_memory_32(REG_A[7]);
        REG_A[7] += 4;
        break;
    }
    USE_CYCLES(CYC_INSTRUCTION[op]);
    return 1;
}

int fa18_os_wait_blit_signature_matches(const uint8_t *rom) {
    static const uint8_t source[] = {
        0x08, 0x39, 0x00, 0x06, 0x00, 0xDF, 0xF0, 0x02,
        0x08, 0x39, 0x00, 0x06, 0x00, 0xDF, 0xF0, 0x02,
        0x66, 0x02, 0x4E, 0x75, 0x4E, 0x71, 0x4E, 0x71,
        0x08, 0x39, 0x00, 0x06, 0x00, 0xDF, 0xF0, 0x02,
        0x66, 0xF2, 0x4E, 0x75
    };
    return rom && memcmp(rom + 0x5A58, source, sizeof source) == 0;
}

int fa18_os_wait_blit_step(void) {
    uint32_t pc = REG_PC, op, address;
    if (pc != 0xFC5A58u && pc != 0xFC5A60u && pc != 0xFC5A68u &&
        pc != 0xFC5A6Au && pc != 0xFC5A6Cu && pc != 0xFC5A6Eu &&
        pc != 0xFC5A70u && pc != 0xFC5A78u && pc != 0xFC5A7Au) return 0;
    op = fa18_bus_read16(pc);
    fa18_bus_begin(pc);
    fa18_bus_fetch(pc);
    REG_PPC = pc;
    REG_IR = op;
    REG_PC = pc + 2;
    switch (pc) {
    case 0xFC5A58u:
    case 0xFC5A60u:
    case 0xFC5A70u:
        (void)m68k_read_immediate_16(REG_PC); /* Source bit number: 6. */
        REG_PC += 2;
        address = m68k_read_immediate_32(REG_PC);
        REG_PC += 4;
        FLAG_Z = fa18_os_blitter_busy((uint8_t)m68k_read_memory_8(address));
        break;
    case 0xFC5A68u:
    case 0xFC5A78u:
        if (COND_NE()) REG_PC = 0xFC5A6Cu;
        else USE_CYCLES(CYC_BCC_NOTAKE_B);
        break;
    case 0xFC5A6Cu:
    case 0xFC5A6Eu:
        break; /* NOP; only fetch, PC advance, and cycles. */
    case 0xFC5A6Au:
    case 0xFC5A7Au:
        REG_PC = m68k_read_memory_32(REG_A[7]);
        REG_A[7] += 4;
        break;
    }
    USE_CYCLES(CYC_INSTRUCTION[op]);
    return 1;
}
