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

int glue_C31A64(void) {
    SmallTextProbe p;
    SmallText line;

    if ((int8_t)rd_u8(SCALE_REDRAWS) <= 0) return glue_return();
    line = small_text_line(3, TEXT_LINE, 0xC3198Cu, 0x1CD2, 0x12, 4, DRAW, 1);
    small_text_probe(&p, &line);
    draw_scale_readout();
    small_tail(&p, TEXT_LINE + 3, 3, 0, 1);
    return glue_return();
}

int glue_C31ACC(void) {
    SmallTextProbe p;
    SmallText line;
    int16_t zoom = rd_s16(ZOOM_SCALE);
    uint16_t mode = DRAW;
    int32_t shown;

    if ((int8_t)rd_u8(DISPLAY_UPDATE) <= 0) return glue_return();
    if (zoom == 0x80) {
        shown = 10;
    } else {
        shown = zoom == 0x40 ? 20 : 40;
        if (rd_u8(ZOOM_READOUT_FLAGS) & 1) mode = CLEAR;
    }
    line = small_text_line(2, TEXT_LINE, 0xC31A1Cu, 0x1CDE, 0x1E, 4, mode, 1);
    small_text_probe(&p, &line);
    draw_zoom_readout();
    D(0) = (uint32_t)shown;
    SET_W(D(0), 0xF6);
    SET_W(D(1), 0xBC);
    plot_in_view_registers();
    D(0) = (uint32_t)shown; /* MOVEM.W back: sign-extended */
    D(5) = 4;
    D(6) = mode;
    small_tail(&p, TEXT_LINE + 2, 2, 0, 1);
    return glue_return();
}

int glue_C31F4C(void) {
    gaddr record = viewed_record(), chars = TEXT_LINE + 5;
    int16_t speed = 0;

    A(1) = record;
    D(0) = 0;
    if (!(rd_u8(record) & 0x80)) {
        speed = rd_s16(record + 0x6E);
        if (speed < 0) speed = (int16_t)-speed;
        SET_W(D(0), (uint16_t)speed);
    }
    if (rd_u8(CONTEXT_SELECT)) {
        TwoLines t;
        if (!context_allows()) {
            draw_speed_readout();
            return glue_return();
        }
        probe_two(&t, 7, chars, 0xC31958u, 0x1CD2, 0x12, 0, DRAW, INVERSE, 0);
        draw_speed_readout();
        two_tails(&t, chars + 4, 7, 0, 0);
    } else {
        SmallTextProbe p;
        SmallText line = small_text_line(4, chars, 0xC31948u, 0x1A0E, 0x1E, 4, DRAW, 1);
        A(1) = SPEED_SHOWN;
        cached_value_registers(SPEED_SHOWN, speed);
        if (W(0) < 0) {
            draw_speed_readout();
            return glue_return();
        }
        small_text_probe(&p, &line);
        draw_speed_readout();
        small_tail(&p, chars + 4, 4, 0, 1);
    }
    return glue_return();
}

int glue_C3201A(void) {
    gaddr chars = TEXT_LINE + 4;
    int context = rd_u8(CONTEXT_SELECT) != 0;

    if (rd_u8(FIXED_READOUTS)) {
        D(0) = rd_u32(FIXED_ALTITUDE);
        if ((int32_t)D(0) > 99999) D(0) = 99999;
    } else {
        A(0) = viewed_record();
        D(1) = (uint32_t)(rd_s32(A(0) + 0x18) >> 10);
        D(0) = D(1) * 5;
    }
    if (context) {
        TwoLines t;
        if (!context_allows()) {
            draw_altitude_readout();
            return glue_return();
        }
        probe_two(&t, 8, chars, 0xC31928u, 0x1CDA, 0x1A, 0, DRAW, INVERSE, 0);
        draw_altitude_readout();
        two_tails(&t, chars + 6, 8, 0, 0);
    } else {
        SmallTextProbe p;
        SmallText line = small_text_line(6, chars, 0xC31928u, 0x18CE, 0x1E, 4, DRAW, 1);
        if ((int8_t)rd_u8(GAUGE_REFRESH) <= 0) {
            D(2) = rd_u32(ALTITUDE_SHOWN);
            if ((int32_t)D(2) < 0) {
                D(2) &= 0x7FFFFFFF;
            } else if (D(0) == D(2) || (rd_u8(DISPLAY_FORCE) & 1)) {
                draw_altitude_readout();
                return glue_return();
            }
        }
        small_text_probe(&p, &line);
        draw_altitude_readout();
        small_tail(&p, chars + 6, 6, 0, 1);
    }
    return glue_return();
}

