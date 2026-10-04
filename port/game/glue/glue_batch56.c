/* Glue for the 8-pixel text entries $C32AB4, $C32AA6 and $C32AA4: the
 * line at the view's column (and, for the last two, its decimal digits
 * first). Their callers read every register: the loop's per-character
 * loads and, from the last glyph plotted, plot_glyph8's leftovers. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "text.h"
#include "glue_text.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

/* The loop from $C32B00, after the C drew the line. On entry D0.w holds
 * count - 1 and its high word the x origin; A1/A2 the layout and chars. */
static void text_regs(void) {
    int16_t count = (int16_t)((uint16_t)D(0) + 1), x_origin = (int16_t)(D(0) >> 16);
    uint8_t colour = rd_u8(CURRENT_COLOUR + 1);
    int i, k;

    A(4) += D(7);
    SET_W(D(7), 0x142);
    A(5) = rd_u32(PAGE_PLANE_TABLE);
    SET_W(D(6), (uint16_t)(W(6) * 2));
    for (i = 0; i < count; i++) {
        uint8_t ch;
        SET_W(D(5), rd_u16(A(1)));
        SET_W(D(3), rd_u16(A(1) + 2));
        A(1) += 4;
        ch = rd_u8(A(2));
        SET_B(D(4), ch);
        A(2) += 1;
        if (ch == ' ') continue;
        SET_W(D(2), (uint16_t)(x_origin + W(6) + W(5)));
        if (W(2) < 0 || W(2) >= 0x28) continue;
        SET_W(D(5), (uint16_t)(W(5) + W(6)));
        D(5) = SEXT(D(5)) + A(4);
        SET_W(D(4), (uint16_t)((ch - 0x20) * 2));
        A(3) = SMALL_GLYPHS + SEXT(rd_u16(SMALL_GLYPHS + SEXT(D(4))));
        D(4) = A(3);
        D(1) = rd_u32(A(5)) + D(5);
        if (D(1) & 1) continue;
        for (k = 0; k < 4; k++) {
            uint16_t d3 = (uint16_t)D(3);
            int shift;
            D(1) = rd_u32(A(5) + (gaddr)(4 * k)) + D(5);
            SET_W(D(2), ((colour >> (3 - k)) & 1) ? 0x0BFA : 0x0B0A);
            SET_W(D(2), (uint16_t)(D(2) | d3));
            shift = (int)((D(2) >> 12) & 15);
            /* plot_glyph8's epilogue (glue_misc.c): its last row as written. */
            D(2) = (uint32_t)shift << 16 | d3;
            D(3) = (rd_u32(D(1) + 4 * 40) & 0xFFFF0000u) | d3;
            A(0) = D(4) + 5;
            A(3) = D(1) + 5 * 40;
        }
    }
    SET_W(D(0), 0xFFFF);
}

static Text line_from_registers(void) {
    Text t;
    t.count = (int)(uint16_t)D(0) + 1;
    t.x_origin = (int16_t)(D(0) >> 16);
    t.layout = A(1);
    t.chars = A(2);
    t.column = 0;
    t.rows = A(4);
    return t;
}

void text_in_view_registers(void) {
    SET_W(D(6), rd_u16(SPAN_ORIGIN));
    D(7) = rd_u32(REDRAW_STATE_LONG);
    text_regs();
}



/* The digits ($C32AD0): D2.w count - 1, A0 the end; D4.b nonzero keeps
 * leading zeros. */
void bcd_text_registers(void) {
    int count = (int)(uint16_t)D(2) + 1, keep = (uint8_t)D(4) != 0, k;
    uint32_t bcd = rd_u32(DISPLAY_VALUE_BCD);
    gaddr end = A(0);

    SET_W(D(6), rd_u16(SPAN_ORIGIN));
    D(7) = rd_u32(REDRAW_STATE_LONG);
    SET_W(D(5), D(2));
    for (k = 0; k < count; k++) {
        SET_W(D(1), (uint16_t)((bcd & 15) + 0x30));
        bcd >>= 4;
    }
    D(3) = bcd;
    A(0) = end - (gaddr)count;
    SET_W(D(2), 0xFFFF);
    if (!keep) {
        SET_W(D(5), (uint16_t)(W(5) - 1));
        for (;;) {
            uint8_t c = rd_u8(A(0));
            A(0) += 1;
            if (c != ' ') break; /* a zero the C blanked reads back as a space */
            SET_W(D(5), (uint16_t)(W(5) - 1));
            if (W(5) < 0) break;
        }
    }
    text_regs();
}

static int digits_glue(void) {
    Text t = line_from_registers();
    print_bcd_in_view(A(0), (int)(uint16_t)D(2) + 1, (uint8_t)D(4) != 0, &t);
    bcd_text_registers();
    return glue_return();
}
