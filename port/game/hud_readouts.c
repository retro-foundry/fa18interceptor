/* The cockpit's numeric readouts (see hud_readouts.h). */
#include "hud_readouts.h"

#include "globals.h"
#include "numbers.h"
#include "plot.h"
#include "stages.h"
#include "text.h"

/* Small-text modes: glyphs set, glyphs cleared on a set cell, cell cleared. */
#define SMALL_DRAW    0x0FCA
#define SMALL_INVERSE 0x0F3A
#define SMALL_CLEAR   0x0F0A

/* Character layouts (byte column, glyph shift per character) in the game. */
#define LAYOUT_ALTITUDE 0xC31928u
#define LAYOUT_SPEED    0xC31948u
#define LAYOUT_SPEED_C  0xC31958u /* in a context */
#define LAYOUT_HEADING  0xC31974u
#define LAYOUT_THREE    0xC3198Cu
#define LAYOUT_FIVE     0xC31994u
#define LAYOUT_EIGHT    0xC31998u /* 8-pixel text, one character per 8 pixels */
#define LAYOUT_ZOOM     0xC31A1Cu
#define LAYOUT_GRID_X   0xC31A24u
#define LAYOUT_GRID_Z   0xC31A38u
#define LAYOUT_LOAD     0xC31900u
#define LAYOUT_LOAD_G   0xC3190Cu /* after the "G" */
#define LAYOUT_G        0xC31918u
#define LAYOUT_STATUS   0xC3191Cu
#define TEXT_G          0xC31E5Eu /* "G" */
#define TEXT_IN_RANGE   0xC31E5Fu /* "IN RNG" */
#define TEXT_SHOOT      0xC31E65u /* " SHOOT" */

static gaddr viewed_record(void) {
    return CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
}

static void show(int32_t value) {
    wr_u32(DISPLAY_VALUE, (uint32_t)value);
    pack_display_value();
}

/* DIVU.W / DIVS.W: on overflow the dividend's low word stays. */
static int16_t divu_word(int32_t dividend, uint16_t divisor) {
    uint32_t q = (uint32_t)dividend / divisor;
    return (int16_t)(q > 0xFFFF ? (uint16_t)dividend : (uint16_t)q);
}

static int16_t divs_word(int32_t dividend, int16_t divisor) {
    int32_t q = dividend / divisor;
    return (int16_t)(q != (int16_t)q ? (uint16_t)dividend : (uint16_t)q);
}

/* In a context the readouts are drawn only while both allow it. */
static int context_allows(void) {
    return rd_u8(CONTEXT_READOUTS) && (rd_u16(COCKPIT_FLAGS) & 0x40);
}

static void put(gaddr at, const char *s) {
    while (*s) wr_u8(at++, (uint8_t)*s++);
}

/* `count` characters from `chars`, `digits` hex digits of DISPLAY_VALUE_BCD
 * ending at `end` written into them first. */
static void small_readout(int count, gaddr chars, gaddr end, int digits, int keep_zeros, gaddr layout, gaddr rows,
                          int16_t x_origin, int16_t plane, uint16_t mode, int in_view) {
    SmallText line = small_text_line(count, chars, layout, rows, x_origin, plane, mode, in_view);
    format_digits(end, digits, 1, keep_zeros);
    draw_small_text(&line);
}

/* In a context: the line in plane 0 and, inverted, in plane $C. */
static void small_readout_twice(int count, gaddr chars, gaddr end, int digits, int keep_zeros, gaddr layout,
                                gaddr rows, int16_t x_origin) {
    small_readout(count, chars, end, digits, keep_zeros, layout, rows, x_origin, 0, SMALL_DRAW, 0);
    small_readout(count, chars, end, digits, keep_zeros, layout, rows, x_origin, 0xC, SMALL_INVERSE, 0);
}

void draw_scale_readout(void) {
    uint8_t mode;

    if ((int8_t)rd_u8(SCALE_REDRAWS) <= 0) return;
    wr_u8(SCALE_REDRAWS, (uint8_t)(rd_u8(SCALE_REDRAWS) - 1));
    mode = rd_u8(viewed_record() + 0x63) & 15;
    show(mode == 9 ? 2 : mode == 0xB ? 10 : 40);
    small_readout(3, TEXT_LINE, TEXT_LINE + 3, 3, 0, LAYOUT_THREE, 0x1CD2, 0x12, 4, SMALL_DRAW, 1);
}

