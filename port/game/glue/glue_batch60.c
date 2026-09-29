/* Glue for the cockpit's blitter pieces (hud_bars.c). Their callers read
 * every register: what the last bound_span, bar fill, image blit or marker
 * line left. The countdowns are read before the C runs; the register flow
 * is then replayed step by step (bound_span has no other effect, and the
 * blitter waits leave the registers alone). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "hud_bars.h"
#include "memory.h"
#include "glue_text.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

void line_registers_to_row(int16_t last_row); /* glue_render_polygon.c */

static gaddr viewed_record(void) {
    return CONTROL_RECORDS + SEXT(rd_u16(VIEW_RECORD));
}

/* $C30CC4 from its entry registers. */
static void fill_registers(void) {
    A(2) = rd_u32(PAGE_PLANE_TABLE);
    D(4) = rd_u32(A(2) + SEXT(D(4)));
    if (W(5)) SET_W(D(3), 0xFFFF);
    if (W(7)) SET_W(D(0), 0xFFFF);
    SET_W(D(5), (uint16_t)(W(5) + W(7)));
    D(4) += D(1);
    SET_W(D(6), (uint16_t)(W(6) - W(5)));
    SET_W(D(5), (uint16_t)(W(5) * 2));
    SET_W(D(5), (uint16_t)(W(5) + (int16_t)A(5)));
    A(0) = 0xDFF000;
}

int glue_C30CC4(void) {
    fill_bar_words((uint16_t)D(2), (int16_t)D(4), D(1), W(5), W(7), (uint16_t)D(6), (int16_t)A(5), (uint16_t)D(0),
                   (uint16_t)D(3));
    fill_registers();
    return glue_return();
}

/* The loads before a bar's bound_span: 1 when the bar is drawn. */
static int bounded(uint32_t rows, int16_t words, uint16_t size, int16_t modulo, int16_t position) {
    D(1) = rows;
    A(4) = SEXT((uint16_t)words);
    SET_W(D(6), size);
    A(5) = SEXT((uint16_t)modulo);
    SET_W(D(7), (uint16_t)position);
    D(1) += rd_u32(REDRAW_STATE_LONG);
    bound_span_registers();
    return W(5) >= 0;
}

static void bar(uint16_t con0, uint16_t first_mask, uint16_t last_mask) {
    SET_W(D(2), con0);
    D(4) = 0xC;
    SET_W(D(0), first_mask);
    SET_W(D(3), last_mask);
    fill_registers();
}

/* x + SPAN_ORIGIN_Y into D`n`: 1 when the column is 0..last. */
static int column(int n, int16_t x, int16_t last) {
    int32_t across = (int32_t)x + rd_s16(SPAN_ORIGIN_Y);
    SET_W(D(n), (uint16_t)across);
    return across >= 0 && (int16_t)across <= last;
}

int glue_C30B5C(void) {
    gaddr record = viewed_record();
    int8_t a = (int8_t)rd_u8(BAR_REDRAWS_A), b = (int8_t)rd_u8(BAR_REDRAWS_B), c = (int8_t)rd_u8(BAR_REDRAWS_C),
           e = (int8_t)rd_u8(BAR_REDRAWS_E);

    draw_indicator_bars();
    if (a > 0 && column(0, 0xF1, 0x137)) {
        SET_W(D(1), (uint16_t)(0xB4 + rd_u16(REDRAW_STATE_WORD)));
        SET_W(D(2), (uint16_t)(W(0) + 9));
        SET_W(D(3), (uint16_t)D(1));
        A(2) = record;
        line_registers_to_row(0xC7);
    }
    if (b > 0) {
        if (!bounded(0x1A68, 3, 0x1C3, 0x23, 0)) goto last;
        A(0) = CONTROL_RECORDS;
        SET_W(D(0), rd_u16(VIEW_RECORD));
        SET_W(D(0), rd_u16(A(0) + SEXT(D(0))) & 0x800);
        bar(W(0) ? BAR_SET : BAR_CLEAR, 0x7F, 0x8000);
    }
    if (c > 0) {
        if (bounded(0x1A14, 2, 0x1C2, 0x25, 0x12)) {
            bar(rd_u8(PLAYER_FLAGS_G) ? BAR_SET : BAR_CLEAR, 0xFFF, 0xFC00);
            return glue_return();
        }
    }
last:
    if (e > 0 && bounded(0x1888, 2, 0x202, 0x25, 0))
        bar(rd_u8(BAR_E_FLAG) && ((e - 1) & 1) ? BAR_SET : BAR_CLEAR, 0x7F, 0xFFFF);
    return glue_return();
}

