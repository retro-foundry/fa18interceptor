/* Glue for the postflight HUD (postflight_hud.c). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "numbers.h"
#include "postflight_hud.h"
#include "text.h"
#include "glue_text.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

void projection_mode_registers(int16_t mode, int entry); /* glue_projection.c */

/* MULS.W of D`n`'s word by the word at `at`. */
static void muls_at(int n, gaddr at) {
    D(n) = (uint32_t)((int32_t)W(n) * rd_s16(at));
}

/* One matrix row: D5-D7 from D2-D4, summed into D7 >> 8. */
static void row_registers(gaddr row) {
    SET_W(D(5), (uint16_t)D(2));
    SET_W(D(6), (uint16_t)D(3));
    SET_W(D(7), (uint16_t)D(4));
    muls_at(5, row);
    muls_at(6, row + 2);
    muls_at(7, row + 4);
    D(7) += D(6);
    D(7) += D(5);
    D(7) = (uint32_t)((int32_t)D(7) >> 8);
}

typedef struct {
    uint32_t previous[3], current[3];
} Vectors;

static void save_vectors(Vectors *v) {
    int k;
    for (k = 0; k < 3; k++) {
        v->previous[k] = rd_u32(POSTFLIGHT_VECTOR_PREVIOUS + (gaddr)(4 * k));
        v->current[k] = rd_u32(POSTFLIGHT_VECTOR_CURRENT + (gaddr)(4 * k));
    }
}

/* $C33CD2's registers from the vectors as they were before it ran; the
 * projector's are replayed in the middle. */
static void transform_registers(const Vectors *v) {
    gaddr record = CONTROL_RECORDS + SEXT(rd_u16(VIEW_RECORD));
    const uint32_t *previous = v->previous, *current = v->current;
    int k;

    A(1) = record;
    for (k = 0; k < 3; k++)
        D(2 + k) = (uint32_t)((int32_t)(previous[k] - rd_u32(record + 0x14 + (gaddr)(4 * k))) >> 8);
    A(2) = VIEW_ANGLE_MATRIX + 18;
    row_registers(VIEW_ANGLE_MATRIX);
    SET_W(D(0), (uint16_t)D(7));
    row_registers(VIEW_ANGLE_MATRIX + 6);
    muls_at(2, VIEW_ANGLE_MATRIX + 12);
    muls_at(3, VIEW_ANGLE_MATRIX + 14);
    muls_at(4, VIEW_ANGLE_MATRIX + 16);
    D(4) += D(3);
    D(4) += D(2);
    D(4) = (uint32_t)((int32_t)D(4) >> 8);
    SET_W(D(1), (uint16_t)D(7));
    SET_W(D(2), (uint16_t)D(4));
    projection_mode_registers(-5, 0);
    SET_W(D(4), 0x600);
    SET_W(D(5), 0x4000);
    A(2) = record + 0x92;
    SET_W(D(0), (uint16_t)D(4));
    SET_W(D(7), (uint16_t)D(5));
    muls_at(0, A(2) + 2);
    muls_at(7, A(2) + 4);
    D(0) += D(7);
    SET_W(D(1), (uint16_t)D(4));
    SET_W(D(7), (uint16_t)D(5));
    muls_at(1, A(2) + 8);
    muls_at(7, A(2) + 10);
    D(1) += D(7);
    SET_W(D(2), (uint16_t)D(4));
    muls_at(2, A(2) + 14);
    muls_at(5, A(2) + 16);
    D(2) += D(5);
    for (k = 0; k < 3; k++)
        D(k) = (uint32_t)((int32_t)D(k) >> 6) + rd_u32(record + 0x14 + (gaddr)(4 * k));
    for (k = 0; k < 3; k++) D(3 + k) = current[k];
}

int glue_C33CD2(void) {
    Vectors v;
    save_vectors(&v);
    transform_postflight_record();
    transform_registers(&v);
    return glue_return();
}

/* $C33B38: its steps' registers in order after the C, each plot with the
 * colour then in force. */
