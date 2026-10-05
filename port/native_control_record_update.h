#ifndef FA18_NATIVE_CONTROL_RECORD_UPDATE_H
#define FA18_NATIVE_CONTROL_RECORD_UPDATE_H

#include "native_scene_records.h"
#include "native_record_selection.h"
#include "native_record_range.h"
#include "native_record_view.h"
#include "native_record_control.h"
#include "native_record_pose.h"
#include "native_record_action_placement.h"
#include "native_postflight.h"
#include "native_scene_regions.h"

typedef enum {
    FA18_RECORD_UPDATE_DISPATCH
} FA18NativeControlRecordChild;

typedef struct FA18NativeControlRecordUpdate FA18NativeControlRecordUpdate;
typedef struct {
    /* Return zero on unavailable/failing child. Decision is consumed only by
     * dispatch and is separate from completion. */
    int (*consume)(void *context,FA18NativeControlRecordUpdate *state,
                   FA18NativeControlRecordChild child,unsigned slot,
                   unsigned companion_slot,int *decision);
    void *context;
} FA18NativeControlRecordOps;

struct FA18NativeControlRecordUpdate {
    FA18NativeSceneRecords *records;
    FA18NativeRecordSelection *selection;
    FA18NativeRecordRange *range;
    FA18NativeRecordView *view;
    FA18NativeRecordViewWork *view_work;
    FA18NativeRecordControl *control;
    FA18NativeRecordPose *pose;
    FA18NativeRecordActionPlacement *placement;
    FA18NativePostflight *postflight;
    FA18NativeSceneRegions *regions;
    const FA18NativeControlRecordOps *ops;
    uint8_t *post_input_event,*counter_first,*counter_second;
    uint8_t *primary_gate,*secondary_gate;
    uint16_t *periodic_word,*current_slot,*current_stride;
    unsigned companion_slot; /* source A2 identity retained between groups */
};

/* Complete C22C80-C230AE scheduler with direct selection, control, pose, placement, view and range owners.
 * Remaining game-specific children operate on the same live bank. */
int fa18_update_native_control_records(FA18NativeControlRecordUpdate *state);

#endif
