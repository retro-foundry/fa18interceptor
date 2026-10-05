#ifndef FA18_OUTER_LOOP_CHILD_H
#define FA18_OUTER_LOOP_CHILD_H

#include <stddef.h>
#include <stdint.h>

#include "outer_page_selector.h"
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
typedef int (*FA18OuterLoopWaitBlit)(void *context);
typedef int (*FA18OuterLoopLoadView)(void *context,
                                     const FA18OuterPagePublication *publication);

typedef struct {
    FA18OuterLoopWaitViewport wait_viewport;
    FA18OuterLoopLoadRGB4 load_rgb4;
    void *context;
    FA18OuterLoopWaitBlit wait_blit;
    FA18OuterLoopLoadView load_view;
    const uint16_t *static_palette;
} FA18OuterLoopChildOps;

typedef struct {
    uint8_t mode_state_decremented;
    uint8_t dynamic_palette_loaded;
    uint8_t prefix_waited;
    uint8_t display_view_loaded;
    uint8_t activity_loop_completed;
    uint16_t selected_index_after;
    FA18OuterPagePublication publication;
} FA18OuterLoopChildStep;

/* `$C1612C-$C16283`: source-ordered outer-loop display child.  It waits for
 * the caller-owned viewport, publishes the selected pointer pair, and calls
 * the caller-owned LoadView boundary before the proved activity or idle tail.
 * Both palettes and every OS boundary remain supplied by the caller. */
int fa18_run_outer_loop_child(FA18OuterLoopChildState *outer,
                              FA18ViewportModeState *viewport_mode,
                              const uint32_t *pointer_table_1,
                              const uint32_t *pointer_table_2,
                              uint16_t pointer_table_entries,
                              const FA18OuterLoopChildOps *ops,
                              FA18OuterLoopChildStep *step);

/* Idle branch of `$C1617E-$C16283`. It consumes the same mode-state byte
 * written by `$C1718E`, optionally waits and applies the caller-owned 32-word
 * RGB4 buffer, then performs the source's word-sized `1 - selected` toggle.
 * The full wrapper above includes the activity branch. The shared native
 * input/display owner is separately implemented in outer_display.c/.h. */
int fa18_advance_outer_loop_idle_child(FA18OuterLoopChildState *outer,
                                       FA18ViewportModeState *viewport_mode,
                                       const FA18OuterLoopChildOps *ops,
                                       FA18OuterLoopChildStep *step);

#endif
