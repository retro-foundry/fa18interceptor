#ifndef FA18_RECORD_SCAN_INDEXED_STAGE_H
#define FA18_RECORD_SCAN_INDEXED_STAGE_H

#include <stdint.h>

#include "flagged_slot_scan.h"

typedef int (*FA18RecordScanIndexedTail)(void *context, uint16_t index);

typedef struct {
    int16_t matrix[9];
    uint8_t mode;
    uint16_t turn_word;
    uint16_t slot_word_30;
    uint16_t slot_word_32;
    int32_t origin_x;
    int32_t origin_y;
    int32_t origin_z;
} FA18RecordScanIndexedStageInput;

typedef struct {
    int32_t primary_x;
    int32_t primary_y;
    int32_t primary_z;
    int32_t secondary_x;
    int32_t secondary_y;
    int32_t secondary_z;
} FA18RecordScanIndexedStageResult;

/* `$C15688-$C158D6`: derive and publish a selected scan slot's two matrix
 * triples, then enter the caller-owned `$C159AE` tail. */
int fa18_run_record_scan_indexed_stage(
    FA18FlaggedSlot *slot, uint16_t index,
    const FA18RecordScanIndexedStageInput *input,
    FA18RecordScanIndexedTail tail, void *context,
    FA18RecordScanIndexedStageResult *result);

#endif
