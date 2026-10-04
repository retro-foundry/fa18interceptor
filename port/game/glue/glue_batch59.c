/* Glue for the cockpit readouts (hud_readouts.c). Their callers read every
 * register: the text plotter's leftovers from the line each ends with, or
 * the values loaded on the way to an early return. The branches are taken
 * from the state before the C runs; the small-text lines are probed then
 * too (glue_text.h), and their registers rebuilt once the C has drawn. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "hud_readouts.h"
#include "memory.h"
#include "text.h"
#include "glue_text.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

#define DRAW    0x0FCA
#define INVERSE 0x0F3A
#define CLEAR   0x0F0A

static gaddr viewed_record(void) {
    return CONTROL_RECORDS + SEXT(rd_u16(VIEW_RECORD));
}

/* DIVU.W / DIVS.W #divisor on D0: remainder above the quotient, or D0 left
 * as it was on overflow. */
static void divu_d0(uint16_t divisor) {
    uint32_t q = D(0) / divisor;
    if (q <= 0xFFFF) D(0) = (D(0) % divisor) << 16 | q;
}

static void divs_d0(int16_t divisor) {
    int32_t dividend = (int32_t)D(0), q = dividend / divisor;
    if (q == (int16_t)q) D(0) = (uint32_t)(uint16_t)(dividend % divisor) << 16 | (uint16_t)q;
}

static int context_allows(void) {
    return rd_u8(CONTEXT_READOUTS) && (rd_u16(COCKPIT_FLAGS) & 0x40);
}

/* A line's registers: its entry, then the loop's leftovers. */
static void small_tail(const SmallTextProbe *p, gaddr end, int digits, int keep_zeros, int in_view) {
    small_line_entry(&p->text, end, digits, keep_zeros, in_view);
    small_text_registers(p);
}

/* Two lines of the same characters (planes 0 and $C, or 4 and $C). */
typedef struct {
    SmallTextProbe first, second;
} TwoLines;

static void probe_two(TwoLines *t, int count, gaddr chars, gaddr layout, gaddr rows, int16_t x, int16_t plane,
                      uint16_t mode, uint16_t second_mode, int in_view) {
    SmallText a = small_text_line(count, chars, layout, rows, x, plane, mode, in_view);
    SmallText b = small_text_line(count, chars, layout, rows, x, 0xC, second_mode, in_view);
    small_text_probe(&t->first, &a);
    small_text_probe(&t->second, &b);
}

static void two_tails(const TwoLines *t, gaddr end, int digits, int keep_zeros, int in_view) {
    small_tail(&t->first, end, digits, keep_zeros, in_view);
    small_tail(&t->second, end, digits, keep_zeros, in_view);
}

/* ---- small text ---------------------------------------------------------- */





/* The readouts behind a display_value_to_draw cache: 1 when drawn. */
static int cached_readout(gaddr cache, int16_t value) {
    A(1) = cache;
    cached_value_registers(cache, value);
    return W(0) >= 0;
}





static int grid_glue(void (*readout)(void), uint32_t base, int field, int16_t divisor, int16_t offset, gaddr shown,
                     gaddr redraws, gaddr layout, gaddr rows) {
    TwoLines t;

    A(1) = viewed_record();
    D(0) = (uint32_t)((int32_t)(rd_u32(A(1) + (gaddr)field) - base) >> 8);
    divs_d0(divisor);
    SET_W(D(0), (uint16_t)(W(0) + offset));
    if (W(0) == rd_s16(shown) && (int8_t)rd_u8(redraws) <= 0) {
        readout();
        return glue_return();
    }
    probe_two(&t, 4, TEXT_LINE, layout, rows, 0x24, 4, INVERSE, INVERSE, 1);
    readout();
    two_tails(&t, TEXT_LINE + 4, 4, 0, 1);
    return glue_return();
}







/* ---- 8-pixel text ---------------------------------------------------------- */

/* The registers at $C32AA4/$C32AA6: D0 = x origin over count - 1, D2.w
 * digits - 1, D6/D7 as loaded (MOVEQ). */
void bcd_entry(gaddr layout, gaddr rows, int16_t x, int count, int digits, gaddr end, int keep_zeros) {
    A(0) = end;
    A(1) = layout;
    A(2) = TEXT_LINE;
    A(4) = rows;
    D(6) = (uint32_t)(count - 1);
    D(7) = (uint32_t)(digits - 1);
    SET_W(D(2), (uint16_t)(digits - 1));
    D(0) = (uint32_t)(uint16_t)x << 16 | (uint16_t)(count - 1);
    D(4) = (uint32_t)keep_zeros;
    bcd_text_registers();
}



void signed_readout_registers(void) {
    bcd_entry(0xC31998u, 0x134E, 0x16, 5, 4, TEXT_LINE + 5, 1);
}



void shoot_cue_registers(void) {
    gaddr record = viewed_record();
    uint8_t kind = rd_u8(record + 0x63) & 0xF0;

    A(1) = record;
    SET_B(D(0), kind);
    A(2) = kind == 0x10 ? 0xC31E65u : 0xC31E5Fu;
    A(0) = A(2) + 6;
    A(1) = 0xC31998u;
    A(4) = 0x1440;
    D(0) = 0x18u << 16 | 5;
    text_in_view_registers();
}







/* ---- lines without digits and squares -------------------------------------- */

/* The registers at the loop ($C32794) for `line`, D3/D4 left as they are. */
static void plain_entry(const SmallText *line, int in_view) {
    D(0) = (uint32_t)(line->count - 1);
    A(1) = line->layout;
    A(2) = line->chars;
    A(5) = (uint32_t)(int32_t)line->x_origin;
    SET_W(D(5), (uint16_t)line->plane_offset);
    D(6) = (uint32_t)line->mode << 16 | (uint16_t)line->column;
    D(7) = in_view ? rd_u32(REDRAW_STATE_LONG) : 0;
    A(4) = line->rows;
}
