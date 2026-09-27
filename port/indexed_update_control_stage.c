#include "indexed_update_control_stage.h"

int fa18_run_indexed_update_control_stage(
    uint16_t selected_record_index,
    int32_t record_offset_42,
    FA18IndexedUpdateControlRecordStage control_record_stage,
    void *context,
    FA18IndexedUpdateControlStageRoute *route) {
    if (!control_record_stage || !route) return -1;
    control_record_stage(context);
    if (selected_record_index != 0)
        *route = FA18_INDEXED_UPDATE_SELECTED_RECORD_ROUTE;
    else if (record_offset_42 >= 0)
        *route = FA18_INDEXED_UPDATE_NONNEGATIVE_VALUE_ROUTE;
    else
        *route = FA18_INDEXED_UPDATE_NEGATIVE_VALUE_CONTINUATION;
    return 0;
}
