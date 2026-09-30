/* Level and linked-volume selection ($C27504-$C27666). */
#include "candidate_level_walk.h"

#include "candidate_record_scan.h"
#include "globals.h"

static int point_inside_bounds(gaddr bounds, int32_t x, int32_t y, int32_t z) {
    return (int16_t)y <= rd_s16(bounds) &&
           x > rd_s16(bounds + 2) && x < rd_s16(bounds + 4) &&
           z > rd_s16(bounds + 6) && z < rd_s16(bounds + 8);
}

void select_candidate_level(CandidateLevelWork *work) {
    gaddr selected = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(SCRIPT_RECORD);
    gaddr level = rd_u32(0xC459C6u);
    int16_t count = rd_s8(0xC4585Eu);
    int16_t iteration = 0;
    int16_t index = -1;

    work->selected = selected;
    work->volume_stream = 0;
    work->bounds_stream = 0;
    work->level_cursor = level;
    work->register_a0 = CONTROL_RECORDS;
    work->table_index = index;
    work->level_iteration = iteration;
    work->x = work->y = work->z = 0;
    work->x_adjustment = work->z_adjustment = 0;
    work->route = rd_s32(selected + 0x10) > 0x7FFF ?
                  CANDIDATE_LEVEL_STOP_ZERO : CANDIDATE_LEVEL_TERMINAL;
    if (work->route == CANDIDATE_LEVEL_STOP_ZERO) return;

    for (;;) {
        uint16_t level_word = rd_u16(level);
        gaddr volume, bounds;
        int8_t delta_x, delta_z;
        int32_t x, y, z;

        level -= 0x18;
        iteration = (int16_t)(iteration + 1);
        work->level_cursor = level;
        work->level_iteration = iteration;
        if ((level_word & 0x50u) || index >= 10) goto next_level;
        index = (int16_t)(index + 1);
        work->table_index = index;
        work->register_a0 = 0xC4D790u;
        volume = rd_u32(0xC4D790u + (gaddr)(4 * index));
        if ((int32_t)volume < 0) goto next_level;
        work->register_a0 = volume;
        bounds = 0xC4D7BCu + (gaddr)(0x100 * index) + 2;
        delta_x = (int8_t)(rd_u8(selected + 7) - rd_u8(bounds));
        bounds += 1;
        if (delta_x > 1 || delta_x < -1) goto next_level;
        delta_z = (int8_t)(rd_u8(selected + 9) - rd_u8(bounds));
        bounds += 1;
        if (delta_z > 1 || delta_z < -1) goto next_level;

        work->x_adjustment = (int32_t)delta_x * 0x4000;
        work->z_adjustment = (int32_t)delta_z * 0x4000;
        x = (int32_t)((uint32_t)(int32_t)rd_s16(selected + 0xC) +
                      (uint32_t)work->x_adjustment);
        y = rd_s32(selected + 0x10);
        z = (int32_t)((uint32_t)(int32_t)rd_s16(selected + 0xE) +
                      (uint32_t)work->z_adjustment);
        work->x = x;
        work->y = y;
        work->z = z;
        if (!point_inside_bounds(bounds, x, y, z)) goto next_level;

        wr_u8(0xC4589Fu, 1);
        volume += 0xA;
        bounds += 0xA;
        for (;;) {
            gaddr next;
            if (point_inside_bounds(bounds, x, y, z)) {
                work->volume_stream = volume;
                work->bounds_stream = bounds;
                work->route = (rd_u16(selected + 2) & 2u) ?
                              CANDIDATE_LEVEL_PLANES : CANDIDATE_LEVEL_STOP_ZERO;
                return;
            }
            next = rd_u32(bounds + 0xA);
            if ((int32_t)next < 0) break;
            bounds = next;
            volume = rd_u32(volume + 0xA);
            work->register_a0 = volume;
        }
next_level:
        if (iteration >= count || index >= 10) return;
    }
}

/* The three MULS products and the final ADD.L sign test ($C276E2-$C27706). */
static int plane_is_nonnegative(CandidateLevelWork *work,
                                gaddr *normals, gaddr *bounds,
                                int32_t x, int32_t y, int32_t z) {
    int32_t dx, dz, first, third;
    int16_t dy;
    gaddr n = *normals + 6, b = *bounds;
    dx = (int32_t)((uint32_t)x - (uint32_t)(int32_t)rd_s16(b));
    dy = (int16_t)(y - rd_s16(b + 2));
    dz = (int32_t)((uint32_t)z - (uint32_t)(int32_t)rd_s16(b + 4));
    first = (int32_t)((uint32_t)((int32_t)rd_s16(n) * (int16_t)dx) +
                      (uint32_t)((int32_t)rd_s16(n + 2) * dy));
    third = (int32_t)rd_s16(n + 4) * (int16_t)dz;
    work->plane_d3 = dx;
    work->plane_d4 = (int32_t)(((uint32_t)y & 0xFFFF0000u) | (uint16_t)dy);
    work->plane_normals_end = n + 6;
    *normals = n + 6;
    *bounds = b + 6;
    return (int32_t)((uint32_t)first + (uint32_t)third) >= 0;
}

static int16_t shift_point(gaddr selected, int offset, int shift) {
    return (int16_t)(rd_s16(selected + (gaddr)offset) >> shift);
}

