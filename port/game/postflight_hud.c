/* The postflight cockpit tapes and their projected target mark. */
#include "postflight_hud.h"

#include "globals.h"
#include "hud_marks.h"
#include "hud_readouts.h"
#include "memory.h"
#include "numbers.h"
#include "plot.h"
#include "projection.h"
#include "render_line.h"
#include "text.h"
#include "view_marks.h"

#define LAYOUT_ALTITUDE_VALUE 0xC33270u
#define LAYOUT_ALTITUDE_LABEL 0xC3328Cu
#define LAYOUT_SPEED_VALUE    0xC33294u
#define LAYOUT_SPEED_LABEL    0xC332A8u
#define LAYOUT_ALTITUDE_TICK  0xC33258u
#define LAYOUT_SPEED_TICK     0xC33264u
#define LAYOUT_HEADING_TICK   0xC33A16u

#define ALTITUDE_LABEL        0xC3341Au
#define SPEED_LABEL           0xC33644u
#define RANGE_RING            0xC3494Cu
#define RANGE_POINT           0xC34976u

static gaddr viewed_record(void) {
    return CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
}

/* DIVU.W leaves its dividend unchanged when its quotient will not fit in a
 * word.  The callers immediately EXT.L the result word. */
static uint32_t divu_word(uint32_t dividend, uint16_t divisor) {
    uint32_t quotient = dividend / divisor;
    if (quotient > 0xFFFFu) return dividend;
    return ((dividend % divisor) << 16) | quotient;
}

static int16_t divs_word(int32_t dividend, int16_t divisor) {
    int32_t quotient = dividend / divisor;
    return quotient == (int16_t)quotient ? (int16_t)quotient : (int16_t)dividend;
}

static int16_t word_abs(int16_t value) {
    return value < 0 ? (int16_t)-value : value;
}

/* ABCD/SBCD on the three low bytes of DISPLAY_VALUE_BCD.  ADDI.W #0 before
 * each original sequence clears X, so the first byte has no carry in. */
static uint8_t bcd_add_byte(uint8_t left, uint8_t right, int *carry) {
    int lo = (left & 15) + (right & 15) + *carry;
    int hi = (left >> 4) + (right >> 4);
    if (lo > 9) {
        lo -= 10;
        hi++;
    }
    if (hi > 9) {
        hi -= 10;
        *carry = 1;
    } else {
        *carry = 0;
    }
    return (uint8_t)((hi << 4) | lo);
}

static uint8_t bcd_sub_byte(uint8_t left, uint8_t right, int *borrow) {
    int lo = (left & 15) - (right & 15) - *borrow;
    int hi = (left >> 4) - (right >> 4);
    if (lo < 0) {
        lo += 10;
        hi--;
    }
    if (hi < 0) {
        hi += 10;
        *borrow = 1;
    } else {
        *borrow = 0;
    }
    return (uint8_t)((hi << 4) | lo);
}

static void adjust_tape_bcd(int add) {
    int carry = 0, i;
    for (i = 0; i < 3; i++) {
        gaddr destination = DISPLAY_VALUE_BCD + 3u - (gaddr)i;
        gaddr step = POSTFLIGHT_BCD_STEP - 1u - (gaddr)i;
        uint8_t value = add ? bcd_add_byte(rd_u8(destination), rd_u8(step), &carry)
                            : bcd_sub_byte(rd_u8(destination), rd_u8(step), &carry);
        wr_u8(destination, value);
    }
}

static void draw_bcd_in_view(gaddr layout, gaddr rows, int16_t origin, int digits, int keep_zeros) {
    Text line = text_line(digits, TEXT_LINE, layout, rows, origin);
    print_bcd_in_view(TEXT_LINE + (gaddr)digits, digits, keep_zeros, &line);
}

static void draw_altitude_tick(gaddr rows) {
    draw_bcd_in_view(LAYOUT_ALTITUDE_TICK, rows, 0x1A, 3, 1);
}

