/* Cockpit panel pieces drawn with the blitter (see hud_bars.h). */
#include "hud_bars.h"

#include "control_records.h"
#include "globals.h"
#include "hardware.h"
#include "plot.h"
#include "render_line.h"
#include "render_polygon.h"
#include "render_span.h"
#include "render_state.h"
#ifdef FA18_NATIVE
#include "native/raster.h"
#include <stdlib.h>
#endif

#define ROW_BYTES 40

static gaddr viewed_record(void) {
    return CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
}

static gaddr plane_address(int16_t plane) {
    return rd_u32(rd_u32(PAGE_PLANE_TABLE) + (gaddr)(int32_t)plane);
}

/* A countdown byte: 1 (and one pass fewer) while it runs. */
static int counts_down(gaddr redraws) {
    if ((int8_t)rd_u8(redraws) <= 0) return 0;
    wr_u8(redraws, (uint8_t)(rd_u8(redraws) - 1));
    return 1;
}

void fill_bar_words(uint16_t con0, int16_t plane, uint32_t cursor, int16_t shown_right, int16_t cut_left,
                    uint16_t size, int16_t modulo, uint16_t first_mask, uint16_t last_mask) {
    gaddr dest = plane_address(plane) + cursor;
    int16_t cut = (int16_t)(shown_right + cut_left);

    /* A cut end is a whole word. */
    if (shown_right) last_mask = 0xFFFF;
    if (cut_left) first_mask = 0xFFFF;
#ifdef FA18_NATIVE
    if(con0!=BAR_CLEAR && con0!=BAR_SET) abort();
    native_raster_bar(dest,(uint16_t)(size-cut),(int16_t)(cut*2+modulo),first_mask,last_mask,con0==BAR_SET);
#else
    wait_blitter();
    custom_write(BLTCON0, con0);
    custom_write(BLTCON1, 0);
    custom_write(BLTADAT, 0xFFFF);
    custom_write(BLTAFWM, first_mask);
    custom_write(BLTALWM, last_mask);
    custom_write(BLTCMOD, (uint16_t)(cut * 2 + modulo));
    custom_write(BLTDMOD, (uint16_t)(cut * 2 + modulo));
    custom_write_ptr(BLTCPT, dest);
    custom_write_ptr(BLTDPT, dest);
    custom_write(BLTSIZE, (uint16_t)(size - cut));
#endif
}

int fill_bar(uint16_t con0, int16_t plane, uint32_t rows, int16_t position, int16_t words, uint16_t size,
             int16_t modulo, uint16_t first_mask, uint16_t last_mask) {
    int32_t cursor = (int32_t)(rows + rd_u32(REDRAW_STATE_LONG));
    int16_t shown_right = bound_span(&position, words, &cursor);

    if (shown_right < 0) return 0;
    fill_bar_words(con0, plane, (uint32_t)cursor, shown_right, position, size, modulo, first_mask, last_mask);
    return 1;
}

static int is_on_screen(int32_t x, int16_t last) {
    return x >= 0 && (int16_t)x <= last;
}

void draw_indicator_bars(void) {
    gaddr record = viewed_record();

    if (counts_down(BAR_REDRAWS_A)) {
        int32_t x = (int32_t)0xF1 + rd_s16(SPAN_ORIGIN_Y);
        if (is_on_screen(x, 0x137)) {
            int16_t y = (int16_t)(0xB4 + rd_s16(REDRAW_STATE_WORD));
            wr_u32(LINE_STYLE, 0xFFFFF);
            wr_u16(CURRENT_COLOUR, (rd_u8(record + 3) & 8) ? 9 : 0);
            draw_line_to_row((int16_t)x, y, (int16_t)(x + 9), y, 0xC7);
        }
    }
    if (counts_down(BAR_REDRAWS_B)) {
        if (!fill_bar(rd_u16(record) & 0x800 ? BAR_SET : BAR_CLEAR, 0xC, 0x1A68, 0, 3, 0x1C3, 0x23, 0x7F, 0x8000))
            goto last;
    }
    if (counts_down(BAR_REDRAWS_C)) {
        if (fill_bar(rd_u8(PLAYER_FLAGS_G) ? BAR_SET : BAR_CLEAR, 0xC, 0x1A14, 0x12, 2, 0x1C2, 0x25, 0xFFF, 0xFC00))
            return;
    }
last:
    if (counts_down(BAR_REDRAWS_E)) {
        int set = rd_u8(BAR_E_FLAG) && (rd_u8(BAR_REDRAWS_E) & 1); /* flashing */
        fill_bar(set ? BAR_SET : BAR_CLEAR, 0xC, 0x1888, 0, 2, 0x202, 0x25, 0x7F, 0xFFFF);
    }
}

