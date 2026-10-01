/* Kickstart 1.3 graphics.library VBeamPos at $FC5ECE-$FC5EDD. Each call
 * completes one original instruction so chipset service and interrupts stay
 * at instruction boundaries. The pure row calculation is in graphics.c. */
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
