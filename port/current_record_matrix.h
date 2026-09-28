#ifndef FA18_CURRENT_RECORD_MATRIX_H
#define FA18_CURRENT_RECORD_MATRIX_H

#include <stddef.h>
#include <stdint.h>

#include "flight.h"

enum { FA18_CURRENT_RECORD_MATRIX_RECORD_BYTES = 0x6a };

typedef struct {
    const uint8_t *active_record;
    size_t active_record_size;
    const FA18FlightTrigTable *trig_table;
    int16_t matrix[3][3];
} FA18CurrentRecordMatrixState;

/* `$C2DAF2-$C2DB17`: build `$C45C0E` from active record `+$68`. */
int fa18_build_current_record_matrix(FA18CurrentRecordMatrixState *state);

/* Callback adapter for source callers such as `$C29042`. */
void fa18_build_current_record_matrix_callback(void *context);

#endif
