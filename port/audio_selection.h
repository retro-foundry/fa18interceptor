#ifndef FA18_AUDIO_SELECTION_H
#define FA18_AUDIO_SELECTION_H

#include "command_effects.h"

/* Complete $C17B08/$C17B2C game behavior, with the channel masks and slots
 * owned by the same audio state as commands and audio updates. Sound entries
 * are resolved native voice pointers imported from the original assets.
 * Volume is the original integer argument; its 16-bit shift wraps at 32 bits.
 * Return 0 for missing bounds/services/data; preceding writes remain applied. */
int fa18_release_native_audio_channel(FA18CommandAudio *audio, unsigned channel);
int fa18_select_native_sound(FA18CommandAudio *audio,
                              PortVoice *const *voices, size_t count,
                              size_t sound, unsigned channel, uint32_t volume);

/* Complete $C17B96. The fading gate suppresses all work. Bit 7 of sound_flags
 * releases every channel then selects sounds 13/14 at volume 63; otherwise
 * bit 2 of effect_flags selects 35/36 at the supplied volume. The remaining
 * branch releases all channels. Successful pair selection sets fading to 2,
 * even when its imported sound entries are empty. */
int fa18_start_native_menu_sound(FA18CommandAudio *audio,
                                  PortVoice *const *voices, size_t count,
                                  uint32_t volume);

#endif
