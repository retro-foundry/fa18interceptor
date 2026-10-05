#ifndef FA18_NATIVE_RECORD_RANGE_H
#define FA18_NATIVE_RECORD_RANGE_H

#include "native_scene_records.h"
#include "scene_component_magnitude.h"

typedef struct FA18NativeRecordRange FA18NativeRecordRange;
typedef struct {
    /* Actual C3316E sound child, invoked with source argument 4. */
    int (*tone)(void *context,FA18NativeRecordRange *state,unsigned program);
    void *context;
} FA18NativeRecordRangeOps;
struct FA18NativeRecordRange {
    FA18NativeSceneRecords *records;
    const PortFieldWindow *table;
    const FA18NativeRecordRangeOps *ops;
    uint16_t *selected_record,*current_stride,*magnitude;
    uint8_t *bar_redraw_f;
};

/* Complete C244E2 with shared C245AA-C2467C range/classification tail.
 * Uses the actual native C1D974 table primitive. Missing sound/table/record
 * owners fail explicitly, preserving preceding writes. */
int fa18_classify_native_selected_range(FA18NativeRecordRange *state,unsigned slot);
/* Complete C24568 over the current record's live view point, sharing the
 * selected-range magnitude/classification tail. No sound prefix. */
int fa18_classify_native_view_range(FA18NativeRecordRange *state,unsigned slot,uint32_t *axis);
#endif
