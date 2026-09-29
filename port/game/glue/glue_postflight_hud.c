/* Glue for the postflight HUD (postflight_hud.c). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "numbers.h"
#include "postflight_hud.h"
#include "view_transform.h"
#include "render_polygon.h"
#include "draw_stream.h"
#include "glue_clip.h"
#include "machine.h"
#include "projection.h"
#include "control_records.h"
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

/* $C28E28's exit scan on A3: 1 when an exit matched. */
static int zone_exit_registers(int16_t index) {
    SET_W(D(0), rd_u16(A(3)));
    A(3) += 2;
    SET_W(D(0), (uint16_t)(W(0) - 1));
    if (W(0) < 0) return 0;
    for (;;) {
        int k;
        A(3) += 4;
        SET_W(D(1), rd_u16(A(3)));
        SET_W(D(2), rd_u16(A(3) + 2));
        A(3) += 6;
        A(4) = 0xC295E0u + SEXT(rd_u16(0xC295E0u + SEXT(D(2))));
        for (k = 0; k < 5; k++) D(2 + k) = SEXT(rd_u16(A(4) + (gaddr)(2 * k)));
        A(4) += 10;
        SET_W(D(1), W(1) & 0x7F);
        if (W(1) == index) return 1;
        SET_W(D(0), (uint16_t)(W(0) - 1));
        if (W(0) == -1) return 0;
    }
}

int glue_C28E28(void) {
    int16_t index = rd_s16(STREAM_MODE), x, y;
    gaddr record;
    uint8_t mode;
    int8_t zone;
    int k;

    record = CONTROL_RECORDS + SEXT((uint16_t)(index << 9));
    mode = rd_u8(record + 0x7A);
    check_zone_exit();
    A(0) = CONTROL_RECORDS;
    SET_W(D(0), (uint16_t)(index << 9));
    if (!W(0)) return glue_return();
    A(0) = record;
    SET_B(D(5), rd_u8(record + 0x62) & 0xF0);
    if ((uint8_t)D(5) != 0x10 || rd_u8(record + 5) == 8) return glue_return();
    zone = (int8_t)rd_u8(record + 0x5D);
    SET_B(D(0), (uint8_t)zone);
    if (zone < 0) return glue_return();
    SET_B(D(0), (uint8_t)(zone - 1));
    if ((int8_t)D(0) < 0) return glue_return();
    SET_W(D(0), (uint16_t)((int8_t)D(0) * 4));
    x = rd_s16(record + 6);
    y = rd_s16(record + 8);
    SET_W(D(5), (uint16_t)x);
    SET_W(D(6), (uint16_t)y);
    A(3) = rd_u32(0xC29720u + SEXT(D(0)));
    for (k = 0; k < 4; k++) D(1 + k) = SEXT(rd_u16(A(3) + (gaddr)(2 * k)));
    A(3) += 8;
    if (x >= W(1) && x <= W(2) && y >= W(3) && y <= W(4)) {
        if (mode == 5) zone_exit_registers(index);
        return glue_return();
    }
    zone_exit_registers(index);
    return glue_return();
}

void draw_polygon_registers(uint16_t last_size, uint16_t colour); /* glue_batch35.c */
void filled_circle_registers(void);                              /* glue_circle.c */

static void divs_into(int n, int16_t divisor) {
    int32_t dividend = (int32_t)D(n), q = dividend / divisor;
    if (q == (int16_t)q) D(n) = (uint32_t)(uint16_t)(dividend % divisor) << 16 | (uint16_t)q;
}

/* One point of $C2D16C's last part from D3-D5 (its offsets): the
 * transform's and projection's registers; 1 when it is inside the view
 * (its screen pair then stored at A1). */
static int shape_point_registers(int16_t z, int16_t shift) {
    gaddr m = VIEW_ANGLE_MATRIX;
    int k;

    SET_W(D(3), (uint16_t)(W(3) + (int16_t)A(4)));
    SET_W(D(4), (uint16_t)(W(4) + (int16_t)A(5)));
    SET_W(D(5), (uint16_t)(W(5) + z));
    SET_W(D(7), (uint16_t)shift);
    for (k = 3; k <= 5; k++) SET_W(D(k), (uint16_t)(W(k) >> (shift & 63)));
    for (k = 0; k < 3; k++) D(k) = (uint32_t)((int32_t)rd_s16(m + (gaddr)(2 * k)) * W(3 + k));
    D(0) = (uint32_t)((int32_t)(D(0) + D(1) + D(2)) >> 8);
    SET_W(D(6), (uint16_t)D(0));
    for (k = 0; k < 3; k++) D(k) = (uint32_t)((int32_t)rd_s16(m + 6 + (gaddr)(2 * k)) * W(3 + k));
    D(2) = (uint32_t)((int32_t)(D(2) + D(0) + D(1)) >> 8);
    D(3) = (uint32_t)((int32_t)rd_s16(m + 12) * W(3));
    D(4) = (uint32_t)((int32_t)rd_s16(m + 14) * W(4));
    D(5) = (uint32_t)((int32_t)rd_s16(m + 16) * W(5));
    D(5) = (uint32_t)((int32_t)(D(5) + D(3) + D(4)) >> 8);
    A(0) = m + 18;
    if ((int32_t)D(5) <= 0 || W(6) > W(5)) return 0;
    SET_W(D(4), (uint16_t)-W(6));
    if (W(4) > W(5) || W(2) > W(5)) return 0;
    SET_W(D(4), (uint16_t)-W(2));
    if (W(4) > W(5)) return 0;
    D(6) = (uint32_t)((int32_t)W(6) * 160);
    divs_into(6, W(5));
    SET_W(D(6), (uint16_t)(W(6) + 160));
    if (W(6) < 0) SET_W(D(6), 0);
    else if (W(6) >= 320) SET_W(D(6), 319);
    SET_W(D(7), (uint16_t)D(2));
    D(7) = (uint32_t)((int32_t)W(7) * 90);
    divs_into(7, W(5));
    SET_W(D(7), (uint16_t)(W(7) + 90));
    if (W(7) < 0) SET_W(D(7), 0);
    else if (W(7) >= 180) SET_W(D(7), 179);
    SET_W(D(6), (uint16_t)(319 - W(6)));
    SET_W(D(7), (uint16_t)(179 - W(7)));
    A(1) += 4;
    return 1;
}

