#ifndef FA18_CONTROL_RECORD_MATRIX_ROUTE_H
#define FA18_CONTROL_RECORD_MATRIX_ROUTE_H

#include <stddef.h>
#include <stdint.h>

#include "flight.h"

enum { FA18_CONTROL_RECORD_MATRIX_ROUTE_RECORD_BYTES = 0x6c };

typedef struct {
    uint8_t matrix_route_state;
    uint8_t matrix_selector;
    uint8_t matrix_mode;
    int16_t row_scale[3];
} FA18ControlRecordMatrixRouteInput;

typedef enum {
    FA18_CONTROL_RECORD_MATRIX_ROUTE_DEFAULT,
    FA18_CONTROL_RECORD_MATRIX_ROUTE_UNPORTED_SELECTION
} FA18ControlRecordMatrixRouteResult;

typedef struct {
    int16_t first_matrix[3][3];
    int16_t second_matrix[3][3];
    int16_t third_matrix[3][3];
    int16_t auxiliary_angle[3];
} FA18ControlRecordMatrixRouteOutput;

/* Observed default path of `$C2DB18-$C2DCC0`: record type is not `$30`, the
 * route-state and selector bytes are zero, and mode is at most one.  It uses
 * record +$68 for `$C45BFC`, copies +$66/+68/+6A to `$C45A88`, builds the
 * unscaled `$C45BEA`, then builds/scales `$C45BD8`. */
int fa18_update_default_control_record_matrices(
    const FA18FlightTrigTable *trig_table, const uint8_t *record,
    size_t record_size, const FA18ControlRecordMatrixRouteInput *input,
    FA18ControlRecordMatrixRouteOutput *output,
    FA18ControlRecordMatrixRouteResult *result);

#endif
