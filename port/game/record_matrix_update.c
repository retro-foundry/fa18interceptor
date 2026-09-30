/* Post-transform record angle and velocity update ($C2D704-$C2D99A).
 * This is a source block of the unregistered $C2D408 parent. */
#include "record_matrix_update.h"

#include "globals.h"
#include "matrix.h"
#include "tracking.h"
#include "control_records.h"

static int16_t sw(uint16_t value) { return (int16_t)value; }
static uint32_t low_word(uint32_t old, uint16_t value) {
    return (old & 0xFFFF0000u) | value;
}
static uint32_t low_byte(uint32_t old, uint8_t value) {
    return (old & 0xFFFFFF00u) | value;
}
static uint32_t signed_word(uint16_t value) {
    return (uint32_t)(int32_t)(int16_t)value;
}

/* DIVS.W leaves remainder above quotient, and leaves the dividend untouched
 * on quotient overflow. The call sites here guard a zero divisor. */
static uint32_t divide_word(uint32_t dividend, int16_t divisor) {
    int32_t value = (int32_t)dividend;
    int32_t quotient = value / divisor;
    if (quotient != (int16_t)quotient) return dividend;
    return ((uint32_t)(uint16_t)(value % divisor) << 16) | (uint16_t)quotient;
}

static uint32_t halve_sum_word(uint32_t lhs, uint32_t rhs) {
    int16_t sum = (int16_t)((uint16_t)lhs + (uint16_t)rhs);
    return low_word(lhs, (uint16_t)(sum >> 1));
}

/* Source branch $C2D4B2-$C2D5F6. Returns 1 for the direct smoothing branch,
 * 2 for the default record velocity inputs, 0 for the zero-angle transform. */
static int choose_record_motion(gaddr record, uint32_t d[8]) {
    int bit0;
    uint8_t controls;
    int16_t angle;
    uint8_t state;

    d[0] = 0;
    d[2] = low_word(d[2], 0);
    d[4] = low_word(d[4], 0);
    if (!rd_u16(STREAM_MODE)) return rd_u8(POST_INPUT_EVENT) ? 0 : 2;

    bit0 = (rd_u8(record + 2) & 1) != 0;
    if (!bit0) {
        d[1] = low_byte(d[1], rd_u8(record + 0x64));
        d[3] = low_byte(d[3], (uint8_t)d[1] & 0x10u);
        if (!((uint8_t)d[3])) {
            wr_u8(record + 0x65, rd_u8(record + 0x65) & 0xC3u);
            return 2;
        }
    }
    state = rd_u8(record + 5);
    if (state != 6 && state != 10) {
        if (bit0) return 2;
        controls = (uint8_t)d[1];
        d[3] = low_byte(d[3], controls & 0x08u);
        if (controls & 0x08) {
            d[3] = low_byte(d[3], controls & 0x04u);
            if (!(controls & 0x04)) {
                wr_u16(record, rd_u16(record) & 0xDFFFu);
                d[3] = low_byte(d[3], controls & 0x40u);
                if (!(controls & 0x40)) return 2;
            }
            d[3] = low_byte(d[3], controls & 0x20u);
            if (!(controls & 0x20)) return 2;
        } else {
            d[3] = low_byte(d[3], controls & 0x02u);
            if (!(controls & 0x02)) {
                wr_u16(record, rd_u16(record) & 0xDFFFu);
                d[3] = low_byte(d[3], controls & 0x20u);
                if (!(controls & 0x20)) return 2;
            }
            d[3] = low_byte(d[3], controls & 0x40u);
            if (!(controls & 0x40)) return 2;
        }
        d[0] = low_byte(d[0], rd_u8(record + 0x62) & 0xF0u);
        if (((uint8_t)d[0]) != 0x10) {
            d[4] = low_word(d[4], 0xFC40u);
            angle = rd_s16(record + 0x6A);
            d[3] = low_word(d[3], (uint16_t)angle);
            if (angle > 0x3840) {
                d[4] = low_word(d[4], (uint16_t)-(int16_t)d[4]);
                if (angle >= 0x6F68) d[4] = low_word(d[4], 0);
            } else if (angle <= 0x118) d[4] = low_word(d[4], 0);
            return 1;
        }
        d[0] = low_byte(d[0], state);
        if (state != 0 && state != 1 && state != 8) return 2;
        if (state == 0 || state == 1) {
            wr_u8(record + 5, 6);
            wr_u16(record + 0x4C, 0x1E);
        }
    }

    wr_u16(record, rd_u16(record) | 0x2000u);
    angle = rd_s16(record + 0x6C);
    d[4] = low_word(d[4], angle < 0x1000 ? 0xFD80u : angle < 0x2000 ? 0xFCE0u : 0xFBF0u);
    angle = rd_s16(record + 0x6A);
    d[3] = low_word(d[3], (uint16_t)angle);
    if (angle > 0x3840) {
        d[4] = low_word(d[4], (uint16_t)-(int16_t)d[4]);
        d[2] = 8;
        if (angle >= 0x6F68) d[2] = 0;
    } else {
        d[2] = 4;
        if (angle <= 0x118) d[2] = 0;
    }
    d[1] = low_byte(d[1], rd_u8(record + 0x65) & 3u);
    d[1] = low_byte(d[1], (uint8_t)d[1] | (uint8_t)d[2]);
    wr_u8(record + 0x65, (uint8_t)d[1]);
    d[1] = low_byte(d[1], rd_u8(record + 0x64) & 6u);
    if ((uint8_t)d[1] == 6) wr_u8(record + 3, rd_u8(record + 3) | 0x10u);
    return 2;
}

