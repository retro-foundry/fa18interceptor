#ifndef FA18_GAME_HARDWARE_H
#define FA18_GAME_HARDWARE_H

/* Amiga custom-chip access for the recreated source, in the spirit of
 * <hardware/custom.h> and <hardware/blit.h>. Register numbers are offsets from
 * $DFF000. */

#include <stdint.h>

enum {
    DMACONR = 0x002,
    BLTCON0 = 0x040, BLTCON1 = 0x042, BLTAFWM = 0x044, BLTALWM = 0x046,
    BLTCPT = 0x048, BLTBPT = 0x04C, BLTAPT = 0x050, BLTDPT = 0x054, BLTSIZE = 0x058,
    BLTCMOD = 0x060, BLTBMOD = 0x062, BLTAMOD = 0x064, BLTDMOD = 0x066,
    BLTCDAT = 0x070, BLTBDAT = 0x072, BLTADAT = 0x074
};

/* BLTCON0 channel enables and BLTCON1 mode bits. */
enum {
    SRCA = 0x0800, SRCB = 0x0400, SRCC = 0x0200, DEST = 0x0100,
    BLITREVERSE = 0x0002, FILL_OR = 0x0008, FILL_XOR = 0x0010, LINEMODE = 0x0001,
    ONEDOT = 0x0002, SIGNFLAG = 0x0040
};

/* Minterms over sources A, B, C. */
enum {
    MINTERM_A = 0xF0, MINTERM_B = 0xCC, MINTERM_C = 0xAA,
    MINTERM_A_OR_B = 0xFC, MINTERM_NOTA_AND_B = 0x0C, MINTERM_A_XOR_B = 0x3C
};

/* Make BLTSIZE from a height in rows and a width in words. */
#define BLTSIZE_OF(rows, words) ((uint16_t)(((rows) & 0x3FF) << 6 | ((words) & 0x3F)))

void custom_write(unsigned reg, uint16_t value);
/* A 32-bit pointer register pair (high word first, as a MOVE.L writes it). */
void custom_write_ptr(unsigned reg, uint32_t value);
uint16_t custom_read(unsigned reg);

/* Wait until the blitter has finished its current operation. */
void wait_blitter(void);

#endif
