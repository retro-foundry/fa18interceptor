#ifndef FA18_GAME_CONTEXT_REFRESH_H
#define FA18_GAME_CONTEXT_REFRESH_H
#include "memory.h"

enum ContextRefreshChild {
    CONTEXT_REFRESH_TEMPLATES, CONTEXT_REFRESH_SORT, CONTEXT_REFRESH_CACHE,
    CONTEXT_REFRESH_CONDITION_A, CONTEXT_REFRESH_CONDITION_B, CONTEXT_REFRESH_RENDER
};
enum ContextRefreshPhase {
    CONTEXT_REFRESH_GUARD, CONTEXT_REFRESH_SAVE, CONTEXT_REFRESH_FRAME_GATE,
    CONTEXT_REFRESH_REQUESTS, CONTEXT_REFRESH_FLAGGED, CONTEXT_REFRESH_SELECTOR,
    CONTEXT_REFRESH_TEMPLATE_CALL, CONTEXT_REFRESH_SORT_CALL,
    CONTEXT_REFRESH_CACHE_CALL, CONTEXT_REFRESH_CONDITION_CALL,
    CONTEXT_REFRESH_RESTORE, CONTEXT_REFRESH_RENDER_CALL, CONTEXT_REFRESH_DONE
};
typedef struct {
    enum ContextRefreshPhase phase;
    uint32_t value, x, z;
    gaddr record;
    unsigned bit, shift;
} ContextRefreshEvent;
typedef struct {
    void (*consume)(void *context, enum ContextRefreshChild child);
    void (*observe)(void *context, const ContextRefreshEvent *event);
    void *context;
} ContextRefreshHooks;
/* Complete C1C860. The semantic frame-gate observations describe its caller
 * local, which the CPU adapter exposes to the existing source children. */
void refresh_context_packet(const ContextRefreshHooks *hooks);
#endif
