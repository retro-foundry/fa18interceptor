/* Source block $C26EBE-$C270AA: bounded candidate selection and proximity. */
#include "candidate_record_scan.h"

#include "globals.h"

static int32_t distance_component(int32_t position, int32_t relative) {
    int32_t difference = (int32_t)((uint32_t)position - (uint32_t)relative);
    return difference < 0 ? (int32_t)(0u - (uint32_t)difference) : difference;
}

static int inside_cube(gaddr candidate, int32_t x, int32_t y, int32_t z,
                       int32_t bound, int subtract_half_velocity) {
    int32_t position[3];
    int32_t relative[3] = {x, y, z};
    int i;

    for (i = 0; i < 3; i++) {
        position[i] = rd_s32(candidate + 0x14u + (gaddr)(4 * i));
        if (subtract_half_velocity)
            position[i] = (int32_t)((uint32_t)position[i] -
                                    (uint32_t)(rd_s32(candidate + 0x3Eu +
                                                       (gaddr)(4 * i)) >> 1));
        if (distance_component(position[i], relative[i]) > bound) return 0;
    }
    return 1;
}

void scan_candidate_record(CandidateScanWork *work,
                           int32_t relative_x, int32_t relative_y,
                           int32_t relative_z) {
    int16_t offset = rd_s16(SCRIPT_RECORD);
    gaddr selected = CONTROL_RECORDS + (gaddr)(int32_t)offset;
    uint16_t selected_first = rd_u16(selected);
    uint16_t selected_second = rd_u16(selected + 2);
    uint8_t selected_byte_5e = rd_u8(selected + 0x5E);
    uint8_t selected_class = rd_u8(selected + 0x62) & 0xF0u;

    work->selected_record = selected;
    work->candidate_record = 0;
    work->candidate_offset = 0;
    work->candidate_class = 0;
    work->route = CANDIDATE_SCAN_DONE;
    wr_u8(0xC4589Fu, 0);
    wr_u8(selected + 4, rd_u8(selected + 4) & 0x3Fu);
    if (selected_class == 0x20u) return;

    for (;;) {
        gaddr candidate;
        uint16_t first, second;
        uint8_t kind, candidate_class;
        int32_t far_bound, near_bound;

        offset = (int16_t)(offset + 0x200);
        if (offset > 0x1E00) return;
        candidate = CONTROL_RECORDS + (gaddr)(int32_t)offset;
        first = rd_u16(candidate);
        second = rd_u16(candidate + 2);
        if (!(first & 0x40u) || (first & 0x600u)) continue;
        if (rd_u8(candidate + 0x5E) == selected_byte_5e) continue;
        if (!(selected_first & 0x08u) && !(first & 0x08u)) continue;

        kind = rd_u8(candidate + 0x62);
        candidate_class = kind & 0xF0u;
        if (kind != 0x15u && candidate_class == 0x30u) continue;
        if (kind == 0x15u) {
            far_bound = 0x5000;
            near_bound = 0x1000;
        } else if (candidate_class == 0x20u) {
            far_bound = 0xA0000;
            near_bound = 0;
        } else if (!selected_class || selected_class != candidate_class) {
            far_bound = 0x10000;
            near_bound = 0x5000;
        } else if (!(selected_second & 0x0100u) && !(second & 0x0100u)) {
            far_bound = 0x7000;
            near_bound = 0x1000;
        } else {
            far_bound = 0x6000;
            near_bound = 0x800;
        }

        if (!inside_cube(candidate, relative_x, relative_y, relative_z,
                         far_bound, 0)) continue;
        wr_u8(0xC4589Fu, 1);
        wr_u16(candidate + 2, second | 1u);
        work->candidate_record = candidate;
        work->candidate_offset = offset;
        work->candidate_class = candidate_class;

        if (candidate_class == 0x20u) {
            work->route = CANDIDATE_SCAN_CLASS_20;
            return;
        }
        if (!inside_cube(candidate, relative_x, relative_y, relative_z,
                         near_bound, 0) &&
            !inside_cube(candidate, relative_x, relative_y, relative_z,
                         near_bound, 1)) continue;

        if (first & 0x1000u) {
            wr_u16(candidate, first | 0x0200u);
            if (candidate_class) {
                wr_u8(candidate + 0x20, rd_u8(candidate + 0x20) | 2u);
                wr_u16(HISTORY_RECORD, (uint16_t)offset);
                wr_u8(candidate + 2, rd_u8(candidate + 2) | 0x10u);
                wr_u8(candidate + 5, 1);
            }
        }
        work->route = CANDIDATE_SCAN_SIDE_RESULT;
        return;
    }
}

int candidate_side_result(gaddr candidate, gaddr selected) {
    uint16_t selected_first = rd_u16(selected);
    uint16_t candidate_first = rd_u16(candidate);

    if (selected_first & 0x08u) {
        if (!rd_u8(selected + 0x5E)) {
            gaddr metadata = rd_u32(0xC1AB74u);
            if (rd_u8(selected + 0x62) == 0 && !(candidate_first & 0x08u))
                wr_u16(metadata + 0x44, rd_u16(metadata + 0x44) + 1u);
            else if (rd_u8(selected + 0x62) == 1 && !(candidate_first & 0x08u))
                wr_u16(metadata + 0x40, rd_u16(metadata + 0x40) + 1u);
        }
        if (candidate_first & 0x08u) {
            wr_u8(candidate + 0x20, rd_u8(candidate + 0x20) | 0x80u);
            wr_u8(selected + 0x20, rd_u8(selected + 0x20) | 0x80u);
            return 0x40;
        }
    }

    wr_u8(candidate + 0x20, rd_u8(candidate + 0x20) & 0x7Fu);
    wr_u8(selected + 0x20, rd_u8(selected + 0x20) & 0x7Fu);
    if (!rd_u16(SCRIPT_RECORD) && !(rd_u8(candidate + 0x62) & 0xF0u)) {
        gaddr metadata = rd_u32(0xC1AB74u);
        wr_u16(metadata + 0x48, rd_u16(metadata + 0x48) + 1u);
    }
    return 0x40;
}

int candidate_terminal_result(gaddr selected) {
    int32_t height = rd_s32(selected + 0x10);
    int32_t result = height;

    if (rd_s8(selected + 0x7B) >= 0 &&
        (rd_u8(selected + 0x62) & 0xF0u) == 0x10u &&
        !(rd_u8(selected + 0x7B) & 0x0Fu)) {
        int shift = rd_u8(selected + 0x7D) & 0x0Fu;
        int offset;
        for (offset = 0xA6; offset <= 0xB2; offset += 6) {
            result = (int32_t)((uint32_t)height +
                               (uint32_t)(int32_t)(rd_s16(selected +
                                    (gaddr)offset) >> shift));
            if (result < 0) return 0x10;
        }
    }
    return result < 0 ? 0x10 : 0;
}
