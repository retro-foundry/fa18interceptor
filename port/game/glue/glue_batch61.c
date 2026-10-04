/* Glue for the HUD's fixed marks (hud_marks.c). Their callers read every
 * register. The C draws first; the pixel and line calls' registers are
 * then replayed in order (they read the plot state, not the pixels). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "hud_marks.h"
#include "plot.h"
#include "view_marks.h"
#include "memory.h"
#include "glue_text.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))



/* A dash's registers ($C34180 and the two after). */










/* One axis of the seeker's step on D`m` (the mark) against D`t`. */




void pair_registers(void); /* glue_batch33.c */

/* $C2F66E: a pair from LINE_LAST_ROW down, otherwise two rows of pairs. */
void block_registers(void) {
    if (W(1) >= rd_s16(LINE_LAST_ROW)) pair_registers();
    else plot_registers(PAIR_MASKS, PLOT_ROWS_2);
}



/* $C347F2: the walk in D0/D1 (MOVE.B keeps the upper bytes) and A0. */




/* $C33DA4's D5. */