/* $C2D16C: only its last part (index 0) decides what the caller reads. */
int glue_C2D16C(void) {
    int16_t x = W(0), y = W(1), z = W(2), shift = W(7), radius = W(6);
    uint16_t scale = (uint16_t)D(3);
    int8_t kind = (int8_t)D(4);
    gaddr offsets = kind <= 2 ? 0xC2CF6Eu : kind == 3 ? 0xC2CFE3u : kind == 4 ? 0xC2CFBCu : 0xC2CF95u;
    uint16_t set, colour;
    int drawn, inside = 1;
    int16_t pz;
    gaddr points;

    drawn = draw_shape(x, y, z, scale, kind, radius, shift);
    colour = kind ? rd_u8((kind <= 1 ? 0xC2D001u : kind == 2 ? 0xC2D00Du : kind == 3 ? 0xC2D019u
                           : kind == 4 ? 0xC2D025u : 0xC2D031u)) : 13;
    D(0) = SEXT((uint16_t)(int16_t)((int16_t)((int8_t)rd_u8(offsets) * scale) >> (shift & 63)));
    D(1) = SEXT((uint16_t)(int16_t)((int16_t)((int8_t)rd_u8(offsets + 1) * scale) >> (shift & 63)));
    D(2) = SEXT((uint16_t)(int16_t)((int16_t)((int8_t)rd_u8(offsets + 2) * scale) >> (shift & 63)));
    A(4) = SEXT((uint16_t)(W(0) + x));
    A(5) = SEXT((uint16_t)(W(1) + y));
    pz = (int16_t)(W(2) + z);
    A(3) = 0xC2D03Eu;
    set = rd_u16(0xC2D03Eu + (gaddr)(2 * scale));
    points = rd_u32(0xC2CEBEu + set);
    A(1) = POLY_VERTICES + 2;
    do {
        if (kind == 3) {
            D(3) = 0;
            D(4) = 0;
            D(5) = 0;
        } else {
            D(3) = SEXT(rd_u16(points));
            D(4) = SEXT(rd_u16(points + 2));
            D(5) = SEXT(rd_u16(points + 4));
            points += 6;
        }
        if (!shape_point_registers(pz, shift)) {
            inside = 0;
            break;
        }
    } while (kind != 3 && A(1) < POLY_VERTICES + 14);
    A(2) = points;
    if (inside) {
        uint16_t now = rd_u16(CURRENT_COLOUR);
        wr_u16(CURRENT_COLOUR, colour);
        SET_B(D(0), (uint8_t)kind);
        if (kind == 0) {
            int k;
            for (k = 0; k < 4; k++) D(k) = SEXT(rd_u16(POLY_VERTICES + 2 + (gaddr)(2 * k)));
            line_registers();
        } else if (kind == 3) {
            D(0) = SEXT(rd_u16(POLY_VERTICES + 2));
            D(1) = SEXT(rd_u16(POLY_VERTICES + 4));
            SET_W(D(6), (uint16_t)radius);
            filled_circle_registers();
        } else {
            SET_B(D(0), (uint8_t)(kind - 3));
            draw_polygon_registers(fa18_bltsize_at_draw_start, colour);
        }
        wr_u16(CURRENT_COLOUR, now);
    }
    SET_W(D(0), (uint16_t)drawn);
    return glue_return();
}

/* $C21500: A2 the stream (advanced by 4), A3 the block. */
int glue_C21500(void) {
    gaddr stream = A(2), block = WORKSPACES + SEXT(rd_u16(A(2) + 2)), in = CLIP_INPUT + 4;
    uint32_t a1 = A(1), a2 = A(2) + 4, a5 = A(5);
    uint16_t colour = rd_u16(A(2));
    int k, drawn;

    drawn = draw_block_face(&stream);
    A(2) = a2;
    A(3) = block;
    if (rd_s32(PROJECTION_Y) < -0x80) {
        D(0) = 0;
        flags_logic_l(0);
        return glue_return();
    }
    A(0) = in + 18;
    for (k = 0; k < 6; k++) D(2 + k) = SEXT(rd_u16(block + (gaddr)(2 * k)));
    for (k = 0; k < 3; k++) SET_W(D(5 + k), (uint16_t)(W(5 + k) - W(2 + k)));
    A(1) = SEXT((uint16_t)D(5));
    A(4) = SEXT((uint16_t)D(6));
    A(5) = SEXT((uint16_t)D(7));
    for (k = 0; k < 3; k++) D(k) = SEXT(rd_u16(in + 6 + (gaddr)(2 * k)));
    A(2) = SEXT(rd_u16(in + 10));
    for (k = 0; k < 6; k++) D(2 + k) = SEXT(rd_u16(block + 6 + (gaddr)(2 * k)));
    for (k = 0; k < 3; k++) SET_W(D(5 + k), (uint16_t)(W(5 + k) - W(2 + k)));
    SET_W(D(2), (uint16_t)A(2));
    for (k = 0; k < 3; k++) SET_W(D(k), rd_u16(in + 12 + (gaddr)(2 * k)));
    SET_W(D(0), (uint16_t)(W(0) - (int16_t)A(1)));
    SET_W(D(1), (uint16_t)(W(1) - (int16_t)A(4)));
    SET_W(D(2), (uint16_t)(W(2) - (int16_t)A(5)));
    SET_W(D(7), rd_u16(in + 4) & rd_u16(in + 10) & rd_u16(in + 16) & rd_u16(in + 22));
    if (W(7) < 0) {
        A(1) = a1;
        A(2) = a2;
        A(5) = a5;
        D(0) = 0;
        flags_logic_l(0);
        return glue_return();
    }
    {
        ClipperSnapshot snapshot;
        clipper_snapshot(&snapshot);
        snapshot.last_size = fa18_bltsize_at_draw_start;
        clipper_registers(&snapshot, colour, drawn);
    }
    A(1) = a1;
    A(2) = a2;
    A(5) = a5;
    return glue_return();
}