/* The panel image: a pointer to its address, and pointers to each plane's. */
#define PANEL_IMAGE        0xC309A2u
#define PANEL_PLANE_IMAGES 0xC309A6u

/* The images $C30D34 shows by the record's +$7C bits 5-6. */
#define MODE_IMAGE      0x129FCu
#define MODE_MASKS      0xC30D22u /* long[4] */

void draw_mode_bar(void) {
    gaddr record = viewed_record();

    fill_bar((int8_t)rd_u8(BAR_REDRAWS_F) > 0 && (rd_u8(DISPLAY_FORCE) & 2) ? BAR_SET : BAR_CLEAR, 0xC, 0x1C70, 0,
             2, 0x202, 0x25, 0x3F, 0xFFFE);
    if (!counts_down(BAR_REDRAWS_D)) return;
    {
        int index = (int8_t)(rd_u8(record + 0x7C) & 0x7F) >> 5;
        uint32_t mask = rd_u32(MODE_MASKS + (gaddr)(4 * index));
        int32_t cursor = (int32_t)(0x1D88 + rd_u32(REDRAW_STATE_LONG));
        int16_t position = 0, shown_right = bound_span(&position, 2, &cursor), cut;
        uint16_t size = 0x1C2, modulo;
        gaddr dest;
        uint32_t skip;

        if (shown_right < 0) return;
        cut = (int16_t)(shown_right + position);
        skip = (uint32_t)(int32_t)(int16_t)(position * 2);
        size = (uint16_t)(size - cut);
        modulo = (uint16_t)(cut * 2 + 1);
        dest = plane_address(0) + (gaddr)cursor;
#ifdef FA18_NATIVE
        native_raster_panel_inverted(MODE_IMAGE+skip,mask+skip,dest,size,(int16_t)modulo,(int16_t)(modulo-1+0x25));
#else
        wait_blitter();
        custom_write(BLTCON0, 0x0F3A);
        custom_write(BLTCON1, 0);
        custom_write(BLTAFWM, 0xFFFF);
        custom_write(BLTALWM, 0xFFFF);
        custom_write(BLTAMOD, modulo);
        custom_write(BLTBMOD, modulo);
        custom_write(BLTCMOD, (uint16_t)(modulo - 1 + 0x25));
        custom_write(BLTDMOD, (uint16_t)(modulo - 1 + 0x25));
        custom_write_ptr(BLTAPT, MODE_IMAGE + skip);
        custom_write_ptr(BLTBPT, mask + skip);
        custom_write_ptr(BLTCPT, dest);
        custom_write_ptr(BLTDPT, dest);
        custom_write(BLTSIZE, size);
#endif
    }
    if (rd_u8(record + 2) & 0x80) {
        int32_t x0 = (int32_t)0xE + rd_s16(SPAN_ORIGIN_Y), x1 = (int32_t)0xC + rd_s16(SPAN_ORIGIN_Y);
        if (!is_on_screen(x0, 0x13F) || !is_on_screen(x1, 0x13F)) return;
        wr_u32(LINE_STYLE, 0xFFFFF);
        wr_u16(CURRENT_COLOUR, 0);
        draw_line_to_row((int16_t)x0, (int16_t)(0xC0 + rd_s16(REDRAW_STATE_WORD)), (int16_t)x1,
                         (int16_t)(0xC3 + rd_s16(REDRAW_STATE_WORD)), 0xC7);
    }
}

