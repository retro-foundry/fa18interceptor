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

static int16_t origin(void) { return rd_s16(SPAN_ORIGIN_Y); }

/* A dash's registers ($C34180 and the two after). */
static void dash_registers(int16_t x, int16_t y, int16_t first) {
    SET_W(D(0), (uint16_t)(x + origin()));
    if (W(0) < first) return;
    SET_W(D(1), (uint16_t)y);
    SET_W(D(2), (uint16_t)(W(0) + 4));
    if (W(2) > 0x13B) return;
    SET_W(D(3), (uint16_t)D(1));
    SET_W(D(1), (uint16_t)(W(1) + rd_s16(REDRAW_STATE_WORD)));
    SET_W(D(3), (uint16_t)(W(3) + rd_s16(REDRAW_STATE_WORD)));
    line_registers();
}

int glue_C34146(void) {
    gaddr record = CONTROL_RECORDS + SEXT(rd_u16(VIEW_RECORD));
    int32_t centre = (int32_t)0x9F + origin();
    uint8_t type;
    uint16_t colour;
    int i;

    draw_hud_marks();
    /* The plots read the colour then in force; the frame lines set theirs. */
    colour = rd_u16(CURRENT_COLOUR);
    wr_u16(CURRENT_COLOUR, 10);
    SET_W(D(0), 0x9F);
    SET_W(D(1), 0x47);
    plot_in_view_registers();
    if (centre >= 0 && (int16_t)centre < 0x140) {
        SET_W(D(1), (uint16_t)(W(1) + 1));
        restored_plot_registers();
        SET_W(D(1), (uint16_t)(W(1) + 2));
        restored_plot_registers();
        SET_W(D(1), (uint16_t)(W(1) + 1));
        plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
        dash_registers(0x9D, 0x48, 4);
    }
    dash_registers(0x8D, 0x5A, 5);
    dash_registers(0xAD, 0x5A, 5);
    A(0) = record;
    type = rd_u8(record + 0x62);
    SET_B(D(0), type);
    if (type != 0x10 && type != 0x11 && type != 0x12) {
        wr_u16(CURRENT_COLOUR, colour);
        return glue_return();
    }
    for (i = 0; i < 5; i++) {
        int32_t left, right;
        int draw = 1;
        A(0) = 0xC34560u + (gaddr)(4 * i);
        D(0) = SEXT(rd_u16(A(0)));
        D(1) = SEXT(rd_u16(A(0) + 2));
        D(2) = SEXT(rd_u16(A(0) + 4));
        D(3) = SEXT(rd_u16(A(0) + 6));
        A(0) = 0xC34578u + (gaddr)(4 * i);
        wr_u16(CURRENT_COLOUR, rd_u16(A(0)));
        left = (int32_t)W(0) + origin();
        right = (int32_t)W(2) + origin();
        SET_W(D(0), (uint16_t)left);
        if (left < 0) {
            SET_W(D(0), 0);
            SET_W(D(2), (uint16_t)right);
            if (right < 0) draw = 0;
            else if (W(2) > 0x13F) SET_W(D(2), 0x13F);
        } else if (W(0) > 0x13F) {
            SET_W(D(0), 0x13F);
            SET_W(D(2), (uint16_t)right);
            if (W(2) > 0x13F) draw = 0;
        } else {
            SET_W(D(2), (uint16_t)right);
            if (right < 0) SET_W(D(2), 0);
            if (W(2) > 0x13F) SET_W(D(2), 0x13F);
        }
        if (!draw) continue;
        SET_W(D(1), (uint16_t)(W(1) + rd_s16(REDRAW_STATE_WORD)));
        SET_W(D(3), (uint16_t)(W(3) + rd_s16(REDRAW_STATE_WORD)));
        line_registers();
    }
    wr_u16(CURRENT_COLOUR, colour);
    return glue_return();
}

static void tick_registers(int16_t y, int k) {
    uint16_t x = (uint16_t)D(0);
    if (k == 1 || k == 3) {
        restored_plot_registers();
        SET_W(D(1), (uint16_t)(y - 1));
        plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
    } else {
        plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
    }
    SET_W(D(0), x);
}

