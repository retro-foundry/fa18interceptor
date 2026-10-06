/* Publish results of the flight parent's direct C matrix call. No CPU
 * entry registration or generated child dispatch owns this game call. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "record_matrix_update.h"
#include "flight_dynamics.h"
#include "glue_record_matrix_call.h"
#include "glue_matrix_side_values.h"
#include "recomp_ports.h"
#include "glue_flight_record_calls.h"

void transform_matrix_registers(const int16_t angles[3],
                                const MatrixTransformAngleState *state,
                                uint32_t last_term); /* glue_transform_matrix.c */
void record_orientation_registers(gaddr record, uint32_t d4, uint32_t d5,
                                  uint32_t d6); /* glue_batch23.c */
void track_direction_registers(int32_t elevation, int32_t azimuth, int32_t before,
                               int32_t *x_io, int32_t *y_io, int32_t *z_io,
                               int32_t max_step, int snap); /* glue_batch41.c */

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))

static void matrix_side_observe(void *context, enum RecordMatrixSidePhase phase,
                                uint32_t d[8]) {
    MatrixSideValues *values = context;
    if (phase == RECORD_MATRIX_SIDE_BEFORE) matrix_side_capture(values);
    else {
        matrix_side_finish(values, &d[0], &d[1]);
        fa18_ports_note_native_edge(0xC2D408, 0xC1342C);
    }
}

static void class30_registers(gaddr record, int tracked, int32_t old_azimuth,
                              int32_t x, int32_t y, int32_t z, int snap) {
    if (tracked) {
        D(2) = (uint32_t)x;
        D(3) = (uint32_t)y;
        D(4) = (uint32_t)z;
        D(5) = 0x7D0;
        D(6) = 0;
        D(0) = SEXT(rd_u16(record + 0x66));
        D(1) = SEXT(rd_u16(record + 0x68));
        track_direction_registers(rd_s16(TRACKED_PITCH), rd_s16(TRACKED_HEADING),
                                  old_azimuth, &x, &y, &z, 0x7D0, snap);
        SET_W(D(4), rd_u16(TRACKED_PITCH));
        SET_W(D(5), rd_u16(TRACKED_HEADING));
        SET_W(D(6), rd_u16(record + 0x6A));
    } else {
        D(4) = SEXT(rd_u16(record + 0x66));
        D(5) = SEXT(rd_u16(record + 0x68));
        D(6) = SEXT(rd_u16(record + 0x6A));
    }
    record_orientation_registers(record, D(4), D(5), D(6));
}

static int call_record_matrix(const void *arguments) {
    const RecordMatrixInput *input = arguments;
    gaddr record = input->record;
    RecordMatrixResult result;
    MatrixSideValues side;
    int i;
    update_dynamics_record_matrix(input, &result, matrix_side_observe, &side);
    fa18_ports_note_native_edge(0xC25B66, 0xC2D408);
    if (result.class30) {
        class30_registers(record, result.tracked, result.old_azimuth,
                          result.x, result.y, result.z, result.snap);
    } else {
        RecordMatrixRun run = result.run;
        if (run.used_depth) fa18_ports_note_native_edge(0xC2D408, 0xC2DD4E);
        for (i = 0; i < 8; i++) D(i) = run.d[i];
        if (run.used_matrix_side) A(0) = side.final_address;
        A(4) = record + 0x80;
        transform_matrix_registers(run.transformed_angles, &run.transform,
                                   run.last_term);
        if (run.orientation_written) {
            SET_W(D(4), run.angles[0]);
            SET_W(D(5), run.angles[1]);
            SET_W(D(6), run.angles[2]);
            record_orientation_registers(record, D(4), D(5), D(6));
        } else {
            SET_W(D(4), run.angles[0]);
            SET_W(D(5), run.angles[1]);
            SET_W(D(6), run.angles[2]);
        }
    }
    return glue_return();
}

int glue_schedule_record_matrix(void) {
    RecordMatrixInput input;
    int i;
    input.record = A(1);
    for (i = 0; i < 8; ++i) input.working[i] = D(i);
    /* Preserve the existing atomic timing debt. Native ownership is separate
     * from repairing the ordered matrix/event schedule. */
    return fa18_ports_schedule_native_child(call_record_matrix, &input, sizeof input, 9500);
}
int glue_complete_native_record_matrix(void) {
    RecordMatrixInput input; int i,result;
    input.record=A(1); for(i=0;i<8;++i) input.working[i]=D(i);
    result=call_record_matrix(&input); USE_CYCLES(9500); return result;
}
