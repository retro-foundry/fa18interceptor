#include "menu_cold.h"
#include "globals.h"
#include "view.h"
#include <stdlib.h>
static void observe(const MenuColdHooks *h,enum MenuColdPhase phase,uint32_t value,gaddr address) {
    if(h->observe) h->observe(h->context,phase,value,address);
}
static void consume(const MenuColdHooks *h,enum MenuColdChild child) {
    if(!h->consume) abort();
    h->consume(h->context,child);
}
static void byte(const MenuColdHooks *h,gaddr address,uint8_t value) {
    wr_u8(address,value); observe(h,MC_BYTE_STORE,value,address);
}
static void callback(const MenuColdHooks *h,gaddr value,int lea) {
    wr_u32(STAGE_CALLBACK,value); observe(h,MC_CALLBACK,value,(gaddr)lea);
}

void leave_menu_after_countdown(const MenuColdHooks *h,int set_context) {
    uint16_t countdown;
    if(set_context) byte(h,CONTEXT_GATE,1);
    countdown=rd_u16(POST_INPUT_COUNTDOWN); observe(h,MC_WORD_D0,countdown,0);
    if((int16_t)countdown<0) {
        observe(h,MC_WORD_D0,0,1);
        byte(h,POST_INPUT_AUX,0); byte(h,VIEWPORT_TARGET,15); byte(h,VIEWPORT_MODE,0);
        callback(h,set_context?0xc10418:0xc1029e,1);
    }
}
void set_menu_position_preset(const MenuColdHooks *h,int alternate) {
    uint32_t x=alternate?0x0f248000u:0x10800000u;
    uint32_t y=alternate?0x5000u:0x03000000u;
    uint32_t z=alternate?0x0fccf000u:0x10c00000u;
    set_observer_position((int32_t)x,(int32_t)y,(int32_t)z);
    observe(h,MC_POSITION_PRESET,(uint32_t)alternate,0);
}
void refresh_menu_cockpit(const MenuColdHooks *h) {
    uint16_t flags;
    consume(h,MENU_COLD_POSITION);
    flags=rd_u16(COCKPIT_FLAGS)|0x800u;
    observe(h,MC_COCKPIT,flags,0); wr_u16(COCKPIT_FLAGS,flags);
    consume(h,MENU_COLD_UPDATE);
}
