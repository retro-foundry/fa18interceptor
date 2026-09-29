/* The head-up display's fixed marks (see hud_marks.h). */
#include "hud_marks.h"

#include "globals.h"
#include "hud_readouts.h"
#include "memory.h"
#include "plot.h"
#include "render_line.h"
#include "view_marks.h"

#define HUD_COLOUR  10
#define HUD_FRAME_LINES 5
#define HUD_FRAME_POINTS  0xC34560u /* word x, y per corner */
#define HUD_FRAME_COLOURS 0xC34578u /* word per line, 4 bytes apart */

static int16_t across(int16_t x) {
    return (int16_t)(x + rd_s16(SPAN_ORIGIN_Y));
}

static int16_t down(int16_t y) {
    return (int16_t)(y + rd_s16(REDRAW_STATE_WORD));
}

/* A 5-pixel dash at column x (moved) unless it leaves columns first..$13F. */
static void dash(int16_t x, int16_t y, int16_t first) {
    int16_t left = across(x);
    if (left < first || (int16_t)(left + 4) > 0x13B) return;
    draw_line(left, down(y), (int16_t)(left + 4), down(y));
}

/* A frame line from corner i to i + 1, its columns clamped to the screen
 * (one wholly off it is skipped). */
static void frame_line(int i) {
    gaddr p = HUD_FRAME_POINTS + (gaddr)(4 * i);
    int16_t x0 = rd_s16(p), y0 = rd_s16(p + 2), x1 = rd_s16(p + 4), y1 = rd_s16(p + 6);
    int16_t origin = rd_s16(SPAN_ORIGIN_Y);
    int32_t left = (int32_t)x0 + origin, right = (int32_t)x1 + origin;

    wr_u16(CURRENT_COLOUR, rd_u16(HUD_FRAME_COLOURS + (gaddr)(4 * i)));
    x0 = (int16_t)left;
    x1 = (int16_t)right;
    if (left < 0) {
        x0 = 0;
        if (right < 0) return;
        if (x1 > 0x13F) x1 = 0x13F;
    } else if (x0 > 0x13F) {
        x0 = 0x13F;
        if (x1 > 0x13F) return;
    } else {
        if (right < 0) x1 = 0;
        if (x1 > 0x13F) x1 = 0x13F;
    }
    draw_line(x0, down(y0), x1, down(y1));
}

void draw_hud_marks(void) {
    int32_t centre = (int32_t)0x9F + rd_s16(SPAN_ORIGIN_Y);
    uint8_t type;
    int i;

    wr_u32(LINE_STYLE, 0xFFFFF);
    wr_u16(CURRENT_COLOUR, HUD_COLOUR);
    if (centre >= 0 && (int16_t)centre < 0x140) {
        int16_t x = (int16_t)centre, y = down(0x47);
        plot_pixel(x, y);
        plot_pixel(x, (int16_t)(y + 1));
        plot_pixel(x, (int16_t)(y + 3));
        plot_pixel(x, (int16_t)(y + 4));
        dash(0x9D, 0x48, 4);
    }
    wr_u16(CURRENT_COLOUR, HUD_COLOUR);
    dash(0x8D, 0x5A, 5);
    dash(0xAD, 0x5A, 5);
    type = rd_u8(CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD) + 0x62);
    if (type != 0x10 && type != 0x11 && type != 0x12) return;
    for (i = 0; i < HUD_FRAME_LINES; i++) {
        wr_u16(HUD_LINE_INDEX, (uint16_t)(4 * i));
        frame_line(i);
    }
    wr_u16(HUD_LINE_INDEX, 4 * HUD_FRAME_LINES);
}

static void tick(int16_t x, int16_t y, int k) {
    plot_pixel(x, y);
    if (k == 1 || k == 3) plot_pixel(x, (int16_t)(y - 1));
}

void draw_tick_row(int16_t y) {
    int16_t centre = rd_s16(HUD_CENTRE_X), x;
    int32_t middle = (int32_t)centre + rd_s16(SPAN_ORIGIN_Y);
    int k;

    if (middle >= 0 && (int16_t)middle < 0x140) {
        plot_pixel((int16_t)middle, down(y));
        plot_pixel((int16_t)middle, (int16_t)(down(y) - 1));
    }
    for (x = (int16_t)middle, k = 0;; k++) {
        if ((int32_t)x + 10 < 0) break;
        x = (int16_t)(x + 10);
        if (x > 0x13F || x >= across(0xB9)) break;
        tick(x, y, k);
    }
    for (x = across(centre), k = 0;; k++) {
        if ((int32_t)x - 10 < 0) break;
        x = (int16_t)(x - 10);
        if (x > 0x13F || x <= across(0x85)) break;
        tick(x, y, k);
    }
}