/* The readouts behind a display_value_to_draw cache: 1 when drawn. */
static int cached_readout(gaddr cache, int16_t value) {
    A(1) = cache;
    cached_value_registers(cache, value);
    return W(0) >= 0;
}

int glue_C3212A(void) {
    SmallTextProbe p;
    SmallText line = small_text_line(5, TEXT_LINE + 1, 0xC31994u, 0x1CA4, 0xC, 0, INVERSE, 1);

    D(0) = (uint32_t)(rd_s32(viewed_record() + 0x72) >> 8);
    if (!cached_readout(GAUGE_SOURCE, W(0))) {
        draw_record_72_readout();
        return glue_return();
    }
    small_text_probe(&p, &line);
    draw_record_72_readout();
    small_tail(&p, TEXT_LINE + 6, 5, 0, 1);
    return glue_return();
}

int glue_C32178(void) {
    SmallTextProbe p;
    SmallText line = small_text_line(3, TEXT_LINE, 0xC3198Cu, 0x1B76, 0x1E, 4, DRAW, 1);
    int16_t value = (int16_t)((int8_t)rd_u8(viewed_record() + 0x2B) << 8);

    if (value < 0) value = (int16_t)-value;
    D(0) = SEXT((uint16_t)value);
    divu_d0(0x133);
    if (!cached_readout(BYTE_2B_SHOWN, W(0))) {
        draw_record_2b_readout();
        return glue_return();
    }
    small_text_probe(&p, &line);
    draw_record_2b_readout();
    small_tail(&p, TEXT_LINE + 3, 3, 0, 1);
    return glue_return();
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

int glue_C321D2(void) {
    return grid_glue(draw_grid_z_readout, 0x10000000u, 0x1C, 0x7000, 0x177, GRID_Z_SHOWN, GRID_Z_REDRAWS, 0xC31A38u,
                     0x1C44);
}

int glue_C32260(void) {
    return grid_glue(draw_grid_x_readout, 0x0F000000u, 0x14, 0x5999, 0x4C4, GRID_X_SHOWN, GRID_X_REDRAWS, 0xC31A24u,
                     0x1D84);
}

int glue_C31EB6(void) {
    TwoLines t;

    if (!rd_u8(CONTEXT_SELECT) || !context_allows()) return glue_return();
    probe_two(&t, 6, TEXT_LINE, 0xC31974u, 0x1CCA, 0xA, 0, DRAW, INVERSE, 0);
    draw_heading_readout();
    D(2) = 2;
    two_tails(&t, TEXT_LINE + 6, 3, 1, 0);
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

int glue_C31C60(void) {
    gaddr record = viewed_record();
    uint8_t kind = rd_u8(record + 0x63) & 0xF0;

    draw_weapon_readout();
    A(1) = record;
    A(2) = TEXT_LINE;
    D(0) = 0;
    SET_B(D(2), kind);
    if (!kind) return glue_return();
    if (kind == 0x10)
        bcd_entry(0xC31998u, 0x1432, 0xA, 7, 3, TEXT_LINE + 7, 0);
    else
        bcd_entry(0xC31998u, 0x1432, 0xA, 5, 1, TEXT_LINE + 5, 0);
    return glue_return();
}

void signed_readout_registers(void) {
    bcd_entry(0xC31998u, 0x134E, 0x16, 5, 4, TEXT_LINE + 5, 1);
}

int glue_C31D16(void) {
    draw_signed_readout(W(0));
    signed_readout_registers();
    return glue_return();
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

int glue_C31E6C(void) {
    draw_shoot_cue();
    shoot_cue_registers();
    return glue_return();
}

int glue_C31D64(void) {
    gaddr record = viewed_record();
    int g = rd_u8(record + 0x62) == 0x11;

    draw_load_readout();
    if (!g) {
        SET_W(D(0), 0x72);
        SET_W(D(1), 0x41);
        plot_in_view_registers();
        A(4) = 0x994;
        SET_W(D(2), 0xC);
        A(1) = 0xC31900u;
    } else {
        A(2) = 0xC31E5Eu;
        A(0) = A(2) + 1;
        A(1) = 0xC31918u;
        A(4) = 0x12F2;
        D(0) = 0xAu << 16;
        text_in_view_registers();
        SET_W(D(0), 0x64);
        SET_W(D(1), 0x7D);
        plot_in_view_registers();
        A(4) = 0x12F2;
        SET_W(D(2), 0xA);
        A(1) = 0xC3190Cu;
    }
    {
        int16_t x = W(2), trim = rd_s16(LOAD_TRIM);
        A(0) = record;
        SET_W(D(0), rd_u16(record + 0x56));
        SET_W(D(1), (uint16_t)(W(0) >> 3));
        SET_W(D(0), (uint16_t)(W(0) + W(1)));
        SET_W(D(6), (uint16_t)trim);
        if (W(6) < 0) SET_W(D(6), (uint16_t)-W(6));
        if (W(6) > 1) SET_W(D(0), (uint16_t)(W(0) + trim));
        SET_W(D(0), (uint16_t)(W(0) >> 3));
        SET_W(D(1), rd_u16(STATUS_CA) & 2);
        SET_W(D(0), (uint16_t)(W(0) + (W(1) ? 10 : -10)));
        if (W(0) < 0) SET_W(D(0), (uint16_t)-W(0));
        bcd_entry(A(1), A(4), x, 3, 2, TEXT_LINE + 3, 0);
    }
    return glue_return();
}

int glue_C33F54(void) {
    draw_three_digits(A(1), 0x858);
    D(5) = 0x858;
    bcd_entry(A(1), 0x858, 0x10, 2, 3, TEXT_LINE + 3, 1);
    return glue_return();
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

int glue_C328A8(void) {
    gaddr record = viewed_record();
    uint8_t kind = rd_u8(record + 0x63) & 0xF0, stores = rd_u8(record + 0x5F);
    SmallText top = small_text_line(3, TEXT_LINE, 0xC3191Cu, 0x1CC6, 6, 4, DRAW, 1);
    SmallText bottom = small_text_line(3, TEXT_LINE, 0xC3191Cu, 0x1B86, 6, 4, DRAW, 1);
    SmallTextProbe pt, pb;
    uint16_t left = 0;

    if ((int8_t)rd_u8(WEAPON_REDRAWS) <= 0) return glue_return();
    small_text_probe(&pt, &top);
    small_text_probe(&pb, &bottom);
    draw_weapon_status();
    A(1) = record;
    D(0) = stores;
    A(2) = TEXT_LINE;
    SET_B(D(2), kind);
    if (kind == 0x10) {
        SET_W(D(0), rd_u16(record + 0x60));
        A(0) = TEXT_LINE + 7;
    } else if (kind) {
        SET_B(D(0), kind == 0x30 ? stores & 15 : stores >> 4);
        A(0) = TEXT_LINE + 5;
    }
    left = (uint16_t)D(0);
    plain_entry(&top, 1);
    small_text_registers(&pt);
    if (kind) SET_W(D(0), left);
    plain_entry(&bottom, 1);
    small_text_registers(&pb);
    return glue_return();
}

int glue_C3112A(void) {
    uint8_t events = rd_u8(THREAT_EVENTS);
    int32_t across = (int32_t)0x125 + rd_s16(SPAN_ORIGIN_Y);
    uint16_t down = (uint16_t)(0x9C + rd_u16(REDRAW_STATE_WORD));
    gaddr record = viewed_record();

    draw_threat_lights();
    SET_W(D(2), events & 4 ? 1 : 3);
    SET_W(D(0), 0x125);
    SET_W(D(1), 0x9C);
    if (across < 0 || (int16_t)across >= 0x13F) {
        square_in_view_registers();
        return glue_return();
    }
    /* Each square's registers replace the last's: only the last stand. */
    A(0) = record;
    SET_W(D(2), (rd_u8(record + 0x20) & 4) && (rd_u8(DISPLAY_FORCE) & 2) ? 1 : 3);
    SET_W(D(0), (uint16_t)(across + 12));
    SET_W(D(1), (uint16_t)(down + 3));
    square_registers();
    return glue_return();
}