int glue_C33B38(void) {
    gaddr record = CONTROL_RECORDS + SEXT(rd_u16(VIEW_RECORD));
    uint8_t kind = rd_u8(record + 0x63) & 0xF0, cue = rd_u8(SHOOT_CUE), flag = rd_u8(record + 4) & 1;
    int16_t selected = rd_s16(SELECTED_RECORD), mx = rd_s16(SELECTION_MARKER), my = rd_s16(SELECTION_MARKER_Y);
    int16_t px = rd_s16(POSTFLIGHT_MARK), py = rd_s16(POSTFLIGHT_MARK + 2), range = rd_s16(record + 0x4A);
    int16_t rate = rd_s16(RANGE_RATE), last = rd_s16(POSTFLIGHT_RANGE_LAST);
    uint16_t colour = rd_u16(CURRENT_COLOUR), final_colour;
    int event = rd_u8(POST_INPUT_EVENT) != 0;
    int32_t diff;
    Vectors v;

    if (rd_s16(ZOOM_SCALE) != 0x80) return glue_return();
    save_vectors(&v);
    draw_postflight_variant();
    final_colour = rd_u16(CURRENT_COLOUR);
    A(1) = record;
    SET_B(D(5), kind);
    if (kind != 0x10) {
        if (selected < 0) goto transform;
        SET_W(D(0), 0x9F);
        SET_W(D(1), 0x5B);
        if (mx <= 0) {
            px = 0x9F;
            py = 0x5B;
        }
        goto display;
    }
    SET_W(D(0), (uint16_t)mx);
    if (mx <= 0) goto clear;
    diff = (int32_t)mx - px;
    SET_W(D(0), (uint16_t)diff);
    if (diff < 0) SET_W(D(0), (uint16_t)-W(0));
    if (W(0) > 8) goto clear;
    SET_W(D(1), (uint16_t)my);
    diff = (int32_t)my - py;
    SET_W(D(1), (uint16_t)diff);
    if (diff < 0) SET_W(D(1), (uint16_t)-W(1));
    if (W(1) > 8) goto clear;
    if (range > 0x900) goto clear;
    if (!cue && selected < 0) goto clear;
    colour = 13;
    wr_u16(CURRENT_COLOUR, 13);
    shoot_cue_registers();
    goto dot;
clear:
dot:
    D(0) = SEXT((uint16_t)px);
    D(1) = SEXT((uint16_t)py);
    if (px <= 0) goto transform;
    wr_u16(CURRENT_COLOUR, colour);
    plot_in_view_registers();
display:
    D(0) = SEXT((uint16_t)px);
    D(1) = SEXT((uint16_t)py);
    if (px <= 0) goto transform;
    SET_W(D(1), (uint16_t)(W(1) + rd_s16(REDRAW_STATE_WORD)));
    SET_W(D(2), 0x50);
    A(0) = 0xC3494Cu;
    wr_u16(CURRENT_COLOUR, 10);
    ring_registers();
    if (selected < 0) goto transform;
    D(0) = SEXT((uint16_t)px);
    D(1) = SEXT((uint16_t)py);
    SET_W(D(1), (uint16_t)(W(1) + rd_s16(REDRAW_STATE_WORD)));
    A(0) = record;
    SET_W(D(2), (uint16_t)range);
    D(2) = (uint32_t)(uint16_t)D(2) * 0x4Cu;
    D(2) = (D(2) % 0x8CA0u) << 16 | (D(2) / 0x8CA0u);
    A(0) = 0xC34976u;
    wr_u16(CURRENT_COLOUR, 13);
    ring_point_registers();
    A(0) = record;
    SET_W(D(0), (uint16_t)range);
    if (range > 0x7F00) goto transform;
    if (event || !flag) {
        SET_W(D(0), (uint16_t)rate);
    } else {
        SET_W(D(1), (uint16_t)range);
        SET_W(D(0), (uint16_t)(range - last));
    }
    signed_readout_registers();
transform:
    wr_u16(CURRENT_COLOUR, final_colour);
    if (!event) transform_registers(&v);
    return glue_return();
}

