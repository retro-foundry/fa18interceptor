/* Post-transform record angle and velocity update ($C2D704-$C2D99A).
 * This is a source block of the unregistered $C2D408 parent. */
#include "record_matrix_update.h"

#include "globals.h"
#include "matrix.h"

static int16_t sw(uint16_t value) { return (int16_t)value; }

static uint16_t half_capped_step(uint16_t distance) {
    int16_t step = sw(distance);
    if (step > 1) {
        step = (int16_t)(step >> 1);
        if (step > 0x50) step = 0x50;
    }
    return (uint16_t)step;
}

/* Bit 4 of the record's third byte asks the source to move its third angle
 * toward the nearest end of the half-turn interval. */
static uint16_t settle_third_angle(gaddr record, uint16_t angle) {
    uint8_t flags = rd_u8(record + 3);
    wr_u8(record + 3, (uint8_t)(flags & ~0x10u));
    if (!(flags & 0x10) || angle == 0x3840) return angle;

    if (sw(angle) > 0x3840) {
        if (sw(angle) < 0x5460)
            angle = (uint16_t)(angle - half_capped_step((uint16_t)(angle - 0x3840)));
        else {
            angle = (uint16_t)(angle + half_capped_step((uint16_t)(0x7080 - angle)));
            if (sw(angle) >= 0x7080) angle = 0;
        }
    } else {
        if (sw(angle) > 0x1C20)
            angle = (uint16_t)(angle + half_capped_step((uint16_t)(0x3840 - angle)));
        else {
            angle = (uint16_t)(angle - half_capped_step(angle));
            if (sw(angle) < 0) angle = 0;
        }
    }
    return angle;
}

static uint16_t move_first_angle(uint16_t angle, uint16_t step) {
    if (sw(angle) > 0x3840) {
        angle = (uint16_t)(angle + step);
        if (sw(angle) >= 0x7080) angle = 0;
    } else {
        angle = (uint16_t)(angle - step);
        if (sw(angle) <= 0) angle = 0;
    }
    return angle;
}

static int16_t abs_word(int16_t value) {
    return value < 0 ? (int16_t)-value : value;
}

static void add_response_to_velocity(gaddr record) {
    uint8_t old = rd_u8(record + 3);
    gaddr velocity;
    int16_t delta;

    wr_u8(record + 3, (uint8_t)(old | 0x04u));
    if (!(old & 0x04u) && !rd_u8(record + 0x28)) return;
    velocity = record + ((old & 0x04u) ? 0x58 : 0x56);
    delta = rd_s16(0xC3D69Cu);
    if (rd_s16(velocity) < 0) delta = (int16_t)-delta;
    wr_u16(velocity, (uint16_t)(rd_u16(velocity) + delta));
}

static uint16_t speed_adjusted_first_angle(gaddr record, uint16_t old_first) {
    uint16_t speed = (uint16_t)(rd_u16(record + 0x26) +
                      (uint16_t)((int16_t)(0x50 - rd_s16(record + 0x26)) >> 2));
    uint16_t angle = (uint16_t)(old_first + speed);
    wr_u16(record + 0x56, 0);
    wr_u16(record + 0x26, speed);
    if (sw(angle) >= 0x7080) angle = 0;
    else if (sw(angle) < 0x3840 && sw(angle) > 0x12C0)
        angle = (uint16_t)(angle - 0x50);
    return angle;
}

int finish_record_matrix_angles(gaddr record, uint16_t angles[3]) {
    uint16_t old_first = rd_u16(record + 0x66);
    uint8_t state = rd_u8(record + 5);
    uint8_t flags = rd_u8(record + 3);

    angles[2] = settle_third_angle(record, angles[2]);
    if (state == 10) angles[0] = move_first_angle(old_first, 0x50);
    else if (flags & 0x40) angles[0] = move_first_angle(old_first, 0xA0);
    else {
        if (rd_u8(RECORD_UPDATES_ON) && !rd_u16(STREAM_MODE) &&
            (rd_u8(record + 0x20) & 0x02)) {
            angles[0] = speed_adjusted_first_angle(record, old_first);
            goto write_orientation;
        }
        if (rd_u8(RECORD_UPDATES_ON) && !(flags & 0x80) &&
            (rd_u8(record + 0x62) & 0xF0) == 0x10) {
            int16_t half = (int16_t)(sw(old_first) >> 1);
            int16_t threshold;
            if (half < 0x1C20) {
                if (half > 0xE10) half = 0xE10;
                half = (int16_t)(half + 0x3840);
            }
            threshold = (int16_t)(0x4650 - half);
            threshold = (int16_t)(((uint32_t)(uint16_t)threshold * 0x6Cu) >> 8);
            threshold = (int16_t)(threshold + (int16_t)(((uint32_t)(uint16_t)(rd_s16(record + 0x22) >> 8) *
                                                        (uint16_t)threshold) >> 6));
            if (threshold < rd_s16(record + 0x6E)) {
                wr_u16(record + 0x26, 0);
                wr_u8(record + 0x20, (uint8_t)(rd_u8(record + 0x20) & ~0x04u));
            } else {
                wr_u8(record + 0x20, (uint8_t)(rd_u8(record + 0x20) | 0x04u));
                if (!(rd_u16(record + 0x56) | rd_u16(record + 0x58) | rd_u16(record + 0x5A)) ||
                    (int16_t)(threshold >> 1) >= rd_s16(record + 0x6E)) {
                    angles[0] = speed_adjusted_first_angle(record, old_first);
                    goto write_orientation;
                }
            }
        }
        if (sw(old_first) > 0x1770 && sw(old_first) < 0x5910) {
            int16_t yaw_distance = abs_word((int16_t)(angles[1] - rd_u16(record + 0x68)));
            int yaw_far = yaw_distance > 0x4B0 && yaw_distance < 0x6BD0;
            int16_t roll_distance = abs_word((int16_t)(angles[2] - rd_u16(record + 0x6A)));
            if (yaw_far) yaw_distance = abs_word((int16_t)(yaw_distance - 0x3840));
            if (roll_distance > 0x4B0 && roll_distance < 0x6BD0) {
                roll_distance = abs_word((int16_t)(roll_distance - 0x3840));
                if (abs_word((int16_t)(roll_distance - yaw_distance)) >= 0x320) {
                    add_response_to_velocity(record);
                    return 0;
                }
            } else if (yaw_far) {
                add_response_to_velocity(record);
                return 0;
            }
        }
        wr_u8(record + 3, (uint8_t)(rd_u8(record + 3) & ~0x04u));
    }

write_orientation:
    set_record_orientation(record, angles[0], angles[1], angles[2]);
    return 1;
}