void blit_image(uint16_t con0, uint32_t mask, gaddr images, uint32_t rows, int16_t position, int16_t words, uint16_t size,
                       int16_t modulo) {
    int32_t cursor = (int32_t)(rows + rd_u32(REDRAW_STATE_LONG));
    int16_t shown_right = bound_span(&position, words, &cursor), cut;
    uint32_t skip;
    uint16_t source_modulo;
    int k;

    if (shown_right < 0) return;
    cut = (int16_t)(shown_right + position);
    skip = (uint32_t)(int32_t)(int16_t)(position * 2);
    size = (uint16_t)(size - cut);
    source_modulo = (uint16_t)(cut * 2 + 1);
    while (cursor < ROW_BYTES) {
        cursor += ROW_BYTES;
        skip += (uint32_t)(2 * words);
        size = (uint16_t)(size - 0x40);
        if ((int16_t)size < 0) return;
    }
    mask += skip;
    for (k = 0; k < 4; k++) {
        gaddr dest = plane_address((int16_t)(4 * k)) + (gaddr)cursor;
        uint32_t image = rd_u32(rd_u32(images + (gaddr)(4 * k))) + skip;
#ifdef FA18_NATIVE
        if(con0!=0x0fce) abort();
        native_raster_panel_image(mask,image,dest,size,(int16_t)source_modulo,(int16_t)(source_modulo-1+modulo));
#else
        wait_blitter();
        if (k == 0) {
            custom_write(BLTCON1, 0);
            custom_write(BLTAFWM, 0xFFFF);
            custom_write(BLTALWM, 0xFFFF);
            custom_write(BLTBMOD, source_modulo);
            custom_write(BLTAMOD, source_modulo);
            custom_write(BLTCMOD, (uint16_t)(source_modulo - 1 + modulo));
            custom_write(BLTDMOD, (uint16_t)(source_modulo - 1 + modulo));
        }
        custom_write(BLTCON0, con0);
        custom_write_ptr(BLTAPT, mask);
        custom_write_ptr(BLTBPT, image);
        custom_write_ptr(BLTCPT, dest);
        custom_write_ptr(BLTDPT, dest);
        custom_write(BLTSIZE, size);
#endif
    }
}

void draw_panel_image(void) {
    blit_image(0x0FCE, rd_u32(rd_u32(PANEL_IMAGE)), PANEL_PLANE_IMAGES, 0x14CA, 1, 0x12, 0x312, 5);
}

#define COMPASS_IMAGE 0x12AFCu

void draw_compass_tape(void) {
    int refresh = (int8_t)rd_u8(GAUGE_REFRESH) > 0;
    int16_t tape, half, position = 0xC, shown_right, cut;
    int32_t cursor, across;
    uint16_t first_mask = 0xFFFF, last_mask = 0, shift = 0;
    gaddr dest;
    uint32_t source;

    update_compass();
    tape = rd_s16(COMPASS_TAPE);
    if (!refresh && rd_s16(TAPE_SHOWN) < 0) {
        /* A position still to be drawn: that one, now marked drawn. */
        tape = (int16_t)(rd_s16(TAPE_SHOWN) & 0x7FFF);
        wr_u16(TAPE_SHOWN, (uint16_t)tape);
    } else {
        if (!refresh && (tape == rd_s16(TAPE_SHOWN) || (rd_u8(DISPLAY_FORCE) & 1))) return;
        wr_u16(TAPE_SHOWN, (uint16_t)(tape | 0x8000));
    }
    cursor = (int32_t)(0x17B0 + rd_u32(REDRAW_STATE_LONG));
    shown_right = bound_span(&position, 2, &cursor);
    if (shown_right < 0) return;
    cut = (int16_t)(shown_right + position);
    dest = plane_address(0xC) + (gaddr)cursor;
    half = (int16_t)(tape >> 1);
    if (half & 15) {
        /* Shifted in from the word before. */
        dest -= 2;
        first_mask = 0;
        last_mask = 0xFFFF;
        shift = (uint16_t)((16 - (half & 15)) << 12);
    }
    source = COMPASS_IMAGE + (uint32_t)((half & 0xF0) >> 3) + (uint32_t)(int32_t)(int16_t)(position * 2);
#ifdef FA18_NATIVE
    native_raster_compass(source,dest,(uint16_t)(0x1c3-cut),(int16_t)(cut*2+7),(int16_t)(cut*2+0x23),first_mask,last_mask,shift>>12);
#else
    wait_blitter();
    custom_write(BLTCON0, 0x073A);
    custom_write(BLTCON1, shift);
    custom_write(BLTADAT, 0xFFFF);
    custom_write(BLTALWM, last_mask);
    custom_write(BLTAFWM, first_mask);
    custom_write(BLTBMOD, (uint16_t)(cut * 2 + 7));
    custom_write(BLTCMOD, (uint16_t)(cut * 2 + 0x23));
    custom_write(BLTDMOD, (uint16_t)(cut * 2 + 0x23));
    custom_write_ptr(BLTBPT, source);
    custom_write_ptr(BLTCPT, dest);
    custom_write_ptr(BLTDPT, dest);
    custom_write(BLTSIZE, (uint16_t)(0x1C3 - cut));
#endif
    wr_u32(LINE_STYLE, 0xFFFFF);
    across = (int32_t)0xD0 + rd_s16(SPAN_ORIGIN_Y);
    if (across < 0 || (int16_t)across >= 0x140) return;
    wr_u16(CURRENT_COLOUR, 1);
    draw_line_to_row((int16_t)across, 0x96, (int16_t)across, 0x9D, 0xC7);
}

