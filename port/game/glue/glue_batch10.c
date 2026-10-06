/* Glue for message reset, mode_offset, stream skip,
 * cell steps, the 2.8 rotation matrix and the cached display value. */
#include "glue.h"
#include "glue_text.h"
#include "ports_glue.h"

#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "stages.h"

/* $C11312: leaves D0 = 0 and A0 past the two cleared queue words. */
int glue_C11312(void) {
    reset_message_sequence();
    D(0) = 0;
    A(0) = MESSAGE_QUEUE + 4;
    flags_logic_b(0); /* final MOVE.B D0,MESSAGE_STATE_D */
    return glue_return();
}

/* $C287DA: D0.w = the offset; D1.b = the mode (extended when used); A0 =
 * the table when looked up. */
int glue_C287DA(void) {
    int8_t mode = (int8_t)rd_u8(MODE_SELECT);
    int16_t offset = mode_offset();

    D(0) = 0;
    SET_B(D(1), mode);
    if (mode != 0x7E && mode != 0x7F) {
        SET_W(D(1), mode);
        A(0) = rd_u32(MODE_TABLE);
        SET_W(D(0), offset);
    }
    return glue_return();
}

/* $C1FEF2: A2 advanced; D0 = 0 (MOVEQ flags), D1.w = -1 after the DBRA. */
int glue_C1FEF2(void) {
    A(2) = skip_stream_records(A(2));
    SET_W(D(1), 0xFFFF);
    D(0) = 0;
    flags_logic_l(D(0));
    return glue_return();
}

/* Add the cell offsets from the grid origin to the positions in D2 and D4;
 * D1 keeps the last step. */
static int add_cell_steps(int16_t column, int16_t row) {
    int32_t dx = cell_step((int16_t)((rd_u16(GRID_ORIGIN_X) & 0xFF) - column));
    int32_t dz = cell_step((int16_t)((rd_u16(GRID_ORIGIN_Z) & 0xFF) - row));
    D(2) += (uint32_t)dx;
    D(4) += (uint32_t)dz;
    D(1) = (uint32_t)dz;
    return glue_return();
}

/* $C1ECFC: grid origin minus the caller's cell at -$1C/-$1E(A6). */
int glue_C1ECFC(void) {
    return add_cell_steps(rd_s16(A(6) - 0x1C), rd_s16(A(6) - 0x1E));
}

/* $C1ECD4: the entry's own cell (+$0E high/low bytes) minus the caller's. */
int glue_C1ECD4(void) {
    uint16_t cell = rd_u16(A(1) + 0x0E);
    int32_t dx = cell_step((int16_t)((cell >> 8) - rd_s16(A(6) - 0x1C)));
    int32_t dz = cell_step((int16_t)((cell & 0xFF) - rd_s16(A(6) - 0x1E)));
    D(2) += (uint32_t)dx;
    D(4) += (uint32_t)dz;
    D(1) = (uint32_t)dz;
    return glue_return();
}

/* The registers $C2E346 leaves for angle `angle` and output `out`. */
void y_rotation8_registers(int16_t angle, gaddr out);
void y_rotation8_registers(int16_t angle, gaddr out) {
    int16_t tenths = (int16_t)(angle >> 3), off = (int16_t)(tenths * 2);
    Fixed14 s, c;

    sin_cos(tenths, &s, &c);
    SET_W(D(4), -(s >> 6));
    SET_W(D(5), c >> 6);
    SET_W(D(6), s >> 6);
    SET_W(D(7), off);
    if (off >= 0x708 && off < 0xE10) SET_W(D(7), off - 0x708);
    else if (off >= 0x1518) SET_W(D(7), off - 0x1518);
    A(0) = SINE_TABLE;
    A(1) = out + 18;
}

/* $C2E346: D4.w angle, A1 output (advanced). */
int glue_C2E346(void) {
    int16_t angle = (int16_t)D(4);
    gaddr out = A(1);

    y_rotation_matrix8(angle, out);
    y_rotation8_registers(angle, out);
    return glue_return();
}

/* $C31C20: A1 cache, D0.w value -> D0 (value, cached value, or -1) and
 * its flags; D2.w keeps the cached word it loaded. */
void cached_value_registers(gaddr cache, int16_t value) {
    int16_t cached = rd_s16(cache);
    int forced = (int8_t)rd_u8(REDRAW_FIRST + 1) > 0;

    if (!forced) {
        SET_W(D(2), cached);
        if (cached < 0 || (cached != value && !(rd_u8(DISPLAY_FORCE) & 1) && rd_u8(POST_INPUT_EVENT))) {
            SET_W(D(2), cached & 0x7FFF);
            SET_W(D(0), cached & 0x7FFF);
        } else if (cached == value || (rd_u8(DISPLAY_FORCE) & 1)) {
            D(0) = 0xFFFFFFFFu; /* MOVEQ #-1 */
        }
    }
    flags_logic_w(D(0));
}
