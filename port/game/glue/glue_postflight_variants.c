/* Register replay for the two postflight variant drawing heads. Their shared
 * $C31392 tail still needs a bridge before either parent can be registered. */
#include "glue.h"
#include "glue_postflight_variants.h"

#include "globals.h"
#include "glue_text.h"
#include "memory.h"

#define POSTFLIGHT_TUPLES 0xC3128Au
#define POSTFLIGHT_PREFIX_GATE 0xC45838u
#define POSTFLIGHT_PREFIX_TICK 0xC45883u

static int horizontal_visible(void) {
    return (int16_t)D(0) >= 0 && (int16_t)D(0) < 0x140;
}

static void add_word_register(int n, uint16_t value) {
    uint32_t sum = (uint16_t)D(n) + (uint32_t)value;
    SET_W(D(n), sum);
    FLAG_X = sum > 0xFFFFu ? XFLAG_SET : XFLAG_CLEAR;
}

static void line_final_add_extend(void) {
    int bit, last_bit = -1;
    uint32_t table = rd_u32(PAGE_PLANE_TABLE);
    uint64_t sum;
    if (A(0) != 0xDFF000u || A(2) != table) return;
    for (bit = 0; bit < 4; ++bit)
        if (rd_u8(LINE_PLANES) & (1u << bit)) last_bit = bit;
    if (last_bit < 0) return;
    sum = (uint64_t)rd_u32(table + (gaddr)(4 * (3 - last_bit))) + A(1);
    FLAG_X = sum > 0xFFFFFFFFu ? XFLAG_SET : XFLAG_CLEAR;
}

void postflight_tuple_head_registers(void) {
    int pair, i;
    for (pair = 0; pair < 2; ++pair) {
        gaddr tuple = POSTFLIGHT_TUPLES + (gaddr)(8 * pair);
        for (i = 0; i < 4; ++i)
            D(i) = (uint32_t)(int32_t)rd_s16(tuple + (gaddr)(2 * i));
        A(0) = tuple + 8;
        add_word_register(0, rd_u16(SPAN_ORIGIN_Y));
        if (!horizontal_visible()) continue;
        add_word_register(2, rd_u16(SPAN_ORIGIN_Y));
        if ((int16_t)D(2) < 0 || (int16_t)D(2) >= 0x140) continue;
        add_word_register(1, rd_u16(REDRAW_STATE_WORD));
        add_word_register(3, rd_u16(REDRAW_STATE_WORD));
        if (pair == 0) {
            gaddr saved_a0 = A(0);
            line_registers_to_row(0xC7);
            line_final_add_extend();
            A(0) = saved_a0;
        } else {
            line_registers_to_row(0xC7);
            line_final_add_extend();
        }
    }
}

void postflight_fixed_head_registers(void) {
    SET_W(D(0), 0x9D);
    add_word_register(0, rd_u16(SPAN_ORIGIN_Y));
    if (!horizontal_visible()) return;
    SET_W(D(1), 0xA8);
    add_word_register(1, rd_u16(REDRAW_STATE_WORD));
    plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, 0xC);

    SET_W(D(0), 0x9F);
    add_word_register(0, rd_u16(SPAN_ORIGIN_Y));
    if (!horizontal_visible()) return;
    SET_W(D(1), 0xA8);
    add_word_register(1, rd_u16(REDRAW_STATE_WORD));
    plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, 0xC);

    SET_W(D(0), 0x9E);
    add_word_register(0, rd_u16(SPAN_ORIGIN_Y));
    SET_W(D(1), 0xA8);
    add_word_register(1, rd_u16(REDRAW_STATE_WORD));
    plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, 0xC);
    SET_W(D(0), 0x9E);
    add_word_register(0, rd_u16(SPAN_ORIGIN_Y));
    SET_W(D(1), 0xA7);
    add_word_register(1, rd_u16(REDRAW_STATE_WORD));
    plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, 0xC);
}

/* $C31392-$C3141D. Called while the C tail still holds the original point
 * table, before record submissions replace its entries. */