#define FRAME_PLANE_IMAGES 0xC30752u /* pointers to each plane's image */

void draw_panel_frame(void) {
    int16_t origin, row, words, lift = rd_s16(REDRAW_STATE_WORD);
    uint16_t size, modulo;
    int32_t cursor;
    int k;

    if (!counts_down(REDRAW_FIRST)) return;
    {
        int16_t position = 0, height = (int16_t)(0x37 - (lift > 0 ? lift : 0)), shown_right, cut;
        uint32_t skip;
        cursor = (int32_t)(0x16A8 + rd_u32(REDRAW_STATE_LONG));
        shown_right = bound_span(&position, 0x14, &cursor);
        if (shown_right >= 0) {
            cut = (int16_t)(shown_right + position);
            skip = (uint32_t)(int32_t)(int16_t)(position * 2);
            size = (uint16_t)((uint16_t)(height << 6) + 0x14 - cut);
            modulo = (uint16_t)(cut * 2 + 1);
            for (k = 0; k < 4; k++) {
                gaddr dest = plane_address((int16_t)(4 * k)) + (gaddr)cursor;
                uint32_t image = rd_u32(rd_u32(FRAME_PLANE_IMAGES + (gaddr)(4 * k))) + skip;
#ifdef FA18_NATIVE
                native_raster_panel_copy(image,dest,size,(int16_t)modulo);
#else
                wait_blitter();
                if (k == 0) {
                    custom_write(BLTCON1, 0);
                    custom_write(BLTAFWM, 0xFFFF);
                    custom_write(BLTALWM, 0xFFFF);
                    custom_write(BLTAMOD, modulo);
                    custom_write(BLTDMOD, modulo);
                }
                restart_blit_ad(0x09F0, image, dest, size);
#endif
            }
        }
    }
    origin = rd_s16(SPAN_ORIGIN);
    if (!origin) return;
    row = (int16_t)(rd_s16(LINE_LAST_ROW) - 0x90);
    cursor = 0x16A8;
    if (origin > 0) {
        words = origin >= 20 ? 20 : origin;
    } else if (origin <= -20) {
        words = 20;
    } else {
        words = (int16_t)-origin;
        cursor += (int16_t)(origin * 2 + 40);
    }
    modulo = (uint16_t)((20 - words) * 2 + 1);
    size = (uint16_t)(words + 0xDC0);
    if (lift > 0) size = (uint16_t)(size - (uint16_t)(lift << 6));
    size = (uint16_t)(size - (uint16_t)(row << 6));
    cursor += (int16_t)(row * 40);
    for (k = 0; k < 4; k++) {
        gaddr dest = plane_address((int16_t)(4 * k)) + (gaddr)cursor;
#ifdef FA18_NATIVE
        native_raster_bar(dest,size,(int16_t)modulo,0xffff,0xffff,k>=2);
#else
        wait_blitter();
        if (k == 0) {
            custom_write(BLTCON1, 0);
            custom_write(BLTADAT, 0xFFFF);
            custom_write(BLTAFWM, 0xFFFF);
            custom_write(BLTALWM, 0xFFFF);
            custom_write(BLTCMOD, modulo);
            custom_write(BLTDMOD, modulo);
        }
        restart_blit_cd(k < 2 ? BAR_CLEAR : BAR_SET, dest, size);
#endif
    }
}