static void point_for_planes(const CandidateLevelWork *work, int point_offset,
                             int32_t xyz[3]) {
    int shift = rd_u8(work->selected + 0x7D) & 0x0F;
    xyz[0] = (int16_t)(rd_s16(work->selected + 0xC) +
                       shift_point(work->selected, point_offset, shift));
    xyz[1] = (int32_t)((uint32_t)rd_s32(work->selected + 0x10) +
                       (uint32_t)(int32_t)shift_point(work->selected,
                                                        point_offset + 2, shift));
    xyz[2] = (int16_t)(rd_s16(work->selected + 0xE) +
                       shift_point(work->selected, point_offset + 4, shift));
    xyz[0] = (int32_t)((uint32_t)xyz[0] + (uint32_t)work->x_adjustment);
    xyz[2] = (int32_t)((uint32_t)xyz[2] + (uint32_t)work->z_adjustment);
}

static int any_plane_nonnegative(CandidateLevelWork *work,
                                  const int32_t xyz[3]) {
    gaddr normals = work->volume_stream + 0x10;
    gaddr bounds = work->bounds_stream + 0xE;
    int16_t remaining = rd_s16(work->volume_stream + 0xE);
    for (;;) {
        if (plane_is_nonnegative(work, &normals, &bounds,
                                 xyz[0], xyz[1], xyz[2]))
            return 1;
        remaining = (int16_t)(remaining - 1);
        if (remaining <= 0) return 0;
    }
}

static int special_point_result(CandidateLevelWork *work) {
    int32_t xyz[3];
    point_for_planes(work, 0xAA, xyz);
    if (!any_plane_nonnegative(work, xyz)) return 0x20;
    point_for_planes(work, 0xB0, xyz);
    if (any_plane_nonnegative(work, xyz)) {
        work->terminal_from_planes = 1;
        return candidate_terminal_result(work->selected);
    }
    return 0x20;
}

static void settle_zero_class_velocity(CandidateLevelWork *work,
                                       const int32_t accumulated[3]) {
    gaddr selected = work->selected;
    int32_t response[3];
    uint16_t level_flags;
    gaddr level = work->level_cursor + 0x18;
    gaddr linked;
    int i;

    for (i = 0; i < 3; i++)
        response[i] = (int32_t)((uint32_t)rd_s32(selected + 0x3E + (gaddr)(4 * i)) -
                                (uint32_t)accumulated[i]);
    level_flags = rd_u16(level);
    linked = rd_u32(level + 2);
    if (!(level_flags & 0x50u) && rd_s32(linked + 4) > 0) {
        gaddr data = rd_u32(linked + 4);
        int16_t word = rd_s16(data);
        int16_t offset;
        if (word < 0) offset = word;
        else offset = (word & 0x4000u) ? rd_s16(data + 2) : rd_s16(data + 4);
        if (offset != -1 &&
            (rd_u8(data + 7 + (gaddr)(uint16_t)(offset & 0x0FFF)) & 0x10u)) {
            for (i = 0; i < 3; i++)
                response[i] = (int32_t)((uint32_t)response[i] +
                               (uint32_t)(rd_s32(selected + 0x3E +
                                            (gaddr)(4 * i)) >> 1));
        }
    }
    for (i = 0; i < 3; i++) {
        gaddr address = selected + 0x14 + (gaddr)(4 * i);
        wr_u32(address, rd_u32(address) - (uint32_t)response[i]);
    }
}

int walk_candidate_level_planes(CandidateLevelWork *work) {
    gaddr selected = work->selected;
    int32_t xyz[3] = {work->x, (int16_t)work->y, work->z};
    int32_t accumulated[3] = {0, 0, 0};
    uint8_t kind = rd_u8(selected + 0x62) & 0xF0u;
    int remaining_steps = 8;
    int i;

    for (i = 0; i < 3; i++)
        xyz[i] = (int32_t)((uint32_t)xyz[i] +
                 (uint32_t)(int32_t)shift_point(selected, 0xA4 + 2 * i,
                                             rd_u8(selected + 0x7D) & 0x0F));
    xyz[1] = (int16_t)xyz[1];
    if (!kind) {
        for (i = 0; i < 3; i++) {
            int32_t velocity = rd_s32(selected + 0x3E + (gaddr)(4 * i));
            accumulated[i] = (int32_t)(0u - (uint32_t)(velocity >> 3));
            xyz[i] = (int32_t)((uint32_t)xyz[i] - (uint32_t)(velocity >> 8));
        }
    }
    for (;;) {
        if (!any_plane_nonnegative(work, xyz)) {
            if (!kind) settle_zero_class_velocity(work, accumulated);
            return 0x20;
        }
        if (kind == 0x10u) return special_point_result(work);
        if (kind) return rd_s32(selected + 0x10) < 0 ? 0x10 : 0;
        if (--remaining_steps < 0) return rd_s32(selected + 0x10) < 0 ? 0x10 : 0;
        for (i = 0; i < 3; i++) {
            int32_t velocity = rd_s32(selected + 0x3E + (gaddr)(4 * i));
            accumulated[i] = (int32_t)((uint32_t)accumulated[i] +
                                       (uint32_t)(velocity >> 3));
            xyz[i] = (int32_t)((uint32_t)xyz[i] + (uint32_t)(velocity >> 8));
        }
    }
}