/* ABCD/SBCD of the three low bytes of `bcd` by `step` (X cleared first). */
static uint32_t bcd_step(uint32_t bcd, uint32_t step, int add) {
    int carry = 0, i;
    for (i = 0; i < 3; i++) {
        int shift = 8 * i;
        int a = (int)((bcd >> shift) & 0xFF), b = (int)((step >> shift) & 0xFF);
        int lo = add ? (a & 15) + (b & 15) + carry : (a & 15) - (b & 15) - carry;
        int hi = add ? (a >> 4) + (b >> 4) : (a >> 4) - (b >> 4);
        if (add ? lo > 9 : lo < 0) {
            lo += add ? -10 : 10;
            hi += add ? 1 : -1;
        }
        if (add ? hi > 9 : hi < 0) {
            hi += add ? -10 : 10;
            carry = 1;
        } else {
            carry = 0;
        }
        bcd = (bcd & ~(0xFFu << shift)) | (uint32_t)(((hi << 4) | lo) & 0xFF) << shift;
    }
    return bcd;
}

static uint32_t from_bcd(uint32_t bcd) {
    uint32_t value = 0;
    int i;
    for (i = 7; i >= 0; i--) value = value * 10 + ((bcd >> (4 * i)) & 15);
    return value;
}

/* An 8-pixel digits line's registers with `bcd` as its value. */
static void bcd_line_with(uint32_t bcd, gaddr layout, gaddr rows, int16_t x, int count, int digits, int keep) {
    uint32_t now = rd_u32(DISPLAY_VALUE_BCD);
    wr_u32(DISPLAY_VALUE_BCD, bcd);
    format_digits(TEXT_LINE + (gaddr)digits, digits, 0, keep); /* the line as it was then */
    bcd_entry(layout, rows, x, count, digits, TEXT_LINE + (gaddr)digits, keep);
    wr_u32(DISPLAY_VALUE_BCD, now);
}

/* A two-character label at the view's column ($C32AB4). */
static void label_registers(gaddr chars, gaddr layout, gaddr rows, int16_t x) {
    A(2) = chars;
    A(0) = chars + 2;
    A(1) = layout;
    A(4) = rows;
    D(0) = (uint32_t)(uint16_t)x << 16 | 1;
    text_in_view_registers();
}

/* $C33370: every step's registers in order after the C (later steps that
 * draw nothing leave earlier ones' registers). The tape forms (record type
 * $10) are replayed for their last steps only: the recordings never show
 * them. The heading's BCD walk is simulated to place each digits line. */