/* The panel image behind the mark ($C3003A). */
#define MARK_PANEL_IMAGE 0x00012A88u
#define MARK_PANEL_ROWS  0x1990
#define MARK_PANEL_SIZE  0x0542
#define MARK_PANEL_MODULO 0x25

/* plot_pixel_in_view moves the column by SPAN_ORIGIN_Y, and the row by
 * REDRAW_STATE_WORD only while that column is in view; the marks that
 * follow step on from where it left the pen. */
static void plot_in_view_from(int16_t *x, int16_t *y) {
    int32_t across = (int32_t)*x + rd_s16(SPAN_ORIGIN_Y);

    plot_pixel_in_view(*x, *y);
    *x = (int16_t)across;
    if (across >= 0 && (int16_t)across < 0x140) *y = (int16_t)(*y + rd_u16(REDRAW_STATE_WORD));
}

void draw_panel_mark(void) {
    int16_t position = 0x0C, origin = rd_s16(SPAN_ORIGIN), result, cut;
    int32_t cursor = MARK_PANEL_ROWS + rd_s32(REDRAW_STATE_LONG);
    uint16_t first_mask, last_mask;
    gaddr dest;
    int16_t x, y;

    if ((int16_t)(0x0C - origin) < 0) return;
    if ((int16_t)(6 - origin) < 0) return;
    result = bound_span(&position, 2, &cursor);
    if (result < 0) return;
    last_mask = result ? 0xFFFF : 0xFFF0;
    first_mask = position ? 0xFFFF : 0x0FFF;
    cut = (int16_t)(result + position);
    dest = rd_u32(rd_u32(PAGE_PLANE_TABLE) + 4) + (uint32_t)cursor;
#ifdef FA18_NATIVE
    native_raster_mark_clear(MARK_PANEL_IMAGE,dest,(uint16_t)(MARK_PANEL_SIZE-cut),(int16_t)(cut*2+MARK_PANEL_MODULO));
#else
    wait_blitter();
    custom_write(BLTCON0, 0x0722);
    custom_write(BLTCON1, 0);
    custom_write(BLTADAT, 0xFFFF);
    custom_write(BLTAFWM, first_mask);
    custom_write(BLTALWM, last_mask);
    custom_write(BLTBMOD, 1);
    custom_write(BLTCMOD, (uint16_t)(cut * 2 + MARK_PANEL_MODULO));
    custom_write(BLTDMOD, (uint16_t)(cut * 2 + MARK_PANEL_MODULO));
    custom_write_ptr(BLTBPT, MARK_PANEL_IMAGE);
    custom_write_ptr(BLTCPT, dest);
    custom_write_ptr(BLTDPT, dest);
    custom_write(BLTSIZE, (uint16_t)(MARK_PANEL_SIZE - cut));
#endif

    draw_mark_polygon();

    wr_u32(LINE_STYLE, 0x0007FFFF);
    x = (int16_t)(0xCC + rd_s16(SPAN_ORIGIN_Y));
    y = (int16_t)(0xAC + rd_u16(REDRAW_STATE_WORD));
    wr_u16(CURRENT_COLOUR, 2);
    draw_line_to_row(x, y, (int16_t)(x + 0x0A), y, 0xC7);

    x = 0xD1;
    y = 0xAB;
    plot_in_view_from(&x, &y);
    plot_pixel(x, 0xAC);

    wr_u16(CURRENT_COLOUR, 0x0D);
    x = 0xCE;
    y = 0xA5;
    plot_in_view_from(&x, &y);
    plot_pixel(x = (int16_t)(x - 2), y = (int16_t)(y + 1));
    plot_pixel(x = (int16_t)(x - 1), y = (int16_t)(y + 1));
    plot_pixel(x = (int16_t)(x - 2), y = (int16_t)(y + 3));
    plot_pixel(x = (int16_t)(x + 0x11), y = (int16_t)(y + 1));
    plot_pixel(x, y = (int16_t)(y + 1));
    plot_pixel(x, y = (int16_t)(y + 1));
    plot_pixel(x = (int16_t)(x - 1), y = (int16_t)(y + 2));
}