static void draw_speed_tick(gaddr rows) {
    draw_bcd_in_view(LAYOUT_SPEED_TICK, rows, 0xA, 3, 0);
}

/* $C33FB4: the two sweeps around HUD_CENTRE_X. */
static void draw_postflight_sweep(int16_t x, int16_t offset) {
    int16_t shifted = (int16_t)(x + rd_s16(SPAN_ORIGIN_Y));
    int16_t y, index;

    if (shifted <= 0 || shifted >= 0x13F) return;
    plot_pixel_pair((int16_t)(shifted + offset), rd_s16(HUD_CENTRE_X));

    y = rd_s16(HUD_CENTRE_X);
    for (index = 0;; index++) {
        y = (int16_t)(y - 3);
        if (y <= 0x47) break;
        if (index == 4 || index == 9) plot_pixel_pair((int16_t)(shifted + offset), y);
        else plot_pixel(shifted, y);
    }
    y = rd_s16(HUD_CENTRE_X);
    for (index = 0;; index++) {
        y = (int16_t)(y + 3);
        if (y >= 0x70) break;
        if (index == 4 || index == 9) plot_pixel_pair((int16_t)(shifted + offset), y);
        else plot_pixel(shifted, y);
    }
}

/* The byte-identical $C33AD6/$C33B06 entry bodies. */
static void draw_postflight_bound(int16_t x) {
    x = (int16_t)(x + rd_s16(SPAN_ORIGIN_Y));
    if (x >= 0x13B || (int16_t)(x + 4) <= 4) return;
    draw_line(x, (int16_t)(0x5A + rd_s16(REDRAW_STATE_WORD)), (int16_t)(x + 4),
              (int16_t)(0x5A + rd_s16(REDRAW_STATE_WORD)));
}

/* $C34066, called after the heading tape puts its centre in HUD_CENTRE_X.
 * Its first mark uses a view-relative plot even though the caller has
 * already moved its row; the following tick rows retain that exact offset. */
static void draw_heading_ticks(int16_t row) {
    int16_t x = (int16_t)(rd_s16(HUD_CENTRE_X) + rd_s16(SPAN_ORIGIN_Y));
    int16_t first_y = (int16_t)(row + rd_s16(REDRAW_STATE_WORD));
    int16_t saved_row = row;
    int index;

    if (x >= 0 && x < 0x140) {
        plot_pixel(x, first_y);
        plot_pixel(x, (int16_t)(first_y - 1));
    }

    for (index = 0;; index++) {
        x = (int16_t)(x + 10);
        if (x < 0 || x > 0x13F || x >= (int16_t)(0xB9 + rd_s16(SPAN_ORIGIN_Y))) break;
        if (index == 1 || index == 3) {
            plot_pixel(x, saved_row);
            plot_pixel(x, (int16_t)(saved_row - 1));
        } else {
            plot_pixel(x, saved_row);
        }
    }

    x = (int16_t)(rd_s16(HUD_CENTRE_X) + rd_s16(SPAN_ORIGIN_Y));
    for (index = 0;; index++) {
        x = (int16_t)(x - 10);
        if (x < 0 || x > 0x13F || x <= (int16_t)(0x85 + rd_s16(SPAN_ORIGIN_Y))) break;
        if (index == 1 || index == 3) {
            plot_pixel(x, saved_row);
            plot_pixel(x, (int16_t)(saved_row - 1));
        } else {
            plot_pixel(x, saved_row);
        }
    }
}

/* Put the selected BCD digits into the lower three bytes, preserving the
 * original high half in the same way as ANDI.W.  The returned value is the
 * decoded BCD used to place a tape's centre. */
