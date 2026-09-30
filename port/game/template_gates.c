/* Terrain template selector gates ($C1C40C). */
#include "template_gates.h"

#include "fault.h"
#include "globals.h"
#include "memory.h"

static const gaddr directory[3] = {TEMPLATE_SELECTOR_X, TEMPLATE_SELECTOR_Y, TEMPLATE_SELECTOR_Z};
static const gaddr gates[3] = {TEMPLATE_GATES_X, TEMPLATE_GATES_Y, TEMPLATE_GATES_Z};

void build_template_bit_gates(void) {
    int axis, row;

    for (axis = 0; axis < 3; ++axis)
        for (row = 0; row < 2048; row += 4)
            wr_u32(gates[axis] + (gaddr)row, 0);

    for (axis = 0; axis < 3; ++axis) {
        for (row = 0; row < 128; ++row) {
            int16_t relative = rd_s16(directory[axis] + (gaddr)(2 * row));
            gaddr list;
            int16_t bytes;
            uint32_t n, i;

            if (relative <= 0) {
                wr_u16(0xC4599Eu, (uint16_t)(0x43 + axis));
                fault_hook();
                return;
            }
            list = directory[axis] + (gaddr)(int32_t)relative;
            bytes = rd_s16(list);
            if (bytes < 0) continue;
            n = (uint16_t)((uint16_t)(bytes >> 1) - 1u);
            for (i = 0; i <= n; ++i) {
                uint16_t bit = rd_u16(list + 2 + (gaddr)(2 * i));
                int16_t word_offset = (int16_t)bit >> 5;
                gaddr at = gates[axis] + (gaddr)(16 * row + 4 * word_offset);
                wr_u32(at, rd_u32(at) | (1u << (bit & 31u)));
            }
        }
    }
}