int glue_C34066(void) {
    int16_t y = W(1), centre = rd_s16(HUD_CENTRE_X);
    int32_t middle = (int32_t)centre + origin();
    int k;

    draw_tick_row(y);
    SET_W(D(0), (uint16_t)centre);
    plot_in_view_registers();
    if (middle >= 0 && (int16_t)middle < 0x140) {
        SET_W(D(1), (uint16_t)(W(1) - 1));
        restored_plot_registers();
    }
    for (k = 0;; k++) {
        int32_t next;
        SET_W(D(1), (uint16_t)y);
        next = (int32_t)W(0) + 10;
        SET_W(D(0), (uint16_t)next);
        if (next < 0 || W(0) > 0x13F) break;
        SET_W(D(2), (uint16_t)(0xB9 + origin()));
        if (W(0) >= W(2)) break;
        tick_registers(y, k);
    }
    SET_W(D(0), (uint16_t)(centre + origin()));
    for (k = 0;; k++) {
        int32_t next;
        SET_W(D(1), (uint16_t)y);
        next = (int32_t)W(0) - 10;
        SET_W(D(0), (uint16_t)next);
        if (next < 0 || W(0) > 0x13F) break;
        SET_W(D(2), (uint16_t)(0x85 + origin()));
        if (W(0) <= W(2)) break;
        tick_registers(y, k);
    }
    return glue_return();
}

/* One axis of the seeker's step on D`m` (the mark) against D`t`. */
static void seek_registers(int m, int t) {
    int32_t diff = (int32_t)W(m) - W(t);
    SET_W(D(m), (uint16_t)diff);
    if (diff >= 0) {
        if (W(m) > 15) {
            SET_W(D(m), (uint16_t)(W(m) >> (D(6) & 63)));
        } else {
            SET_W(D(m), (uint16_t)(W(m) >> (D(7) & 63)));
            if (W(m) <= 5) SET_W(D(5), (uint16_t)(W(5) + 1));
        }
        if (W(7) == 1) {
            if (W(m) > 2) D(m) = 2;
        } else if (W(m) > 1) {
            D(m) = 1;
        }
    } else {
        if (W(m) < -15) {
            SET_W(D(m), (uint16_t)(W(m) >> (D(6) & 63)));
        } else {
            SET_W(D(m), (uint16_t)(W(m) >> (D(7) & 63)));
            if (W(m) >= -5) SET_W(D(5), (uint16_t)(W(5) + 1));
        }
        if (W(7) == 1) {
            if (W(m) < -2) D(m) = 0xFFFFFFFEu;
        } else if (W(m) < -1) {
            D(m) = 0xFFFFFFFFu;
        }
    }
}