#define BOX_CORNERS  0xC3458Cu /* word dx, dy per corner, the first repeated */
#define JITTER_STEPS 0xC34540u /* word dx, dy, by record 0's +$16 bits 2-4 */

/* One axis of the seeker's step: a quarter (a half when fast) of the
 * distance, an eighth beyond 15, at most 1 (2) pixels. */
static int16_t seek(int16_t mark, int16_t target, int fast) {
    int16_t d = (int16_t)(mark - target), limit = fast ? 2 : 1;
    int near_shift = fast ? 1 : 2, far_shift = fast ? 2 : 4;

    if ((int32_t)mark - target >= 0) {
        d = (int16_t)(d > 15 ? d >> far_shift : d >> near_shift);
        if (d > limit) d = limit;
    } else {
        d = (int16_t)(d < -15 ? d >> far_shift : d >> near_shift);
        if (d < -limit) d = (int16_t)-limit;
    }
    return (int16_t)(mark - d);
}

static void step_seeker(int16_t tx, int16_t ty) {
    uint8_t kind = rd_u8(CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD) + 0x63) & 0xF0;
    int16_t sx = rd_s16(SEEKER_MARK), sy = rd_s16(SEEKER_MARK + 2);
    int fast = (rd_u32(WARNING_CAUSES) & 0x4000) != 0;

    if (!kind || tx <= 0) {
        wr_u16(SEEKER_MARK, 0xFFFF);
        wr_u16(SEEKER_MARK + 2, (uint16_t)sx); /* the old x, as the original stores it */
    } else if (sx <= 0) {
        wr_u16(SEEKER_MARK, 0x9F);
        wr_u16(SEEKER_MARK + 2, 0x5B);
    } else {
        wr_u16(SEEKER_MARK, (uint16_t)seek(sx, tx, fast));
        wr_u16(SEEKER_MARK + 2, (uint16_t)seek(sy, ty, fast));
    }
}

static int16_t clamp_column(int32_t x) {
    if (x < 0) return 0;
    return (int16_t)x > 0x13F ? 0x13F : (int16_t)x;
}

/* One side of the box around (x, y), clipped to the HUD frame's inside; a
 * side lying wholly outside it is skipped. */
static void box_side(int i, int16_t x, int16_t y) {
    gaddr c = BOX_CORNERS + (gaddr)(4 * i);
    int16_t dx0 = rd_s16(c), dy0 = rd_s16(c + 2), origin = rd_s16(SPAN_ORIGIN_Y);
    int16_t x0 = clamp_column((int32_t)x + dx0), x1 = clamp_column((int32_t)x + rd_s16(c + 4));
    int16_t y0 = down((int16_t)(y + dy0)), y1 = down((int16_t)(y + rd_s16(c + 6)));
    int16_t left = (int16_t)(rd_s16(HUD_FRAME_POINTS) + 1 + origin);
    int32_t right = (int32_t)(int16_t)(rd_s16(HUD_FRAME_POINTS + 0x10) - 1) + origin;
    int16_t top = down(rd_s16(HUD_FRAME_POINTS + 6)), bottom = down(rd_s16(HUD_FRAME_POINTS + 2));

    if (left > 0x13F) return;
    if (x0 < left) {
        if (x0 == x1 && dx0 < 0) return;
        x0 = left;
    }
    if (x1 < left) x1 = left;
    if (right < 0) return;
    if (x0 > (int16_t)right) {
        if (x0 == x1 && dx0 >= 0) return;
        x0 = (int16_t)right;
    }
    if (x1 > (int16_t)right) x1 = (int16_t)right;
    if (y0 < top) {
        if (y0 == y1 && dy0 < 0) return;
        y0 = top;
    }
    if (y1 < top) y1 = top;
    if (y0 > bottom) y0 = bottom;
    if (y1 > bottom) y1 = bottom;
    draw_line(x0, y0, x1, y1);
}