void clipped_segment_registers(const int16_t p[3], const int16_t q[3]); /* glue_batch51.c */

/* $C2122A: its count and result words live in the caller's frame; the
 * registers are the last segment's, with the edge words popped back
 * sign-extended into D6, D7 and A4. */
int glue_C2122A(void) {
    gaddr stream = A(2), base = WORKSPACES + SEXT(rd_u16(A(2) + 2)), points;
    uint32_t a1 = A(1), a2 = A(2) + 6, a5 = A(5);
    int16_t count = (int16_t)(rd_u16(A(2)) >> 8), shift[3], p[3], q[3];
    int drawn, k;

    points = WORKSPACES + SEXT(rd_u16(A(2) + 4));
    for (k = 0; k < 3; k++) shift[k] = (int16_t)(rd_s16(base + 6 + (gaddr)(2 * k)) - rd_s16(base + (gaddr)(2 * k)));
    drawn = draw_offset_run(&stream);
    if (count > 1) points += (gaddr)(12 * (count - 1));
    for (k = 0; k < 3; k++) {
        p[k] = (int16_t)(rd_s16(points + (gaddr)(2 * k)) - shift[k]);
        q[k] = (int16_t)(rd_s16(points + 6 + (gaddr)(2 * k)) - shift[k]);
    }
    wr_u16(A(6) - 0x30, (uint16_t)(count > 1 ? 0 : count - 1));
    wr_u16(A(6) - 0x7E, (uint16_t)drawn);
    /* The registers at the last call: the points as loaded less the edge. */
    for (k = 0; k < 6; k++) {
        int16_t loaded = rd_s16(points + (gaddr)(2 * k));
        D(k) = (SEXT((uint16_t)loaded) & 0xFFFF0000u) | (uint16_t)(k < 3 ? p[k] : q[k - 3]);
    }
    D(6) = (SEXT(rd_u16(base + 8)) & 0xFFFF0000u) | (uint16_t)shift[1];
    D(7) = (SEXT(rd_u16(base + 10)) & 0xFFFF0000u) | (uint16_t)shift[2];
    A(4) = SEXT((uint16_t)shift[0]);
    A(3) = points + 12;
    clipped_segment_registers(p, q);
    D(6) = SEXT((uint16_t)shift[1]);
    D(7) = SEXT((uint16_t)shift[2]);
    A(4) = SEXT((uint16_t)shift[0]);
    A(3) = points + 12;
    A(1) = a1;
    A(2) = a2;
    A(5) = a5;
    SET_W(D(0), (uint16_t)drawn);
    flags_logic_w(D(0));
    return glue_return();
}

/* $C20592: its steps' registers, then the clipper's. */
int glue_C20592(void) {
    gaddr stream = A(2);
    uint32_t a1 = A(1), a5 = A(5);
    int16_t shift = rd_s16(BOUND_SHIFT);
    int32_t sx, sz;
    int drawn;

    D(0) = rd_u32(A(2));
    drawn = draw_split_square(&stream);
    A(2) += 4;
    A(3) = WORKSPACES;
    D(1) = SEXT(rd_u16(WORKSPACES + 6));
    D(2) = SEXT(rd_u16(WORKSPACES + 8));
    D(3) = SEXT(rd_u16(WORKSPACES + 10));
    if (W(1) > W(3)) goto outside;
    SET_W(D(1), (uint16_t)-W(1));
    if (W(1) > W(3) || W(2) > W(3)) goto outside;
    SET_W(D(2), (uint16_t)-W(2));
    if (W(2) > W(3)) goto outside;
    A(0) = CLIP_INPUT + 4;
    SET_W(D(6), (uint16_t)-rd_s16(PROJECTION_WORDS));
    SET_W(D(7), (uint16_t)-rd_s16(PROJECTION_WORDS + 4));
    SET_W(D(2), rd_u16(BOUND_OFFSET_X));
    SET_W(D(3), rd_u16(BOUND_OFFSET_Z));
    SET_W(D(5), (uint16_t)shift);
    SET_W(D(2), (shift & 63) >= 16 ? 0 : (uint16_t)((uint16_t)W(2) << (shift & 63)));
    SET_W(D(3), (shift & 63) >= 16 ? 0 : (uint16_t)((uint16_t)W(3) << (shift & 63)));
    sx = (int32_t)W(6) - W(2);
    SET_W(D(6), (uint16_t)sx);
    sz = (int32_t)W(7) - W(3);
    SET_W(D(7), (uint16_t)sz);
    if (sx < 0) SET_W(D(6), (uint16_t)-W(6));
    if (sz < 0) SET_W(D(7), (uint16_t)-W(7));
    if (!(W(7) > W(6))) D(0) = D(0) << 16 | D(0) >> 16;
    SET_W(D(7), (uint16_t)D(0));
    A(0) = CLIP_INPUT + 4 + 18;
    {
        ClipperSnapshot snapshot;
        clipper_snapshot(&snapshot);
        snapshot.last_size = fa18_bltsize_at_draw_start;
        clipper_registers(&snapshot, rd_u16(CURRENT_COLOUR), drawn);
    }
    A(1) = a1;
    A(2) = stream;
    A(5) = a5;
    return glue_return();
outside:
    D(0) = 0xFFFFFFFFu;
    flags_logic_l(D(0));
    return glue_return();
}