static int32_t prepare_tape_value(int32_t value, int16_t first_limit, int16_t second_limit,
                                  int16_t middle_adjust, int16_t high_adjust) {
    uint32_t bcd, low_digits, shown;
    int16_t adjust = 0;

    wr_u32(DISPLAY_VALUE, (uint32_t)value);
    pack_display_value();
    bcd = rd_u32(DISPLAY_VALUE_BCD);
    shown = bcd & 0xFFFF00FFu;
    low_digits = bcd & 0xFFFFFF0Fu;
    if ((int16_t)shown > first_limit) {
        if ((int16_t)shown <= second_limit) {
            low_digits |= 0x50u;
            adjust = middle_adjust;
        } else {
            adjust = high_adjust;
            wr_u32(DISPLAY_VALUE, (uint32_t)(value + 100));
            pack_display_value();
            low_digits = rd_u32(DISPLAY_VALUE_BCD) & 0xFFFFFF00u;
        }
    }
    wr_u32(DISPLAY_VALUE_BCD, low_digits >> 4);
    bcd = rd_u32(DISPLAY_VALUE_BCD);
    wr_u32(DISPLAY_VALUE_BCD, shown);
    unpack_display_value();
    value = rd_s32(DISPLAY_VALUE) + adjust;
    wr_u32(DISPLAY_VALUE_BCD, bcd);
    return value;
}

static void draw_altitude_tape(int32_t altitude) {
    int16_t centre, row;
    int16_t initial_row;
    int32_t rows, initial_rows;
    uint32_t bcd;

    altitude = (int16_t)divu_word((uint32_t)altitude, 10);
    centre = divs_word(prepare_tape_value(altitude, 0x20, 0x70, -50, -100), 3);
    row = (int16_t)(centre + 0x5B + rd_s16(REDRAW_STATE_WORD));
    wr_s16(HUD_CENTRE_X, row);
    row = (int16_t)(row + 2);
    initial_row = row;
    rows = 0xE02 + (int16_t)(centre * 40);
    initial_rows = rows;
    draw_altitude_tick((gaddr)rows);
    wr_u32(POSTFLIGHT_BCD_TICK, 5);

    bcd = rd_u32(DISPLAY_VALUE_BCD);
    for (rows -= 0x258u; rows >= 0xAF0u; rows -= 0x258u) {
        row = (int16_t)(row - 15);
        plot_pixel_in_view(0xDF, row);
        adjust_tape_bcd(1);
        draw_altitude_tick((gaddr)rows);
    }
    wr_u32(DISPLAY_VALUE_BCD, bcd);
    if (!bcd) goto end;
    rows = initial_rows;
    row = initial_row;
    for (rows += 0x258u; rows <= 0x1130u; rows += 0x258u) {
        row = (int16_t)(row + 15);
        plot_pixel_in_view(0xDF, row);
        adjust_tape_bcd(0);
        draw_altitude_tick((gaddr)rows);
    }
end:
    draw_postflight_sweep(0xD4, 1);
    draw_postflight_bound(0xCE);
}

