/* Register flow for the fixed tuple multiplied by the 2.8 view matrix. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "projection.h"

void projection_mode_registers(int16_t mode, int entry);

static int32_t product(int16_t value, gaddr coefficient) {
    return (int32_t)value * rd_s16(coefficient);
}

int glue_C0DAEE(void) {
    gaddr m = VIEW_ANGLE_MATRIX;
    draw_fixed_matrix_mark();
    SET_W(D(2), 0xE000);
    SET_W(D(3), 0x3800);
    SET_W(D(4), 0xE000);
    SET_W(D(5), (uint16_t)D(2));
    SET_W(D(6), (uint16_t)D(3));
    SET_W(D(0), (uint16_t)D(4));
    D(5) = (uint32_t)product((int16_t)D(5), m);
    D(6) = (uint32_t)product((int16_t)D(6), m + 2);
    D(0) = (uint32_t)product((int16_t)D(0), m + 4);
    D(0) = (uint32_t)((int32_t)(D(0) + D(6) + D(5)) >> 8);
    SET_W(D(5), (uint16_t)D(2));
    SET_W(D(6), (uint16_t)D(3));
    SET_W(D(1), (uint16_t)D(4));
    D(5) = (uint32_t)product((int16_t)D(5), m + 6);
    D(6) = (uint32_t)product((int16_t)D(6), m + 8);
    D(1) = (uint32_t)product((int16_t)D(1), m + 10);
    D(1) = (uint32_t)((int32_t)(D(1) + D(6) + D(5)) >> 8);
    D(2) = (uint32_t)product((int16_t)D(2), m + 12);
    D(3) = (uint32_t)product((int16_t)D(3), m + 14);
    D(4) = (uint32_t)product((int16_t)D(4), m + 16);
    D(2) = (uint32_t)((int32_t)(D(2) + D(3) + D(4)) >> 8);
    D(6) = 8;
    A(0) = m + 18;
    projection_mode_registers(-4, 2);
    return glue_return();
}