void draw_zoom_readout(void) {
    int16_t zoom;
    int32_t shown;
    uint16_t mode = SMALL_DRAW, colour = 4;

    if ((int8_t)rd_u8(DISPLAY_UPDATE) <= 0) return;
    wr_u8(DISPLAY_UPDATE, (uint8_t)(rd_u8(DISPLAY_UPDATE) - 1));
    zoom = rd_s16(ZOOM_SCALE);
    if (zoom == 0x80) {
        shown = 10;
    } else {
        shown = zoom == 0x40 ? 20 : 40;
        wr_u8(DISPLAY_UPDATE, 1);
        if (rd_u8(ZOOM_READOUT_FLAGS) & 1) {
            mode = SMALL_CLEAR;
            colour = 0;
        }
    }
    wr_u16(CURRENT_COLOUR, colour);
    wr_u32(LINE_STYLE, 0xFFFFF);
    plot_pixel_in_view(0xF6, 0xBC);
    show(shown);
    small_readout(2, TEXT_LINE, TEXT_LINE + 2, 2, 0, LAYOUT_ZOOM, 0x1CDE, 0x1E, 4, mode, 1);
}

void draw_speed_readout(void) {
    gaddr record = viewed_record(), chars = TEXT_LINE + 5;
    int16_t speed = 0;

    if (!(rd_u8(record) & 0x80)) {
        speed = rd_s16(record + 0x6E);
        if (speed < 0) speed = (int16_t)-speed;
    }
    if (rd_u8(CONTEXT_SELECT)) {
        if (!context_allows()) return;
        show(divu_word(speed, 12));
        put(chars + 4, "KTS");
        small_readout_twice(7, chars, chars + 4, 7, 0, LAYOUT_SPEED_C, 0x1CD2, 0x12);
        return;
    }
    speed = display_value_to_draw(SPEED_SHOWN, speed);
    if (speed < 0) return;
    show(divu_word(speed, 12));
    small_readout(4, chars, chars + 4, 4, 0, LAYOUT_SPEED, 0x1A0E, 0x1E, 4, SMALL_DRAW, 1);
}

void draw_altitude_readout(void) {
    gaddr chars = TEXT_LINE + 4;
    int context = rd_u8(CONTEXT_SELECT) != 0, cached = !context && (int8_t)rd_u8(GAUGE_REFRESH) <= 0;
    int32_t altitude;

    if (rd_u8(FIXED_READOUTS)) {
        altitude = rd_s32(FIXED_ALTITUDE);
        if (altitude > 99999) altitude = 99999;
    } else {
        altitude = (int32_t)((uint32_t)(rd_s32(viewed_record() + 0x18) >> 10) * 5);
    }
    if (context && !context_allows()) return;
    if (cached && rd_s32(ALTITUDE_SHOWN) < 0) {
        /* A value still to be drawn: that one, now marked drawn. */
        altitude = rd_s32(ALTITUDE_SHOWN) & 0x7FFFFFFF;
        wr_u32(ALTITUDE_SHOWN, (uint32_t)altitude);
    } else {
        if (cached && (altitude == rd_s32(ALTITUDE_SHOWN) || (rd_u8(DISPLAY_FORCE) & 1))) return;
        wr_u32(ALTITUDE_SHOWN, (uint32_t)altitude | 0x80000000u);
    }
    show(altitude);
    if (!context) {
        small_readout(6, chars, chars + 6, 6, 0, LAYOUT_ALTITUDE, 0x18CE, 0x1E, 4, SMALL_DRAW, 1);
        return;
    }
    put(chars + 6, "FT");
    small_readout_twice(8, chars, chars + 6, 8, 0, LAYOUT_ALTITUDE, 0x1CDA, 0x1A);
}

void draw_record_72_readout(void) {
    int16_t value = (int16_t)(rd_s32(viewed_record() + 0x72) >> 8);

    value = display_value_to_draw(GAUGE_SOURCE, value);
    if (value < 0) return;
    show(value);
    small_readout(5, TEXT_LINE + 1, TEXT_LINE + 6, 5, 0, LAYOUT_FIVE, 0x1CA4, 0xC, 0, SMALL_INVERSE, 1);
}

void draw_record_2b_readout(void) {
    int16_t value = (int16_t)((int8_t)rd_u8(viewed_record() + 0x2B) << 8);

    if (value < 0) value = (int16_t)-value;
    value = display_value_to_draw(BYTE_2B_SHOWN, divu_word(value, 0x133));
    if (value < 0) return;
    show(value);
    small_readout(3, TEXT_LINE, TEXT_LINE + 3, 3, 0, LAYOUT_THREE, 0x1B76, 0x1E, 4, SMALL_DRAW, 1);
}