static void draw_speed_tape(gaddr record) {
    int16_t speed = 0, centre, row;
    int32_t rows, initial_rows;
    uint32_t bcd;

    if (!(rd_u8(record) & 0x80)) {
        speed = rd_s16(record + 0x6E);
        if (speed >= 0) speed = (int16_t)-speed;
    }
    speed = (int16_t)divu_word((uint32_t)(int32_t)speed, 12);
    if (rd_u8(record + 0x62) != 0x10) {
        wr_u32(DISPLAY_VALUE, (uint32_t)(int32_t)speed);
        pack_display_value();
        draw_bcd_in_view(LAYOUT_SPEED_VALUE, 0xD2Au, 0xA, 4, 0);
        {
            Text label = text_line(2, SPEED_LABEL, LAYOUT_SPEED_LABEL, 0xE44u, 0xC);
            draw_text_in_view(&label);
        }
    } else {
        centre = divs_word(prepare_tape_value(speed, 0x20, 0x70, -50, -100), 3);
        row = (int16_t)(centre + 0x5B + rd_s16(REDRAW_STATE_WORD));
        wr_s16(HUD_CENTRE_X, row);
        rows = 0xDF2 + (int16_t)(centre * 40);
        initial_rows = rows;
        draw_speed_tick((gaddr)rows);
        wr_u32(POSTFLIGHT_BCD_TICK, 5);

        bcd = rd_u32(DISPLAY_VALUE_BCD);
        for (rows -= 0x258u; rows >= 0xAF0u; rows -= 0x258u) {
            adjust_tape_bcd(1);
            draw_speed_tick((gaddr)rows);
        }
        wr_u32(DISPLAY_VALUE_BCD, bcd);
        if (bcd) {
            rows = initial_rows;
            for (rows += 0x258u; rows <= 0x1130u; rows += 0x258u) {
                adjust_tape_bcd(0);
                draw_speed_tick((gaddr)rows);
            }
        }
        draw_postflight_sweep(0x69, 0);
        draw_postflight_bound(0x6B);

        {
            int16_t x = (int16_t)(0x6E + rd_s16(SPAN_ORIGIN_Y));
            if (x > 5 && x < 0x13B) {
                int16_t y = (int16_t)(0x56 + rd_s16(REDRAW_STATE_WORD));
                plot_pixel_pair(x, y);
                plot_pixel_in_view(0x6C, 0x57);
                plot_pixel((int16_t)(0x6C + rd_s16(SPAN_ORIGIN_Y)),
                           (int16_t)(0x58 + rd_s16(REDRAW_STATE_WORD)));
                plot_pixel_in_view((int16_t)(0x6E + rd_s16(SPAN_ORIGIN_Y)),
                                   (int16_t)(0x59 + rd_s16(REDRAW_STATE_WORD)));
            }
        }
    }

    {
        int16_t x = (int16_t)(0x9F + rd_s16(SPAN_ORIGIN_Y));
        int16_t y = (int16_t)(0x3F + rd_s16(REDRAW_STATE_WORD));
        if (x >= 0 && x < 0x140) {
            plot_pixel(x, y);
            plot_pixel(x, (int16_t)(y + 1));
            plot_pixel(x, (int16_t)(y + 2));
        }
    }
}

static void draw_heading_tape(gaddr record) {
    int16_t heading, centre, offset, initial_offset;
    uint32_t bcd;

    heading = (int16_t)(rd_s16(record + 0x68) >> 3);
    centre = (int16_t)-divs_word(prepare_tape_value(heading, 0x40, 0x40, 0, -100), 5);
    wr_s16(HUD_CENTRE_X, (int16_t)(centre + 0x9F));
    offset = (int16_t)(centre << 3);
    initial_offset = offset;
    draw_three_digits(LAYOUT_HEADING_TICK + (gaddr)(int32_t)offset, 0x858u);
    wr_u32(POSTFLIGHT_BCD_TICK, 10);

    bcd = rd_u32(DISPLAY_VALUE_BCD);
    for (;;) {
        offset = (int16_t)(offset + 0xA0);
        if (offset > 0xB0) break;
        adjust_tape_bcd(1);
        if (rd_s32(DISPLAY_VALUE_BCD) >= 0x360) wr_u32(DISPLAY_VALUE_BCD, 0);
        draw_three_digits(LAYOUT_HEADING_TICK + (gaddr)(int32_t)offset, 0x858u);
    }
    wr_u32(DISPLAY_VALUE_BCD, bcd);
    offset = initial_offset;
    if (rd_s32(DISPLAY_VALUE_BCD) <= 0) wr_u32(DISPLAY_VALUE_BCD, 0x360);
    for (;;) {
        offset = (int16_t)(offset - 0xA0);
        if (offset < -0xB0) break;
        adjust_tape_bcd(0);
        draw_three_digits(LAYOUT_HEADING_TICK + (gaddr)(int32_t)offset, 0x858u);
    }
    draw_heading_ticks((int16_t)(0x3D + rd_s16(REDRAW_STATE_WORD)));
}

