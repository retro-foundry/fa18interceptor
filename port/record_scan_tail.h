#ifndef FA18_RECORD_SCAN_TAIL_H
#define FA18_RECORD_SCAN_TAIL_H

#include <stdint.h>

#include "flagged_slot_evaluator.h"

typedef int (*FA18RecordScanTailScalar)(void *context, int16_t selector,
                                        int16_t x, int16_t y, int16_t z,
                                        int16_t result[3]);

typedef struct {
    const FA18FlaggedLinkedRecord *linked_record;
    uint16_t turn_word;
    int32_t origin_x;
    int32_t origin_y;
    int32_t origin_z;
    int32_t primary_x;
    int32_t primary_y;
    int32_t primary_z;
    int32_t motion_x;
    int32_t motion_y;
    int32_t motion_z;
} FA18RecordScanTailInput;

typedef struct {
    int16_t selector;
    int16_t shifted_x;
    int16_t shifted_y;
    int16_t shifted_z;
} FA18RecordScanTailResult;

/* `$C159AE-$C15BF4`: derive the selected slot's scalar arguments, publish the
 * `$C257EC` result triples, and apply the flag-selected motion additions. */
int fa18_run_record_scan_tail(FA18FlaggedSlot *slot,
                              const FA18RecordScanTailInput *input,
                              FA18RecordScanTailScalar scalar, void *context,
                              FA18RecordScanTailResult *result);

#endif