int glue_C342D0(void) {
    int16_t tx = rd_s16(TARGET_MARK), ty = rd_s16(TARGET_MARK + 2);
    int16_t sx = rd_s16(SEEKER_MARK), sy = rd_s16(SEEKER_MARK + 2);
    int event = rd_u8(POST_INPUT_EVENT) != 0, fast = (rd_u32(WARNING_CAUSES) & 0x4000) != 0;
    int jitter = rd_u16(CONTROL_RECORDS + 0x56) != 0;
    uint16_t phase = rd_u16(CONTROL_RECORDS + 0x16) & 0x1C;
    gaddr record = CONTROL_RECORDS + SEXT(rd_u16(VIEW_RECORD));
    uint8_t kind = rd_u8(record + 0x63) & 0xF0;
    int16_t rsw = rd_s16(REDRAW_STATE_WORD);
    int i;

    draw_target_box();
    for (i = 0; i < 4; i++) {
        int16_t dx0, dy0, dx1, dy1;
        int32_t sum;
        D(0) = SEXT((uint16_t)tx);
        D(1) = SEXT((uint16_t)ty);
        if (!event) {
            D(2) = SEXT((uint16_t)sx);
            D(3) = SEXT((uint16_t)sy);
            D(5) = 0;
            A(1) = record;
            SET_B(D(4), kind);
            if (!kind || W(0) <= 0) {
                SET_W(D(4), 0xFFFF);
            } else if (W(2) <= 0) {
                SET_W(D(4), 0x9F);
                SET_W(D(2), 0x5B);
            } else {
                D(6) = fast ? 2 : 4;
                D(7) = fast ? 1 : 2;
                SET_W(D(4), (uint16_t)D(2));
                seek_registers(2, 0);
                SET_W(D(4), (uint16_t)(W(4) - W(2)));
                SET_W(D(2), (uint16_t)D(3));
                seek_registers(3, 1);
                SET_W(D(2), (uint16_t)(W(2) - W(3)));
            }
            sx = W(4);
            sy = W(2);
        }
        if (jitter) {
            SET_W(D(2), phase);
            A(0) = 0xC34540u;
            SET_W(D(0), (uint16_t)(W(0) + rd_s16(A(0) + phase)));
            SET_W(D(1), (uint16_t)(W(1) + rd_s16(A(0) + phase + 2)));
        }
        SET_W(D(0), (uint16_t)(W(0) + origin()));
        SET_W(D(1), (uint16_t)(W(1) + rsw));
        SET_W(D(2), (uint16_t)D(0));
        if (W(2) <= 0) return glue_return();
        SET_W(D(3), (uint16_t)D(1));
        if (W(3) <= 0) return glue_return();
        A(0) = 0xC3458Cu + (gaddr)(4 * i);
        dx0 = rd_s16(A(0));
        dy0 = rd_s16(A(0) + 2);
        dx1 = rd_s16(A(0) + 4);
        dy1 = rd_s16(A(0) + 6);
        sum = (int32_t)W(0) + dx0;
        SET_W(D(0), (uint16_t)sum);
        if (sum < 0) SET_W(D(0), 0);
        if (W(0) > 0x13F) SET_W(D(0), 0x13F);
        sum = (int32_t)W(2) + dx1;
        SET_W(D(2), (uint16_t)sum);
        if (sum < 0) SET_W(D(2), 0);
        if (W(2) > 0x13F) SET_W(D(2), 0x13F);
        SET_W(D(1), (uint16_t)(W(1) + dy0 + rsw));
        SET_W(D(3), (uint16_t)(W(3) + dy1 + rsw));
        SET_W(D(4), (uint16_t)(0x56 + origin()));
        if (W(4) > 0x13F) continue;
        if (W(0) < W(4)) {
            if (W(0) == W(2) && dx0 < 0) continue;
            SET_W(D(0), (uint16_t)D(4));
        }
        if (W(2) < W(4)) SET_W(D(2), (uint16_t)D(4));
        sum = (int32_t)0xE8 + origin();
        SET_W(D(4), (uint16_t)sum);
        if (sum < 0) continue;
        if (W(0) > W(4)) {
            if (W(0) == W(2) && dx0 >= 0) continue;
            SET_W(D(0), (uint16_t)D(4));
        }
        if (W(2) > W(4)) SET_W(D(2), (uint16_t)D(4));
        SET_W(D(4), (uint16_t)(0x2D + rsw));
        if (W(1) < W(4)) {
            if (W(1) == W(3) && dy0 < 0) continue;
            SET_W(D(1), (uint16_t)D(4));
        }
        if (W(3) < W(4)) SET_W(D(3), (uint16_t)D(4));
        SET_W(D(4), (uint16_t)(0x90 + rsw));
        if (W(1) > W(4)) SET_W(D(1), (uint16_t)D(4));
        if (W(3) > W(4)) SET_W(D(3), (uint16_t)D(4));
        line_registers();
    }
    return glue_return();
}

void pair_registers(void); /* glue_batch33.c */

/* $C2F66E: a pair from LINE_LAST_ROW down, otherwise two rows of pairs. */
void block_registers(void) {
    if (W(1) >= rd_s16(LINE_LAST_ROW)) pair_registers();
    else plot_registers(PAIR_MASKS, PLOT_ROWS_2);
}

int glue_C2F66E(void) {
    plot_pixel_block(W(0), W(1));
    block_registers();
    return glue_return();
}