static void load_words(int first, gaddr at) {
    int k;
    for (k = 0; k < 6; k++) D(first + k) = SEXT(rd_u16(at + (gaddr)(2 * k)));
}

/* $C2168A: its steps' registers in order, then the clipper's. */
int glue_C2168A(void) {
    gaddr stream = A(2), bound = rd_u32(BOUND_RECORD), in = CLIP_INPUT + 4;
    uint32_t a1 = A(1), a5 = A(5);
    int16_t off1 = rd_s16(A(2) + 2), a = rd_s16(A(2) + 4), b = rd_s16(A(2) + 6), c = rd_s16(A(2) + 8);
    int16_t blk = rd_s16(A(2) + 10), grid = rd_s16(BOUND_SHIFT);
    int drawn, first_path, k;

    drawn = draw_side_triangle(&stream);
    A(3) = WORKSPACES;
    A(0) = in;
    SET_W(D(0), (uint16_t)off1);
    for (k = 0; k < 6; k++) D(2 + k) = SEXT(rd_u16(WORKSPACES + SEXT((uint16_t)off1) + (gaddr)(2 * k)));
    for (k = 0; k < 3; k++) SET_W(D(5 + k), (uint16_t)(W(5 + k) - W(2 + k)));
    D(6) = D(6) << 16 | D(6) >> 16;
    D(7) = D(7) << 16 | D(7) >> 16;
    A(4) = bound;
    SET_W(D(0), (uint16_t)a);
    SET_W(D(1), rd_u16(bound + 0xA + SEXT((uint16_t)a)));
    SET_W(D(2), rd_u16(bound + 0xE + SEXT((uint16_t)a)));
    SET_W(D(3), (uint16_t)D(1));
    SET_W(D(4), (uint16_t)D(2));
    SET_W(D(0), (uint16_t)b);
    SET_W(D(6), (uint16_t)b);
    D(0) &= ~0x8000u;
    SET_W(D(1), (uint16_t)(W(1) - rd_s16(bound + 0xA + SEXT(D(0)))));
    SET_W(D(2), (uint16_t)(W(2) - rd_s16(bound + 0xE + SEXT(D(0)))));
    SET_B(D(7), rd_u8(bound + 6));
    SET_W(D(7), W(7) & 15);
    SET_W(D(3), (uint16_t)(W(3) >> W(7)));
    SET_W(D(4), (uint16_t)(W(4) >> W(7)));
    SET_W(D(3), (uint16_t)(W(3) + rd_s16(BOUND_OFFSET_X)));
    SET_W(D(4), (uint16_t)(W(4) + rd_s16(BOUND_OFFSET_Z)));
    SET_W(D(7), (uint16_t)grid);
    SET_W(D(3), (grid & 63) >= 16 ? 0 : (uint16_t)((uint16_t)W(3) << (grid & 63)));
    SET_W(D(4), (grid & 63) >= 16 ? 0 : (uint16_t)((uint16_t)W(4) << (grid & 63)));
    SET_W(D(3), (uint16_t)(W(3) + rd_s16(PROJECTION_WORDS)));
    SET_W(D(4), (uint16_t)(W(4) + rd_s16(PROJECTION_WORDS + 4)));
    D(3) = (uint32_t)((int32_t)W(3) * W(1));
    D(4) = (uint32_t)((int32_t)W(4) * W(2));
    first_path = ((int64_t)(int32_t)D(3) + (int32_t)D(4) >= 0) != (b < 0);
    D(4) += D(3);
    D(6) = D(6) << 16 | D(6) >> 16;
    D(7) = D(7) << 16 | D(7) >> 16;
    SET_W(D(0), (uint16_t)c);
    for (k = 0; k < 3; k++) D(1 + k) = SEXT(rd_u16(WORKSPACES + SEXT((uint16_t)c) + (gaddr)(2 * k)));
    for (k = 0; k < 3; k++) SET_W(D(1 + k), (uint16_t)(W(1 + k) - W(5 + k)));
    A(0) = in + 6;
    A(3) = WORKSPACES + SEXT((uint16_t)blk);
    load_words(2, A(3));
    for (k = 0; k < 3; k++) SET_W(D(5 + k), (uint16_t)(W(5 + k) - W(2 + k)));
    if (first_path) {
        for (k = 0; k < 3; k++) D(k) = SEXT(rd_u16(A(3) + 0x12 + (gaddr)(2 * k)));
        A(0) += 6;
        for (k = 0; k < 3; k++) SET_W(D(k), (uint16_t)(W(k) + W(5 + k)));
        A(2) = SEXT((uint16_t)D(2));
        load_words(2, A(3) + 6);
        for (k = 0; k < 3; k++) SET_W(D(5 + k), (uint16_t)(W(5 + k) - W(2 + k)));
        SET_W(D(2), (uint16_t)A(2));
        for (k = 0; k < 3; k++) SET_W(D(k), (uint16_t)(W(k) + W(5 + k)));
    } else {
        A(1) = SEXT((uint16_t)D(5));
        A(4) = SEXT((uint16_t)D(6));
        A(5) = SEXT((uint16_t)D(7));
        for (k = 0; k < 3; k++) D(k) = SEXT(rd_u16(A(3) + 0x12 + (gaddr)(2 * k)));
        for (k = 0; k < 3; k++) SET_W(D(k), (uint16_t)(W(k) + W(5 + k)));
        A(0) += 6;
        A(2) = SEXT((uint16_t)D(2));
        load_words(2, A(3) + 6);
        for (k = 0; k < 3; k++) SET_W(D(5 + k), (uint16_t)(W(5 + k) - W(2 + k)));
        SET_W(D(2), (uint16_t)A(2));
        for (k = 0; k < 3; k++) SET_W(D(k), (uint16_t)(W(k) + W(5 + k)));
        SET_W(D(0), (uint16_t)(W(0) - (int16_t)A(1)));
        SET_W(D(1), (uint16_t)(W(1) - (int16_t)A(4)));
        SET_W(D(2), (uint16_t)(W(2) - (int16_t)A(5)));
    }
    SET_W(D(2), (uint16_t)(W(2) & rd_s16(in + 10) & rd_s16(in + 4)));
    if (W(2) < 0) {
        A(1) = a1;
        A(2) = stream;
        A(5) = a5;
        D(0) = 0;
        flags_logic_l(0);
        return glue_return();
    }
    {
        ClipperSnapshot snapshot;
        clipper_snapshot(&snapshot);
        snapshot.last_size = fa18_bltsize_at_draw_start;
        clipper_registers(&snapshot, rd_u16(CURRENT_COLOUR), drawn);
    }
    A(1) = a1;
    A(2) = stream;
    A(5) = a5;
    return glue_return();
}

