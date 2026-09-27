#ifndef FA18_RECORD_DELTA_SCAN_H
#define FA18_RECORD_DELTA_SCAN_H

#include <stddef.h>
#include <stdint.h>

enum { FA18_RECORD_DELTA_SCAN_RECORDS = 16 };

/* Fields read by `$C1CFD6-$C1D0A3` from consecutive `$C46184` records. */
typedef struct {
    uint8_t flags_byte_1;
    int32_t component[3];
} FA18RecordDeltaScanRecord;

typedef int (*FA18RecordDeltaScanSubmit)(void *context, uint16_t selector,
                                         int32_t component_0,
                                         int32_t component_1,
                                         int32_t component_2);

typedef struct {
    uint8_t scene_active;
    uint8_t activity;
    uint16_t selector;
    uint16_t selected_offset;
    const FA18RecordDeltaScanRecord *records;
    FA18RecordDeltaScanSubmit submit;
    void *submit_context;
} FA18RecordDeltaScanInput;

typedef struct {
    uint8_t latch;
    uint8_t mode;
    uint16_t result_offset;
    uint16_t submitted_count;
} FA18RecordDeltaScanState;

/* `$C1CFD6-$C1D0A3`: clear the scan state and submit every eligible
 * `$C46184` record relative to the selected record. `$C25876` remains a
 * caller-owned submission boundary. */
int fa18_scan_record_delta_submission(const FA18RecordDeltaScanInput *input,
                                      FA18RecordDeltaScanState *state);

#endif
