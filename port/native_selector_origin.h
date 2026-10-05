#ifndef FA18_NATIVE_SELECTOR_ORIGIN_H
#define FA18_NATIVE_SELECTOR_ORIGIN_H

#include "field_window.h"
#include "native_scene_records.h"

typedef enum {
    FA18_SELECTOR_ORIGIN_PREPARE,
    FA18_SELECTOR_ORIGIN_MATRIX_A,
    FA18_SELECTOR_ORIGIN_MATRIX_B,
    FA18_SELECTOR_ORIGIN_REGENERATE,
    FA18_SELECTOR_ORIGIN_NORMALIZE
} FA18NativeSelectorOriginChild;

typedef struct FA18NativeSelectorOrigin FA18NativeSelectorOrigin;
typedef struct {
    /* Input and output are three ordinary signed components. PREPARE and
     * REGENERATE receive no input; PREPARE has no output. */
    int (*consume)(void *context,FA18NativeSelectorOrigin *state,
                   FA18NativeSelectorOriginChild child,
                   const int32_t input[3],int32_t output[3]);
    void *context;
} FA18NativeSelectorOriginOps;

typedef struct {
    PortFieldWindow general,class_11,class_14,class_30;
} FA18NativeSelectorOriginTables;

struct FA18NativeSelectorOrigin {
    FA18NativeSceneRecords *records;
    FA18NativeSceneRecord **active_record;
    const FA18NativeSelectorOriginOps *ops;
    const FA18NativeSelectorOriginTables *tables;
    const int32_t *root_preset; /* three longs */
    int32_t *origin,*candidate,*smoothed_delta,*negated_companion; /* three each */
    int32_t *auxiliary_delta;
    uint16_t *angle_history,*status_word;
    uint8_t *enable,*gate_b,*gate_a,*gate_mode,*detail_mode,*detail_index;
    uint8_t *adjustment_mode,*threshold_flag,*auxiliary_flag;
    uint8_t *variant_selector,*detail_counter;
};

/* Complete C29042 active-origin function, including every internally reached
 * adjustment path through C295D0. The matrix builder, transforms,
 * regeneration and normalizer remain explicit lower routines. Gate exits are
 * successful source exits and retain the current origin. */
int fa18_update_native_selector_origin(FA18NativeSelectorOrigin *state);

#endif
