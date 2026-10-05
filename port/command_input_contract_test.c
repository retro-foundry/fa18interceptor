#include "command_input.h"
#include <assert.h>
#include <string.h>

static uint32_t status_tone(void *context, FA18IndexedControls *s) {
    unsigned *calls = context;
    ++*calls;
    assert(s->mode == 1 || s->mode == 3);
    return 2;
}
int main(void) {
    FA18CommandInput s;
    CommandRequest request;
    uint32_t event;
    unsigned calls = 0;
    memset(&s, 0, sizeof s);
    s.event_counter = 1;
    s.indexed.mode_gate = 1;
    s.indexed.mode = 1;
    s.indexed.player_ready = 1;
    assert(fa18_select_keyboard_command(&s, 0x60, &request));
    assert(request.action == COMMAND_FINISH_EVENT && s.modifier == 1);
    assert(fa18_select_keyboard_command(&s, 0xe0, &request));
    assert(request.action == COMMAND_FINISH_EVENT && s.modifier == 1);
    assert(fa18_select_keyboard_command(&s, 0x59, &request));
    assert(request.action == COMMAND_FUNCTION_LEVEL && request.modifier == 1);
    assert(fa18_apply_selected_indexed_command(&s, &request, 0, NULL,
                                               status_tone, &calls, &event));
    assert(s.indexed.function_level == 0x79 && s.indexed.throttle == 0x3c0);
    assert(event == 0x59 && !calls);

    s.indexed.mode = 0;
    assert(fa18_select_keyboard_command(&s, 0x50, &request));
    assert(request.action == COMMAND_FUNCTION_LEVEL);
    s.indexed.modes.status = 1;
    assert(fa18_apply_selected_indexed_command(&s, &request, 0, NULL,
                                               status_tone, &calls, &event));
    assert(s.indexed.mode == 3 && calls == 1 && event == 2);
    s.indexed.mode = 0;
    assert(fa18_select_keyboard_command(&s, 2, &request));
    assert(request.action == COMMAND_LOW_INDEX);
    assert(fa18_apply_selected_indexed_command(&s, &request, 0, NULL,
                                               status_tone, &calls, &event));
    assert(s.indexed.mode == 1 && s.indexed.mode_request == 2 && calls == 2);

    /* The original visits the high-byte bits before low-byte bits. */
    s.pending_a = 0x0301;
    assert(fa18_select_pending_command(&s, &request));
    assert(request.action == COMMAND_GEAR && s.pending_a == 0x0201);
    assert(fa18_select_pending_command(&s, &request));
    assert(request.action == COMMAND_HOOK && s.pending_a == 1);
    s.pending_a = 0xf0;
    assert(fa18_select_pending_command(&s, &request));
    assert(request.action == COMMAND_INVALID_WORD && s.pending_a == 0xf0);
    s.pending_a = s.pending_b = 0;
    s.indexed.function_modifier = s.other_modifier = 1;
    s.indexed.recorder_mode = 1;
    assert(fa18_select_pending_command(&s, &request));
    assert(request.action == COMMAND_PENDING_EMPTY && s.indexed.function_modifier == 1);
    s.indexed.recorder_mode = 0;
    assert(fa18_select_pending_command(&s, &request));
    assert(!s.indexed.function_modifier && !s.other_modifier && s.modifier == 1);

    /* Source counter zero strips a release bit on the first accepted event. */
    s.event_counter = 0;
    s.indexed.mode = 0;
    assert(fa18_select_keyboard_command(&s, 0x12340082, &request));
    assert(s.event_counter == 1 && request.raw_event == 0x12340002 &&
           request.action == COMMAND_LOW_INDEX);
    s.event_counter = 0xff;
    assert(fa18_select_keyboard_command(&s, 2, &request));
    assert(s.event_counter == 0 && request.action == COMMAND_COUNTER_WAIT);
    return 0;
}
