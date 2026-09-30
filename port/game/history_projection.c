/* Slot selection and point preparation of the history renderer ($C0D04C). */
#include "history_projection.h"

#include "globals.h"
#include "fixed_math.h"
#include "projection.h"
#include "view_transform.h"

static int32_t neg_if_negative(int32_t value) {
    return value < 0 ? (int32_t)(0u - (uint32_t)value) : value;
}

int begin_history_projection(HistoryProjectionWork *work) {
    gaddr record;
    int32_t velocity[3], relative[3];
    uint32_t dot;
    int i;

    work->drawn = 0;
    work->active = 0;
    work->previous_radius = -1;
    if (rd_u16(SCRIPT_RECORD) != rd_u16(HISTORY_RECORD)) return 0;
    record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(SCRIPT_RECORD);
    work->record = record;
    work->direction = -1;
    work->remaining = rd_s8(record + 0x3D);
    work->slot_index = rd_s8(HISTORY_COUNT) >= 6 ? rd_s8(HISTORY_NEXT) : 0;
    if (work->remaining <= 0) return 0;
    work->active = 1;
    if (rd_s8(HISTORY_COUNT) < 6)
        work->remaining = (int8_t)((uint8_t)work->remaining - 1u);

    for (i = 0; i < 3; ++i) {
        velocity[i] = rd_s32(record + 0x3E + (gaddr)(4 * i));
        relative[i] = (int32_t)(rd_u32(record + 0x14 + (gaddr)(4 * i)) -
                                rd_u32(PROJECTION_ORIGIN + (gaddr)(4 * i)));
        relative[i] >>= 8;
        if (rd_s16(record + 0x6E) >= 0x100) velocity[i] >>= 8;
    }
    dot = 0;
    for (i = 0; i < 3; ++i)
        dot += (uint32_t)((int32_t)(int16_t)velocity[i] * (int16_t)relative[i]);
    if ((int32_t)dot >= 0) {
        uint8_t slot = (uint8_t)(rd_u8(HISTORY_NEXT) - rd_u8(HISTORY_COUNT));
        work->direction = 0;
        if (rd_s8(HISTORY_COUNT) < 6) slot = (uint8_t)(slot - 1u);
        slot = (uint8_t)(slot + rd_u8(record + 0x3D));
        if ((int8_t)slot < 0) slot = (uint8_t)(slot + rd_u8(HISTORY_COUNT));
        work->slot_index = (int8_t)slot;
    }
    return 1;
}

void prepare_history_projection_point(HistoryProjectionWork *work) {
    int32_t furthest = 0;
    int i;
    int alternate = !!(rd_u8(work->record + 0x20) & 2u);

    work->slot_address = HISTORY_SLOTS +
                         (gaddr)(int32_t)(int16_t)(work->slot_index * 12);
    for (i = 0; i < 3; ++i) {
        work->delta[i] = (int32_t)(rd_u32(work->slot_address + (gaddr)(4 * i)) -
                                   rd_u32(PROJECTION_ORIGIN + (gaddr)(4 * i)));
        work->absolute[i] = neg_if_negative(work->delta[i]);
        if (work->absolute[i] > furthest) furthest = work->absolute[i];
    }
    work->shift = furthest <= 0x4000 ? 0 : furthest <= 0x40000 ? 4 : 8;
    for (i = 0; i < 3; ++i) {
        work->delta[i] >>= work->shift;
        work->absolute[i] >>= work->shift;
    }
    wr_u16(CURRENT_COLOUR, (work->slot_index & 1) ?
           (alternate ? 3 : 8) : (alternate ? 12 : 10));
}

static int16_t word_asr(int16_t value, int count) {
    return (int16_t)(value >> count);
}

static int16_t word_asl(int16_t value, int count) {
    count &= 63;
    return count >= 16 ? 0 : (int16_t)((uint16_t)value << count);
}

