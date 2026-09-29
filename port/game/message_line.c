/* The cockpit message line (see message_line.h). */
#include "message_line.h"

#include "globals.h"
#include "memory.h"
#include "numbers.h"
#include "text.h"

#define LINE_LENGTH  26
#define ENTRY_SIZE   28
#define LINE_LAYOUT  0xC31998u /* small text, 4 pixels a character */

/* Strings in the game, each ending in a zero. */
#define TEXT_BLANK   0xC326B8u /* 26 spaces */
#define TEXT_ALT     0xC326D3u /* "ALT:" */
#define TEXT_HDG     0xC326D8u /* "HDG:" */
#define TEXT_SPD     0xC326DDu /* "SPD:" */
#define TEXT_MIG29   0xC326E2u
#define TEXT_MIG25   0xC326E9u
#define TEXT_AF1     0xC326F0u
#define TEXT_F16     0xC326F7u
#define TEXT_CRUISE  0xC326FEu
#define TEXT_NO_SIG  0xC32705u
#define TEXT_707     0xC3270Cu
#define TEXT_767     0xC32713u

#define SMALL_DRAW    0x0FCA
#define SMALL_INVERSE 0x0F3A
#define SMALL_CLEAR   0x0F0A

static void copy_string(gaddr to, gaddr from) {
    uint8_t c;
    while ((c = rd_u8(from++)) != 0) wr_u8(to++, c);
}

static void set_kind(uint8_t kind) {
    wr_u8(MESSAGE_FLAGS, (uint8_t)((rd_u8(MESSAGE_FLAGS) & 0x3F) | kind));
}

static int8_t count_down(gaddr at) {
    int8_t left = (int8_t)(rd_u8(at) - 1);
    wr_u8(at, (uint8_t)left);
    return left;
}

/* DIVU.W: on overflow the dividend's low word stays. */
static int16_t divu_word(int32_t dividend, uint16_t divisor) {
    uint32_t q = (uint32_t)dividend / divisor;
    return (int16_t)(q > 0xFFFF ? (uint16_t)dividend : (uint16_t)q);
}

/* The selected record's type, with the line's kind: 1 friendly, 2 hostile. */
static void write_type(gaddr record) {
    gaddr name = 0;

    set_kind(0x80);
    if (!(rd_u8(record + 0x20) & 0x40)) {
        name = TEXT_NO_SIG;
        set_kind(0);
    } else {
        switch (rd_u8(record + 0x62)) {
        case 0x12: name = TEXT_MIG29; break;
        case 0x13: name = TEXT_MIG25; break;
        case 0x14: name = TEXT_AF1; set_kind(0x40); break;
        case 0x16: name = TEXT_707; set_kind(0x40); break;
        case 0x17: name = TEXT_767; set_kind(0x40); break;
        case 0x10: name = TEXT_F16; set_kind(0x40); break;
        case 0x15: name = TEXT_CRUISE; set_kind(0x80); break;
        }
    }
    if (name) copy_string(MESSAGE_LINE + 4, name);
}

static void write_page(gaddr record, uint16_t page) {
    gaddr label = MESSAGE_LINE + 13;

    if (page == 1) {
        copy_string(label, TEXT_ALT);
        format_decimal(MESSAGE_LINE + 0x17, (uint32_t)(rd_s32(record + 0x18) >> 10) * 5, 5, 0);
    } else if (page == 2) {
        copy_string(label, TEXT_HDG);
        format_decimal(MESSAGE_LINE + 0x16, (uint32_t)(int32_t)divu_word((int32_t)rd_s16(record + 0x68) >> 3, 10), 3,
                       1);
    } else if (page == 3) {
        int16_t speed = 0;
        copy_string(label, TEXT_SPD);
        if (!(rd_u8(record) & 0x80)) {
            speed = rd_s16(record + 0x6E);
            if (speed < 0) speed = (int16_t)-speed;
        }
        format_decimal(MESSAGE_LINE + 0x16, (uint32_t)(int32_t)divu_word(speed, 12), 4, 0);
    }
}