void postflight_tail_prefix_registers(const PostflightVariantWork *work) {
    uint16_t count;
    if (!rd_u8(POSTFLIGHT_PREFIX_GATE)) {
        flags_logic_b(0);
        return;
    }
    A(2) = work->table;
    D(2) = 10;
    for (;;) {
        uint16_t x = rd_u16(A(2));
        gaddr cursor;
        A(2) += 2;
        SET_W(D(0), x);
        if ((int16_t)x < 0 && x == 0xFFFFu) break;
        if ((int16_t)x < 0) D(0) &= ~0x8000u;
        SET_W(D(1), rd_u16(A(2)));
        A(2) += 2;
        count = (uint16_t)D(2);
        cursor = A(2);
        if ((int16_t)x < 0) {
            pair_registers_colour(0);
        } else {
            plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, 0);
        }
        A(2) = cursor;
        SET_W(D(2), count);
        SET_W(D(2), (uint16_t)(D(2) - 1u));
        if ((uint16_t)D(2) == 0xFFFFu) break;
    }
    /* ADDQ.B #1 of the cadence byte is the tail's last X-writing operation
     * before the vector handoff. The C path has already incremented it. */
    FLAG_X = rd_u8(POSTFLIGHT_PREFIX_TICK) == 0 ? XFLAG_SET : XFLAG_CLEAR;
    D(7) = 0;
    D(6) = 1;
    A(2) = work->table;
    A(0) = work->vector_stream;
    D(0) = (uint32_t)work->vector[0];
    D(1) = (uint32_t)work->vector[1];
    D(2) = (uint32_t)work->vector[2];
    D(3) = D(0) | D(1) | D(2);
    flags_logic_l(D(3));
}

static void postflight_compare_long(uint32_t source, uint32_t dest) {
    uint32_t result = dest - source;
    FLAG_N = NFLAG_32(result);
    FLAG_Z = result;
    FLAG_V = VFLAG_SUB_32(source, dest, result);
    FLAG_C = CFLAG_SUB_32(source, dest, result);
}

static void postflight_compare_word(uint16_t source, uint16_t dest) {
    uint32_t result = (uint16_t)(dest - source);
    FLAG_N = NFLAG_16(result);
    FLAG_Z = result;
    FLAG_V = VFLAG_SUB_16(source, dest, result);
    FLAG_C = CFLAG_16((uint32_t)dest - source);
}

static void postflight_negate_long(int n) {
    uint32_t before = D(n);
    D(n) = 0u - before;
    FLAG_N = NFLAG_32(D(n));
    FLAG_Z = D(n);
    FLAG_V = before == 0x80000000u ? VFLAG_SET : VFLAG_CLEAR;
    FLAG_C = before ? CFLAG_SET : CFLAG_CLEAR;
    FLAG_X = before ? XFLAG_SET : XFLAG_CLEAR;
}

/* ASR.L D4,Dn and the source's BCC / ADDQ.L #1 rounding pair. */
static void postflight_shift_round_by(int n, unsigned shift) {
    uint32_t before = D(n);
    uint32_t carry = shift ? (before >> (shift - 1)) & 1u : 0u;
    D(n) = (uint32_t)((int32_t)before >> shift);
    flags_logic_l(D(n));
    FLAG_C = carry ? CFLAG_SET : CFLAG_CLEAR;
    if (shift) FLAG_X = carry ? XFLAG_SET : XFLAG_CLEAR;
    if (carry) {
        uint32_t old = D(n);
        D(n)++;
        FLAG_N = NFLAG_32(D(n));
        FLAG_Z = D(n);
        FLAG_V = VFLAG_ADD_32(1u, old, D(n));
        FLAG_C = CFLAG_ADD_32(1u, old, D(n));
        FLAG_X = FLAG_C ? XFLAG_SET : XFLAG_CLEAR;
    }
}

/* $C3141E-$C3149B, stopping at the $C3149C classify entry or the
 * $C31714 skip entry. Called immediately after C selection, before later
 * writes change the selected record or its status fields. */