int glue_C30D34(void) {
    gaddr record = viewed_record();
    int8_t d = (int8_t)rd_u8(BAR_REDRAWS_D);

    draw_mode_bar();
    if (bounded(0x1C70, 2, 0x202, 0x25, 0))
        bar((int8_t)rd_u8(BAR_REDRAWS_F) > 0 && (rd_u8(DISPLAY_FORCE) & 2) ? BAR_SET : BAR_CLEAR, 0x3F, 0xFFFE);
    if (d <= 0) return glue_return();
    A(1) = record;
    SET_B(D(0), rd_u8(record + 0x7C) & 0x7F);
    SET_B(D(0), (uint8_t)((int8_t)D(0) >> 5));
    SET_W(D(0), (uint16_t)((int8_t)D(0) * 4));
    A(1) = 0xC30D22u + SEXT(D(0));
    D(0) = 0x129FC;
    A(2) = rd_u32(PAGE_PLANE_TABLE);
    if (!bounded(0x1D88, 2, 0x1C2, 0x25, 0)) return glue_return();
    SET_W(D(5), (uint16_t)(W(5) + W(7)));
    D(7) = SEXT((uint16_t)(W(7) * 2));
    SET_W(D(6), (uint16_t)(W(6) - W(5)));
    D(5) = SEXT((uint16_t)(W(5) * 2 + 1));
    D(4) = rd_u32(A(2)) + D(1);
    D(3) = rd_u32(A(1)) + D(7);
    A(1) += 4;
    D(0) += D(7);
    SET_W(D(2), 0xF3A);
    A(0) = 0xDFF000;
    SET_W(D(5), (uint16_t)(W(5) - 1 + (int16_t)A(5)));
    A(1) = record;
    if (!(rd_u8(record + 2) & 0x80)) return glue_return();
    SET_W(D(1), 0xC0);
    SET_W(D(2), 0xC);
    SET_W(D(3), 0xC3);
    if (!column(0, 0xE, 0x13F) || !column(2, 0xC, 0x13F)) return glue_return();
    SET_W(D(1), (uint16_t)(W(1) + rd_s16(REDRAW_STATE_WORD)));
    SET_W(D(3), (uint16_t)(W(3) + rd_s16(REDRAW_STATE_WORD)));
    line_registers_to_row(0xC7);
    return glue_return();
}

/* $C30EAA from its entry registers: D2.w BLTCON0, D0 mask, A1 plane image
 * pointers, D1 rows, D7.w position, A4 words, D6.w size, A5 modulo. */