void draw_postflight_tape(void) {
    gaddr record = viewed_record();
    int32_t altitude;

    wr_u16(CURRENT_COLOUR, 13);
    if (rd_u8(FIXED_READOUTS)) {
        altitude = rd_s32(FIXED_ALTITUDE);
        if (altitude > 99999) altitude = 99999;
    } else {
        altitude = (rd_s32(record + 0x18) >> 10) * 5;
    }
    if (rd_u8(record + 0x62) != 0x10) {
        wr_u32(DISPLAY_VALUE, (uint32_t)altitude);
        pack_display_value();
        draw_bcd_in_view(LAYOUT_ALTITUDE_VALUE, 0xD38u, 0x18, 6, 0);
        {
            Text label = text_line(2, ALTITUDE_LABEL, LAYOUT_ALTITUDE_LABEL, 0xE52u, 0x1A);
            draw_text_in_view(&label);
        }
    } else {
        draw_altitude_tape(altitude);
    }
    draw_speed_tape(record);
    draw_heading_tape(record);
}

static int16_t matrix_dot(int16_t x, int16_t y, int16_t z, gaddr matrix) {
    uint32_t sum = (uint32_t)((int32_t)x * rd_s16(matrix));
    sum += (uint32_t)((int32_t)y * rd_s16(matrix + 2));
    sum += (uint32_t)((int32_t)z * rd_s16(matrix + 4));
    return (int16_t)((int32_t)sum >> 8);
}

void transform_postflight_record(void) {
    gaddr record = viewed_record();
    int16_t x, y, z;
    int32_t current[3];

    x = (int16_t)((int32_t)(rd_u32(POSTFLIGHT_VECTOR_PREVIOUS) - rd_u32(record + 0x14)) >> 8);
    y = (int16_t)((int32_t)(rd_u32(POSTFLIGHT_VECTOR_PREVIOUS + 4) - rd_u32(record + 0x18)) >> 8);
    z = (int16_t)((int32_t)(rd_u32(POSTFLIGHT_VECTOR_PREVIOUS + 8) - rd_u32(record + 0x1C)) >> 8);
    project_view_point(matrix_dot(x, y, z, VIEW_ANGLE_MATRIX),
                       matrix_dot(x, y, z, VIEW_ANGLE_MATRIX + 6),
                       matrix_dot(x, y, z, VIEW_ANGLE_MATRIX + 12));
    wr_u32(POSTFLIGHT_MARK, rd_u32(PROJECTED_PAIR));

    current[0] = ((int32_t)0x600 * rd_s16(record + 0x94) + (int32_t)0x4000 * rd_s16(record + 0x96)) >> 6;
    current[1] = ((int32_t)0x600 * rd_s16(record + 0x9A) + (int32_t)0x4000 * rd_s16(record + 0x9C)) >> 6;
    current[2] = ((int32_t)0x600 * rd_s16(record + 0xA0) + (int32_t)0x4000 * rd_s16(record + 0xA2)) >> 6;
    current[0] = (int32_t)((uint32_t)current[0] + rd_u32(record + 0x14));
    current[1] = (int32_t)((uint32_t)current[1] + rd_u32(record + 0x18));
    current[2] = (int32_t)((uint32_t)current[2] + rd_u32(record + 0x1C));
    wr_u32(POSTFLIGHT_VECTOR_PREVIOUS, rd_u32(POSTFLIGHT_VECTOR_CURRENT));
    wr_u32(POSTFLIGHT_VECTOR_PREVIOUS + 4, rd_u32(POSTFLIGHT_VECTOR_CURRENT + 4));
    wr_u32(POSTFLIGHT_VECTOR_PREVIOUS + 8, rd_u32(POSTFLIGHT_VECTOR_CURRENT + 8));
    wr_u32(POSTFLIGHT_VECTOR_CURRENT, (uint32_t)current[0]);
    wr_u32(POSTFLIGHT_VECTOR_CURRENT + 4, (uint32_t)current[1]);
    wr_u32(POSTFLIGHT_VECTOR_CURRENT + 8, (uint32_t)current[2]);
}

