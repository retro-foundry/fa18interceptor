#ifndef FA18_NATIVE_SELECTOR_ORIGIN_H
#define FA18_NATIVE_SELECTOR_ORIGIN_H

#include "field_window.h"
#include "flight.h"
#include "native_scene_records.h"
#include "native_vector_math.h"

typedef struct FA18NativeSelectorOrigin FA18NativeSelectorOrigin;
typedef struct {
    PortFieldWindow general,class_11,class_14,class_30;
} FA18NativeSelectorOriginTables;

struct FA18NativeSelectorOrigin {
    FA18NativeSceneRecords *records;
    FA18NativeSceneRecord **active_record;
    const FA18NativeVectorMath *vector_math;
    const FA18FlightTrigData *trig;
    int16_t (*matrix)[3]; /* shared full-scale C45C0E owner */
    const FA18NativeSelectorOriginTables *tables;
    int32_t *origin,*candidate,*smoothed_delta,*negated_companion; /* three each */
    int32_t *auxiliary_delta;
    uint16_t *angle_history,*status_word;
    uint8_t *enable,*gate_b,*gate_a,*gate_mode,*detail_mode,*detail_index;
    uint8_t *adjustment_mode,*threshold_flag,*auxiliary_flag;
    uint8_t *variant_selector,*detail_counter;
};

/* Complete C29042 active-origin function, including every internally reached
 * adjustment path through C295D0. Matrix preparation, both transforms,
 * normalization and candidate regeneration are direct. Gate exits are
 * successful source exits and retain the current origin. */
int fa18_update_native_selector_origin(FA18NativeSelectorOrigin *state);
/* Complete C2DAF2, C091A8 (prepared matrix) and C091CE (record inverse). */
int fa18_prepare_native_selector_matrix(FA18NativeSelectorOrigin *state);
int fa18_transform_native_selector_components(FA18NativeSelectorOrigin *state,
    int prepared,const int32_t input[3],int32_t output[3]);

#endif
