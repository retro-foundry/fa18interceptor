/* Register flow of the selected control-record matrix route ($C2DB18). */
#include "glue.h"
#include "globals.h"
#include "matrix_route.h"
#include "memory.h"

void rotation_matrix8_registers(void); /* glue_batch19.c */
void aim_view_with_registers(void);     /* glue_batch43.c */

static void after_transform(void *context) {
    (void)context;
    A(4) = CONTROL_RECORDS + (uint32_t)(int32_t)rd_s16(VIEW_RECORD) + 0x80u;
    A(2) = MATRIX_TRANSFORM_ROTATION;
    A(3) = MATRIX_TRANSFORM_PRODUCT + 32;
}

static void before_final_scale(void *context) {
    int i;
    (void)context;
    SET_W(D(0), rd_u16(ATTITUDE_A + 2));
    SET_W(D(2), rd_u16(ATTITUDE_A + 6));
    SET_W(D(4), rd_u16(ATTITUDE_A + 10));
    A(1) = VIEW_ANGLE_MATRIX;
    rotation_matrix8_registers();
    for (i = 0; i < 3; ++i) {
        int16_t scale = rd_s16(MATRIX_ROW_SCALES + (gaddr)(2 * i));
        D(i) = (uint32_t)(int32_t)scale;
        D(3 + i) = (uint32_t)(((int32_t)rd_s16(VIEW_ANGLE_MATRIX + 12 +
                              (gaddr)(2 * i)) * rd_s16(MATRIX_ROW_SCALES + 4)) >> 8);
    }
    A(1) = VIEW_ANGLE_MATRIX + 12;
}

void matrix_route_with_registers(void) {
    MatrixRouteHooks hooks = {after_transform, before_final_scale, 0};
    update_control_record_matrix_route(&hooks);
}

int glue_C2DB18(void) {
    matrix_route_with_registers();
    return glue_return();
}

int glue_C2D99C(void) {
    dispatch_matrix_route(aim_view_with_registers, matrix_route_with_registers);
    return glue_return();
}
