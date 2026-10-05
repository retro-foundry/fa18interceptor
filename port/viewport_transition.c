#include "viewport_transition.h"
#include <stddef.h>

static int signed_byte(uint8_t value) { return value<128?(int)value:(int)value-256; }

int fa18_advance_native_viewport_transition(FA18ViewportModeState *s,
                                            const FA18ViewportTransitionOps *ops) {
    const uint16_t *palette;
    uint16_t bits;
    int index;
    unsigned i;
    if(!s || !ops || !ops->select_palette || !ops->load_palette ||
       !ops->publish_pair || !ops->draw_page || !ops->stable_palette) return 0;
    if(s->current==s->target)
        return !s->state || ops->load_palette(ops->context,FA18_PALETTE_STABLE,ops->stable_palette);
    s->countdown=(uint8_t)(s->countdown-1);
    if(signed_byte(s->countdown)>=0) return 1;
    if(signed_byte(s->current)>signed_byte(s->target)) {
        s->current=(uint8_t)(s->current-1); s->countdown=1;
    } else {
        s->current=(uint8_t)(s->current+1); s->countdown=2;
    }
    palette=ops->select_palette(ops->context,signed_byte(s->current));
    if(!palette || !ops->load_palette(ops->context,FA18_PALETTE_FIRST,palette)) return 0;
    bits=(uint16_t)(1u-*ops->draw_page);
    index=bits<0x8000u?(int)bits:(int)bits-0x10000;
    if(!ops->publish_pair(ops->context,index) ||
       !ops->load_palette(ops->context,FA18_PALETTE_SECOND,palette) ||
       !ops->publish_pair(ops->context,index)) return 0;
    if(s->current==s->target) {
        for(i=0;i<16;++i) ops->stable_palette[i]=palette[i];
        s->state=3;
    }
    return 1;
}
