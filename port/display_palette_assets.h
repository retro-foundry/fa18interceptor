#ifndef FA18_DISPLAY_PALETTE_ASSETS_H
#define FA18_DISPLAY_PALETTE_ASSETS_H
#include "hunk.h"

typedef struct {
    /* Mode bank in original source order. Its first 32 words are also the
     * complete $C0F812 seed; mode pointers select 16-word views of this bank. */
    uint16_t initial[32],static_words[32],mode_words[256];
    const uint16_t *modes[16];
} FA18DisplayPaletteAssets;
/* Import the actual caller-selected inst5/frnt5 ILBM CMAP and Hunk 21's
 * static/mode words. No BSS/Chip-RAM snapshot, palette synthesis or mode
 * default. The caller chooses the original resource and loading order.
 * Raw Hunk words retain their upper bits; display output does the masking.
 * Keep this owner stable while its palette pointers are used. */
int fa18_import_display_palette_assets(FA18DisplayPaletteAssets *assets,
                                          const FA18Hunks *exe,
                                          const uint8_t *ilbm,size_t ilbm_bytes);
#endif
