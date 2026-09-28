#ifndef FA18_PARENT_UPDATE_TAIL_H
#define FA18_PARENT_UPDATE_TAIL_H

#include <stdint.h>

typedef int (*FA18ParentUpdateTailStage)(void *context);
typedef int (*FA18ParentUpdateTailZeroArgumentStage)(void *context, uint32_t argument);

typedef struct {
    FA18ParentUpdateTailStage postflight_a;
    FA18ParentUpdateTailStage postflight_b;
    FA18ParentUpdateTailStage postflight_c;
    FA18ParentUpdateTailStage postflight_mode_two;
    FA18ParentUpdateTailStage tail_start_a;
    FA18ParentUpdateTailStage tail_start_b;
    FA18ParentUpdateTailStage tail_start_c;
    FA18ParentUpdateTailStage modulo_seven;
    FA18ParentUpdateTailZeroArgumentStage modulo_fifteen;
    FA18ParentUpdateTailStage modulo_thirty_two;
    FA18ParentUpdateTailStage activity_mode_clear;
    FA18ParentUpdateTailStage flight_state_set;
    FA18ParentUpdateTailStage tail_after_counter;
    FA18ParentUpdateTailStage conditional_a;
    FA18ParentUpdateTailStage conditional_b;
    FA18ParentUpdateTailStage tail_end;
    void *context;
} FA18ParentUpdateTailOps;

typedef struct {
    uint8_t postflight_flag;
    uint8_t postflight_latch;
    uint8_t postflight_mode;
    uint8_t activity_mode;
    uint8_t flight_state;
    uint8_t tail_condition_1;
    uint8_t tail_condition_2;
    uint16_t local_frame;
    uint16_t frame_counter;
    uint16_t stage_marker;
} FA18ParentUpdateTailState;

/* `$C0F2A8-$C0F3C3`: source-ordered parent tail. `local_frame` is the
 * caller's `-2(a6)` word for this invocation; no display cadence is inferred.
 * The `$C2F582` clear is the `modulo_thirty_two` callback when low five bits
 * equal eight. */
int fa18_run_parent_update_tail(FA18ParentUpdateTailState *state,
                                const FA18ParentUpdateTailOps *ops);

#endif
