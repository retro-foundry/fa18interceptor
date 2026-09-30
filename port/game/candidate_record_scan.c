/* Source block $C26EBE-$C270AA: bounded candidate selection and proximity. */
#include "candidate_record_scan.h"

#include "globals.h"
#include "plane_tests.h"

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

void prepare_candidate_probe(CandidateProbe *probe, gaddr candidate,
                             int pass, int32_t relative_x,
                             int32_t relative_y, int32_t relative_z) {
    int16_t selected_offset = rd_s16(SCRIPT_RECORD);
    int16_t other_offset = rd_s16(VIEW_RECORD);
    int16_t eye_x, eye_z;
    int32_t eye_y;
    int16_t shift = rd_u8(candidate + 0x7D) & 0x0F;
    gaddr list, first;

    eye_x = (int16_t)(((uint32_t)relative_x & 0x3FFFFFu) >> 8);
    eye_y = relative_y >> 8;
    eye_z = (int16_t)(((uint32_t)relative_z & 0x3FFFFFu) >> 8);
    if (other_offset == selected_offset) {
        gaddr selected = CONTROL_RECORDS + (gaddr)(int32_t)other_offset;
        gaddr point = selected + (pass ? 0xB6u : 0xA4u);
        int offset_shift = rd_u8(selected + 0x7D) & 0x0F;
        eye_x = (int16_t)(eye_x + (rd_s16(point) >> offset_shift));
        eye_y = (int32_t)((uint32_t)eye_y +
                          (uint32_t)(int32_t)(rd_s16(point + 2) >> offset_shift));
        eye_z = (int16_t)(eye_z + (rd_s16(point + 4) >> offset_shift));
    }
    list = rd_u8(candidate + 0x62) == 0x20u ? 0xC39168u : 0xC39E48u;
    first = rd_u32(list);
    list += 4;

    probe->candidate_record = candidate;
    probe->face_list = list;
    probe->first_face = first;
    probe->eye_x = eye_x;
    probe->eye_y = eye_y;
    probe->eye_z = eye_z;
    probe->shift = shift;
    probe->first_height = (int16_t)(rd_s16(candidate +
                            (gaddr)(int32_t)(int16_t)(rd_s16(first) + 0xA4) + 2) >> shift);
    probe->below_first_height = (int16_t)eye_y < probe->first_height;
    probe->edge_cursor = first;
    probe->edge_a_offset = 0;
    probe->edge_b_offset = 0;
}

int scan_candidate_lower_faces(CandidateProbe *probe) {
    gaddr stream = probe->face_list;
    while (rd_s16(stream) >= 0) {
        if (!faces_all_behind(&stream, probe->candidate_record, probe->shift,
                              probe->eye_x, probe->eye_y, probe->eye_z)) {
            probe->face_list = stream;
            return 0x20;
        }
    }
    probe->face_list = stream;
    return 0;
}

int candidate_horizontal_edge_negative(const CandidateProbe *probe,
                                       int16_t point_a_offset,
                                       int16_t point_b_offset) {
    gaddr candidate = probe->candidate_record;
    gaddr a = candidate + (gaddr)(int32_t)(int16_t)(point_a_offset + 0xA4);
    gaddr b = candidate + (gaddr)(int32_t)(int16_t)(point_b_offset + 0xA4);
    int16_t ax = rd_s16(a), az = rd_s16(a + 4);
    int16_t bx = rd_s16(b), bz = rd_s16(b + 4);
    int16_t dx = (int16_t)(bx - ax);
    int16_t dz = (int16_t)(az - bz);
    int16_t from_x = (int16_t)-(int16_t)((int16_t)((ax >> probe->shift) +
                                  rd_s16(candidate + 0xC)) - probe->eye_x);
    int16_t from_z = (int16_t)-(int16_t)((int16_t)((az >> probe->shift) +
                                  rd_s16(candidate + 0xE)) - probe->eye_z);
    int32_t first = (int32_t)dx * from_z;
    int32_t second = (int32_t)dz * from_x;
    return (int64_t)first + second < 0;
}

CandidateEdgeRoute walk_candidate_edges(CandidateProbe *probe,
                                        gaddr selected, int pass) {
    gaddr cursor = probe->edge_cursor;

    for (;;) {
        int16_t first = rd_s16(cursor);
        int wrapped = 0;
        if (first < 0) {
            if (!pass && (rd_u8(selected + 4) & 0x40u))
                wr_u32(selected + 0x42, 0);
            probe->edge_cursor = cursor;
            return CANDIDATE_EDGE_NEXT_PASS;
        }
        for (;;) {
            int16_t a = rd_s16(cursor);
            int16_t b;
            cursor += 2;
            if (a < 0) {
                a = (int16_t)(a & 0x0FFF);
                b = (int16_t)(first & 0x0FFF);
                wrapped = 1;
            } else {
                b = (int16_t)(rd_u16(cursor) & 0x0FFFu);
            }
            if (candidate_horizontal_edge_negative(probe, a, b)) {
                if (!wrapped) continue;
                probe->edge_cursor = cursor;
                probe->edge_a_offset = a;
                probe->edge_b_offset = b;
                return CANDIDATE_EDGE_DETAIL;
            }
            if (wrapped) break;
            while (rd_s16(cursor) >= 0) cursor += 2;
            cursor += 2;
            break;
        }
    }
}
