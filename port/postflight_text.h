#ifndef FA18_NATIVE_POSTFLIGHT_TEXT_H
#define FA18_NATIVE_POSTFLIGHT_TEXT_H
#include "input_callback.h"
#include "stage_callback.h"
#include "display_palette_assets.h"

typedef struct {
    FA18NativeInputDisplay *display;
    FA18FlightCommandState *flight;
    uint16_t *countdown,*checksums[3];
    FA18StageCallback *callback;
    uint8_t transition;
    const uint16_t *palette_seed; /* 32 original source-order words */
    uint8_t *text_descriptor; /* mutable original selector-97 descriptor */
    size_t text_bytes;
    FA18CommandAudio *audio;
    PortVoice *const *sounds;
    size_t sound_count;
} FA18NativePostflightText;

typedef struct {
    /* Actual complete $C08F26 scene bootstrap. Required; returns 1 on success.
     * It may replace the dynamic palette pointer and change shared state.
     * Bootstrap still requires its complete native child graph. */
    int (*bootstrap_scene)(void *context);
    void *context;
} FA18NativePostflightTextOps;

/* Bind the mutable original selector-97 descriptor and source palette bank.
 * Keeps the executable data owner live; no text or palette is synthesized.
 * Executable and palette owner must remain stable while the pointers are used.
 * Does not seed the dynamic palette or initialize the checksum producers. */
int fa18_bind_native_postflight_text_assets(FA18NativePostflightText *state,
                                            const FA18Hunks *exe,
                                            const FA18DisplayPaletteAssets *palettes);

/* Complete $C0F812: bootstrap first; sequentially copy all 32 words into its
 * newly read dynamic palette; publish shared command/pause/transition/countdown
 * and callback fields; preserve ordered signature tests and last differing
 * checksum, including zero. Actual hex/menu-audio children execute directly.
 * Returns 0 on missing owners, data bounds or child failure. Prior bootstrap
 * effects and source stores remain; no bootstrap or next-stage substitute. */
int fa18_publish_native_postflight_text(FA18NativePostflightText *state,
                                        const FA18NativePostflightTextOps *ops);
#endif
