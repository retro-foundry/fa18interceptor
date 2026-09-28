#ifndef FA18_FLAGGED_RECORD_COMPONENT_BOUND_H
#define FA18_FLAGGED_RECORD_COMPONENT_BOUND_H

#include <stdint.h>

typedef struct {
    const uint8_t *record_bytes;
    uint16_t record_size;
    int16_t record_offset; /* D0 at `$C1FC42`. */
    int16_t input_d7;
    int16_t stream_stage_shift; /* `$C45AB8`. */
    int16_t component_x; /* `$C45B2A`. */
    int16_t component_z; /* `$C45B2E`. */
    int16_t bound_x; /* `$C45A72`. */
    int16_t bound_z; /* `$C45A76`. */
    int32_t bound_long; /* `$C45A78`. */
} FA18FlaggedRecordComponentBoundInput;

typedef struct { uint32_t d7; uint8_t zero; } FA18FlaggedRecordComponentBoundResult;

/* `$C1FC42-$C1FCCE`: returns the source D7 representation and Z flag. */
int fa18_test_flagged_record_component_bound(const FA18FlaggedRecordComponentBoundInput *input,
                                             FA18FlaggedRecordComponentBoundResult *result);

#endif
