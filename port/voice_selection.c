#include "voice_selection.h"

PortVoiceSelectionResult port_release_voice_channel(const PortVoiceSelection *s,
                                                     unsigned channel) {
    if(!s || !s->slots || channel>=s->channel_count || !s->acknowledge)
        return PORT_VOICE_SELECTION_INVALID_ARGUMENT;
    s->slots[channel]=NULL;
    s->acknowledge(s->context,channel);
    return PORT_VOICE_SELECTION_OK;
}

PortVoiceSelectionResult port_select_voice(const PortVoiceSelection *s,
                                            size_t sound, unsigned channel,
                                            uint32_t volume) {
    PortVoiceSelectionResult result;
    if(!s || !s->voices || sound>=s->voice_count || !s->slots ||
       channel>=s->channel_count) return PORT_VOICE_SELECTION_INVALID_ARGUMENT;
    if(!s->voices[sound]) return PORT_VOICE_SELECTION_OK;
    result=port_release_voice_channel(s,channel);
    if(result!=PORT_VOICE_SELECTION_OK) return result;
    if(!s->voices[sound]) return PORT_VOICE_SELECTION_MISSING_VOICE;
    s->voices[sound]->volume=volume;
    s->slots[channel]=s->voices[sound];
    s->acknowledge(s->context,channel);
    return PORT_VOICE_SELECTION_OK;
}
