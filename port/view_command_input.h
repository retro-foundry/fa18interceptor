#ifndef FA18_VIEW_COMMAND_INPUT_H
#define FA18_VIEW_COMMAND_INPUT_H

#include "flight_command_input.h"

/* Native view state shares aircraft/command values directly. The source
 * ORIGIN_GATE_MODE is the same byte as the aircraft pause gate. */
typedef struct {
    FA18FlightCommandState *flight;
    uint32_t origin_middle, redraw_state_long;
    uint16_t target_mark, span_origin, span_origin_y, line_last_row;
    uint16_t zoom_scale, redraw_state_word, emitted_requests;
    uint8_t detail_index, update_mask, fire_state;
    uint8_t mode, mode_auxiliary, mode_companion, refresh_request, zoom_flags;
    uint8_t redraw_first, gauge_refresh, grid_z_redraws, grid_x_redraws;
    uint8_t redraw_bar_a, redraw_bar_aux, display_update, redraw_keep_state;
} FA18ViewCommandState;

/* Original signed-byte view index addresses this window, including the
 * surrounding source data for negative/out-of-range modes. Element 0 is
 * mode -128; element 128 is mode 0. Import from the original image. */
typedef struct { int8_t values[256]; } FA18ViewSpanOffsets;

int fa18_is_view_input_command(enum CommandAction action);

/* Actual $C08324 and $C082B8 children. Redraw shares the aircraft counters
 * and honors the original retain-state gate. */
int fa18_set_native_zoom_maximum(FA18ViewCommandState *state);
int fa18_request_native_cockpit_redraw(FA18ViewCommandState *state);

/* All 16 $C1B77C-$C1BB76 view/origin/zoom actions, before publication.
 * No machine, CPU, guest addresses or substitute child behavior.
 * Returns 0 on invalid arguments, with no changes or output assignment. */
int fa18_apply_view_input_command(FA18ViewCommandState *state,
                                  const CommandRequest *request,
                                  const FA18ViewSpanOffsets *spans,
                                  uint32_t *published_event);

#endif
