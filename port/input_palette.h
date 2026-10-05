#ifndef FA18_INPUT_PALETTE_H
#define FA18_INPUT_PALETTE_H
#include "input_callback.h"
#include "amiga/rgb4.h"

typedef struct {
    FA18NativeInputDisplay *display;
    /* The actual viewport's fixed ColorMap. Its DspIns list is the separately
     * published saved_pair.display_list, reread on each LoadRGB4 call. */
    AmigaRgb4Palette *color_map;
} FA18NativeInputPalette;

/* Install the real ordinary-buffer LoadRGB4 service. saved_pair.display_list must
 * point to an actual AmigaRgb4CopperList, or be NULL before list creation.
 * Each call rereads that list after game-side pair publication. */
int fa18_bind_native_input_palette(FA18NativeInputPalette *palette,
                                     FA18NativeInputDisplay *display,
                                     AmigaRgb4Palette *color_map);
int fa18_load_native_input_palette(void *context,FA18ViewportPalettePhase phase,
                                      const uint16_t *words);
#endif
