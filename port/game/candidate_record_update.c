/* Parent paths of the candidate-record stage ($C26EBE). */
#include "candidate_record_update.h"

#include "globals.h"

int update_candidate_record(CandidateUpdateWork *work,
                            int32_t relative_x, int32_t relative_y,
                            int32_t relative_z) {
    int16_t scan_offset;

    work->path = CANDIDATE_UPDATE_EARLY;
    work->result = 0;
    work->pass = 0;
    work->had_probe = 0;
    work->final_geometry_a4 = 0;
    scan_candidate_record(&work->scan, relative_x, relative_y, relative_z);
    if ((rd_u8(work->scan.selected_record + 0x62) & 0xF0u) == 0x20u)
        return 0;

    for (;;) {
        scan_offset = work->scan.candidate_offset;
        if (work->scan.route == CANDIDATE_SCAN_DONE) {
            select_candidate_level(&work->level);
            work->path = CANDIDATE_UPDATE_TERMINAL;
            if (work->level.route == CANDIDATE_LEVEL_STOP_ZERO) return 0;
            if (work->level.route == CANDIDATE_LEVEL_TERMINAL)
                return work->result = candidate_terminal_result(work->scan.selected_record);
            work->path = CANDIDATE_UPDATE_LEVEL;
            return work->result = walk_candidate_level_planes(&work->level);
        }
        if (work->scan.route == CANDIDATE_SCAN_SIDE_RESULT) {
            work->path = CANDIDATE_UPDATE_SIDE;
            return work->result = candidate_side_result(work->scan.candidate_record,
                                                        work->scan.selected_record);
        }

        for (work->pass = 0; work->pass <= 1; work->pass++) {
            work->had_probe = 1;
            prepare_candidate_probe(&work->probe, work->scan.candidate_record,
                                    work->pass, relative_x, relative_y, relative_z);
            if (work->probe.below_first_height) {
                int face_result = scan_candidate_lower_faces(&work->probe);
                work->final_geometry_a4 = work->probe.face_list;
                if (face_result) {
                    work->path = CANDIDATE_UPDATE_FACE;
                    return work->result = 0x20;
                }
                break;
            }
            if (walk_candidate_edges(&work->probe, work->scan.selected_record,
                                     work->pass) == CANDIDATE_EDGE_DETAIL) {
                settle_candidate_edge_detail(&work->probe, work->scan.selected_record,
                                             work->pass);
                {
                    int face_result = scan_candidate_detail_faces(&work->probe,
                                                                  work->scan.selected_record);
                    work->final_geometry_a4 = work->probe.face_list;
                    if (face_result) {
                        work->path = CANDIDATE_UPDATE_FACE;
                        return work->result = 0x20;
                    }
                }
            } else {
                work->final_geometry_a4 = work->probe.edge_cursor;
            }
            if (rd_s16(VIEW_RECORD) != rd_s16(SCRIPT_RECORD)) break;
        }
        scan_candidate_record_from(&work->scan, scan_offset,
                                   relative_x, relative_y, relative_z);
    }
}
