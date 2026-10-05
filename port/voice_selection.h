#ifndef PORT_VOICE_SELECTION_H
#define PORT_VOICE_SELECTION_H

#include "voice_program.h"

/* Caller-owned sound table and channel slots. Acknowledgements may change
 * either table: selection rereads the sound after releasing the channel.
 * Volume is already in the caller's fixed-point representation. */
typedef struct {
    PortVoice *const *voices;
    size_t voice_count;
    PortVoice **slots;
    size_t channel_count;
    void (*acknowledge)(void *context, unsigned channel);
    void *context;
} PortVoiceSelection;

typedef enum {
    PORT_VOICE_SELECTION_OK,
    PORT_VOICE_SELECTION_INVALID_ARGUMENT,
    PORT_VOICE_SELECTION_MISSING_VOICE
} PortVoiceSelectionResult;

/* Clear the slot before acknowledging, including an already empty slot. */
PortVoiceSelectionResult port_release_voice_channel(const PortVoiceSelection *selection,
                                                     unsigned channel);
/* An empty sound entry succeeds without changing anything. Otherwise release,
 * reread the entry, set volume, publish the slot, then acknowledge again.
 * If the release callback removes the selected voice, report missing voice;
 * the preceding release remains applied. No implicit voice is supplied. */
PortVoiceSelectionResult port_select_voice(const PortVoiceSelection *selection,
                                            size_t sound, unsigned channel,
                                            uint32_t volume);

#endif
