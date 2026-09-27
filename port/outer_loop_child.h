#ifndef FA18_OUTER_LOOP_CHILD_H
#define FA18_OUTER_LOOP_CHILD_H

#include <stddef.h>
#include <stdint.h>

#include "viewport_mode.h"

enum { FA18_OUTER_LOOP_CHILD_RGB4_WORDS = 32 };

typedef struct {
    uint8_t activity_counter;
    uint16_t selected_index;
    uint16_t status_word;
    const FA18ViewportPaletteBuffer *dynamic_palette;
} FA18OuterLoopChildState;

typedef int (*FA18OuterLoopWaitViewport)(void *context);
typedef int (*FA18OuterLoopLoadRGB4)(void *context, const uint16_t *palette,
                                     size_t word_count);

typedef struct {
    FA18OuterLoopWaitViewport wait_viewport;
    FA18OuterLoopLoadRGB4 load_rgb4;
    void *context;
} FA18OuterLoopChildOps;

typedef struct {
    uint8_t mode_state_decremented;
    uint8_t dynamic_palette_loaded;
    uint16_t selected_index_after;
} FA18OuterLoopChildStep;

/* Idle branch of `$C1617E-$C16283`. It consumes the same mode-state byte
 * written by `$C1718E`, optionally waits and applies the caller-owned 32-word
 * RGB4 buffer, then performs the source's word-sized `1 - selected` toggle.
 * The nonzero activity-counter loop is a distinct unported branch. */
int fa18_advance_outer_loop_idle_child(FA18OuterLoopChildState *outer,
                                       FA18ViewportModeState *viewport_mode,
                                       const FA18OuterLoopChildOps *ops,
                                       FA18OuterLoopChildStep *step);

#endif
