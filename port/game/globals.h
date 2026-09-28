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

/* ---- lines ($C2FA7E) -------------------------------------------------------- */
#define CURRENT_COLOUR     0xC45954u /* word: colour bits of the object being drawn */
#define LINE_LAST_ROW      0xC45984u /* word: last row lines may reach */
#define LINE_PLANES        0xC456E7u /* byte: planes a line is drawn into (bit n: table entry 3-n) */
#define LINE_COLOUR        0xC456E8u /* word: line colour in the low byte; negative: CURRENT_COLOUR */

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

/* ---- control records ------------------------------------------------------
 * Sixteen 512-byte records at $C46184, selected by index << 9 (memory map). */
#define CONTROL_RECORDS    0xC46184u
#define CONTROL_RECORD_BYTES 512
#define SELECTED_RECORD    0xC459C0u /* word: byte offset of the selected record, or -1 ($C230B0) */
#define SELECTION_ACTIVE   0xC45868u /* byte: cleared with the selection */
#define SELECTION_MARKER   0xC4593Au /* word: set to -1 with the selection */

/* ---- notifications ($C11B44) ----------------------------------------------- */
#define NOTIFY_COUNTDOWN   0xC45890u /* byte: 8..1 cadence counter */
#define NOTIFY_CODE        0xC4588Eu /* byte: code for this step ($86, $06, $04 or 0) */

/* ---- renderer spans ($C310E2) ----------------------------------------------- */
#define SPAN_ORIGIN        0xC45986u /* word: added to a span position (default $32) */

/* ---- screen frame lists ($C0DAA0) ------------------------------------------ */
#define FRAME_POINTS       0xC4B990u /* (x, y) word pairs, 8 bytes apart */

/* ---- view zoom (memory map) --------------------------------------------- */
#define ZOOM_SCALE         0xC45A42u /* word: $20 (wide) .. $80 */
#define ZOOM_FLAGS         0xC457DDu /* byte: bit 7 = zoom at $80 */
#define DISPLAY_UPDATE     0xC4583Du /* byte: display update request */

/* ---- misc ------------------------------------------------------------------ */
#define RECORD_RATE        0xC458BCu /* byte: 5, 3 or 1 from classify_record_rate ($C1C7F6) */
#define LIST_COUNT         0xC46182u /* word: entries in the list at LIST_BUFFER ($C25864) */
#define LIST_WRITE         0xC459CAu /* long: next free entry */
#define LIST_BUFFER        0xC4E2BCu
#define VOICE_TABLE        0xC4FE28u /* long[4]: voice record per channel ($C4FFB4) */

#define CURRENT_RECORD     0xC18210u /* long: address of the current control record (memory map) */

/* ---- post-input sequence (earlier port: post_input_followup) --------------- */
#define POST_INPUT_COUNTDOWN 0xC45AD6u /* word: negative once expired */
#define POST_INPUT_AUX       0xC45795u /* byte */
#define POST_INPUT_EVENT     0xC457AEu /* byte: event flag cleared on completion */
#define VIEWPORT_MODE        0xC458A0u /* byte: current viewport mode */
#define VIEWPORT_TARGET      0xC458A1u /* byte: mode being changed to */
#define STAGE_CALLBACK       0xC1820Cu /* long: routine run by the next update */

/* Routine addresses stored in STAGE_CALLBACK (function pointers once the
 * callers are C). */
#define ROUTINE_COMPLETE_POST_INPUT 0xC0FA80u
#define ROUTINE_AFTER_POST_INPUT    0xC10C08u

/* ---- rounded division ($C25980) ------------------------------------------ */
#define DIVIDE_NUMERATOR   0xC45ACCu /* long */
#define DIVIDE_DENOMINATOR 0xC45AD0u /* word */
#define DIVIDE_QUOTIENT    0xC45AD2u /* word: rounded to nearest, halves away from zero */

/* ---- cockpit display redraw requests (memory map) ------------------------- */
/* One byte per cockpit display (radar range, ECM, weapons, zoom, ...): set to
 * 3 when it must be redrawn. */
#define REDRAW_FIRST       0xC45836u
#define REDRAW_KEEP_STATE  0xC457C0u /* byte: nonzero keeps the two values below */
#define REDRAW_STATE_WORD  0xC458D8u /* word */
#define REDRAW_STATE_LONG  0xC45918u /* long */

/* ---- workspace records (earlier port: context_workspace_flags) ------------ */
#define WORKSPACE_RECORDS  0xC48184u /* sixteen 32-byte records */
#define WORKSPACE_RECORD_BYTES 32

/* ---- player view ---------------------------------------------------------- */
#define VIEW_RECORD        0xC458DEu /* word: offset of the viewed control record */
#define VIEW_MATRIX        0xC45C0Eu /* 3x3 2.14 heading matrix of the viewed record */
#define COMPASS_DEGREES    0xC459A0u /* word: heading in whole degrees */
#define COMPASS_TAPE       0xC458C4u /* word: compass tape position, 0-95 */

/* ---- depth sort ($C1E4A6) -------------------------------------------------- */
#define DEPTH_KEYS_SOURCE  0xC4E778u /* 22 words: keys of this frame, negative = unused */
#define DEPTH_KEYS         0xC4E7A4u /* working copy; values follow at +$2C */
#define DEPTH_VALUES       0xC4E7D0u
#define DEPTH_ORDER        0xC4E828u /* output: values, far to near */

/* ---- post-input sequence, continued --------------------------------------- */
#define CONTEXT_SELECT     0xC45785u /* byte: dispatcher context (memory map) */
#define CONTEXT_STARTED    0xC457B4u /* byte */
#define CONTEXT_STATE      0xC458AEu /* byte */
#define CONTEXT_GATE       0xC458ADu /* byte */
#define CONTEXT_AUX        0xC45788u /* byte */
#define ROUTINE_CONTEXT_STAGE      0xC10C68u
#define ROUTINE_VIEWPORT_CHANGE    0xC11A26u

/* ---- misc ------------------------------------------------------------------ */
#define TABLE_CLEAR_MODE   0xC458A4u /* byte: set to 2 by clear_long_table */
#define LONG_TABLE         0xC45660u /* long: address of a 16-long table */

#endif
