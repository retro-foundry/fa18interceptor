#ifndef FA18_NATIVE_RECORD_SELECTION_H
#define FA18_NATIVE_RECORD_SELECTION_H

#include "native_scene_records.h"

typedef enum {
    FA18_RECORD_ACTION_RELEASE,
    FA18_RECORD_ACTION_SOUND,
    FA18_RECORD_ACTION_MANOEUVRE
} FA18NativeRecordActionChild;

typedef struct FA18NativeRecordSelection FA18NativeRecordSelection;
typedef struct {
    int (*consume)(void *context,FA18NativeRecordSelection *state,
                   FA18NativeRecordActionChild child);
    void *context;
} FA18NativeRecordActionOps;

struct FA18NativeRecordSelection {
    FA18NativeSceneRecords *records;
    const FA18NativeRecordActionOps *ops;
    uint16_t *selected_record,*selection_marker,*action_pending;
    uint8_t *selection_active,*origin_enable,*action_first,*action_second;
    uint8_t *action_third,*pair_override;
};

/* Complete C230B0, C230E8/C23116 and C231A2 native leaves. Decisions are
 * ordinary results separate from missing-state or child failure. */
int fa18_release_lost_native_selection(FA18NativeRecordSelection *state);
int fa18_select_native_record_action(FA18NativeRecordSelection *state,
                                     int allow_release,int *decision);
int fa18_native_paired_record_ready(FA18NativeRecordSelection *state,
                                    unsigned companion_slot,int *decision);

#endif
