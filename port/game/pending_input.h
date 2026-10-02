#ifndef FA18_GAME_PENDING_INPUT_H
#define FA18_GAME_PENDING_INPUT_H
#include "memory.h"
enum PendingInputChild {
    PENDING_PREPARE, PENDING_BUTTONS, PENDING_CHANGED, PENDING_WAIT_ONE,
    PENDING_WAIT_BOTH, PENDING_POLL_KEY, PENDING_DISPATCH_KEY
};
enum PendingInputPhase {
    PENDING_BEGIN, PENDING_INPUT_WORD, PENDING_CHANGED_WORD,
    PENDING_MODE, PENDING_MODE_THREE, PENDING_WORD_TEST, PENDING_WORD_PAIR,
    PENDING_KEY, PENDING_ARGUMENT, PENDING_DROP_ARGUMENT,
    PENDING_MODE_END, PENDING_MODE_ONE, PENDING_COPY_WORD,
    PENDING_CLEAR_WORDS, PENDING_CLEAR_SECOND, PENDING_END
};
typedef struct { enum PendingInputPhase phase; uint32_t value, previous; } PendingInputEvent;
typedef struct {
    uint32_t (*consume)(void *context,enum PendingInputChild child,uint8_t key);
    void (*observe)(void *context,const PendingInputEvent *event);
    void *context;
} PendingInputHooks;
/* Complete C0F3C4-C0F4A4: poll external events and mouse buttons, drain mode-owned
 * command words and raw keys, then publish pending commands for the saved mode. */
void process_pending_key_events(const PendingInputHooks *hooks);
#endif
