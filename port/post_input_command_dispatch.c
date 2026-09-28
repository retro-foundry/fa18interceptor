#include "post_input_command_dispatch.h"

int fa18_dispatch_post_input_commands(
    FA18PostInputCommandDispatchState *state,
    FA18PostInputCommandPrepare prepare, void *context,
    FA18PostInputCommandDispatchRoute *route) {
    if (!state || !route) return -1;
    if (state->countdown >= 0) {
        *route = FA18_POST_INPUT_COMMAND_WAIT;
        return 0;
    }
    state->activity_flag = 0;
    state->command_count = 0;
    if (state->mode >= 3u && state->mode <= 8u) {
        if (!prepare) return -1;
        if (prepare(context, state->mode) >= 0)
            state->commands[state->command_count++] = 0x58;
    }
    if (state->mode == 2u)
        state->completion_flag = UINT8_MAX;
    else
        state->commands[state->command_count++] =
            (state->record_flags & 0x08u) ? 0x5a : 0x59;
    state->commands[state->command_count++] = 0;
    *route = FA18_POST_INPUT_COMMAND_C10CFE;
    return 0;
}
