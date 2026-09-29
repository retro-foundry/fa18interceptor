/* Glue for the message line $C322EE (message_line.c). Its caller reads
 * every register: the last small-text loop's leftovers, or on an early
 * return the values loaded on the way. The branches are taken from the
 * state before the C runs, and every line it may draw is probed then (a
 * line's cells depend only on its plane and place, not on its mode). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "message_line.h"
#include "stages.h"
#include "text.h"
#include "glue_text.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

#define LAYOUT 0xC31998u

/* C3267A's registers after the info page's number (D0 value, D2.w count
 * - 1, D4.b keep, A2 end), glue_batch13.c's without the C. */
static void decimal_registers(int count) {
    uint32_t bcd = rd_u32(DISPLAY_VALUE_BCD);
    SET_W(D(1), ((count <= 8 ? (bcd >> (4 * (count - 1))) : 0) & 15) + '0');
    SET_W(D(2), 0xFFFF);
    D(3) = count >= 8 ? 0 : bcd >> (4 * count);
}

static void line_entry(const SmallText *line, int in_view) {
    D(0) = (uint32_t)(line->count - 1);
    A(1) = line->layout;
    A(2) = line->chars;
    A(5) = (uint32_t)(int32_t)line->x_origin;
    SET_W(D(5), (uint16_t)line->plane_offset);
    D(6) = (uint32_t)line->mode << 16 | (uint16_t)line->column;
    D(7) = in_view ? rd_u32(REDRAW_STATE_LONG) : 0;
    A(4) = line->rows;
}

/* A probed line drawn with `mode`. */
static void probed_line_registers(SmallTextProbe *p, uint16_t mode, int in_view) {
    p->text.mode = mode;
    line_entry(&p->text, in_view);
    small_text_registers(p);
}

int glue_C322EE(void) {
    int16_t selected = rd_s16(SELECTED_RECORD), page = rd_s16(INFO_PAGE);
    uint16_t flags = rd_u16(COCKPIT_FLAGS), shown = rd_u16(MESSAGE_SHOWN), drawn = rd_u16(MESSAGE_DRAWN);
    int8_t delay = (int8_t)rd_u8(INFO_DELAY), redraws = (int8_t)rd_u8(INFO_REDRAWS);
    int8_t request = (int8_t)rd_u8(INFO_REQUEST);
    int context = rd_u8(CONTEXT_SELECT) != 0, always = rd_u8(TEXT_ALWAYS) != 0;
    int8_t message_redraws = (int8_t)rd_u8(MESSAGE_REDRAWS);
    SmallTextProbe planes[3], below;
    SmallText line;
    int k, info = 0;
    uint8_t kind;
    int16_t first, second, third;

    for (k = 0; k < 3; k++) {
        line = small_text_line(26, MESSAGE_LINE, LAYOUT, 0x1E0C, 0xC, (int16_t)(4 * (k == 2 ? 3 : k)), 0x0FCA, 1);
        small_text_probe(&planes[k], &line);
    }
    line = small_text_line(26, MESSAGE_LINE, LAYOUT, 0x1B14, 0xC, 0xC, 0x0F3A, 0);
    small_text_probe(&below, &line);

    draw_message_line();

    SET_W(D(2), (uint16_t)selected);
    if (selected >= 0) {
        SET_W(D(3), flags & 0x81);
        if (!W(3) && (int8_t)(delay - 1) < 0) info = 1;
    }
    if (info) {
        uint16_t now;
        A(1) = CONTROL_RECORDS;
        if (redraws <= 0) {
            if (request < 0) {
                SET_W(D(0), (uint16_t)(page + 1));
                if (W(0) > 3) D(0) = 1;
            } else if (!(request & 1)) {
                return glue_return();
            } else {
                SET_W(D(0), (uint16_t)page);
            }
            D(0) |= 0x8000;
            page = W(0);
            redraws = 2;
        }
        SET_W(D(0), (uint16_t)page);
        now = (uint16_t)D(0);
        D(0) &= ~0x8000u;
        if (!(now & 0x8000)) {
            if ((int8_t)(redraws - 1) < 0) return glue_return();
        } else {
            gaddr record = CONTROL_RECORDS + SEXT((uint16_t)selected);
            A(4) = record;
            A(2) = MESSAGE_LINE + 13;
            if (W(0) >= 1 && W(0) <= 3) {
                int count = W(0) == 1 ? 5 : W(0) == 2 ? 3 : 4;
                D(2) = (uint32_t)(count - 1);
                D(4) = W(0) == 2;
                decimal_registers(count);
            }
        }
    } else if (page == 0) {
        SET_W(D(0), shown);
        if (shown == drawn) {
            if (redraws <= 0) return glue_return();
        } else {
            SET_W(D(0), (uint16_t)((shown & 0xFF) * 16));
            D(1) = 0xFFFF; /* MOVEQ #26 run down by DBRA */
        }
    } else {
        D(1) = 0xFFFF;
    }

    /* The draw ($C325A6). */
    kind = (uint8_t)(rd_u8(MESSAGE_FLAGS) >> 6);
    first = kind == 0 ? 0 : kind == 1 ? 1 : 2;
    second = kind < 2 ? 2 : 1;
    third = kind == 0 ? 1 : 0;
    SET_B(D(3), kind);
    if (!context || always) probed_line_registers(&planes[first], 0x0FCA, 1);
    line = small_text_line(26, MESSAGE_LINE, LAYOUT, 0x1E0C, 0xC, 0, 0x0FCA, 1);
    line_entry(&line, 1);
    A(4) = 0x1E0C;
    SET_W(D(5), (uint16_t)(4 * (second == 2 ? 3 : second)));
    SET_W(D(3), (uint16_t)(4 * (third == 2 ? 3 : third)));
    if (context) {
        if (!always) return glue_return();
        probed_line_registers(&below, 0x0F3A, 0);
        return glue_return();
    }
    if (message_redraws < 0) return glue_return();
    probed_line_registers(&planes[second], 0x0F0A, 1);
    SET_W(D(3), (uint16_t)(4 * (third == 2 ? 3 : third)));
    probed_line_registers(&planes[third], 0x0F0A, 1);
    return glue_return();
}

/* Not registered yet: the first recording's calls differ in SORT_LIST_NEXT,
 * BOUND_SHIFT and the sorted list (see CURRENT_PORT_HANDOFF.md).
 *
 * $C1E328 saves and restores every register it uses; `all` is its
 * caller's byte at -$2C(A6). */
int glue_C1E328(void) {
    sort_display_list(rd_u8(A(6) - 0x2C) != 0);
    return glue_return();
}
