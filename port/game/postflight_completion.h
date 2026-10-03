#ifndef FA18_POSTFLIGHT_COMPLETION_H
#define FA18_POSTFLIGHT_COMPLETION_H
#include "memory.h"
enum PostflightCompletionChild { PFC_RESET_SCENE, PFC_CLEAR_RENDER, PFC_VIEW_ZERO, PFC_RESTART_SCENE, PFC_LOAD_TABLE };
enum PostflightCompletionPhase {
    PFC_D0_BYTE, PFC_D0_WORD, PFC_D0_ZERO, PFC_D1_BYTE,
    PFC_TEST_BYTE, PFC_TEST_WORD, PFC_BYTE_STORE, PFC_WORD_STORE,
    PFC_CALLBACK_LEA, PFC_CALLBACK_DIRECT, PFC_AND_WORD, PFC_OR_WORD,
    PFC_COMPARE_BYTE, PFC_VIEWPORT_COMPARE, PFC_SUB_BYTE, PFC_BIT_WORD
};
typedef struct {
    void (*consume)(void *context,enum PostflightCompletionChild child);
    void (*observe)(void *context,enum PostflightCompletionPhase phase,uint32_t value,uint32_t other);
    void *context;
} PostflightCompletionHooks;
void advance_postflight_completion(const PostflightCompletionHooks *hooks);
void restart_postflight_completion(const PostflightCompletionHooks *hooks);
void expire_postflight_completion(const PostflightCompletionHooks *hooks);
void queue_postflight_failure(const PostflightCompletionHooks *hooks);
void end_postflight_message(const PostflightCompletionHooks *hooks);
void follow_postflight_message(const PostflightCompletionHooks *hooks);
void clear_postflight_phase(const PostflightCompletionHooks *hooks);
void follow_postflight_message_or_phase(const PostflightCompletionHooks *hooks);
void restart_postflight_after_countdown(const PostflightCompletionHooks *hooks);
void queue_postflight_end(const PostflightCompletionHooks *hooks);
void await_postflight_viewport(const PostflightCompletionHooks *hooks);
void mark_postflight_viewport_ready(const PostflightCompletionHooks *hooks);
#endif
