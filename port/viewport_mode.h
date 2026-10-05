#ifndef FA18_VIEWPORT_MODE_H
#define FA18_VIEWPORT_MODE_H

#include <stddef.h>
#include <stdint.h>

#include "outer_page_selector.h"
#include "viewport_palette.h"
#include "viewport_mode_state.h"

/* Caller-owned equivalents of `$C4566C`, `$C182BA/$C182C2`, and `$C45660`.
 * The final buffer is required because `$C1718E` copies exactly the selected
 * 16 RGB4 words into its lower half once current reaches target. */
typedef struct {
    uint16_t outer_selected_index;
    const uint32_t *left_pointer_table;
    const uint32_t *right_pointer_table;
    uint16_t pointer_table_entries;
    FA18ViewportPaletteBuffer *mode_palette_buffer;
} FA18ViewportModeBindings;

typedef struct {
    uint8_t palette_loaded;
    uint8_t palette_load_count;
    uint8_t pointer_pair_published;
    uint8_t pointer_pair_publish_count;
    uint8_t mode_words_copied;
    FA18OuterPagePublication publication;
    uint32_t copper_updated_mask;
} FA18ViewportModeStep;

/* `$C1718E` mode-buffer tail, after the JOY0DAT accumulator work. It models
 * only its established state and display outputs: signed-byte countdown,
 * one-step current-mode movement, the ordered LoadRGB4/pointer-pair calls, and
 * the terminal 16-word copy. The caller remains responsible for scheduling
 * this callback and for the meanings/owners of its pointer tables. */
int fa18_advance_viewport_mode(FA18ViewportModeState *state,
                               const FA18ViewportModeBindings *bindings,
                               const FA18Hunks *exe,
                               FA18CopperMutableInstructionStream *streams,
                               size_t stream_count,
                               FA18ViewportModeStep *step);

#endif
