#ifndef FA18_POST_INPUT_TICK_H
#define FA18_POST_INPUT_TICK_H

#include <stdint.h>
#include "stage_callback.h"

/* Native names for the four concrete callback addresses selected by
 * `$C0F5F8-$C0F7D1`. The callback body remains an explicit owner boundary. */

typedef struct {
    int8_t entry_guard, enable, signed_guard, later_guard;
    uint8_t phase, phase_flag, secondary_guard, mode_flag, mode_byte;
    uint8_t mode_latch, event_flag, result_flag, auxiliary_byte, tick_count;
    uint8_t command_input_pending;
    uint32_t counter_source, primary_offset, secondary_offset, tertiary_offset;
    uint32_t quaternary_offset, additional_offset, result_target_offset8;
    uint16_t result_code, countdown, configured_countdown;
    FA18PostInputCallback callback;
} FA18PostInputTickState;

/* External edges at `$C06C02` and `$C0F804`. The caller supplies their
 * original-port owners; this layer never assigns an invented callback body. */
typedef int (*FA18PostInputTickCall)(void *context, FA18PostInputCallback callback);
typedef int (*FA18PostInputTickInvalidOffset)(void *context);
typedef struct {
    FA18PostInputTickCall call_callback;
    FA18PostInputTickInvalidOffset invalid_offset;
    void *context;
} FA18PostInputTickHooks;

/* Direct port of `$C0F5F8-$C0F811`. Returns -1 for invalid state, -2 if the
 * source invalid-offset edge lacks an owner, -3 if callback dispatch lacks an
 * owner, or a nonzero result returned by a supplied owner. */
int fa18_run_post_input_tick(FA18PostInputTickState *state,
                             const FA18PostInputTickHooks *hooks);

#endif