void draw_target_box(void) {
    int i;

    wr_u16(HUD_LINE_INDEX, 0);
    wr_u16(CURRENT_COLOUR, HUD_COLOUR);
    for (i = 0; i < 4; i++) {
        int16_t x = rd_s16(TARGET_MARK), y = rd_s16(TARGET_MARK + 2);
        if (!rd_u8(POST_INPUT_EVENT)) step_seeker(x, y);
        if (rd_u16(CONTROL_RECORDS + 0x56)) {
            gaddr step = JITTER_STEPS + (rd_u16(CONTROL_RECORDS + 0x16) & 0x1C);
            x = (int16_t)(x + rd_s16(step));
            y = (int16_t)(y + rd_s16(step + 2));
        }
        x = across(x);
        y = down(y);
        if (x <= 0 || y <= 0) return;
        box_side(i, x, y);
        wr_u16(HUD_LINE_INDEX, (uint16_t)(4 * (i + 1)));
    }
    wr_u32(SELECTION_MARKER, rd_u32(TARGET_MARK));
    wr_u16(TARGET_MARK, 0xFFFF);
}

#define LOCKED   0x4000u /* WARNING_CAUSES: the seeker is on the target */
#define RELEASED 0x0200u /* WARNING_CAUSES: it has come off it */

static void set_bits(gaddr at, uint32_t bits, uint32_t clear) {
    wr_u32(at, (rd_u32(at) & ~clear) | bits);
}

/* $C33DA4. */
static void drop_lock(void) {
    if (!(rd_u32(WARNING_CAUSES) & (LOCKED | RELEASED))) return;
    set_bits(WARNING_CAUSES, 0, LOCKED | RELEASED);
    set_bits(EVENT_BITS, 8, 0);
}

static void lose_lock(void) {
    set_bits(WARNING_CAUSES, RELEASED, LOCKED);
    set_bits(EVENT_BITS, 0x100, 0);
}

static int display_step_9(void) {
    return (rd_u8(DISPLAY_FORCE) & 0x1F) == 9;
}

static int16_t distance(int16_t a, int16_t b) {
    int16_t d = (int16_t)(a - b);
    return (int32_t)a - b < 0 ? (int16_t)-d : d;
}

void update_missile_cue(void) {
    gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    uint8_t kind = rd_u8(record + 0x63) & 0xF0;
    int16_t x = rd_s16(SEEKER_MARK), y = rd_s16(SEEKER_MARK + 2), reach;
    int on_target = 0;

    if (rd_s16(SELECTED_RECORD) < 0 || kind == 0x10 || x < 0) {
        drop_lock();
        return;
    }
    if (x <= 0x60 || x >= 0xDE || y <= 0x2E || y >= 0x86) {
        drop_lock();
        wr_u8(SHOOT_CUE, 0);
    } else if (distance(rd_s16(SELECTION_MARKER), x) > 5 || distance(rd_s16(SELECTION_MARKER + 2), y) > 5) {
        uint32_t causes = rd_u32(WARNING_CAUSES);
        wr_u8(SHOOT_CUE, 0);
        if ((causes & LOCKED) || !(causes & RELEASED) || display_step_9()) lose_lock();
    } else {
        on_target = 1;
        set_bits(WARNING_CAUSES, LOCKED, RELEASED);
        if (kind == 0x30) reach = 0x2700;
        else if (kind == 0x20) reach = 0x1800;
        else return;
        if (rd_s16(record + 0x4A) >= reach || rd_s16(SELECTED_RECORD) < 0) {
            wr_u8(SHOOT_CUE, 0);
            if (display_step_9()) lose_lock();
        } else if (rd_s16(RANGE_RATE) < -0x2A3) {
            if (rd_u8(SHOOT_CUE) != 2) {
                set_bits(EVENT_BITS, 0x10, 0);
                wr_u8(SHOOT_CUE, 2);
            }
        } else if (rd_u8(SHOOT_CUE) != 1 || display_step_9()) {
            set_bits(EVENT_BITS, 4, 0);
            wr_u8(SHOOT_CUE, 1);
        }
    }
    wr_u16(CURRENT_COLOUR, HUD_COLOUR);
    plot_symbol(x, y, (int16_t)on_target);
    if (rd_u8(SHOOT_CUE)) draw_shoot_cue();
}