/* $C347F2: the walk in D0/D1 (MOVE.B keeps the upper bytes) and A0. */
int glue_C347F2(void) {
    int16_t x = W(0), y = W(1), steps = W(2), shift = origin();
    int found = 0, quarter;

    plot_ring_point(x, y, steps, A(0));
    for (quarter = 0; quarter < 4 && !found; quarter++) {
        if (quarter == 1 || quarter == 3) {
            A(0) -= 2;
            y = (int16_t)(quarter == 1 ? y - 1 : y + 1);
        } else if (quarter == 2) {
            A(0) += 2;
        }
        for (;;) {
            if (quarter == 0 || quarter == 2) {
                SET_B(D(0), rd_u8(A(0)));
                SET_B(D(1), rd_u8(A(0) + 1));
                A(0) += 2;
                if ((int8_t)D(1) < 0) break;
                if (quarter == 0) SET_B(D(1), (uint8_t)-(int8_t)D(1));
                else SET_B(D(0), (uint8_t)-(int8_t)D(0));
            } else {
                A(0) -= 2;
                SET_B(D(1), rd_u8(A(0) + 1));
                SET_B(D(0), rd_u8(A(0)));
                if ((int8_t)D(0) < 0) {
                    if (quarter == 3) return glue_return();
                    break;
                }
                if (quarter == 3) {
                    SET_B(D(0), (uint8_t)-(int8_t)D(0));
                    SET_B(D(1), (uint8_t)-(int8_t)D(1));
                }
            }
            if (--steps < 0) {
                found = 1;
                break;
            }
        }
    }
    SET_W(D(0), (uint16_t)(int8_t)D(0));
    SET_W(D(1), (uint16_t)(int8_t)D(1));
    SET_W(D(0), (uint16_t)(W(0) + x));
    SET_W(D(1), (uint16_t)(W(1) + y));
    if (W(0) <= 2 || W(0) >= 0x13E || W(0) <= (int16_t)(0x55 + shift) || W(0) >= (int16_t)(0xE9 + shift))
        return glue_return();
    if (W(1) <= 0x2D || W(1) >= 0x90) return glue_return();
    block_registers();
    return glue_return();
}

/* $C33DA4's D5. */
static void drop_lock_registers(uint32_t causes) {
    D(5) = causes & 0x4200;
}

int glue_C33DC8(void) {
    gaddr record = CONTROL_RECORDS + SEXT(rd_u16(VIEW_RECORD));
    uint8_t kind = rd_u8(record + 0x63) & 0xF0, cue = rd_u8(SHOOT_CUE);
    int16_t selected = rd_s16(SELECTED_RECORD), x = rd_s16(SEEKER_MARK), y = rd_s16(SEEKER_MARK + 2);
    int16_t mx = rd_s16(SELECTION_MARKER), my = rd_s16(SELECTION_MARKER + 2);
    int16_t range = rd_s16(record + 0x4A), rate = rd_s16(RANGE_RATE);
    uint32_t causes = rd_u32(WARNING_CAUSES);
    uint8_t step = rd_u8(DISPLAY_FORCE) & 0x1F;
    uint16_t colour;
    int32_t diff;

    update_missile_cue();
    if (selected < 0) {
        drop_lock_registers(causes);
        return glue_return();
    }
    A(1) = record;
    SET_B(D(5), kind);
    if (kind == 0x10) {
        drop_lock_registers(causes);
        return glue_return();
    }
    D(0) = SEXT((uint16_t)x);
    D(1) = SEXT((uint16_t)y);
    if (x < 0) {
        drop_lock_registers(causes);
        return glue_return();
    }
    if (x <= 0x60 || x >= 0xDE || y <= 0x2E || y >= 0x86) {
        drop_lock_registers(causes);
        D(4) = 0;
        goto symbol;
    }
    D(2) = SEXT((uint16_t)mx);
    D(3) = SEXT((uint16_t)my);
    diff = (int32_t)W(2) - W(0);
    SET_W(D(2), (uint16_t)diff);
    if (diff < 0) SET_W(D(2), (uint16_t)-W(2));
    if (W(2) > 5) goto miss;
    diff = (int32_t)W(3) - W(1);
    SET_W(D(3), (uint16_t)diff);
    if (diff < 0) SET_W(D(3), (uint16_t)-W(3));
    if (W(3) > 5) goto miss;
    D(4) = 1;
    SET_W(D(7), 0x2700);
    if (kind != 0x30) {
        SET_W(D(7), 0x1800);
        if (kind != 0x20) return glue_return();
    }
    if (range >= W(7) || selected < 0) goto step_check;
    if (rate >= -0x2A3 && cue == 1) SET_B(D(7), step);
    goto symbol;
miss:
    D(4) = 0;
    D(6) = causes & 0x4000;
    D(5) = causes;
    if (D(6)) goto symbol;
    D(5) = causes & 0x200;
    if (!D(5)) goto symbol;
step_check:
    SET_B(D(7), step);
symbol:
    colour = rd_u16(CURRENT_COLOUR);
    wr_u16(CURRENT_COLOUR, 10);
    symbol_registers();
    wr_u16(CURRENT_COLOUR, colour);
    if (rd_u8(SHOOT_CUE)) shoot_cue_registers();
    return glue_return();
}