/* The info pages for the selected record: 1 when the line is to be drawn. */
static int info_line(int16_t selected) {
    uint16_t page;

    if ((int8_t)rd_u8(INFO_REDRAWS) <= 0) {
        int8_t request = (int8_t)rd_u8(INFO_REQUEST);
        if (request < 0) {
            wr_u8(INFO_REQUEST, 1);
            page = (uint16_t)(rd_u16(INFO_PAGE) + 1);
            if ((int16_t)page > 3) page = 1;
        } else if (!(request & 1)) {
            return 0;
        } else {
            page = rd_u16(INFO_PAGE);
        }
        wr_u16(INFO_PAGE, (uint16_t)(page | 0x8000));
        wr_u8(INFO_REDRAWS, 2);
    }
    page = rd_u16(INFO_PAGE);
    if (!(page & 0x8000)) return count_down(INFO_REDRAWS) >= 0;
    page &= 0x7FFF;
    wr_u16(INFO_PAGE, page);
    copy_string(MESSAGE_LINE, TEXT_BLANK);
    {
        gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)selected;
        write_type(record);
        write_page(record, page);
    }
    return 1;
}

static void copy_entry(uint16_t entry) {
    gaddr from = MESSAGE_TABLE + (gaddr)(ENTRY_SIZE * (entry & 0xFF)) + 1;
    int i;
    for (i = 0; i <= LINE_LENGTH; i++) wr_u8(MESSAGE_LINE + (gaddr)i, rd_u8(from + (gaddr)i));
}

/* The message entry: 1 when the line is to be drawn. */
static int message_text(void) {
    int16_t page = rd_s16(INFO_PAGE);

    if (page == 0) {
        uint16_t entry = rd_u16(MESSAGE_SHOWN);
        if (entry == rd_u16(MESSAGE_DRAWN)) {
            if ((int8_t)rd_u8(INFO_REDRAWS) <= 0) return 0;
            wr_u8(INFO_REDRAWS, (uint8_t)(rd_u8(INFO_REDRAWS) - 1));
            return 1;
        }
        wr_u16(MESSAGE_DRAWN, entry);
        copy_entry(entry);
        wr_u8(INFO_REDRAWS, 2);
        return 1;
    }
    /* The info has gone: a blank line (entry 0). */
    wr_u16(INFO_PAGE, page > 0 ? 0xFFFF : 0);
    copy_entry(0);
    return 1;
}

static void line_in_plane(int16_t plane, uint16_t mode) {
    SmallText line = small_text_line(LINE_LENGTH, MESSAGE_LINE, LINE_LAYOUT, 0x1E0C, 0xC, plane, mode, 1);
    if (rd_u8(CONTEXT_SELECT) && !rd_u8(TEXT_ALWAYS)) return;
    draw_small_text(&line);
}

static void draw_line(void) {
    uint8_t kind = (uint8_t)(rd_u8(MESSAGE_FLAGS) >> 6);
    int16_t first = kind == 0 ? 0 : kind == 1 ? 4 : 0xC, second = kind < 2 ? 0xC : 4, third = kind == 0 ? 4 : 0;

    line_in_plane(first, SMALL_DRAW);
    if (rd_u8(CONTEXT_SELECT)) {
        SmallText below = small_text_line(LINE_LENGTH, MESSAGE_LINE, LINE_LAYOUT, 0x1B14, 0xC, 0xC, SMALL_INVERSE, 0);
        if (!rd_u8(TEXT_ALWAYS)) return;
        draw_small_text(&below);
        return;
    }
    if ((int8_t)rd_u8(MESSAGE_REDRAWS) < 0) return;
    wr_u8(MESSAGE_REDRAWS, (uint8_t)(rd_u8(MESSAGE_REDRAWS) - 1));
    line_in_plane(second, SMALL_CLEAR);
    line_in_plane(third, SMALL_CLEAR);
}

void draw_message_line(void) {
    int16_t selected = rd_s16(SELECTED_RECORD);

    if (selected >= 0 && !(rd_u16(COCKPIT_FLAGS) & 0x81) && count_down(INFO_DELAY) < 0) {
        wr_u8(INFO_DELAY, 0xFF);
        if (!info_line(selected)) return;
    } else if (!message_text()) {
        return;
    }
    draw_line();
}
