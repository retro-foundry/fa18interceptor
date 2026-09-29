/* Glue for component_beyond_bound $C1FC42 and queue_view_key $C1BA86. */
#include "glue.h"
#include "ports_glue.h"

#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "player_input.h"

/* $C1FC42: D0.w offset, D7.w selector. The caller reads D1, D2, D5-D7 and
 * all four flags. */
int glue_C1FC42(void) {
    uint16_t selector = (uint16_t)D(7);
    int16_t offset = (int16_t)D(0);
    gaddr r = rd_u32(BOUND_RECORD);
    int shift = rd_u8(r + 6) & 15, less, bit12 = (selector & 0x1000) != 0;
    uint32_t d2 = D(2), d5 = D(5), d6 = D(6), d7 = D(7);
    int32_t cmp_dst, cmp_src;
    int word_compare;

    SET_W(D(1), selector);
    SET_W(d7, (uint16_t)(((selector & 0x0C00) >> 10) - 1));
    SET_W(d5, (uint16_t)(rd_u8(r + 6) & 15));
    if (((selector & 0x0C00) >> 10) == 1) {
        d2 = (uint32_t)(int32_t)(int16_t)(rd_s16(r + (gaddr)(int32_t)offset + 0x0C) >> shift);
        d7 = (uint32_t)-rd_s32(CONDITION_VALUE);
        cmp_dst = (int32_t)d7;
        cmp_src = (int32_t)d2;
        word_compare = 0;
    } else {
        int second = ((selector & 0x0C00) >> 10) == 2;
        int16_t base = (int16_t)(rd_s16(r + (gaddr)(int32_t)offset + (second ? 0x0A : 0x0E)) >> shift);
        int16_t add = (int16_t)(rd_s16(second ? BOUND_OFFSET_X : BOUND_OFFSET_Z) << (rd_u16(BOUND_SHIFT) & 63));
        SET_W(d7, (uint16_t)-rd_s16(PROJECTION_WORDS + (second ? 0 : 4)));
        SET_W(d6, (uint16_t)add);
        SET_W(d5, rd_u16(BOUND_SHIFT));
        SET_W(d2, (uint16_t)(base + add));
        cmp_dst = (int16_t)d7;
        cmp_src = (int16_t)d2;
        word_compare = 1;
    }
    (void)component_beyond_bound(selector, offset);
    less = cmp_dst < cmp_src;
    if (less) {
        if (bit12) {
            SET_W(d7, 0);
            flags_logic_w(0);
        } else {
            d7 = 1;
            flags_logic_l(1);
        }
    } else {
        /* CMP's N, V, C; BTST #12's Z. */
        uint32_t res = word_compare ? (uint32_t)(uint16_t)(cmp_dst - cmp_src) : (uint32_t)cmp_dst - (uint32_t)cmp_src;
        uint32_t src = (uint32_t)cmp_src, dst = (uint32_t)cmp_dst;
        if (word_compare) {
            FLAG_N = NFLAG_16(res);
            FLAG_V = VFLAG_SUB_16(src, dst, res);
            FLAG_C = CFLAG_16((uint32_t)(uint16_t)dst - (uint32_t)(uint16_t)src);
        } else {
            FLAG_N = NFLAG_32(res);
            FLAG_V = VFLAG_SUB_32(src, dst, res);
            FLAG_C = CFLAG_SUB_32(src, dst, res);
        }
        FLAG_Z = bit12 ? 1 : 0;
    }
    D(2) = d2;
    D(5) = d5;
    D(6) = d6;
    D(7) = d7;
    return glue_return();
}

/* $C1BA86: D0.b raw key. The caller reads A3, D0 and D4. */
int glue_C1BA86(void) {
    uint8_t raw = (uint8_t)D(0);
    int queued = !rd_u8(KEY_TAKEN) && !(raw & 0x80) && (int8_t)rd_u8(KEY_COUNT) < 10;
    queue_view_key(raw);
    D(4) = rd_u8(VIEW_MODE);
    if (queued) {
        SET_W(D(0), rd_u8(KEY_TABLE + raw));
        A(3) = KEY_TRANSLATED;
        D(4) = (uint16_t)(int16_t)(int8_t)rd_u8(KEY_TRANSLATED_WRITE);
    }
    return glue_return();
}