/* $C203D0: its frame words, then the last clipper call's registers. */
int glue_C203D0(void) {
    gaddr stream = A(2), in = CLIP_INPUT + 4;
    uint32_t a1 = A(1), a5 = A(5), colours = rd_u32(A(2));
    uint16_t second = rd_u16(A(2) + 4), colour;
    int drawn, same, both = rd_u8(ATTITUDE_LATCH) != 0, k;
    int16_t v[12];

    for (k = 0; k < 12; k++) v[k] = rd_s16(WORKSPACES + (gaddr)(2 * k));
    same = square_diagonal(colours, &colour);
    drawn = draw_square_faces(&stream);
    wr_u16(A(6) - 0x7E, 0);
    wr_u16(A(6) - 0x5E, second);
    D(0) = colours;
    A(2) = stream;
    A(3) = WORKSPACES;
    if (drawn < 0) {
        D(1) = SEXT((uint16_t)v[3]);
        D(2) = SEXT((uint16_t)v[4]);
        D(3) = SEXT((uint16_t)v[5]);
        if (W(1) <= W(3)) {
            SET_W(D(1), (uint16_t)-W(1));
            if (W(1) <= W(3) && W(2) <= W(3)) SET_W(D(2), (uint16_t)-W(2));
        }
        D(0) = 0xFFFFFFFFu;
        flags_logic_l(D(0));
        return glue_return();
    }
    wr_u16(A(6) - 0x7E, (uint16_t)drawn);
    /* The registers at the last clipper call. */
    A(0) = in;
    for (k = 0; k < 9; k++) {
        int r = k < 8 ? k : -1;
        if (r >= 0) D(r) = SEXT((uint16_t)v[k]);
    }
    A(4) = SEXT((uint16_t)v[8]);
    if (same) {
        if (!both) {
            for (k = 0; k < 3; k++) SET_W(D(k), (uint16_t)(v[k] - v[3 + k]));
            SET_W(D(6), (uint16_t)(v[6] - v[3]));
            SET_W(D(7), (uint16_t)(v[7] - v[4]));
            A(4) = SEXT((uint16_t)(v[8] - v[5]));
            for (k = 0; k < 3; k++) SET_W(D(3 + k), rd_u16(in + 18 + (gaddr)(2 * k)));
            A(3) = WORKSPACES + 18;
        } else {
            SET_W(D(6), (uint16_t)(v[6] - v[3]));
            SET_W(D(7), (uint16_t)(v[7] - v[4]));
            A(4) = SEXT((uint16_t)(v[8] - v[5]));
            for (k = 0; k < 3; k++) SET_W(D(k), rd_u16(in + 6 + (gaddr)(2 * k)));
            for (k = 0; k < 3; k++) SET_W(D(k), (uint16_t)(W(k) - rd_s16(in + 12 + (gaddr)(2 * k))));
            for (k = 0; k < 3; k++) SET_W(D(3 + k), rd_u16(in + 18 + (gaddr)(2 * k)));
        }
    } else {
        if (!both) {
            for (k = 0; k < 3; k++) SET_W(D(k), (uint16_t)(v[k] - v[3 + k]));
            for (k = 0; k < 3; k++) SET_W(D(3 + k), rd_u16(in + 12 + (gaddr)(2 * k)));
            for (k = 0; k < 2; k++) SET_W(D(6 + k), rd_u16(in + 18 + (gaddr)(2 * k)));
            A(4) = SEXT(rd_u16(in + 22));
            A(3) = WORKSPACES + 18;
        } else {
            for (k = 0; k < 3; k++) D(k) = SEXT((uint16_t)v[k]);
            for (k = 0; k < 3; k++) D(3 + k) = SEXT(rd_u16(in + 18 + (gaddr)(2 * k)));
            for (k = 0; k < 3; k++) SET_W(D(3 + k), (uint16_t)(W(3 + k) - W(k)));
            for (k = 0; k < 2; k++) SET_W(D(6 + k), rd_u16(in + 12 + (gaddr)(2 * k)));
            A(4) = SEXT(rd_u16(in + 16));
        }
    }
    {
        ClipperSnapshot snapshot;
        clipper_snapshot(&snapshot);
        snapshot.last_size = fa18_bltsize_at_draw_start;
        clipper_registers(&snapshot, both ? second : colour, -1);
    }
    A(1) = a1;
    A(2) = stream;
    A(5) = a5;
    SET_W(D(0), (uint16_t)drawn);
    flags_logic_w(D(0));
    return glue_return();
}

