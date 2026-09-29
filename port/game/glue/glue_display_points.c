/* Display-stream wrappers preserve A1/A2 around the projector. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "projection.h"

void projection_mode_registers(int16_t mode, int entry);

static int display_stream_point(int16_t mode, int entry) {
    uint32_t stream = A(2), a1 = A(1);
    gaddr point = DISPLAY_VERTEX_BASE + (gaddr)(int32_t)rd_s16(stream);
    int16_t size = rd_s16(A(6) - 0x28), radius = (int16_t)D(6);
    draw_display_stream_point(stream, mode, size, radius);
    A(3) = point;
    A(2) = stream + 4;
    /* MOVEM.W into data registers sign-extends each word. */
    D(0) = (uint32_t)(int32_t)rd_s16(point);
    D(1) = (uint32_t)(int32_t)rd_s16(point + 2);
    D(2) = (uint32_t)(int32_t)rd_s16(point + 4);
    projection_mode_registers(mode, entry);
    A(1) = a1;
    A(2) = stream + 4;
    return glue_return();
}

int glue_C1FE24(void) { return display_stream_point(rd_s16(0xC45AB8u), 1); }
int glue_C1FE46(void) { return display_stream_point(-2, 2); }