static void smooth_record_velocity(gaddr record, uint32_t d[8]) {
    int i;
    for (i = 0; i < 3; i++) {
        int lane = i * 2;
        d[5 + i] = signed_word(rd_u16(record + 0x56 + (gaddr)lane));
        d[5 + i] = halve_sum_word(d[5 + i], d[lane]);
        d[lane] = halve_sum_word(d[lane], d[5 + i]);
        wr_u16(record + 0x56 + (gaddr)lane, (uint16_t)d[lane]);
    }
}

static void load_record_velocity(gaddr record, uint32_t d[8],
                                 RecordMatrixSideHook side_hook, void *context,
                                 RecordMatrixRun *run) {
    int i;
    if ((rd_u8(record + 0x62) & 0xF0u) == 0x10) {
        d[0] = low_byte(d[0], 0x10);
        d[0] = low_word(d[0], rd_u16(COCKPIT_FLAGS) & 0x40u);
        if ((uint16_t)d[0]) {
            run->used_matrix_side = 1;
            if (side_hook) side_hook(context, d);
            else update_matrix_side_record();
        }
        for (i = 0; i < 3; i++) d[2 * i] = low_word(d[2 * i], rd_u16(record + 0x56 + (gaddr)(2 * i)));
        return;
    }

    for (i = 0; i < 3; i++) d[2 * i] = low_word(d[2 * i], rd_u16(record + 0x50 + (gaddr)(2 * i)));
    if (rd_u8(record + 0x20) & 0x20) d[3] = 10;
    else {
        int8_t selector = rd_s8(record + 0x38);
        d[3] = low_byte(d[3], (uint8_t)selector);
        if (selector < 0 && selector != -1) {
            d[3] = low_word(d[3], (uint16_t)d[3] & 0x7Fu);
            if ((int16_t)d[3] == 0) {
                d[3] = low_word(d[3], (uint16_t)d[3] << 8);
                d[3] = low_word(d[3], (uint16_t)d[3] + (uint16_t)d[3]);
                if (rd_u8(PLAYER_FLAGS_G) && rd_u8(record + 0x62) != 1) {
                    d[3] = 5;
                    goto divide;
                }
            }
        }
        d[3] = low_word(d[3], rd_u16(record + 0x26));
        if ((int16_t)d[3] <= 0) goto smoothing;
        if ((int16_t)d[3] > 15) d[3] = 15;
    }
divide:
    for (i = 0; i < 3; i++) d[2 * i] = divide_word(signed_word((uint16_t)d[2 * i]), (int16_t)d[3]);
smoothing:
    smooth_record_velocity(record, d);
}

