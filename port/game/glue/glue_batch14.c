/* Glue for the paired sin_cos, print_number and square_root. */
#include "glue.h"
#include "ports_glue.h"

#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "numbers.h"

/* $C2E5F6: D0.w, D2.w angles -> D0/D1 = sine/cosine of the first, D2/D3 of
 * the second; D7.w keeps the second lookup's table offset, A0 the table. */
void sin_cos_pair_registers(void);
void sin_cos_pair_registers(void) {
    int16_t first = (int16_t)D(0), second = (int16_t)D(2), off = (int16_t)(second * 2);
    Fixed14 s0, c0, s1, c1;

    sin_cos(first, &s0, &c0);
    sin_cos(second, &s1, &c1);

    SET_W(D(0), s0);
    SET_W(D(1), c0);
    SET_W(D(2), s1);
    SET_W(D(3), c1);
    if (off < 0x708 || (off >= 0xE10 && off < 0x1518)) SET_W(D(7), off);
    else if (off < 0xE10) SET_W(D(7), off - 0x708);
    else SET_W(D(7), off - 0x1518);
    A(0) = SINE_TABLE;
}

int glue_C2E5F6(void) {
    sin_cos_pair_registers();
    return glue_return();
}

/* $C24F76: D0 value, A0 field, D3.w offset, D2 width. Leaves D0 = 0, D1 =
 * the BCD, A0 advanced by the offset, A1 = MODE_TABLE's table. */
int glue_C24F76(void) {
    int16_t offset = (int16_t)D(3);
    int8_t width = (int8_t)D(2);
    gaddr p = A(0) + (uint32_t)(int32_t)offset;
    int checks = width - 1, blanks = 0;

    print_number(A(0), offset, D(0), width);

    D(1) = rd_u32(DISPLAY_VALUE_BCD);
    /* format_hex leaves A0 where its last pointer load was: the first digit
     * (digit loop), then each position its zero scan examined. */
    while (blanks < checks && rd_u8(p + 1 + (gaddr)blanks) == ' ') blanks++;
    if (width <= 0) A(0) = p;
    else if (checks <= 0) A(0) = p + 1;
    else if (blanks < checks) A(0) = p + 1 + (gaddr)blanks;
    else A(0) = p + (gaddr)checks;
    D(0) = 0;
    A(1) = rd_u32(MODE_TABLE);
    return glue_return();
}

/* $C2564E: saves and restores D0-D3. */
int glue_C2564E(void) {
    square_root();
    return glue_return();
}
