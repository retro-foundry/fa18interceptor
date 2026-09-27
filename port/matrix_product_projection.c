#include "matrix_product_projection.h"

#include <limits.h>

static int16_t low_word(int32_t value) { return (int16_t)(uint16_t)value; }

/* 68000 DIVS.W divides a signed long dividend by a signed word divisor.  A
 * quotient outside one signed word leaves the destination unmodified. */
static int divs_word(int32_t dividend, int16_t divisor, int16_t *quotient) {
    if (!quotient || divisor == 0) return -1;
    const int64_t value = (int64_t)dividend / divisor;
    if (value < INT16_MIN || value > INT16_MAX) return -1;
    *quotient = (int16_t)value;
    return 0;
}

static int16_t add_word(int16_t left, int16_t right) {
    return (int16_t)(uint16_t)((uint16_t)left + (uint16_t)right);
}

static int16_t reflected_x(int16_t value) {
    return (int16_t)(uint16_t)(UINT16_C(0x013f) - (uint16_t)value);
}

static int16_t reflected_y(int16_t value) {
    return (int16_t)(uint16_t)(UINT16_C(0x00b4) - (uint16_t)value);
}

int fa18_project_matrix_product_tuple(int32_t d0, int32_t d1, int32_t d2,
                                      int16_t *d7,
                                      FA18MatrixProductProjectionState *state,
                                      int16_t *returned_x) {
    if (!d7 || !state || !returned_x) return -1;

    const int16_t divisor = low_word(d2);
    const int32_t x_product = (int32_t)low_word(d0) * 0x00a0;
    const int32_t y_product = (int32_t)low_word(d1) * 0x005a;
    int16_t x, y;
    if (divs_word(x_product, divisor, &x) != 0 ||
        divs_word(y_product, divisor, &y) != 0) return -2;

    x = add_word(x, 0x00a0);
    if (x < 0) x = 0;
    else if (x >= 0x0140) x = 0x013f;
    y = add_word(y, 0x005a);
    if (y < 0) y = 0;
    else if (y >= 0x00b4) y = 0x00b3;

    state->projected_x = reflected_x(x);
    state->projected_y = reflected_y(y);
    if (state->projected_y > state->project_limit) {
        state->projected_x = -1;
        state->projected_y = -1;
        return 0;
    }

    if (*d7 >= 0) return 2;
    for (unsigned step = 0; step != 4; ++step) {
        *d7 = add_word(*d7, 1);
        if (*d7 >= 0) return 2;
    }
    *returned_x = state->projected_x;
    return 1;
}
