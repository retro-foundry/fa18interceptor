#ifndef FA18_NATIVE_SCENE_BOOTSTRAP_H
#define FA18_NATIVE_SCENE_BOOTSTRAP_H
#include "native_scene_placement.h"
#include "viewed_record_word.h"
#include "renderer_clear.h"
#include "template_bitmask_buffers.h"

typedef struct FA18NativeSceneBootstrap {
    FA18ContextCommandState *context;
    FA18NativeScenePlayerSetup *player;
    FA18NativeScenePlacement *placement;
    FA18NativeStartupRanges *startup;
    FA18NativeViewedRecordWord *viewed_word;
    FA18NativeRendererClear *renderer;
    FA18TemplateBitmaskState *gates;
    uint8_t *scene_limit,*previous_scene_limit,*context_state,*menu_transition;
    uint8_t *previous_state_byte,*byte_458be;
    uint16_t *menu_return_word,*word_4fda0,*countdown,*word_459a6,*word_459a8;
    uint16_t *history_record,*readout_minimum,*row_scales[2];
    uint32_t *message_queue_first,*message_timer,*readout_valid[2],*reference_18;
    uint16_t *depth_values;
    size_t depth_count;
} FA18NativeSceneBootstrap;

typedef struct {
    /* Required actual complete children C1C63E and C1C860.
     * Their native graphs remain pending. Completion status is separate from
     * incidental original CPU returns. No missing child is substituted. */
    int (*update_records)(void *context,FA18NativeSceneBootstrap *state);
    int (*refresh_context)(void *context,FA18NativeSceneBootstrap *state);
    void *context;
} FA18NativeSceneBootstrapOps;

/* Import/rebind the known startup-word and queue-byte owners, including the
 * viewed reference. All remaining original word data is supplied by caller.
 * References/metadata stay live; no scalar state is copied around execution.
 * Binding failure can retain preceding imports; no startup stores are run. */
int fa18_bind_native_scene_bootstrap(FA18NativeSceneBootstrap *state,
                                       FA18CommandQueue *queue,
                                       PortFieldByte *startup_words,size_t count);
/* Complete C08F26 parent and real available children. C09266 placement is a
 * direct typed owner; only the two still-pending update children remain callbacks.
 * Return 1 on completion, 0 on missing owner/child failure, preserving stores. */
int fa18_bootstrap_native_scene(FA18NativeSceneBootstrap *state,
                                 const FA18NativeSceneBootstrapOps *ops);
typedef struct {
    FA18NativeSceneBootstrap *state;
    const FA18NativeSceneBootstrapOps *ops;
} FA18NativeSceneBootstrapCall;
/* Actual native body adapter for the existing startup text publisher. */
int fa18_native_scene_bootstrap_callback(void *context);
#endif
