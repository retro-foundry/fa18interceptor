#ifndef FA18_GAME_POST_INPUT_TICK_H
#define FA18_GAME_POST_INPUT_TICK_H
#include "memory.h"

/* Complete C0F5F8 scheduler. The installed stage remains an independent,
 * required child; it may change any sequence state before the tick returns. */
enum PostInputTickPhase {
    POST_TICK_OFFSET_BEGIN, POST_TICK_OFFSET_SECONDARY,
    POST_TICK_OFFSET_FIXED, POST_TICK_OFFSET_EXTRA, POST_TICK_OFFSET_ACCEPTED,
    POST_TICK_PHASE_RESET, POST_TICK_PHASE_START, POST_TICK_DISPATCH
};
typedef struct {
    enum PostInputTickPhase phase;
    uint32_t offset, term;
    gaddr routine;
    uint16_t countdown;
} PostInputTickEvent;
typedef struct {
    void (*consume)(void *context, gaddr routine);
    void (*observe)(void *context, const PostInputTickEvent *event);
    void *context;
} PostInputTickHooks;
void run_post_input_tick(const PostInputTickHooks *hooks);
#endif
