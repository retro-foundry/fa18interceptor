/* Scale a view-space point, find its magnitude, and replay the projection. */
#include "glue.h"
#include "ports_glue.h"

#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "projection.h"

void magnitude_registers(void);
void projection_mode_registers(int16_t mode, int entry);

static int16_t shifted(int16_t value, int16_t count) {
    unsigned bits = (uint16_t)count & 63;
    return bits >= 16 ? 0 : (int16_t)((uint16_t)value << bits);
}

void scaled_circle_registers(void) {
    int16_t shift = rd_s16(A(6) - 8);
    int16_t x = shifted(rd_s16(A(3)), shift);
    int16_t y = shifted(rd_s16(A(3) + 2), shift);
    int16_t z = shifted(rd_s16(A(3) + 4), shift);
    int16_t radius = (int16_t)D(6);
    int16_t ax = x < 0 ? (int16_t)-x : x;
    int16_t ay = y < 0 ? (int16_t)-y : y;
    int16_t az = z < 0 ? (int16_t)-z : z;
    uint16_t length;

    D(3) = (uint32_t)(int32_t)rd_s16(A(3));
    D(4) = (uint32_t)(int32_t)rd_s16(A(3) + 2);
    D(5) = (uint32_t)(int32_t)rd_s16(A(3) + 4);
    SET_W(D(3), (uint16_t)x);
    SET_W(D(4), (uint16_t)y);
    SET_W(D(5), (uint16_t)z);
    SET_W(D(0), (uint16_t)ax);
    SET_W(D(1), (uint16_t)ay);
    SET_W(D(2), (uint16_t)az);
    SET_W(D(4), (uint16_t)D(2));
    SET_W(D(3), (uint16_t)D(1));
    SET_W(D(2), (uint16_t)D(0));
    magnitude_registers();
    length = (uint16_t)D(1);
    SET_W(D(7), length);
    D(0) = (uint32_t)(int32_t)x;
    D(1) = (uint32_t)(int32_t)y;
    D(2) = (uint32_t)(int32_t)z;
    D(6) = (uint32_t)(int32_t)radius;
    if ((int16_t)length > 0) {
        uint32_t q = D(6) / length;
        if (q <= 0xFFFFu) D(6) = (D(6) % length) << 16 | q;
        if ((int16_t)D(6) <= 127) goto project;
    }
    SET_W(D(6), 127);
project:
    projection_mode_registers(-4, 2);
}

int glue_C0CFFA(void) {
    draw_scaled_view_circle(A(3), rd_s16(A(6) - 8), (int16_t)D(6));
    scaled_circle_registers();
    return glue_return();
}

int glue_C0CF98(void) {
    gaddr stream = A(2);
    gaddr point = DISPLAY_VERTEX_BASE + (gaddr)(int32_t)rd_s16(stream);
    gaddr saved_a1 = A(1), saved_a5 = A(5);
    draw_scaled_stream_circle(stream, rd_s16(A(6) - 8));
    A(3) = point;
    A(2) = stream + 6;
    SET_W(D(6), rd_u16(stream + 4));
    scaled_circle_registers();
    A(1) = saved_a1;
    A(2) = stream + 6;
    A(5) = saved_a5;
    return glue_return();
}
