#ifndef FA18_NATIVE_STARTUP_RANGES_H
#define FA18_NATIVE_STARTUP_RANGES_H
#include "command_queue.h"

enum { FA18_STARTUP_WORD_BYTES=104 };
typedef struct {
    FA18CommandQueue *queue;
    /* Complete bounded byte view of the 52 startup words. Every reference
     * must point to its supplied canonical owner; no shadow span is allocated.
     * Unported fields/record identities remain the startup owner's contract. */
    const PortFieldByte *words;
} FA18NativeStartupRanges;

/* Attach an initialized native queue and a complete caller-owned word view.
 * Binding validates all references and does not import/reset any values. */
int fa18_bind_native_startup_ranges(FA18NativeStartupRanges *state,
                                     FA18CommandQueue *queue,
                                     const PortFieldByte *word_bytes,size_t count);
/* Complete $C090C2: clear 53 actual neighboring bytes, then 52 words.
 * Complete $C090F2: enable the preceding 12 actual neighboring bytes.
 * Reuse live queue slots after later owner binding, not copied slot views.
 * Return 0 on a missing owner, retaining preceding writes. No host observer
 * is invoked between the two byte operations of each word store. */
int fa18_clear_native_startup_ranges(FA18NativeStartupRanges *state);
int fa18_enable_native_startup_ranges(FA18NativeStartupRanges *state);
#endif