void draw_postflight_variant(void) {
    gaddr record;
    int16_t x, y;

    if (rd_s16(ZOOM_SCALE) != 0x80) return;
    record = viewed_record();
    if ((rd_u8(record + 0x63) & 0xF0) != 0x10) {
        if (rd_s16(SELECTED_RECORD) < 0) goto transform;
        if (rd_s16(SELECTION_MARKER) <= 0) wr_u32(POSTFLIGHT_MARK, 0x009F005Bu);
        goto display;
    }

    if (rd_s16(SELECTION_MARKER) > 0
        && word_abs((int16_t)(rd_s16(SELECTION_MARKER) - rd_s16(POSTFLIGHT_MARK))) <= 8
        && word_abs((int16_t)(rd_s16(SELECTION_MARKER_Y) - rd_s16(POSTFLIGHT_MARK + 2))) <= 8
        && rd_s16(record + 0x4A) <= 0x900) {
        if (!rd_u8(SHOOT_CUE)) {
            if (rd_s16(SELECTED_RECORD) < 0) goto clear_cue;
            wr_u32(EVENT_BITS, rd_u32(EVENT_BITS) | 4);
            wr_u8(SHOOT_CUE, 1);
        }
        draw_shoot_cue();
        goto dot;
    }
clear_cue:
    if (rd_u8(SHOOT_CUE)) {
        wr_u8(SHOOT_CUE, 0);
        wr_u32(EVENT_BITS, rd_u32(EVENT_BITS) | 8);
    }
dot:
    x = rd_s16(POSTFLIGHT_MARK);
    y = rd_s16(POSTFLIGHT_MARK + 2);
    if (x > 0) plot_pixel_in_view(x, y);
display:
    x = rd_s16(POSTFLIGHT_MARK);
    y = rd_s16(POSTFLIGHT_MARK + 2);
    if (x <= 0) goto transform;
    y = (int16_t)(y + rd_s16(REDRAW_STATE_WORD));
    wr_u16(CURRENT_COLOUR, 10);
    plot_ring(x, y, 0x50, RANGE_RING);
    if (rd_s16(SELECTED_RECORD) < 0) goto transform;
    wr_u16(CURRENT_COLOUR, 13);
    plot_ring_point(x, y, (int16_t)(((uint32_t)rd_u16(record + 0x4A) * 0x4Cu) / 0x8CA0u), RANGE_POINT);
    if (rd_s16(record + 0x4A) > 0x7F00) goto transform;
    if (rd_u8(POST_INPUT_EVENT)) {
        draw_signed_readout(rd_s16(RANGE_RATE));
    } else if (rd_u8(record + 4) & 1) {
        wr_u8(record + 4, (uint8_t)(rd_u8(record + 4) & ~1u));
        draw_signed_readout(rd_s16(RANGE_RATE));
    } else {
        int16_t range = rd_s16(record + 0x4A);
        int16_t rate = (int16_t)(range - rd_s16(POSTFLIGHT_RANGE_LAST));
        wr_s16(RANGE_RATE, rate);
        wr_s16(POSTFLIGHT_RANGE_LAST, range);
        draw_signed_readout(rate);
    }
transform:
    if (!rd_u8(POST_INPUT_EVENT)) transform_postflight_record();
}

void draw_postflight_hud(void) {
    wr_u32(LINE_STYLE, 0xFFFFFu);
    if (rd_u8(CONTEXT_SELECT)) {
        wr_u8(SHOOT_CUE, 0);
        return;
    }
    draw_view_marker();
    draw_hud_marks();
    draw_target_box();
    update_missile_cue();
    draw_weapon_readout();
    if (!rd_u8(POST_INPUT_EXPIRED)) return;
    draw_load_readout();
    draw_postflight_tape();
    draw_postflight_variant();
}
