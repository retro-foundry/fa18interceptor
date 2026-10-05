#ifndef FA18_NATIVE_CONTROL_RECORD_UPDATE_H
#define FA18_NATIVE_CONTROL_RECORD_UPDATE_H

#include "native_scene_records.h"

typedef enum {
    FA18_RECORD_UPDATE_PERIODIC, FA18_RECORD_UPDATE_RELEASE_SELECTION,
    FA18_RECORD_UPDATE_ROOT_CONTROL, FA18_RECORD_UPDATE_ROOT_VIEW,
    FA18_RECORD_UPDATE_ROOT_MARKER, FA18_RECORD_UPDATE_POSE,
    FA18_RECORD_UPDATE_PRIMARY_READY, FA18_RECORD_UPDATE_PRIMARY_PLACE,
    FA18_RECORD_UPDATE_SECONDARY_READY, FA18_RECORD_UPDATE_SECONDARY_PLACE,
    FA18_RECORD_UPDATE_SECONDARY_CONTROL, FA18_RECORD_UPDATE_PAIRED_READY,
    FA18_RECORD_UPDATE_DISPATCH, FA18_RECORD_UPDATE_FINISH
} FA18NativeControlRecordChild;

typedef struct FA18NativeControlRecordUpdate FA18NativeControlRecordUpdate;
typedef struct {
    /* Return zero on unavailable/failing child. Decision is consumed only by
     * ready and dispatch children and is separate from completion. */
    int (*consume)(void *context,FA18NativeControlRecordUpdate *state,
                   FA18NativeControlRecordChild child,unsigned slot,
                   unsigned companion_slot,int *decision);
    void *context;
} FA18NativeControlRecordOps;

struct FA18NativeControlRecordUpdate {
    FA18NativeSceneRecords *records;
    const FA18NativeControlRecordOps *ops;
    uint8_t *post_input_event,*counter_first,*counter_second;
    uint8_t *primary_gate,*secondary_gate;
    uint16_t *periodic_word,*current_slot,*current_stride;
    unsigned companion_slot; /* source A2 identity retained between groups */
};

/* Complete C22C80-C230AE control-record scheduler. Its game-specific children
 * remain explicit and operate on the same live bank before the parent resumes. */
int fa18_update_native_control_records(FA18NativeControlRecordUpdate *state);

#endif
