/* View zoom, panning and aiming. */
#include "view.h"

#include "fixed_math.h"
#include "globals.h"
#include "matrix.h"
#include "memory.h"
#include "tracking.h"

#define ZOOM_MAXIMUM 0x80

void set_zoom_maximum(void) {
    wr_u8(ZOOM_FLAGS, (uint8_t)(rd_u8(ZOOM_FLAGS) | 0x80));
    wr_u16(ZOOM_SCALE, ZOOM_MAXIMUM);
    wr_u8(DISPLAY_UPDATE, 3);
}

#define FULL_TURN 0x7080   /* 360 degrees in 1/80 degree */
#define HALF_TURN 0x3840
#define QUARTER_TURN 0x1C20
#define THREE_QUARTERS 0x5460
#define PAN_STEP 0x1E0     /* 6 degrees */

void pan_view_from_keys(void) {
    int16_t pan, rotate;
    uint8_t keys;

    if (rd_u8(CONTEXT_SELECT) == 0 || rd_u8(PAUSE_A) || rd_u8(CONTEXT_STARTED) || rd_u8(CONTEXT_STATE))
        return;
    pan = rd_s16(VIEW_PAN);
    rotate = rd_s16(VIEW_ROTATE);
    if ((keys = rd_u8(STICK_Y)) != 0) {
        if (!(keys & 0x20)) {
            if (pan >= HALF_TURN) {
                pan = (int16_t)(pan + PAN_STEP);
                if (pan >= FULL_TURN) pan = (int16_t)(pan - FULL_TURN);
            } else {
                pan = (int16_t)(pan + PAN_STEP);
                if (pan >= QUARTER_TURN) return;
            }
        } else {
            if (pan >= HALF_TURN) {
                pan = (int16_t)(pan - PAN_STEP);
                if (pan <= THREE_QUARTERS) return;
            } else {
                pan = (int16_t)(pan - PAN_STEP);
                if (pan < 0) pan = (int16_t)(pan + FULL_TURN);
            }
        }
        wr_s16(VIEW_PAN, pan);
    } else if ((keys = rd_u8(STICK_X)) != 0) {
        if (!(keys & 0x08)) {
            rotate = (int16_t)(rotate + PAN_STEP);
            if (rotate >= FULL_TURN) rotate = (int16_t)(rotate - FULL_TURN);
        } else {
            rotate = (int16_t)(rotate - PAN_STEP);
            if (rotate < 0) rotate = (int16_t)(rotate + FULL_TURN);
        }
        wr_s16(VIEW_ROTATE, rotate);
    }
}

void update_view_octant(void) {
    int16_t angle = rd_u8(CONTEXT_SELECT) ? rd_s16(VIEW_ROTATE) : (int16_t)rd_u32(HEADING_ANGLE);
    int8_t octant = 0;
    while (octant < 7 && angle >= (int16_t)((octant + 1) * 0xE10)) octant++;
    wr_u8(VIEW_OCTANT, (uint8_t)octant);
}

/* The record a context view follows: the viewed record, else a flagged
 * kind-$30 context record, else the player. */
static gaddr followed_record(void) {
    int16_t offset = rd_s16(VIEW_RECORD);
    if (offset) return CONTROL_RECORDS + (gaddr)(int32_t)offset;
    offset = rd_s16(CONTEXT_RECORD);
    if ((rd_u8(CONTROL_RECORDS + (gaddr)(int32_t)offset + 1) & 0x40)
        && rd_u8(CONTROL_RECORDS + (gaddr)(int32_t)offset + 0x62) == 0x30)
        return CONTROL_RECORDS + (gaddr)(int32_t)offset;
    return CONTROL_RECORDS;
}

static int32_t turn_limit(void) {
    uint16_t view;
    if ((int8_t)rd_u8(CONTEXT_STARTED) < 0) return -1;
    if (rd_u8(CONTEXT_STATE)) return 0x230;
    view = (uint16_t)(rd_u16(VIEW_RECORD) | rd_u8(VIEW_SIDE));
    if (view == rd_u16(TRACKED_VIEW)) return 0x7D0;
    wr_u16(TRACKED_VIEW, view);
    return -1;
}

/* A world coordinate relative to the observer's (SUB.L wraps). */
static int32_t relative(int32_t world, gaddr observer) {
    return (int32_t)((uint32_t)world - rd_u32(observer));
}

static void follow_record(void) {
    gaddr record = followed_record();
    int16_t ahead = (rd_u8(record + 0x62) & 0xF0) == 0x30 ? -4 : rd_u8(CONTEXT_STATE) == 6 ? 5 : 1;
    int32_t world[3], limit, pan, rotate;

    local_to_world(record, record + RECORD_INVERSE, 0, 0, ahead, world);
    limit = turn_limit();
    if (rd_u8(POST_INPUT_EVENT)) return;
    if (!rd_u8(CONTEXT_SMOOTH)) limit = -1;
    pan = rd_s16(VIEW_PAN);
    rotate = rd_s16(VIEW_ROTATE);
    track_direction(&pan, &rotate, relative(world[0], OBSERVER + 0xC), relative(world[1], OBSERVER + 0x10),
                    relative(world[2], OBSERVER + 0x14), limit);
    wr_u16(VIEW_PAN, rd_u16(TRACKED_PITCH));
    wr_u16(VIEW_ROTATE, rd_u16(TRACKED_HEADING));
}

void aim_view(void) {
    int k;
    if (rd_u8(CONTEXT_STARTED)) follow_record();
    else pan_view_from_keys();
    two_angle_matrix(rd_u16(VIEW_PAN), rd_u16(VIEW_ROTATE), VIEW_ANGLE_MATRIX);
    scale_matrix_rows(VIEW_ANGLE_MATRIX, MATRIX_ROW_SCALES);
    y_rotation_matrix8(rd_s16(VIEW_ROTATE), LIST_MATRIX);
    for (k = 0; k < 3; k++) wr_s32(ATTITUDE_A + (gaddr)(4 * k), rd_s16(CONTROL_RECORDS + 0x66 + (gaddr)(2 * k)));
}