/* $C201A6: the preamble's registers, then each face's (the turn test, or
 * the placed corners and the clipper's) in order. */
int glue_C201A6(void) {
    gaddr frame = A(6), stream = A(2), record = CONTROL_RECORDS + SEXT(rd_u16(SCRIPT_RECORD)), hull = record + 0xA4;
    uint32_t a1 = A(1), a5 = A(5);
    int result, k;

    result = draw_record_shadow(&stream, frame);
    D(0) = rd_u32(POSITION_BIAS);
    A(4) = record;
    SET_B(D(3), rd_u8(record + 4));
    SET_B(D(7), (uint8_t)D(3) & 0xC0);
    if ((uint8_t)D(7) && (uint8_t)D(7) != 0xC0) goto one;
    SET_B(D(3), (uint8_t)D(3) & 0x40);
    if ((uint8_t)D(3)) {
        SET_W(D(7), (uint16_t)-rd_s16(record + 0x4E));
        D(7) = SEXT(D(7)) << 8;
        if ((int32_t)D(0) >= (int32_t)D(7)) goto one;
        D(0) -= D(7);
        SET_W(D(3), rd_u16(BOUND_SHIFT));
        D(0) = (uint32_t)((int32_t)D(0) >> (W(3) & 63));
    }
    if ((int32_t)D(0) < -0x100000) {
        D(0) = 0xFFFFFFFFu;
        flags_logic_l(D(0));
        return glue_return();
    }
    if (!rd_u8(CONTEXT_SELECT) && (int8_t)rd_u8(ATTITUDE_BAND) < 3) {
        SET_W(D(7), rd_u16(STREAM_MODE));
        if (W(7) == rd_s16(TARGET_RECORD)) {
            if (!rd_u8(ATTITUDE_BAND)) goto one;
            D(2) = 0xFFFFE000u;
            if ((int32_t)D(0) < (int32_t)D(2)) goto one;
        } else if (!(rd_u8(record + 0x62) == 0x30 || (int32_t)D(0) >= -0x10000 || rd_u8(record + 0x62) == 0x14)) {
            if ((int8_t)rd_u8(ATTITUDE_BAND) <= 1) goto one;
            D(2) = 0xFFF60000u;
            if ((int32_t)D(0) < (int32_t)D(2)) goto one;
        }
    }
    /* The draw: A2 from the start of the stream. */
    D(0) = (uint32_t)((int32_t)rd_s16(A(2)) << 8);
    A(2) += 2;
    if ((int32_t)D(0) < rd_s32(record + 0x18)) goto restored_one;
    A(4) = hull;
    D(2) = D(0);
    D(0) = 0u - D(0);
    D(0) = D(0) << 16 | D(0) >> 16;
    D(0) = SEXT(D(0));
    D(0) = (uint32_t)((int32_t)D(0) >> 5);
    SET_W(D(0), (uint16_t)(W(0) - rd_s16(BOUND_SHIFT)));
    if (W(0) <= 0) {
        SET_W(D(0), 0);
    } else {
        D(1) = rd_u32(frame - 0x20);
        D(3) = rd_u32(frame - 0x18);
        SET_W(D(0), rd_u16(frame - 2));
        D(2) = (uint32_t)((int32_t)D(2) >> (W(0) & 63));
    }
    for (k = 0; k < 3; k++) D(1 + k) = rd_u32(frame - 0x94 + (gaddr)(4 * k));
    D(4) = (uint32_t)(rd_s32(SHADOW_OFFSET_X) >> (W(0) & 63));
    D(5) = (uint32_t)(rd_s32(SHADOW_OFFSET_Z) >> (W(0) & 63));
    D(1) += D(4);
    D(3) += D(5);
    D(7) = 8;
    SET_W(D(7), (uint16_t)(8 - rd_s16(frame - 6)));
    for (k = 1; k <= 3; k++) D(k) = (uint32_t)((int32_t)D(k) >> (W(7) & 63));
    for (;;) {
        gaddr face = A(2);
        int16_t count = rd_s16(face);
        int visible = 1;
        if (rd_s16(face + 2) == 0) {
            for (k = 0; k < 3; k++) SET_W(D(1 + k), rd_u16(face + 4 + (gaddr)(2 * k)));
            SET_W(D(0), rd_u16(hull + SEXT(D(1))));
            SET_W(D(5), (uint16_t)(rd_s16(hull + SEXT(D(2))) - W(0)));
            SET_W(D(6), (uint16_t)(rd_s16(hull + SEXT(D(3))) - W(0)));
            SET_W(D(4), rd_u16(hull + 4 + SEXT(D(1))));
            SET_W(D(1), (uint16_t)(rd_s16(hull + 4 + SEXT(D(2))) - W(4)));
            SET_W(D(3), (uint16_t)(rd_s16(hull + 4 + SEXT(D(3))) - W(4)));
            D(6) = (uint32_t)((int32_t)W(6) * W(1));
            D(3) = (uint32_t)((int32_t)W(5) * W(3));
            D(6) -= D(3);
            D(6) = (uint32_t)((int32_t)D(6) >> 8);
            visible = (int32_t)D(6) >= 0;
        }
        if (!visible) {
            SET_W(D(7), (uint16_t)count);
            A(2) = face + 2;
            SET_W(D(7), (uint16_t)(W(7) * 2 + 2));
            A(2) += SEXT(D(7));
        } else {
            gaddr m = VIEW_ANGLE_MATRIX;
            SET_W(D(7), (uint16_t)count);
            A(2) = face + 4;
            A(0) = CLIP_INPUT + 4;
            for (k = 0; k < count || k == 0; k++) {
                int16_t off = rd_s16(A(2));
                A(2) += 2;
                SET_W(D(7), (uint16_t)off);
                SET_W(D(5), rd_u16(frame - 8));
                SET_W(D(2), (uint16_t)(rd_s16(hull + SEXT((uint16_t)off)) >> (W(5) & 63)));
                SET_W(D(4), (uint16_t)(rd_s16(hull + 4 + SEXT((uint16_t)off)) >> (W(5) & 63)));
                SET_W(D(2), (uint16_t)(W(2) + rd_s16(frame - 0x14)));
                SET_W(D(3), rd_u16(frame - 0x12));
                SET_W(D(4), (uint16_t)(W(4) + rd_s16(frame - 0x10)));
                A(5) = m;
                SET_W(D(5), (uint16_t)D(2));
                SET_W(D(6), (uint16_t)D(3));
                SET_W(D(7), (uint16_t)D(4));
                D(5) = (uint32_t)((int32_t)W(5) * rd_s16(m));
                D(6) = (uint32_t)((int32_t)W(6) * rd_s16(m + 2));
                D(7) = (uint32_t)((int32_t)W(7) * rd_s16(m + 4));
                D(7) = (uint32_t)((int32_t)(D(7) + D(6) + D(5)) >> 8);
                SET_W(D(5), (uint16_t)D(2));
                SET_W(D(6), (uint16_t)D(3));
                SET_W(D(7), (uint16_t)D(4));
                D(5) = (uint32_t)((int32_t)W(5) * rd_s16(m + 6));
                D(6) = (uint32_t)((int32_t)W(6) * rd_s16(m + 8));
                D(7) = (uint32_t)((int32_t)W(7) * rd_s16(m + 10));
                D(7) = (uint32_t)((int32_t)(D(7) + D(6) + D(5)) >> 8);
                D(2) = (uint32_t)((int32_t)W(2) * rd_s16(m + 12));
                D(3) = (uint32_t)((int32_t)W(3) * rd_s16(m + 14));
                D(4) = (uint32_t)((int32_t)W(4) * rd_s16(m + 16));
                D(4) = (uint32_t)((int32_t)(D(4) + D(3) + D(2)) >> 8);
                A(5) = m + 18;
                A(0) += 6;
                if (count - 1 - k <= 0) break;
            }
            {
                ClipperSnapshot snapshot;
                uint32_t a2 = A(2), a4 = A(4);
                clipper_snapshot(&snapshot);
                snapshot.last_size = fa18_bltsize_at_draw_start;
                clipper_registers(&snapshot, 0, -1);
                A(2) = a2;
                A(4) = a4;
            }
        }
        D(0) = SEXT(rd_u16(A(2)));
        A(2) += 2;
        if (W(0) < 0) break;
        A(5) = record + 0x10;
        if ((int32_t)D(0) < rd_s32(record + 0x10)) break;
    }
restored_one:
    A(1) = a1;
    A(5) = a5;
one:
    (void)result;
    D(0) = 1;
    flags_logic_l(1);
    return glue_return();
}