void postflight_tail_select_registers(const PostflightVariantWork *work, int selected) {
    uint16_t offset;
    uint32_t sum;
    (void)selected;
    /* $C31410 reloads each vector after $C31718 advances A0. */
    A(0) = work->vector_stream;
    A(2) = work->table;
    SET_W(D(6), work->records_seen);
    D(0) = (uint32_t)work->vector[0];
    D(1) = (uint32_t)work->vector[1];
    D(2) = (uint32_t)work->vector[2];
    D(3) = D(0) | D(1) | D(2);
    flags_logic_l(D(3));
    D(1) = (uint32_t)work->vector[2];
    D(2) = (uint32_t)work->vector[1];
    D(5) = D(0);
    A(5) = D(1);
    SET_W(D(4), 13);
    flags_logic_w(D(4));
    postflight_shift_round_by(0, 13);
    postflight_shift_round_by(1, 13);
    SET_W(D(3), work->record_word);
    flags_logic_w(D(3));
    FLAG_Z = (D(3) & 0x10u) ? 1 : 0;
    if (!(D(3) & 0x10u)) return;

    SET_W(D(4), D(3));
    flags_logic_w(D(4));
    SET_W(D(4), D(4) & 0xFF00u);
    flags_logic_w(D(4));
    sum = (uint16_t)D(4) * 2u;
    SET_W(D(4), sum);
    FLAG_N = NFLAG_16(D(4));
    FLAG_Z = (uint16_t)D(4);
    FLAG_V = VFLAG_ADD_16((uint16_t)(sum >> 1), (uint16_t)(sum >> 1), (uint16_t)sum);
    FLAG_C = sum > 0xFFFFu ? CFLAG_SET : CFLAG_CLEAR;
    FLAG_X = FLAG_C ? XFLAG_SET : XFLAG_CLEAR;
    offset = (uint16_t)D(4);
    A(1) = CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)offset;
    postflight_compare_word(rd_u16(VIEW_RECORD), offset);
    if (offset == rd_u16(VIEW_RECORD)) goto reject;
    FLAG_Z = (rd_u8(A(1) + 1) & 0x40u) ? 1 : 0;
    if (!(rd_u8(A(1) + 1) & 0x40u)) goto reject;
    FLAG_Z = (rd_u8(A(1) + 3) & 0x80u) ? 1 : 0;
    if (rd_u8(A(1) + 3) & 0x80u) goto reject;

    postflight_negate_long(0);
    postflight_compare_long(0x1Bu, D(0));
    if ((int32_t)D(0) > 0x1B) goto bounds_reject;
    postflight_compare_long((uint32_t)-0x1B, D(0));
    if ((int32_t)D(0) < -0x1B) goto bounds_reject;
    postflight_negate_long(1);
    postflight_compare_long(0x16u, D(1));
    if ((int32_t)D(1) >= 0x16) goto bounds_reject;
    postflight_compare_long((uint32_t)-0x10, D(1));
    if ((int32_t)D(1) >= -0x10) return;

bounds_reject:
    FLAG_Z = 1; /* BTST #4,D3 at $C3148A: bit 4 is set here. */
reject:
    FLAG_Z = (work->record_flags_before_select & 0x40u) ? 1 : 0;
}

/* $C3149C-$C315BF. The source may alter many status bytes, but restores
 * D0/D1 from D5/A5 before the second normalization. X can change in the
 * mode subtraction, the absolute-value negations, and the countdown loop.
 * Call before the C classification changes the countdown byte. */
void postflight_tail_classify_registers(const PostflightVariantWork *work) {
    uint8_t mode = rd_u8(MODE_SELECT);
    uint8_t category = rd_u8(work->record + 0x62) & 0xF0u;
    uint32_t x = (uint32_t)work->source_x;
    uint32_t z = (uint32_t)work->source_z;
    uint32_t abs_x = work->source_x < 0 ? 0u - x : x;
    uint32_t abs_z = work->source_z < 0 ? 0u - z : z;
    if (mode != 0x7Du) {
        /* SUBQ.B #2,D1 precedes the mode-two exit. */
        FLAG_X = mode < 2u ? XFLAG_SET : XFLAG_CLEAR;
        if (mode != 2u) {
            if (work->source_x < 0) FLAG_X = XFLAG_SET; /* NEG.L D1 */
            if ((int32_t)abs_x <= 0x10000) {
                if (work->source_z < 0) FLAG_X = XFLAG_SET; /* NEG.L D1 */
                if ((int32_t)abs_z <= 0x10000 && category != 0x20u &&
                    rd_s16(SELECTED_RECORD) >= 0 && category == 0x10u &&
                    rd_u16(VIEW_RECORD) == 0) {
                    int8_t count = rd_s8(PLAYER_FLAGS_F);
                    if (count > 0) FLAG_X = count == 1 ? XFLAG_SET : XFLAG_CLEAR;
                }
            }
        }
    }
    D(0) = D(5);
    D(1) = A(5);
    flags_logic_l(D(1));
}

/* $C315C0-$C31611. The viewed record supplies the low-nibble ASR count.
 * Stops at $C31612 on success, or just before $C31714 on the reject edge. */
int postflight_tail_normalize_registers(void) {
    gaddr viewed = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    unsigned shift = rd_u8(viewed + 0x63) & 15u;
    SET_B(D(4), rd_u8(viewed + 0x63));
    flags_logic_b(D(4));
    SET_B(D(4), D(4) & 15u);
    flags_logic_b(D(4));
    SET_W(D(4), (uint16_t)(int16_t)(int8_t)D(4));
    flags_logic_w(D(4));
    postflight_shift_round_by(0, shift);
    postflight_shift_round_by(1, shift);
    postflight_negate_long(0);
    postflight_compare_long(0x1Bu, D(0));
    if ((int32_t)D(0) > 0x1B) return 0;
    postflight_compare_long((uint32_t)-0x1B, D(0));
    if ((int32_t)D(0) < -0x1B) return 0;
    postflight_negate_long(1);
    postflight_compare_long(0x16u, D(1));
    if ((int32_t)D(1) >= 0x16) return 0;
    postflight_compare_long((uint32_t)-0x10, D(1));
    return (int32_t)D(1) >= -0x10;
}