static void image_registers(void) {
    int k;

    A(0) = 0;
    A(2) = rd_u32(PAGE_PLANE_TABLE);
    D(1) += rd_u32(REDRAW_STATE_LONG);
    bound_span_registers();
    if (W(5) < 0) {
        D(0) = 0xFFFFFFFFu;
        return;
    }
    SET_W(D(5), (uint16_t)(W(5) + W(7)));
    D(7) = SEXT((uint16_t)(W(7) * 2));
    SET_W(D(6), (uint16_t)(W(6) - W(5)));
    SET_W(D(5), (uint16_t)(W(5) * 2 + 1));
    D(2) &= 0xFFFF; /* SWAP, MOVE.W A0 (0), SWAP */
    D(5) = SEXT(D(5));
    while ((int32_t)D(1) < 40) {
        D(1) += 40;
        D(7) += 2 * A(4);
        SET_W(D(6), (uint16_t)(W(6) - 0x40));
        if (W(6) < 0) {
            D(0) = 0xFFFFFFFFu;
            return;
        }
    }
    for (k = 0; k < 4; k++) {
        D(4) = rd_u32(A(2)) + D(1);
        A(2) += 4;
        A(3) = rd_u32(A(1));
        A(1) += 4;
        D(3) = rd_u32(A(3)) + D(7);
        if (k == 0) {
            D(0) += D(7);
            A(0) = 0xDFF000;
            SET_W(D(5), (uint16_t)(W(5) - 1 + (int16_t)A(5)));
        }
    }
}

int glue_C30EAA(void) {
    blit_image((uint16_t)D(2), D(0), A(1), D(1), W(7), (int16_t)A(4), (uint16_t)D(6), (int16_t)A(5));
    image_registers();
    return glue_return();
}

int glue_C309B6(void) {
    draw_panel_image();
    A(1) = 0xC309A6u;
    A(3) = rd_u32(0xC309A2u);
    SET_W(D(2), 0xFCE);
    D(0) = rd_u32(A(3));
    D(1) = 0x14CA;
    A(4) = 0x12;
    SET_W(D(6), 0x312);
    A(5) = 5;
    SET_W(D(7), 1);
    image_registers();
    return glue_return();
}

int glue_C30F78(void) {
    int refresh = (int8_t)rd_u8(GAUGE_REFRESH) > 0;
    int16_t shown = rd_s16(TAPE_SHOWN);
    int force = rd_u8(DISPLAY_FORCE) & 1;

    draw_compass_tape();
    /* update_compass ($C310AA). */
    A(0) = CONTROL_RECORDS;
    SET_W(D(1), rd_u16(VIEW_RECORD));
    D(0) = rd_u16(COMPASS_TAPE);
    if (!refresh) {
        SET_W(D(2), (uint16_t)shown);
        if (shown < 0) {
            SET_W(D(2), shown & 0x7FFF);
            SET_W(D(0), (uint16_t)D(2));
        } else if (W(0) == shown || force) {
            return glue_return();
        }
    }
    A(0) = 0xDFF000;
    A(2) = rd_u32(PAGE_PLANE_TABLE);
    SET_W(D(3), 0x1C3);
    if (!bounded(0x17B0, 2, (uint16_t)D(6), 0x23, 0xC)) return glue_return();
    SET_W(D(5), (uint16_t)(W(5) + W(7)));
    D(1) += rd_u32(A(2) + 0xC);
    SET_W(D(3), (uint16_t)(W(3) - W(5)));
    SET_W(D(5), (uint16_t)(W(5) * 2));
    A(4) = SEXT(D(5));
    D(6) = 0xFFFF0000u;
    SET_W(D(0), (uint16_t)(W(0) >> 1));
    SET_W(D(4), (uint16_t)D(0));
    SET_W(D(0), W(0) & 15);
    if (W(0)) {
        uint16_t v;
        D(1) -= 2;
        D(6) = 0x0000FFFFu;
        v = (uint16_t)(16 - W(0));
        SET_W(D(0), (uint16_t)(v << 12 | v >> 4)); /* ROR.W #4 */
    }
    SET_W(D(4), (uint16_t)((int16_t)(W(4) & 0xF0) >> 3));
    D(4) = SEXT(D(4)) + 0x12AFC;
    D(7) = SEXT((uint16_t)(W(7) * 2));
    D(4) += D(7);
    SET_W(D(2), 0x73A);
    SET_W(D(5), (uint16_t)((int16_t)A(4) + 7));
    D(6) = D(6) << 16 | D(6) >> 16; /* SWAP between the masks */
    SET_W(D(5), (uint16_t)(W(5) - 7 + (int16_t)A(5)));
    if (!column(0, 0xD0, 0x13F)) return glue_return();
    SET_W(D(1), 0x96);
    SET_W(D(2), (uint16_t)D(0));
    SET_W(D(3), 0x9D);
    line_registers_to_row(0xC7);
    return glue_return();
}