/* A grid coordinate: redrawn for two more passes after it changes. */
static void grid_readout(int16_t value, gaddr shown, gaddr redraws, gaddr layout, gaddr rows) {
    if (value == rd_s16(shown)) {
        if ((int8_t)rd_u8(redraws) <= 0) return;
        wr_u8(redraws, (uint8_t)(rd_u8(redraws) - 1));
    } else {
        wr_u16(shown, (uint16_t)value);
        wr_u8(redraws, 2);
    }
    if (!rd_u8(FIXED_READOUTS)) show(value);
    small_readout(4, TEXT_LINE, TEXT_LINE + 4, 4, 0, layout, rows, 0x24, 4, SMALL_INVERSE, 1);
    small_readout(4, TEXT_LINE, TEXT_LINE + 4, 4, 0, layout, rows, 0x24, 0xC, SMALL_INVERSE, 1);
}

void draw_grid_z_readout(void) {
    int32_t z = (int32_t)(rd_u32(viewed_record() + 0x1C) - 0x10000000u) >> 8;
    grid_readout((int16_t)(divs_word(z, 0x7000) + 0x177), GRID_Z_SHOWN, GRID_Z_REDRAWS, LAYOUT_GRID_Z, 0x1C44);
}

void draw_grid_x_readout(void) {
    int32_t x = (int32_t)(rd_u32(viewed_record() + 0x14) - 0x0F000000u) >> 8;
    grid_readout((int16_t)(divs_word(x, 0x5999) + 0x4C4), GRID_X_SHOWN, GRID_X_REDRAWS, LAYOUT_GRID_X, 0x1D84);
}

void draw_weapon_readout(void) {
    gaddr record = viewed_record();
    uint8_t kind = rd_u8(record + 0x63) & 0xF0, stores = rd_u8(record + 0x5F);
    Text line;

    wr_u16(CURRENT_COLOUR, 13);
    if (!kind) return;
    if (kind == 0x10) {
        put(TEXT_LINE, "GUN ");
        show(rd_u16(record + 0x60));
        line = text_line(7, TEXT_LINE, LAYOUT_EIGHT, 0x1432, 0xA);
        print_bcd_in_view(TEXT_LINE + 7, 3, 0, &line);
        return;
    }
    put(TEXT_LINE, kind == 0x30 ? " AM " : " SW ");
    show(kind == 0x30 ? stores & 15 : stores >> 4);
    line = text_line(5, TEXT_LINE, LAYOUT_EIGHT, 0x1432, 0xA);
    print_bcd_in_view(TEXT_LINE + 5, 1, 0, &line);
}

void draw_signed_readout(int16_t value) {
    Text line = text_line(5, TEXT_LINE, LAYOUT_EIGHT, 0x134E, 0x16);

    wr_u16(CURRENT_COLOUR, 13);
    /* The sign is shown inverted: '-' for values at or above zero. */
    wr_u8(TEXT_LINE, value < 0 ? ' ' : '-');
    if (value < 0) value = (int16_t)-value;
    show(value);
    print_bcd_in_view(TEXT_LINE + 5, 4, 1, &line);
}

void draw_load_readout(void) {
    gaddr record = viewed_record(), layout, rows;
    int16_t load, trim = rd_s16(LOAD_TRIM), x_origin;
    Text line;

    wr_u16(CURRENT_COLOUR, 13);
    wr_u32(LINE_STYLE, 0xFFFFF);
    if (rd_u8(record + 0x62) != 0x11) {
        plot_pixel_in_view(0x72, 0x41); /* the decimal point */
        layout = LAYOUT_LOAD;
        rows = 0x994;
        x_origin = 0xC;
    } else {
        line = text_line(1, TEXT_G, LAYOUT_G, 0x12F2, 0xA);
        draw_text_in_view(&line);
        plot_pixel_in_view(0x64, 0x7D);
        layout = LAYOUT_LOAD_G;
        rows = 0x12F2;
        x_origin = 0xA;
    }
    load = rd_s16(record + 0x56);
    load = (int16_t)(load + (load >> 3));
    if ((trim < 0 ? (int16_t)-trim : trim) > 1) load = (int16_t)(load + trim);
    load = (int16_t)(load >> 3);
    load = (int16_t)(load + ((rd_u16(STATUS_CA) & 2) ? 10 : -10));
    wr_u8(TEXT_LINE, load < 0 ? ' ' : '-');
    if (load < 0) load = (int16_t)-load;
    show(load);
    line = text_line(3, TEXT_LINE, layout, rows, x_origin);
    print_bcd_in_view(TEXT_LINE + 3, 2, 0, &line);
}

