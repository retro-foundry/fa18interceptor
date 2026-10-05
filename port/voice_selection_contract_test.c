#include "voice_selection.h"
#include <assert.h>

typedef struct {
    PortVoice **slots, **voices;
    PortVoice *replacement;
    unsigned calls, channel;
    int change;
} Observer;

static void acknowledge(void *context,unsigned channel) {
    Observer *o=context;
    assert(channel==o->channel);
    if(!o->calls) {
        assert(o->slots[channel]==NULL);
        if(o->change) o->voices[0]=o->replacement;
    } else assert(o->slots[channel]==o->voices[0]);
    ++o->calls;
}

int main(void) {
    PortVoice a={0},b={0};
    PortVoice *voices[1]={&a},*slots[2]={&a,NULL};
    Observer o={slots,voices,&b,0,0,1};
    PortVoiceSelection s={voices,1,slots,2,acknowledge,&o};
    a.volume=0x12345678;
    assert(port_select_voice(&s,0,0,0xfedcba98)==PORT_VOICE_SELECTION_OK);
    assert(slots[0]==&b && b.volume==0xfedcba98 && a.volume==0x12345678 && o.calls==2);
    o.calls=0; o.replacement=NULL;
    assert(port_select_voice(&s,0,0,0)==PORT_VOICE_SELECTION_MISSING_VOICE);
    assert(!slots[0] && !voices[0] && o.calls==1);
    /* Empty sounds need neither a backend nor a channel release. */
    s.acknowledge=NULL; slots[0]=&a;
    assert(port_select_voice(&s,0,0,0)==PORT_VOICE_SELECTION_OK && slots[0]==&a);
    assert(port_release_voice_channel(&s,0)==PORT_VOICE_SELECTION_INVALID_ARGUMENT && slots[0]==&a);
    assert(port_select_voice(&s,1,0,0)==PORT_VOICE_SELECTION_INVALID_ARGUMENT && slots[0]==&a);
    assert(port_select_voice(&s,0,2,0)==PORT_VOICE_SELECTION_INVALID_ARGUMENT);
    /* Sound-table/slot aliasing exposes the removal instead of playing a
     * stale captured voice. Source would dereference an invalid address. */
    o.calls=0; o.change=0; s.voices=slots; s.acknowledge=acknowledge;
    assert(port_select_voice(&s,0,0,0)==PORT_VOICE_SELECTION_MISSING_VOICE && !slots[0] && o.calls==1);
    return 0;
}