int glue_C33370(void) {
    gaddr record = CONTROL_RECORDS + SEXT(rd_u16(VIEW_RECORD));
    int tape = rd_u8(record + 0x62) == 0x10;
    int32_t heading = (int16_t)(rd_s16(record + 0x68) >> 3), value, altitude;
    uint32_t bcd = to_packed_bcd((uint32_t)heading), shown = bcd & 0xFFFF00FFu, start, walk;
    int high = (int16_t)shown > 0x40;
    int16_t centre, offset, speed = 0, origin = rd_s16(SPAN_ORIGIN_Y);
    int32_t x;
    uint8_t line[8];
    int k;

    if (rd_u8(FIXED_READOUTS)) {
        altitude = rd_s32(FIXED_ALTITUDE);
        if (altitude > 99999) altitude = 99999;
    } else {
        altitude = (int32_t)((uint32_t)(rd_s32(record + 0x18) >> 10) * 5);
    }
    if (!(rd_u8(record) & 0x80)) {
        speed = rd_s16(record + 0x6E);
        if (speed < 0) speed = (int16_t)-speed;
    }
    {
        uint32_t q = (uint32_t)(int32_t)speed / 12;
        speed = (int16_t)(q > 0xFFFF ? speed : (int16_t)q);
    }

    draw_postflight_tape();
    for (k = 0; k < 8; k++) line[k] = rd_u8(TEXT_LINE + (gaddr)k);

    if (!tape) {
        bcd_line_with(to_packed_bcd((uint32_t)altitude), 0xC33270u, 0xD38, 0x18, 6, 6, 0);
        label_registers(0xC33418u, 0xC3328Cu, 0xE52, 0x1A);
        bcd_line_with(to_packed_bcd((uint32_t)(int32_t)speed), 0xC33294u, 0xD2A, 0xA, 4, 4, 0);
        label_registers(0xC33642u, 0xC332A8u, 0xE44, 0xC);
    }
    SET_W(D(0), 0x9F);
    SET_W(D(1), 0x3F);
    x = (int32_t)0x9F + origin;
    plot_in_view_registers();
    if (x >= 0 && (int16_t)x < 0x140) {
        SET_W(D(1), (uint16_t)(W(1) + 1));
        restored_plot_registers();
        SET_W(D(1), (uint16_t)(W(1) + 1));
        plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
    }

    start = high ? to_packed_bcd((uint32_t)(heading + 100)) & 0xFFFFFF00u : bcd & 0xFFFFFF0Fu;
    start >>= 4;
    if ((int32_t)start >= 0x360) start = 0;
    value = (int32_t)from_bcd(shown) + (high ? -100 : 0);
    centre = (int16_t)-(int16_t)(value / 5);
    D(2) = high ? 0xFFFFFF9Cu : 0;
    D(5) = 0x858;
    offset = (int16_t)(centre << 3);
    bcd_line_with(start, 0xC33A16u + SEXT((uint16_t)offset), 0x858, 0x10, 2, 3, 1);
    walk = start;
    for (;;) {
        offset = (int16_t)(offset + 0xA0);
        if (offset > 0xB0) break;
        walk = bcd_step(walk, 10, 1);
        if ((int32_t)walk >= 0x360) walk = 0;
        bcd_line_with(walk, 0xC33A16u + SEXT((uint16_t)offset), 0x858, 0x10, 2, 3, 1);
    }
    walk = start;
    offset = (int16_t)(centre << 3);
    for (;;) {
        if ((int32_t)walk <= 0) walk = 0x360;
        offset = (int16_t)(offset - 0xA0);
        if (offset < -0xB0) break;
        walk = bcd_step(walk, 10, 0);
        bcd_line_with(walk, 0xC33A16u + SEXT((uint16_t)offset), 0x858, 0x10, 2, 3, 1);
    }
    for (k = 0; k < 8; k++) wr_u8(TEXT_LINE + (gaddr)k, line[k]);
    D(1) = 0x3D;
    SET_W(D(1), (uint16_t)(W(1) + rd_s16(REDRAW_STATE_WORD)));
    tick_row_registers();
    return glue_return();
}

/* A recreated routine called as the original calls it: the return address
 * pushed (dead stack afterwards), its glue run, which returns past it. */
static void call_port(int (*glue)(void), uint32_t return_to) {
    A(7) -= 4;
    wr_u32(A(7), return_to);
    glue();
}

/* $C332BC: draw_postflight_hud's steps are each a recreated routine, so its
 * glue runs theirs in the same order (each with its own C and replay). */
int glue_C332BC(void) {
    wr_u32(LINE_STYLE, 0xFFFFF);
    if (rd_u8(CONTEXT_SELECT)) {
        wr_u8(SHOOT_CUE, 0);
        return glue_return();
    }
    call_port(glue_C332FE, 0xC332D2);
    call_port(glue_C34146, 0xC332D6);
    call_port(glue_C342D0, 0xC332DA);
    call_port(glue_C33DC8, 0xC332DE);
    call_port(glue_C31C60, 0xC332E4);
    if (!rd_u8(POST_INPUT_EXPIRED)) return glue_return();
    call_port(glue_C31D64, 0xC332F2);
    call_port(glue_C33370, 0xC332F6);
    call_port(glue_C33B38, 0xC332FA);
    return glue_return();
}