void draw_shoot_cue(void) {
    int gun = (rd_u8(viewed_record() + 0x63) & 0xF0) == 0x10;
    Text line = text_line(6, gun ? TEXT_SHOOT : TEXT_IN_RANGE, LAYOUT_EIGHT, 0x1440, 0x18);

    wr_u16(CURRENT_COLOUR, 13);
    draw_text_in_view(&line);
}

void draw_heading_readout(void) {
    int16_t degrees;

    if (!rd_u8(CONTEXT_SELECT) || !context_allows()) return;
    degrees = divu_word((int32_t)rd_s16(viewed_record() + 0x68) >> 3, 10);
    wr_u16(COMPASS_DEGREES, (uint16_t)degrees);
    show(degrees);
    put(TEXT_LINE, "HDG");
    small_readout_twice(6, TEXT_LINE, TEXT_LINE + 6, 3, 1, LAYOUT_HEADING, 0x1CCA, 0xA);
}

void draw_three_digits(gaddr layout, gaddr rows) {
    Text line = text_line(2, TEXT_LINE, layout, rows, 0x10);
    print_bcd_in_view(TEXT_LINE + 3, 3, 1, &line);
}

void draw_weapon_status(void) {
    gaddr record = viewed_record();
    uint8_t kind = rd_u8(record + 0x63) & 0xF0, stores = rd_u8(record + 0x5F);
    SmallText top = small_text_line(3, TEXT_LINE, LAYOUT_STATUS, 0x1CC6, 6, 4, SMALL_DRAW, 1);
    SmallText bottom = small_text_line(3, TEXT_LINE, LAYOUT_STATUS, 0x1B86, 6, 4, SMALL_DRAW, 1);
    int16_t left;

    if ((int8_t)rd_u8(WEAPON_REDRAWS) <= 0) return;
    wr_u8(WEAPON_REDRAWS, (uint8_t)(rd_u8(WEAPON_REDRAWS) - 1));
    if (!kind) {
        put(TEXT_LINE, "   ");
        draw_small_text(&top);
        draw_small_text(&bottom);
        return;
    }
    if (kind == 0x10) {
        put(TEXT_LINE, "GUN");
        left = rd_s16(record + 0x60);
    } else {
        put(TEXT_LINE, kind == 0x30 ? " AM" : " SW");
        left = kind == 0x30 ? stores & 15 : stores >> 4;
    }
    put(TEXT_LINE + 3, " ");
    put(TEXT_LINE + 5, "  ");
    draw_small_text(&top);
    put(TEXT_LINE, left > 0 ? "ARM" : " NO");
    draw_small_text(&bottom);
}

static void light_colour(int on, uint16_t colour) {
    wr_u16(CURRENT_COLOUR, on ? colour : 3);
}

void draw_threat_lights(void) {
    uint8_t events = rd_u8(THREAT_EVENTS);
    gaddr record = viewed_record();
    int16_t x = (int16_t)(0x125 + rd_s16(SPAN_ORIGIN_Y)), y = (int16_t)(0x9C + rd_s16(REDRAW_STATE_WORD));

    wr_u32(LINE_STYLE, 0xFFFFF);
    light_colour(events & 4, 1);
    if (!plot_square_in_view(0x125, 0x9C)) return;
    plot_square((int16_t)(x + 2), y);
    light_colour(events & 2, 4);
    plot_square((int16_t)(x + 5), y);
    plot_square((int16_t)(x + 7), y);
    y = (int16_t)(y + 3);
    light_colour(events & 0x10, 2);
    plot_square((int16_t)(x + 5), y);
    plot_square((int16_t)(x + 7), y);
    light_colour(events & 0x20, 2);
    plot_square(x, y);
    plot_square((int16_t)(x + 2), y);
    light_colour((rd_u8(record + 0x20) & 4) && (rd_u8(DISPLAY_FORCE) & 2), 1);
    plot_square((int16_t)(x + 10), y);
    plot_square((int16_t)(x + 12), y);
}
