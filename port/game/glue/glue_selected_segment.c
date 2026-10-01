/* $C1FF9C: selected workspace pair fed to the projected line child. */
#include "glue.h"
#include "ports_glue.h"

#include "draw_stream.h"
#include "globals.h"
#include "memory.h"

void projected_segment_registers(void); /* glue_batch49.c */

int glue_C1FF9C(void) {
    gaddr stream = A(2), first, second;
    uint32_t saved_a1 = A(1), saved_a5 = A(5);
    uint16_t depth0, depth1;

    draw_selected_segment(&stream);
    first = WORKSPACES + (gaddr)(int32_t)rd_s16(A(2));
    second = WORKSPACES + (gaddr)(int32_t)rd_s16(A(2) + 2);
    depth0 = rd_u16(first + 4);
    depth1 = rd_u16(second + 4);

    A(4) = 0xC2ED70u;
    A(3) = WORKSPACES;
    A(2) = stream;
    A(0) = SEGMENT_POINTS + 10;
    SET_W(D(1), rd_u16(stream - 4));
    SET_W(D(6), depth0);
    SET_W(D(7), depth1);
    SET_W(D(6), depth0 & depth1);
    if ((int16_t)(depth0 & depth1) < 0) {
        D(0) = 0;
        flags_logic_l(D(0));
    } else {
        A(0) += 2;
        projected_segment_registers();
    }
    A(1) = saved_a1;
    A(2) = stream;
    A(5) = saved_a5;
    return glue_return();
}
