/* Glue for check_post_input_expiry $C10D8A, seed_projection $C1C54E and
 * condition_table_matches $C09AB8. */
#include "glue.h"
#include "ports_glue.h"

#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "post_input.h"

/* $C1C54E: every register is live after it. D0-D2 = TARGET_POINT >> 8;
 * from the record: D3 = its z masked, D4/D5 = PROJECTION_ORIGIN y/z,
 * D6 = 0 (a product with the zero eye x), D7 = eye z * the matrix's +$9C,
 * A2 the record; in a context update_target_point's A0. */
/* $C09AB8: A0 table. Result in Z (MOVEQ #1 / #0); D6.b = the last byte it
 * compared, A1 = the table. */
/* X after a key search: ADDQ.W #2 from -2 carries only on the first pass.
 * Returns the entry (or 0) and sets *x. */
static gaddr search_x(gaddr list, gaddr base, int16_t key, int *x) {
    gaddr p = list;
    int passes = 0, index = -2;
    int16_t value;
    do {
        index += 2;
        passes++;
        value = rd_s16(p);
        p += 2;
        if (value < 0) break;
    } while (value != key);
    *x = passes == 1;
    if (value < 0) return 0;
    while (rd_s16(p) >= 0) p += 2;
    p += 2;
    return base + (gaddr)(int32_t)rd_s16(p + (gaddr)(int32_t)index);
}

void condition_registers(void);
void condition_registers(void) {
    int last_byte, x, result = condition_table_scan(A(0), &last_byte);
    gaddr p = search_x(A(0), A(0), rd_s16(CONDITION_KEY_B), &x);
    if (p && search_x(p, A(0), rd_s16(CONDITION_KEY_A), &x)) x = rd_s32(CONDITION_VALUE) != 0; /* NEG.L */
    if (last_byte >= 0) SET_B(D(6), last_byte);
    A(1) = A(0);
    D(0) = (uint32_t)result;
    flags_logic_l((uint32_t)result);
    FLAG_X = x ? XFLAG_SET : XFLAG_CLEAR;
}

int glue_C09AB8(void) {
    condition_registers();
    return glue_return();
}

int glue_C1C54E(void) {
    int context = rd_u8(CONTEXT_SELECT) != 0;
    int i;
    if (context) {
        if (rd_u8(TARGET_ENABLED)) A(0) = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    } else {
        gaddr r = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
        uint8_t type = rd_u8(r + 0x62);
        int16_t z = type == 0x30 ? -5 : type == 0x11 ? 20 : 18;
        A(2) = r;
        D(6) = 0;
        D(7) = (uint32_t)((int32_t)z * rd_s16(r + 0x9C));
    }
    seed_projection();
    if (!context) {
        gaddr r = A(2);
        D(3) = rd_u32(r + 0x1C) & 0x3FFFFF;
        D(4) = rd_u32(PROJECTION_ORIGIN + 4);
        D(5) = rd_u32(PROJECTION_ORIGIN + 8);
    }
    for (i = 0; i < 3; i++) D(i) = (uint32_t)(rd_s32(TARGET_POINT + (gaddr)(4 * i)) >> 8);
    return glue_return();
}
