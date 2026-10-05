#ifndef FA18_NATIVE_POST_INPUT_DISPLAY_STAGES_H
#define FA18_NATIVE_POST_INPUT_DISPLAY_STAGES_H
#include "renderer_clear.h"
#include "stage_callback.h"

typedef struct {
    FA18CommandInput *commands;
    FA18ViewportModeState *viewport;
    uint16_t *countdown;
    FA18StageCallback *callback;
    uint8_t *auxiliary;
    FA18NativeRendererClear *renderer;
} FA18NativePostInputDisplayStages;
typedef struct {
    /* Actual complete $C0FAA4; native initialization graph still pending.
     * Return 1 when completed, regardless of incidental original D0 output. */
    int (*initialize_scene)(void *context);
    void *context;
} FA18NativePostInputDisplayOps;

/* Import/bind the queue-reachable auxiliary byte to its canonical owner.
 * Countdown/callback can be those of the native tick/publisher controller.
 * No state is copied around stage execution. */
int fa18_bind_native_post_input_display(FA18NativePostInputDisplayStages *state,
                                         FA18CommandQueue *queue);
/* Complete $C0FA04, including its nonexpired real ten-stream clear branch.
 * On expiry, run the required scene child before all parent stores. */
int fa18_finish_native_post_input_display(FA18NativePostInputDisplayStages *state,
                                           const FA18NativePostInputDisplayOps *ops);
/* Complete $C0FA4C and $C0FA80. Preserve signed expiry, unconditional expired
 * auxiliary clear, viewport equality and shared event/callback write order.
 * Each entry returns 1 when completed, including a source no-action return;
 * 0 signals missing owners/child failure, retaining preceding writes. */
int fa18_match_native_post_input_display(FA18NativePostInputDisplayStages *state);
int fa18_complete_native_post_input_display(FA18NativePostInputDisplayStages *state);
#endif
