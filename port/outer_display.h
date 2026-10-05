#ifndef FA18_NATIVE_OUTER_DISPLAY_H
#define FA18_NATIVE_OUTER_DISPLAY_H
#include "input_callback.h"

typedef enum {
    FA18_DISPLAY_WAIT_PUBLICATION, FA18_DISPLAY_WAIT_ACTIVITY,
    FA18_DISPLAY_WAIT_BLIT, FA18_DISPLAY_WAIT_STATIC_FIRST,
    FA18_DISPLAY_WAIT_STATIC_SECOND, FA18_DISPLAY_WAIT_DYNAMIC_FIRST,
    FA18_DISPLAY_WAIT_DYNAMIC_SECOND, FA18_DISPLAY_WAIT_CLEAR
} FA18NativeDisplayWait;
typedef enum {
    FA18_DISPLAY_PALETTE_STATIC, FA18_DISPLAY_PALETTE_DYNAMIC,
    FA18_DISPLAY_PALETTE_CLEAR
} FA18NativeDisplayPalettePhase;
typedef struct {
    FA18NativeInputDisplay *display;
    FA18ViewportModeState *viewport;
    uint8_t *activity;
    const uint16_t *status_word;
    const uint16_t *static_palette; /* imported 32 original words */
    /* display->stable_palette is the shared dynamic palette pointer, with
     * 32 words here (the input callback copies/loads its lower 16). */
} FA18NativeOuterDisplay;
typedef struct {
    /* Native synchronization/presentation owners may advance input/audio
     * and mutate shared game fields. Return 1 on success, 0 on failure. */
    int (*wait)(void *context,FA18NativeDisplayWait phase);
    int (*load_view)(void *context,FA18NativeInputDisplay *display);
    int (*load_palette)(void *context,FA18NativeDisplayPalettePhase phase,
                         const uint16_t *words,size_t count);
    void *context;
} FA18NativeOuterDisplayOps;

/* Complete $C1612C. Uses the same page/pair/dynamic palette and mode owners
 * as $C1718E. Each wait/load remains an actual supplied native service.
 * Signed activity loops and word-sized page toggles follow the source;
 * shared fields and palette pointers are reloaded after service calls.
 * Missing owners/services fail without substituting a wait or presentation;
 * writes preceding a failure remain. No CPU, bus or guest pointer state. */
int fa18_synchronize_native_outer_display(FA18NativeOuterDisplay *state,
                                             const FA18NativeOuterDisplayOps *ops);
#endif
