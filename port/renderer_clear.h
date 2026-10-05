#ifndef FA18_NATIVE_RENDERER_CLEAR_H
#define FA18_NATIVE_RENDERER_CLEAR_H
#include "graphics_setup.h"

typedef struct {
    FA18NativeGraphicsSetup *graphics;
    /* $C456E2 is separate from graphics->source[0..8]. Its real allocation
     * and lifetime belong to startup loading, not this clear operation. */
    FA18NativeGraphicsPlane *additional_buffer;
    uint8_t *fifth_buffer_used;
} FA18NativeRendererClear;

/* Bind the queue-reachable fifth-buffer byte to its supplied canonical owner,
 * importing the current value. No buffer is allocated or substituted. */
int fa18_bind_native_renderer_clear(FA18NativeRendererClear *state,
                                     FA18CommandQueue *queue);

/* Complete $C2FD22. Capture five A spans, clear four interleaved longwords
 * for 2,000 iterations, sampling the fifth gate AFTER those four writes each
 * time. The fifth cursor advances only when used. Then reread/capture the
 * four B spans plus the separate additional buffer and clear all five streams
 * interleaved for 2,000 iterations. Aliases and overlapping spans are allowed.
 * Returns 0 for missing/bounded-out storage; preceding writes remain. An
 * unused fifth A pointer need not resolve. Descriptors/owners must stay live. */
int fa18_clear_native_renderer(FA18NativeRendererClear *state);
#endif
