/* View zoom. */
#include "view.h"

#include "globals.h"
#include "memory.h"

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
    if ((keys = rd_u8(PAN_KEYS)) != 0) {
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
    } else if ((keys = rd_u8(ROTATE_KEYS)) != 0) {
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