static int16_t quarter_between(int16_t start, int16_t end, int shift) {
    int16_t difference = (int16_t)((uint16_t)end - (uint16_t)start);
    return (int16_t)((uint16_t)start + (uint16_t)word_asr(difference, shift));
}

static void project_intermediate(HistoryProjectionWork *work,
                                 const int16_t current[3], int16_t radius,
                                 int fraction) {
    int16_t scaled[3], point[3], scaled_radius;
    int shift = (work->previous_shift - work->shift) & 63;
    int i;

    for (i = 0; i < 3; ++i) scaled[i] = word_asl(work->previous[i], shift);
    if (fraction == 3) {
        for (i = 0; i < 3; ++i)
            point[i] = quarter_between(current[i], scaled[i], 2);
        scaled_radius = quarter_between(radius, work->previous_radius, 2);
    } else {
        int right = fraction == 1 ? 2 : 1;
        for (i = 0; i < 3; ++i)
            point[i] = quarter_between(scaled[i], current[i], right);
        scaled_radius = quarter_between(work->previous_radius, radius, right);
    }
    work->drawn |= (uint16_t)project_view_point_mode(point[0], point[1], point[2],
                                                     -4, work->previous[2], scaled_radius);
}

uint16_t draw_history_projection(HistoryProjectionWork *work) {
    if (!begin_history_projection(work)) return 0;
    for (;;) {
        int16_t vector[3], current[3], radius, length;
        uint32_t radius_pack;
        int32_t rotated[3];
        int i;

        prepare_history_projection_point(work);
        length = (int16_t)magnitude3((int16_t)work->absolute[0],
                                     (int16_t)work->absolute[1],
                                     (int16_t)work->absolute[2]);
        length = word_asr(length, 8 - work->shift);
        for (i = 0; i < 3; ++i) vector[i] = (int16_t)work->delta[i];
        rotate_by_view_matrix(vector, rotated);
        for (i = 0; i < 3; ++i) current[i] = (int16_t)rotated[i];
        radius_pack = length > 0 ?
                      ((0x9C0u % (uint16_t)length) << 16) |
                      (0x9C0u / (uint16_t)length) : 0x7Fu;
        radius = (int16_t)radius_pack;
        if (radius > 0x7F) radius = 0x7F;
        radius_pack = (radius_pack & 0xFFFF0000u) | (uint16_t)radius;
        work->final_interpolated = work->previous_radius >= 0 &&
                                   rd_s8(work->record + 0x3D) > 1;
        work->final_prior_y = work->previous[1];
        if (work->final_interpolated) {
            project_intermediate(work, current, radius, 1);
            project_intermediate(work, current, radius, 2);
            project_intermediate(work, current, radius, 3);
        }
        for (i = 0; i < 3; ++i) work->previous[i] = current[i];
        work->previous_radius = radius;
        work->previous_shift = work->shift;
        if (work->remaining <= 0)
            radius = (int16_t)((uint16_t)radius -
                               (uint16_t)word_asr(radius, 3));
        for (i = 0; i < 3; ++i) work->final_vector[i] = vector[i];
        work->final_y_full = rotated[1];
        work->final_z_full = rotated[2];
        work->final_d6 = (radius_pack & 0xFFFF0000u) | (uint16_t)radius;
        work->drawn |= (uint16_t)project_view_point_mode(current[0], current[1],
                                                         current[2], -4, current[2], radius);

        if (work->direction >= 0) {
            work->slot_index = (int8_t)((uint8_t)work->slot_index - 1u);
            if (work->slot_index < 0)
                work->slot_index = (int8_t)((uint8_t)rd_u8(HISTORY_COUNT) - 1u);
        } else {
            work->slot_index = (int8_t)((uint8_t)work->slot_index + 1u);
            if (work->slot_index > (int8_t)((uint8_t)rd_u8(HISTORY_COUNT) - 1u))
                work->slot_index = 0;
        }
        work->remaining = (int8_t)((uint8_t)work->remaining - 1u);
        if (work->remaining < 0) return work->drawn;
    }
}
