#ifndef FA18_INDEXED_UPDATE_CONTROL_STAGE_H
#define FA18_INDEXED_UPDATE_CONTROL_STAGE_H

#include <stdint.h>

typedef void (*FA18IndexedUpdateControlRecordStage)(void *context);

typedef enum {
    FA18_INDEXED_UPDATE_SELECTED_RECORD_ROUTE,
    FA18_INDEXED_UPDATE_NONNEGATIVE_VALUE_ROUTE,
    FA18_INDEXED_UPDATE_NEGATIVE_VALUE_CONTINUATION
} FA18IndexedUpdateControlStageRoute;

/* `$C25C70-$C25C85`: call `$C1B27E`, then route from index and record +$42. */
int fa18_run_indexed_update_control_stage(
    uint16_t selected_record_index,
    int32_t record_offset_42,
    FA18IndexedUpdateControlRecordStage control_record_stage,
    void *context,
    FA18IndexedUpdateControlStageRoute *route);

#endif
