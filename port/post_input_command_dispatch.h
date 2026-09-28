#ifndef FA18_POST_INPUT_COMMAND_DISPATCH_H
#define FA18_POST_INPUT_COMMAND_DISPATCH_H

#include <stdint.h>

typedef int (*FA18PostInputCommandPrepare)(void *context, uint8_t mode);

typedef struct {
    int16_t countdown;
    uint8_t mode;
    uint8_t record_flags;
    uint8_t activity_flag;
    uint8_t completion_flag;
    uint16_t commands[3];
    uint8_t command_count;
} FA18PostInputCommandDispatchState;

typedef enum {
    FA18_POST_INPUT_COMMAND_WAIT,
    FA18_POST_INPUT_COMMAND_C10CFE
} FA18PostInputCommandDispatchRoute;

/* `$C10C68-$C10CFC`: wait for the inherited countdown, clear the activity
 * byte, build the caller-owned command sequence, and select `$C10CFE`.
 * `$C25070` remains a callback because its state is outside this slice. */
int fa18_dispatch_post_input_commands(
    FA18PostInputCommandDispatchState *state,
    FA18PostInputCommandPrepare prepare, void *context,
    FA18PostInputCommandDispatchRoute *route);

#endif
