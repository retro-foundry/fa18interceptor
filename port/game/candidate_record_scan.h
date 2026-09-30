#ifndef FA18_GAME_CANDIDATE_RECORD_SCAN_H
#define FA18_GAME_CANDIDATE_RECORD_SCAN_H

#include "memory.h"

/* The bounded record scan at $C26EBE-$C270AA. The rest of $C26EBE is
 * still translated, so this source block is not registered on its own. */
typedef enum CandidateScanRoute {
    CANDIDATE_SCAN_DONE,
    CANDIDATE_SCAN_CLASS_20,
    CANDIDATE_SCAN_SIDE_RESULT
} CandidateScanRoute;

typedef struct CandidateScanWork {
    gaddr selected_record;
    gaddr candidate_record;
    int16_t candidate_offset;
    uint8_t candidate_class;
    CandidateScanRoute route;
} CandidateScanWork;

void scan_candidate_record(CandidateScanWork *work,
                           int32_t relative_x, int32_t relative_y,
                           int32_t relative_z);
void scan_candidate_record_from(CandidateScanWork *work, int16_t start_offset,
                                int32_t relative_x, int32_t relative_y,
                                int32_t relative_z);

/* Terminal source blocks $C278D6-$C279C6. */
int candidate_side_result(gaddr candidate, gaddr selected);
int candidate_terminal_result(gaddr selected);

/* $C270AE-$C2717C: source coordinates and first face-list height. */
typedef struct CandidateProbe {
    gaddr candidate_record;
    gaddr face_list;
    gaddr first_face;
    int16_t eye_x, eye_z;
    int32_t eye_y;
    int16_t shift;
    int16_t first_height;
    int below_first_height;
    gaddr edge_cursor;
    int16_t edge_a_offset, edge_b_offset;
} CandidateProbe;

void prepare_candidate_probe(CandidateProbe *probe, gaddr candidate,
                             int pass, int32_t relative_x,
                             int32_t relative_y, int32_t relative_z);
/* $C2717C-$C27194. Advances face_list; returns 0x20 on the early exit,
 * zero when the scan continues to the next candidate. Only called when the
 * probe is below its first height. */
int scan_candidate_lower_faces(CandidateProbe *probe);

/* $C271D4-$C2720E: signed horizontal side of one record edge. Point
 * offsets are the raw table words, before the +$A4 point-table base. */
int candidate_horizontal_edge_negative(const CandidateProbe *probe,
                                       int16_t point_a_offset,
                                       int16_t point_b_offset);

typedef enum CandidateEdgeRoute {
    CANDIDATE_EDGE_NEXT_PASS,
    CANDIDATE_EDGE_DETAIL
} CandidateEdgeRoute;
/* $C27198-$C27218 and $C2741C-$C2744C, before the detailed side checks. */
CandidateEdgeRoute walk_candidate_edges(CandidateProbe *probe,
                                        gaddr selected, int pass);

/* $C27218-$C273B4: the selected record's three-point side check and
 * per-pass flag/height write after a detailed edge is found. */
void settle_candidate_edge_detail(const CandidateProbe *probe,
                                  gaddr selected, int pass);
/* $C273BA-$C27418: returns 0x20 for the detailed face exit, otherwise zero
 * for the next probe pass. */
int scan_candidate_detail_faces(CandidateProbe *probe, gaddr selected);

#endif
