#ifndef FA18_GAME_GLOBALS_H
#define FA18_GAME_GLOBALS_H

/* Game variables at their original addresses (see memory.h). Names follow
 * the observed use; the routine or capture that established each one is noted
 * beside it. */

/* ---- polygon renderer ------------------------------------------------------
 * A filled polygon is drawn once as a one-bit mask in a scratch buffer, then
 * composited into each bitplane of the draw page according to its colour
 * (run075 frame 393 blit sequence; $C30466, $C304B2). */
#define POLY_MASK_PLANE    0xC456E2u /* long: row 0 of the one-plane mask buffer ($C305AA) */
#define PAGE_PLANE_TABLE   0xC456B6u /* long: address of the draw page's plane-pointer table */
#define PAGE_POINTER_TABLE 0xC456BAu /* long: address of the draw page's second table ($C2F558) */
#define DRAW_PAGE          0xC4566Cu /* word: nonzero when page 1 is the draw page */
#define PAGE0_PLANE_TABLE  0xC4566Eu /* long[4] per page, page 1 follows */
#define PAGE0_POINTER_TABLE 0xC4568Eu /* long[5] per page, page 1 follows */
#define POLY_PLANE_BITS    0xC45956u /* word: colour bits still to composite, one per plane */
#define POLY_MASK_END      0xC45960u /* long: last word of the mask buffer (descending blits) */
#define POLY_MASK_SOURCE   0xC45964u /* long: mask word the compositing blit starts from */
#define POLY_PLANE_OFFSET  0xC45968u /* long: byte offset of the same word within a plane */
#define POLY_BLIT_SIZE     0xC4596Eu /* word: BLTSIZE covering the polygon's bounding box */

/* ---- trigonometry ---------------------------------------------------------- */
#define SINE_TABLE         0xC3E5E8u /* word[901]: sin(i/10 degree), 2.14 ($C2E6DA) */

/* ---- sound ---------------------------------------------------------------- */
#define MASTER_VOLUME        0xC4FF26u /* long: 0-63, 16.16; limits every voice ($C501E0) */
#define MASTER_VOLUME_TARGET 0xC45668u /* long: level the master volume fades toward ($C24FE8) */
#define VOLUME_FADING        0xC457D7u /* byte: nonzero while the master volume fades */

/* ---- numerals ------------------------------------------------------------ */
#define DISPLAY_VALUE      0xC45B1Eu /* long: value to show ($C25A08) */
#define DISPLAY_VALUE_BCD  0xC45B22u /* long: the same as eight packed BCD digits */

/* ---- input ---------------------------------------------------------------- */
#define CIAA_PORT_COPY     0xC1839Au /* byte: last CIA-A port A; bit 6 = /FIR0 ($C1715C) */

#endif
