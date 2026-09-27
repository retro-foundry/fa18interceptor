#ifndef FA18_VIEWPORT_PALETTE_H
#define FA18_VIEWPORT_PALETTE_H

#include <stdint.h>

#include "copper_page.h"
#include "hunk.h"

enum {
    FA18_VIEWPORT_PALETTE_WORDS = 16,
    FA18_VIEWPORT_PALETTE_MODE_COUNT = 16,
    FA18_VIEWPORT_DYNAMIC_PALETTE_WORDS = 32
};

/* Caller-owned counterpart of the 32-word buffer addressed through `$C45660`.
 * `$C0F812` seeds all words from `$C08510`; terminal `$C1718E` mode copies
 * replace only words 0..15 before `$C1617E` applies all 32 via LoadRGB4. */
typedef struct {
    uint16_t words[FA18_VIEWPORT_DYNAMIC_PALETTE_WORDS];
} FA18ViewportPaletteBuffer;

/* `$C1718E -> $C53EC0`: select one 16-word RGB4 table from Hunk 21.  The
 * verified Hunk-21 runtime base is `$C08490`; `$C08510` is the table base.
 * The original uses `(15 - mode) * 32`, then LoadRGB4's count of 16. */
int fa18_load_viewport_mode_palette(const FA18Hunks *exe, uint8_t mode,
                                    uint16_t palette[FA18_VIEWPORT_PALETTE_WORDS]);

int fa18_initialize_viewport_palette_buffer(const FA18Hunks *exe,
                                             FA18ViewportPaletteBuffer *buffer);

int fa18_copy_viewport_mode_palette_to_buffer(const FA18Hunks *exe, uint8_t mode,
                                               FA18ViewportPaletteBuffer *buffer);

/* Apply that same selected table to existing COLOR00--COLOR15 Copper MOVEs.
 * COLOR16--COLOR31 remain caller-owned because the original LoadRGB4 call
 * count is 16. */
int fa18_load_viewport_mode_palette_into_copper(
    const FA18Hunks *exe, uint8_t mode,
    FA18CopperMutableInstructionStream *streams, size_t stream_count,
    uint32_t *updated_mask);

#endif
