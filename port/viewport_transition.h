#ifndef FA18_VIEWPORT_TRANSITION_H
#define FA18_VIEWPORT_TRANSITION_H

#include "viewport_mode_state.h"

typedef enum {
    FA18_PALETTE_FIRST, FA18_PALETTE_SECOND, FA18_PALETTE_STABLE
} FA18ViewportPalettePhase;
typedef struct {
    /* Resolve the actual original 16-word palette selected by signed mode.
     * Return NULL for missing imported data. Its identity is retained across
     * both loads and the terminal copy, including child-induced mode changes. */
    const uint16_t *(*select_palette)(void *context, int mode);
    int (*load_palette)(void *context, FA18ViewportPalettePhase phase,
                         const uint16_t *words);
    /* Actual pointer-pair publication owner, including signed neighbor entries. */
    int (*publish_pair)(void *context, int index);
    const uint16_t *draw_page;
    uint16_t *stable_palette; /* at least 16 original words; may alias source */
    void *context;
} FA18ViewportTransitionOps;

/* Complete $C1718E viewport tail, shared with the native wrapper. Source
 * calls are ordered load/publish/load/publish, with the draw-page index
 * captured after the first load. Stable state loads its dynamic buffer.
 * The terminal word copy is sequential, preserving overlap effects.
 * Returns 0 for missing data or owner failure; prior source writes remain. */
int fa18_advance_native_viewport_transition(FA18ViewportModeState *state,
                                            const FA18ViewportTransitionOps *ops);
#endif
