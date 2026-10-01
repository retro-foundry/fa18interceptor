/* $C1FF9C: selected workspace pair fed to the projected line child. */
#include "glue.h"
#include "ports_glue.h"

#include "draw_stream.h"
#include "globals.h"
#include "memory.h"
#include "render_line.h"

void projected_segment_registers(void); /* glue_batch49.c */
void clipped_segment_registers(const int16_t p[3], const int16_t q[3]); /* glue_batch51.c */

static int selected_segment_glue(int clipped) {
    gaddr stream = A(2), first, second;
    uint32_t saved_a1 = A(1), saved_a5 = A(5);
    uint16_t depth0, depth1;
    int16_t p[3], q[3];
    int k;

    if (clipped && rd_s32(PROJECTION_Y) >= -0xC0) {
        draw_selected_segment_near(&stream);
        A(2) = stream;
        D(0) = 0;
        flags_logic_l(0);
        return glue_return();
    }
    first = WORKSPACES + (gaddr)(int32_t)rd_s16(A(2));
    second = WORKSPACES + (gaddr)(int32_t)rd_s16(A(2) + 2);
    depth0 = rd_u16(first + 4);
    depth1 = rd_u16(second + 4);
    for (k = 0; k < 3; ++k) {
        p[k] = rd_s16(first + (gaddr)(2 * k));
        q[k] = rd_s16(second + (gaddr)(2 * k));
    }
    if (clipped) draw_selected_segment_near(&stream);
    else draw_selected_segment(&stream);

    A(4) = clipped ? 0xC2EE4Au : 0xC2ED70u;
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
        if (clipped) clipped_segment_registers(p, q);
        else projected_segment_registers();
    }
    A(1) = saved_a1;
    A(2) = stream;
    A(5) = saved_a5;
    return glue_return();
}

int glue_C1FF9C(void) { return selected_segment_glue(0); }
int glue_C1FFA4(void) { return selected_segment_glue(1); }
