/* Selected control-record matrix route ($C2DB18). */
#include "matrix_route.h"

#include "fixed_math.h"
#include "globals.h"
#include "matrix.h"
#include "memory.h"


static void record_angles(gaddr record, uint16_t tuple[3]) {
    tuple[0] = rd_u16(record + 0x66);
    tuple[1] = rd_u16(record + 0x68);
    tuple[2] = rd_u16(record + 0x6A);
}

static void publish_angles(const uint16_t tuple[3]) {
    int i;
    for (i = 0; i < 3; ++i)
        wr_u32(ATTITUDE_A + (gaddr)(4 * i), tuple[i]);
}

void update_control_record_matrix_route(const MatrixRouteHooks *hooks) {
    gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    uint16_t tuple[3] = {0, 0, 0};
    int8_t mode = rd_s8(VIEW_MODE);
    uint8_t record_class = rd_u8(record + 0x62) & 0xF0u;
    int generate = 1;

    y_rotation_matrix8(rd_s16(record + 0x68), LIST_MATRIX);
    if (!rd_u8(CONTEXT_SELECT)) {
        record_angles(record, tuple);
        rotation_matrix8(tuple[0], tuple[1], tuple[2], DISPLAY_CANDIDATE_MATRIX_WIDE);
    }
    tuple[0] = tuple[1] = tuple[2] = 0;

    if (mode == 0 && record_class != 0x30) {
        int8_t selector = rd_s8(MATRIX_ROUTE_SELECTOR);
        if (selector <= 1) {
            record_angles(record, tuple);
            publish_angles(tuple);
            generate = 0;
        } else {
            tuple[1] = selector == 2 ? 0x6E00 : 0x0280;
        }
    } else if (mode <= 11) {
        gaddr table = record_class == 0x30 ? MATRIX_ROUTE_SPECIAL_TABLE : MATRIX_ROUTE_TABLE;
        int16_t index = mode == 0 ? 11 : (int16_t)((int16_t)mode - 1);
        tuple[0] = rd_u16(table + (gaddr)(int32_t)(index * 6));
        tuple[1] = rd_u16(table + (gaddr)(int32_t)(index * 6 + 2));
        tuple[2] = rd_u16(table + (gaddr)(int32_t)(index * 6 + 4));
    } else if (record_class == 0x30) {
        int8_t shifted = (int8_t)(mode - 11);
        int16_t heading = rd_s16(record + 0x66);
        int in_band = heading > 0x19F0 && heading < 0x1E50;
        tuple[0] = shifted == 1 ? (in_band ? 0x3610 : 0x3840)
                                : (in_band ? 0x0230 : 0);
    } else {
        int8_t shifted = (int8_t)(mode - 11);
        int16_t heading = rd_s16(record + 0x66);
        int in_band = heading >= 0x0230 && heading < 0x6E50;
        tuple[0] = shifted == 1 ? (in_band ? 0x5460 : 0x5690)
                                : (in_band ? 0x1C20 : 0x19F0);
    }

    if (generate) {
        int16_t result[3];
        rotation_matrix8(tuple[0], tuple[1], tuple[2], CAMERA_MATRIX);
        scale_matrix_rows(CAMERA_MATRIX, MATRIX_ROW_SCALES);
        build_transform_product(record + 0x80, tuple[0], tuple[1], tuple[2]);
        extract_transform_angles(result, 0);
        if (hooks && hooks->after_transform) hooks->after_transform(hooks->context);
        wr_s32(ATTITUDE_A, result[0]);
        wr_s32(ATTITUDE_A + 4, result[1]);
        wr_s32(ATTITUDE_A + 8, result[2]);
    }
    rotation_matrix8(rd_u16(ATTITUDE_A + 2), rd_u16(ATTITUDE_A + 6),
                     rd_u16(ATTITUDE_A + 10), VIEW_ANGLE_MATRIX);
    if (hooks && hooks->before_final_scale) hooks->before_final_scale(hooks->context);
    scale_matrix_rows(VIEW_ANGLE_MATRIX, MATRIX_ROW_SCALES);
}