void update_record_nonclass_matrix(gaddr record, RecordMatrixRun *run,
                                   RecordMatrixSideHook side_hook, void *context) {
    uint32_t *d = run->d;
    int path = choose_record_motion(record, d);
    int i;

    run->used_matrix_side = run->used_depth = 0;
    if (path == 2) load_record_velocity(record, d, side_hook, context, run);
    else if (path == 1) smooth_record_velocity(record, d);

    if (path != 0) {
        d[7] = low_byte(d[7], rd_u8(0xC457AFu));
        d[7] = low_byte(d[7], (uint8_t)d[7] | rd_u8(POST_INPUT_EVENT));
        if ((uint8_t)d[7]) {
            d[0] = low_word(d[0], 0);
            d[2] = low_word(d[2], 0);
            d[4] = low_word(d[4], 0);
        } else {
            d[3] = low_byte(d[3], rd_u8(record + 0x62) & 0xF0u);
            if ((uint8_t)d[3] == 0x10 && (rd_u16(STREAM_MODE) || rd_u8(0xC4578Fu))) {
                MatrixDepthAdjustment depth;
                run->used_depth = 1;
                d[7] = low_word(d[7], rd_u16(record + 0x6A));
                adjust_matrix_record_depth(record, d[3], d[5], d[6], d[7], &depth);
                d[0] = low_word(d[0], (uint16_t)((int16_t)d[0] + (int16_t)depth.d3));
                d[2] = low_word(d[2], (uint16_t)((int16_t)d[2] + (int16_t)depth.d5));
                d[3] = depth.d3;
                d[5] = depth.d5;
                d[6] = depth.d6;
                d[7] = depth.d7;
            }
        }
    }

    run->last_term = build_transform_product(record + 0x80, (uint16_t)d[0],
                                              (uint16_t)d[2], (uint16_t)d[4]);
    extract_transform_angles(run->transformed_angles, &run->transform);
    for (i = 0; i < 3; i++) run->angles[i] = (uint16_t)run->transformed_angles[i];
    run->orientation_written = finish_record_matrix_angles(record, run->angles);
}

int update_record_class30_matrix(gaddr record) {
    uint16_t angle = rd_u16(record + 0x6A);
    int tracked = rd_s16(record + 0x4C) > 0 && !rd_u8(POST_INPUT_EVENT);

    if (tracked) {
        int32_t elevation = rd_s16(record + 0x66);
        int32_t azimuth = rd_s16(record + 0x68);
        if (!rd_u8(CONTEXT_SELECT)) {
            if (sw(angle) <= 0x3840) {
                angle = (uint16_t)(angle - 0x7D0);
                if (sw(angle) < 0) angle = 0;
            } else {
                angle = (uint16_t)(angle + 0x7D0);
                if (sw(angle) >= 0x7080) angle = 0;
            }
        } else {
            angle = 0;
        }
        wr_u16(record + 0x6A, angle);
        track_direction(&elevation, &azimuth, rd_s32(record + 0x3E),
                        rd_s32(record + 0x42), rd_s32(record + 0x46), 0x7D0);
        set_record_orientation(record, rd_u16(TRACKED_PITCH), rd_u16(TRACKED_HEADING), angle);
    } else {
        set_record_orientation(record, rd_u16(record + 0x66),
                               rd_u16(record + 0x68), angle);
    }
    return tracked;
}

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
