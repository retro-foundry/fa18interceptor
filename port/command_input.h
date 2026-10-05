#ifndef FA18_COMMAND_INPUT_H
#define FA18_COMMAND_INPUT_H

#include "command_types.h"
#include "indexed_controls.h"

/* Shared ordinary state for keyboard selection and indexed actions. In the
 * original, CONTEXT_GATE and the indexed pose inhibitor are the same byte;
 * both use indexed.pose_inhibit here. The second modifier likewise lives in
 * indexed.function_modifier, not in a separately synchronized copy. */
typedef struct {
    FA18IndexedControls indexed;
    uint8_t event_counter, origin_mode, modifier, other_modifier;
    uint8_t return_state, block_flags, message_state;
    uint16_t pending_a, pending_b;
} FA18CommandInput;

/* Complete selection prefixes: $C1AD74-$C1B124 and $C1AC28-$C1AD6E.
 * Native callers supply an original raw key byte and release bit in event.
 * Selection updates modifier latches and pending bits before returning a
 * named action. Action execution and queue publication remain separate.
 * Return 0 only for invalid pointers; no state changes occur on that error. */
int fa18_select_keyboard_command(FA18CommandInput *state, uint32_t event,
                                 CommandRequest *request);
/* Parent composition also needs the inherited action word. The block check
 * replaces its low byte with the masked flags; indexed keys replace the word.
 * Other routes retain it. This is semantic action input, not CPU state. */
int fa18_select_keyboard_command_with_carry(FA18CommandInput *state, uint32_t event,
                                             int16_t *carried_word,
                                             CommandRequest *request);
int fa18_select_pending_command(FA18CommandInput *state,
                                CommandRequest *request);

/* Compose the selected indexed action with the same state it selected from.
 * Returns 0 for another action family or errors from the indexed component.
 * Other action families must be dispatched by their actual native owners. */
int fa18_apply_selected_indexed_command(FA18CommandInput *state,
                                        const CommandRequest *request,
                                        int16_t carried_selection,
                                        const FA18IndexedControlPoses *poses,
                                        FA18IndexedStatusTone status_tone,
                                        void *context, uint32_t *published_event);

#endif
