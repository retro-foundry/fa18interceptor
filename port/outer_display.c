#include "outer_display.h"

static int wait_display(const FA18NativeOuterDisplayOps *ops,FA18NativeDisplayWait phase) {
    return ops->wait && ops->wait(ops->context,phase);
}
static int load_display(const FA18NativeOuterDisplayOps *ops,
                          FA18NativeDisplayPalettePhase phase,const uint16_t *words) {
    return words && ops->load_palette && ops->load_palette(ops->context,phase,words,32);
}
int fa18_synchronize_native_outer_display(FA18NativeOuterDisplay *s,
                                             const FA18NativeOuterDisplayOps *ops) {
    uint16_t bits;
    int index;
    if(!s || !s->display || !s->viewport || !s->activity || !s->status_word || !ops) return 0;
    if(!wait_display(ops,FA18_DISPLAY_WAIT_PUBLICATION)) return 0;
    bits=s->display->draw_page;
    index=bits<0x8000u?(int)bits:(int)bits-0x10000;
    if(!fa18_publish_native_display_pair(s->display,index) || !ops->load_view ||
       !ops->load_view(ops->context,s->display)) return 0;
    if(*s->activity) {
        if(!wait_display(ops,FA18_DISPLAY_WAIT_ACTIVITY) ||
           !wait_display(ops,FA18_DISPLAY_WAIT_BLIT)) return 0;
        while(*s->activity && *s->activity<128) {
            if(!load_display(ops,FA18_DISPLAY_PALETTE_STATIC,s->static_palette) ||
               !wait_display(ops,FA18_DISPLAY_WAIT_STATIC_FIRST) ||
               !wait_display(ops,FA18_DISPLAY_WAIT_STATIC_SECOND) ||
               !load_display(ops,FA18_DISPLAY_PALETTE_DYNAMIC,s->display->stable_palette) ||
               !wait_display(ops,FA18_DISPLAY_WAIT_DYNAMIC_FIRST) ||
               !wait_display(ops,FA18_DISPLAY_WAIT_DYNAMIC_SECOND)) return 0;
            *s->activity=(uint8_t)(*s->activity-1);
        }
    } else if(s->viewport->state) {
        s->viewport->state=(uint8_t)(s->viewport->state-1);
        if(!(*s->status_word&0x100)) {
            if(!wait_display(ops,FA18_DISPLAY_WAIT_CLEAR) ||
               !load_display(ops,FA18_DISPLAY_PALETTE_CLEAR,s->display->stable_palette)) return 0;
        }
    }
    s->display->draw_page=(uint16_t)(1u-s->display->draw_page);
    return 1;
}
