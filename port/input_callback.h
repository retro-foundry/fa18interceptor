#ifndef FA18_NATIVE_INPUT_CALLBACK_H
#define FA18_NATIVE_INPUT_CALLBACK_H

#include "audio_update.h"
#include "viewport_transition.h"

typedef struct {
    FA18CommandInput *commands;
    FA18CommandAudio *audio;
    FA18ViewportModeState *viewport;
    uint16_t counter_x, counter_y;
    int16_t mouse_x, ticks;
    /* Mouse Y is commands->indexed.throttle, its original shared word. */
    int16_t min_x, min_y, max_x, max_y;
} FA18NativeInputCallbackState;

/* Actual View.LOFCprList and ViewPort.DspIns owners. The viewport's ColorMap
 * is separate; publication changes the display list read by LoadRGB4. */
typedef struct { void *view, *display_list; } FA18NativeInputDisplayPair;
typedef struct {
    uint16_t draw_page;
    FA18NativeInputDisplayPair saved_pair;
    const FA18NativeInputDisplayPair *pairs;
    size_t pair_count;
    int first_pair;
    const uint16_t *const *mode_palettes;
    size_t mode_count;
    int first_mode;
    uint16_t *stable_palette;
    /* Actual native LoadRGB4 service; words/count are original game data.
     * Current saved pair belongs to this display owner, not a guest address. */
    int (*load_palette)(void *context, FA18ViewportPalettePhase phase,
                         const uint16_t *words);
    void *context;
} FA18NativeInputDisplay;

/* Install actual imported palette selection and pointer-pair publication
 * owners around the required native palette service. No pointer is fabricated. */
int fa18_prepare_native_input_display(FA18NativeInputDisplay *display,
                                       FA18ViewportTransitionOps *ops);

/* Attach queue-reachable X/tick words to their owners, importing current
 * values without replacing the already-shared Y word or readiness byte.
 * Caller imports previous counters, bounds and viewport/audio state. */
int fa18_initialize_native_input_callback(FA18NativeInputCallbackState *state,
                                           FA18CommandQueue *queue,
                                           FA18CommandAudio *audio,
                                           FA18ViewportModeState *viewport);

/* Complete $C1718E game callback. Supply the sampled original 8-bit counter
 * pair (Y in high byte, X in low byte), with the actual palette/publication
 * owners. Preserves signed/wrapped deltas, negative-Y halving when unready,
 * signed bound ordering, source counter/tick writes, viewport calls and the
 * real master fade. Return 0 on owner/data failure, preserving prior writes. */
int fa18_advance_native_input_callback(FA18NativeInputCallbackState *state,
                                        uint16_t counter_pair,
                                        const FA18ViewportTransitionOps *viewport_ops);
#endif
