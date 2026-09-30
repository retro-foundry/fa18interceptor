/* Register bridge for candidate-record stage $C26EBE. */
#include "glue.h"
#include "ports_glue.h"

#include "candidate_record_update.h"
#include "globals.h"

void candidate_face_registers(gaddr input_stream, gaddr final_stream,
                              int16_t shift, int16_t eye_z, int behind);

static void terminal_registers(gaddr selected) {
    int32_t height = rd_s32(selected + 0x10);
    D(1) = D(2) = (uint32_t)height;
    if (rd_s8(selected + 0x7B) >= 0 &&
        (rd_u8(selected + 0x62) & 0xF0u) == 0x10u &&
        !(rd_u8(selected + 0x7B) & 0x0Fu)) {
        int shift = rd_u8(selected + 0x7D) & 0x0F;
        int offset;
        SET_W(D(3), shift);
        for (offset = 0xA6; offset <= 0xB2; offset += 6) {
            D(1) = (uint32_t)((uint32_t)height +
                   (uint32_t)(int32_t)(rd_s16(selected + (gaddr)offset) >> shift));
            if ((int32_t)D(1) < 0) break;
        }
    }
}

int glue_C26EBE(void) {
    CandidateUpdateWork work = {0};
    int result = update_candidate_record(&work, (int32_t)D(2),
                                         (int32_t)D(3), (int32_t)D(4));
    if (work.path == CANDIDATE_UPDATE_SIDE) {
        gaddr candidate = work.scan.candidate_record;
        uint16_t first = rd_u16(candidate);
        A(0) = candidate;
        SET_W(D(1), (first & 0x1000u) ?
                     (0x1000u | work.scan.candidate_class) : 0);
        if ((rd_u16(work.scan.selected_record) & 8u) &&
            !rd_u8(work.scan.selected_record + 0x5E))
            A(2) = rd_u32(0xC1AB74u);
    }
    if (work.path == CANDIDATE_UPDATE_EARLY ||
        work.path == CANDIDATE_UPDATE_TERMINAL) A(0) = CONTROL_RECORDS;
    if (work.path == CANDIDATE_UPDATE_TERMINAL ||
        work.path == CANDIDATE_UPDATE_LEVEL) A(3) = work.scan.selected_record;
    if (work.path == CANDIDATE_UPDATE_TERMINAL &&
        work.level.route == CANDIDATE_LEVEL_TERMINAL)
        terminal_registers(work.scan.selected_record);
    if (work.path == CANDIDATE_UPDATE_TERMINAL) {
        A(0) = work.level.register_a0;
        if (work.level.route == CANDIDATE_LEVEL_STOP_ZERO &&
            work.level.volume_stream) {
            A(0) = work.level.volume_stream;
            D(1) = (uint32_t)work.level.y;
            D(2) = (uint32_t)work.level.z;
        }
        if (work.had_probe) {
            A(2) = (uint32_t)(int32_t)work.probe.eye_x;
            A(4) = work.final_geometry_a4;
        } else if (work.scan.near_bound_assigned) A(2) = (uint32_t)work.scan.near_bound;
        if (rd_s32(work.scan.selected_record + 0x10) <= 0x7FFF)
            A(5) = work.level.level_cursor;
    }
    if (work.path == CANDIDATE_UPDATE_SIDE && work.scan.near_bound_assigned &&
        !((rd_u16(work.scan.selected_record) & 8u) &&
          !rd_u8(work.scan.selected_record + 0x5E)))
        A(2) = (uint32_t)work.scan.near_bound;
    if (work.path == CANDIDATE_UPDATE_FACE) {
        A(0) = CONTROL_RECORDS;
        A(1) = (uint32_t)work.probe.eye_y;
        A(2) = (uint32_t)(int32_t)work.probe.eye_x;
        A(3) = work.scan.candidate_record;
        candidate_face_registers(work.probe.last_face_input,
                                 work.final_geometry_a4, work.probe.shift,
                                 work.probe.eye_z, 1);
    }
    if (work.path == CANDIDATE_UPDATE_LEVEL && work.level.terminal_from_planes) {
        A(0) = work.level.plane_normals_end;
        A(2) = work.level.volume_stream;
        A(4) = work.level.bounds_stream;
        A(5) = work.level.level_cursor;
        D(3) = (uint32_t)work.level.plane_d3;
        D(4) = (uint32_t)work.level.plane_d4;
        terminal_registers(work.scan.selected_record);
    }
    D(0) = (uint32_t)result;
    flags_logic_l(D(0));
    return glue_return();
}
