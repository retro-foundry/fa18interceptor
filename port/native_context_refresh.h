#ifndef FA18_NATIVE_CONTEXT_REFRESH_H
#define FA18_NATIVE_CONTEXT_REFRESH_H

#include "native_scene_records.h"

typedef enum {
    FA18_CONTEXT_REFRESH_TEMPLATES, FA18_CONTEXT_REFRESH_SORT,
    FA18_CONTEXT_REFRESH_CACHE, FA18_CONTEXT_REFRESH_CONDITION_A,
    FA18_CONTEXT_REFRESH_CONDITION_B, FA18_CONTEXT_REFRESH_RENDER
} FA18NativeContextRefreshChild;

typedef struct FA18NativeContextRefresh FA18NativeContextRefresh;
typedef struct {
    int (*consume)(void *context,FA18NativeContextRefresh *state,
                   FA18NativeContextRefreshChild child);
    void *context;
} FA18NativeContextRefreshOps;

struct FA18NativeContextRefresh {
    FA18NativeSceneRecords *records;
    FA18ViewCommandState *view;
    const FA18NativeContextRefreshOps *ops;
    int32_t *position_bias,*origin; /* origin[3] */
    uint16_t *cell_timer,*error_word,*condition_key_a,*condition_key_b;
    uint16_t *stage_selector,*current_colour;
    uint32_t *line_style;
    uint8_t *context_selection,*prepared,*alternate,*cell_checks;
    uint8_t *view_mode,*fixed_readouts,*frame_gate;
};

/* Complete C1C860-C1CA2C and direct C1CA82 record flagging. Ordered content,
 * sort, cache, condition and render children remain explicit native owners. */
int fa18_refresh_native_context(FA18NativeContextRefresh *state);

#endif
