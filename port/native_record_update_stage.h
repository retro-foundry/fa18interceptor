#ifndef FA18_NATIVE_RECORD_UPDATE_STAGE_H
#define FA18_NATIVE_RECORD_UPDATE_STAGE_H

#include "native_control_record_update.h"

typedef struct FA18NativeRecordUpdateStage FA18NativeRecordUpdateStage;
typedef struct {
    /* Complete lower owner C29042 mutates the same live native state before
     * this parent resumes. */
    int (*update_origin)(void *context,FA18NativeRecordUpdateStage *state,
                         int32_t origin[3]);
    void *context;
} FA18NativeRecordUpdateOps;

struct FA18NativeRecordUpdateStage {
    FA18NativeSceneRecords *records;
    FA18ViewCommandState *view;
    FA18NativeControlRecordUpdate *control_records;
    const FA18NativeRecordUpdateOps *ops;
    uint8_t *input_byte,*input_byte_mirror,*change_inhibit,*context_selection;
    uint8_t *origin_detail_mode,*selector_byte_coarse,*selector_byte_fine,*record_rate;
    int32_t *position_bias,*long_mirror,*projection_depth,*origin; /* origin[3] */
    uint16_t *scaled_word,*selector_word_x,*selector_word_z;
};

/* Complete C1C63E-C1C7F4 parent and its C22C80/C1C7F6 children. The active
 * origin producer remains an explicit complete lower owner. */
int fa18_update_native_scene_records(FA18NativeRecordUpdateStage *state);

#endif
