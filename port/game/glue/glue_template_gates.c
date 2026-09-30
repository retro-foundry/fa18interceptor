/* Register flow through the three static template gate tables ($C1C40C). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "template_gates.h"

static const gaddr directory[3] = {TEMPLATE_SELECTOR_X, TEMPLATE_SELECTOR_Y, TEMPLATE_SELECTOR_Z};
static const gaddr gates[3] = {TEMPLATE_GATES_X, TEMPLATE_GATES_Y, TEMPLATE_GATES_Z};

int glue_C1C40C(void) {
    int axis, row;

    build_template_bit_gates();
    D(0) = D(1) = D(2) = D(3) = D(4) = D(5) = D(6) = 0;
    A(3) = 0;
    D(7) = 0xFFFFu;
    A(0) = TEMPLATE_GATES_X + 2048;
    A(1) = TEMPLATE_GATES_Y + 2048;
    A(2) = TEMPLATE_GATES_Z + 2048;

    for (axis = 0; axis < 3; ++axis) {
        A(0) = directory[axis];
        A(2) = gates[axis];
        SET_W(D(2), 0x7F);
        for (row = 0; row < 128; ++row) {
            SET_W(D(3), rd_u16(A(0)));
            if ((int16_t)D(3) <= 0) {
                SET_W(D(0), (uint16_t)(0x43 + axis));
                return glue_return();
            }
            A(1) = directory[axis] + (gaddr)(int32_t)(int16_t)D(3);
            SET_W(D(1), rd_u16(A(1)));
            A(1) += 2;
            if ((int16_t)D(1) >= 0) {
                SET_W(D(1), (uint16_t)((int16_t)D(1) >> 1));
                SET_W(D(1), (uint16_t)(D(1) - 1));
                do {
                    gaddr at;
                    SET_W(D(3), rd_u16(A(1)));
                    A(1) += 2;
                    SET_W(D(5), (uint16_t)D(3));
                    SET_W(D(5), (uint16_t)((int16_t)D(5) >> 5));
                    SET_W(D(5), (uint16_t)(D(5) * 4u));
                    SET_W(D(3), (uint16_t)D(3) & 31u);
                    at = A(2) + (gaddr)(int32_t)(int16_t)D(5);
                    D(4) = rd_u32(at) | (1u << ((uint16_t)D(3) & 31u));
                    SET_W(D(1), (uint16_t)(D(1) - 1));
                } while ((uint16_t)D(1) != 0xFFFFu);
            }
            A(0) += 2;
            A(2) += 16;
            SET_W(D(2), (uint16_t)(D(2) - 1));
        }
    }
    return glue_return();
}
