#ifndef FA18_PARENT_ACTIVITY_STAGES_H
#define FA18_PARENT_ACTIVITY_STAGES_H

#include <stdint.h>

typedef int (*FA18ParentActivityStage)(void *context);

typedef struct {
    FA18ParentActivityStage helper[13];
    void *context;
} FA18ParentActivityStagesOps;

typedef struct {
    uint8_t activity_byte;
    int32_t signed_stage_value;
    uint16_t local_frame;
    uint16_t stage_marker;
} FA18ParentActivityStagesState;

/* `$C0F1E2-$C0F2A7`: activity-gated parent stages. The signed activity byte
 * and `local_frame` are caller-owned state from the enclosing parent update. */
int fa18_run_parent_activity_stages(FA18ParentActivityStagesState *state,
                                    const FA18ParentActivityStagesOps *ops);

#endif