int glue_C30764(void) {
    int8_t first = (int8_t)rd_u8(REDRAW_FIRST);
    int16_t lift = rd_s16(REDRAW_STATE_WORD), origin = rd_s16(SPAN_ORIGIN);
    int k;

    draw_panel_frame();
    if (first <= 0) return glue_return();
    A(0) = 0xDFF000;
    A(2) = rd_u32(PAGE_PLANE_TABLE);
    A(1) = 0xC30752u;
    SET_W(D(2), 0x9F0);
    D(1) = 0x16A8;
    A(4) = 0x14;
    SET_W(D(6), 0x37);
    if (lift > 0) SET_W(D(6), (uint16_t)(W(6) - lift));
    SET_W(D(6), (uint16_t)(W(6) << 6));
    SET_W(D(6), (uint16_t)(W(6) + 0x14));
    A(5) = 1;
    SET_W(D(7), 0);
    D(1) += rd_u32(REDRAW_STATE_LONG);
    bound_span_registers();
    if (W(5) >= 0) {
        SET_W(D(5), (uint16_t)(W(5) + W(7)));
        D(7) = SEXT((uint16_t)(W(7) * 2));
        SET_W(D(6), (uint16_t)(W(6) - W(5)));
        SET_W(D(5), (uint16_t)(W(5) * 2 + 1));
        SET_W(D(5), (uint16_t)(W(5) - 1 + (int16_t)A(5)));
        for (k = 0; k < 4; k++) {
            D(4) = rd_u32(A(2)) + D(1);
            A(2) += 4;
            A(3) = rd_u32(A(1));
            A(1) += 4;
            D(0) = rd_u32(A(3)) + D(7);
        }
    }
    if (!origin) return glue_return();
    SET_W(D(0), (uint16_t)(rd_s16(LINE_LAST_ROW) - 0x90));
    SET_W(D(7), (uint16_t)D(0));
    SET_W(D(0), (uint16_t)(W(0) << 3));
    SET_W(D(1), (uint16_t)D(0));
    SET_W(D(0), (uint16_t)(W(0) * 4 + W(1)));
    D(0) = SEXT(D(0));
    D(1) = 0x16A8;
    SET_W(D(6), (uint16_t)origin);
    if (origin > 0) {
        if (origin >= 20) SET_W(D(6), 20);
    } else if (origin <= -20) {
        SET_W(D(6), 20);
    } else {
        SET_W(D(4), (uint16_t)origin);
        SET_W(D(6), (uint16_t)-origin);
        D(4) = SEXT((uint16_t)(origin * 2 + 0x28));
        D(1) += D(4);
    }
    SET_W(D(5), (uint16_t)((20 - W(6)) * 2 + 1));
    SET_W(D(6), (uint16_t)(W(6) + 0xDC0));
    if (lift > 0) {
        SET_W(D(4), (uint16_t)(lift << 6));
        SET_W(D(6), (uint16_t)(W(6) - W(4)));
    }
    A(2) = rd_u32(PAGE_PLANE_TABLE);
    D(3) = 0xFFFFFFFFu;
    D(1) += D(0);
    SET_W(D(7), (uint16_t)(W(7) << 6));
    SET_W(D(6), (uint16_t)(W(6) - W(7)));
    SET_W(D(2), 0x3FA);
    for (k = 0; k < 4; k++) {
        D(4) = rd_u32(A(2)) + D(1);
        A(2) += 4;
    }
    return glue_return();
}