/* $C3019C: the scaling's registers, then its callees' glue in order (the
 * fill, and when it was started the lane blit and the mask clear). */
int glue_C3019C(void) {
    int16_t count = rd_s16(0xC4B432u);

    scale_mark_polygon();
    A(0) = 0xC4B432u + 2;
    A(4) = POLY_VERTICES;
    SET_W(D(7), (uint16_t)count);
    if (count <= 0) return glue_return();
    A(4) += 2 + (gaddr)(4 * count);
    A(0) += (gaddr)(4 * count);
    SET_W(D(0), rd_u16(POLY_VERTICES + (gaddr)(4 * count)));
    SET_W(D(7), 0xFFFF);
    call_port(glue_C301F0, 0xC301E0);
    if (D(0)) return glue_return();
    D(0) = 4;
    D(3) = 1;
    call_port(glue_C304FA, 0xC301EA);
    call_port(glue_C304B2, 0xC301EE);
    return glue_return();
}

/* $C1FB82: face kind bits 10-11 pick the bound component test ($C1FC3A,
 * the face's word before last as the offset, into $C1FC42); otherwise the
 * face test $C1FB8C. Both are recreated; this only dispatches. */
int glue_C1FB82(void) {
    SET_W(D(1), (uint16_t)D(7) & 0xC00);
    if (W(1)) {
        SET_W(D(0), rd_u16(A(2) - 4) & 0x3FFF);
        return glue_C1FC42();
    }
    return glue_C1FB8C();
}

/* One rotation's registers as $C1F99A leaves them: the first two rows in
 * D5-D7 (the middle row's products and sum), the last in D2-D4. */
