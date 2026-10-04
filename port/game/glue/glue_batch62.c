/* Glue for the message line $C322EE (message_line.c). Its caller reads
 * every register: the last small-text loop's leftovers, or on an early
 * return the values loaded on the way. The branches are taken from the
 * state before the C runs, and every line it may draw is probed then (a
 * line's cells depend only on its plane and place, not on its mode). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "message_line.h"
#include "stages.h"
#include "text.h"
#include "glue_text.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

#define LAYOUT 0xC31998u

/* C3267A's registers after the info page's number (D0 value, D2.w count
 * - 1, D4.b keep, A2 end), glue_batch13.c's without the C. */




/* A probed line drawn with `mode`. */




/* $C1E328 saves D0-D7/A0-A5 before it tests the caller's -$2C(A6) byte.
 * That byte lies inside the saved-register stack area at the observed call
 * site, so the test reads the newly saved value, not the caller's old local. */
static uint8_t saved_stack_byte(gaddr address) {
    gaddr first = A(7) - 56;
    if (address >= first && address < A(7)) {
        gaddr offset = address - first;
        unsigned index = (unsigned)(offset / 4);
        uint32_t value = index < 8 ? D(index) : A(index - 8);
        return (uint8_t)(value >> (24 - 8 * (offset & 3)));
    }
    return rd_u8(address);
}

int glue_C1E328(void) {
    sort_display_list(saved_stack_byte(A(6) - 0x2C) != 0);
    return glue_return();
}
