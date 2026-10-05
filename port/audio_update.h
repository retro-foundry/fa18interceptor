#ifndef FA18_AUDIO_UPDATE_H
#define FA18_AUDIO_UPDATE_H

#include "command_effects.h"

typedef enum { FA18_AUDIO_PERIOD, FA18_AUDIO_VOLUME } FA18AudioParameter;
typedef struct {
    FA18CommandVoice **slot; /* actual descriptor slot; aliases are preserved */
    int (*output)(void *context, FA18AudioParameter parameter, uint16_t value);
    void *context; /* actual native output channel, resolved by its owner */
} FA18AudioUpdateChannel;
typedef struct {
    FA18CommandAudio *audio;
    FA18AudioUpdateChannel channels[4];
} FA18AudioUpdate;

/* Decode all selectors used by the five original sound programs ($C50B78..
 * $C50CD0). Import from actual assets, without implicit terminators/defaults.
 * Returns 0 for unknown selectors or a length that cannot fit a byte cursor.
 * Previously decoded operations may remain on an error. */
int fa18_import_voice_operations(const uint32_t *selectors, size_t count,
                                  PortVoiceOperation *operations);

/* Complete $C50212, $C501E0 and $C50158 game behavior. Outputs are ordered
 * period then volume; a terminated program still outputs/slides its original
 * voice that tick. Empty slots produce no output. Delay and arithmetic wrap
 * match the source. Returns 0 for missing assets/services, an invalid program
 * cursor/jump or an output failure; preceding source writes remain applied. */
int fa18_step_native_voice_program(FA18CommandAudio *audio,
                                    FA18CommandVoice *voice,
                                    FA18CommandVoice **slot, unsigned channel);
int fa18_output_native_voice(FA18CommandAudio *audio,
                              const FA18CommandVoice *voice,
                              const FA18AudioUpdateChannel *channel);
int fa18_update_native_audio(FA18AudioUpdate *state);

/* Complete $C24FE8. Shares the command owner's fade gate, master level and
 * target. Preserves original signed comparisons and 32-bit wrapping. */
int fa18_fade_native_master_volume(FA18CommandAudio *audio);

#endif