static void rotation_registers(gaddr m, int16_t x, int16_t y, int16_t z) {
    D(5) = (uint32_t)((int32_t)x * rd_s16(m + 6));
    D(6) = (uint32_t)((int32_t)y * rd_s16(m + 8));
    D(7) = (uint32_t)((int32_t)z * rd_s16(m + 10));
    D(7) = (uint32_t)((int32_t)(D(7) + D(6) + D(5)) >> 8);
    D(2) = (uint32_t)((int32_t)x * rd_s16(m + 12));
    D(3) = (uint32_t)((int32_t)y * rd_s16(m + 14));
    D(4) = (uint32_t)((int32_t)z * rd_s16(m + 16));
    D(4) = (uint32_t)((int32_t)(D(4) + D(3) + D(2)) >> 8);
}

static int16_t dot_row(gaddr row, int16_t x, int16_t y, int16_t z) {
    return (int16_t)((int32_t)((uint32_t)((int32_t)x * rd_s16(row)) + (uint32_t)((int32_t)y * rd_s16(row + 2)) +
                               (uint32_t)((int32_t)z * rd_s16(row + 4))) >> 8);
}

int glue_C1F99A(void) {
    gaddr frame = A(6), bound = rd_u32(BOUND_RECORD);
    int16_t count = W(0), first = W(7), shift = rd_s16(frame - 8);
    int down = (8 - rd_s16(frame - 6)) & 63, s = shift & 63, n = count > 1 ? count : 1;
    uint8_t mode = rd_u8(bound + 7);
    int32_t x = rd_s32(frame - 0x20) + rd_s32(SHADOW_OFFSET_X), y = rd_s32(frame - 0x1C);
    int32_t z = rd_s32(frame - 0x18) + rd_s32(SHADOW_OFFSET_Z);
    uint32_t a1 = A(1), a2 = A(2), a5 = A(5);
    gaddr last = bound + 0xA + SEXT((uint16_t)first);

    transform_bound_points(count, first, frame);
    wr_u16(frame - 0xA, (uint16_t)(count > 1 ? 0 : count - 1));
    if (mode & 1) {
        y += rd_s32(SHADOW_OFFSET_Y);
        x >>= down;
        y >>= down;
        z >>= down;
        last += (gaddr)(6 * (n - 1));
        A(4) = BOUND_MATRIX;
        A(3) = WORKSPACES + SEXT((uint16_t)first) + (gaddr)(6 * n);
        if (rd_u8(frame - 0x7F) & 1) {
            int16_t v[3];
            int k;
            for (k = 0; k < 3; k++) v[k] = rd_s16(last + (gaddr)(2 * k));
            v[1] = (int16_t)(v[1] - 0x28);
            v[2] = (int16_t)(v[2] - 0xA3);
            for (k = 0; k < 3; k++) v[k] = (int16_t)(v[k] >> s);
            rotation_registers(CAMERA_MATRIX, v[0], v[1], v[2]);
        } else {
            int16_t px = (int16_t)(rd_s16(last) >> s), py = (int16_t)(rd_s16(last + 2) >> s);
            int16_t pz = (int16_t)(rd_s16(last + 4) >> s);
            int16_t qx = (int16_t)(dot_row(BOUND_MATRIX, px, py, pz) + (int16_t)x);
            int16_t qy = (int16_t)(dot_row(BOUND_MATRIX + 6, px, py, pz) + (int16_t)y);
            int16_t qz = (int16_t)(dot_row(BOUND_MATRIX + 12, px, py, pz) + (int16_t)z);
            rotation_registers(VIEW_ANGLE_MATRIX, qx, qy, qz);
            A(0) = SEXT((uint16_t)y);
        }
        D(0) = (uint32_t)x;
        D(1) = (uint32_t)z;
        A(1) = a1;
        A(2) = a2;
        A(5) = a5;
        return glue_return();
    }
    x >>= down;
    y >>= down;
    z >>= down;
    A(4) = VIEW_ANGLE_MATRIX;
    if (!(mode & 2)) {
        int16_t px, py, pz;
        last += (gaddr)(6 * (n - 1));
        px = (int16_t)((rd_s16(last) >> s) + (int16_t)x);
        py = (int16_t)((rd_s16(last + 2) >> s) + (int16_t)y);
        pz = (int16_t)((rd_s16(last + 4) >> s) + (int16_t)z);
        rotation_registers(VIEW_ANGLE_MATRIX, px, py, pz);
        A(0) = VIEW_ANGLE_MATRIX + 18;
        A(3) = WORKSPACES + SEXT((uint16_t)first) + (gaddr)(6 * n);
    } else {
        gaddr m = VIEW_ANGLE_MATRIX;
        int16_t px, pz;
        int16_t spread = (int16_t)(first + (first >> 1));
        last += (gaddr)(4 * (n - 1));
        px = (int16_t)((rd_s16(last) >> s) + (int16_t)x);
        pz = (int16_t)((rd_s16(last + 2) >> s) + (int16_t)z);
        D(3) = SEXT(rd_u16(frame - 0x78));
        D(6) = SEXT(rd_u16(frame - 0x76));
        D(5) = (uint32_t)((int32_t)px * rd_s16(m + 6));
        D(7) = (uint32_t)((int32_t)((uint32_t)((int32_t)pz * rd_s16(m + 10)) + D(5)) >> 8);
        SET_W(D(7), (uint16_t)(W(7) + W(6)));
        D(2) = (uint32_t)((int32_t)px * rd_s16(m + 12));
        D(4) = (uint32_t)((int32_t)((uint32_t)((int32_t)pz * rd_s16(m + 16)) + D(2)) >> 8);
        SET_W(D(4), (uint16_t)(W(4) + rd_s16(frame - 0x74)));
        A(0) = m + 14;
        A(3) = WORKSPACES + SEXT((uint16_t)spread) + (gaddr)(6 * n);
    }
    D(0) = (uint32_t)x;
    D(1) = (uint32_t)z;
    A(1) = a1;
    return glue_return();
}
