#include "menu_start.h"
#include "../menu_setup.h"
#include "../audio.h"
#include "../render_buffers.h"
#include "../stages.h"
#include <stdio.h>
#include <stdlib.h>

static void menu_setup_child(void *context,enum MenuSetupCall call,uint32_t value) {
    NativeMenuSetup *setup=context;
    switch(call) {
    case MENU_SETUP_SOUND: start_menu_sound_pair((int32_t)value); break;
    case MENU_SETUP_CLEAR: clear_render_buffers(); break;
    case MENU_SETUP_RESET: reset_message_sequence(); break;
    case MENU_SETUP_DELAY:
        /* Nominal C0E78A conversion for the menu's short pause. No input/game
         * update runs during this host-clock pause; source CPU timing is not
         * reproduced by this conversion. */
        setup->ready_tick+=(unsigned)((value*66ull*50+7093790-1)/7093790);
        setup->pending=1;
        break;
    case MENU_SETUP_SCRIPT: load_long_table(value); break;
    default: fprintf(stderr,"native menu setup child unavailable: %u\n",(unsigned)call);abort();
    }
}
void native_menu_begin(NativeMenuSetup *setup,unsigned ticks) {
    setup->ready_tick=ticks;setup->pending=0;
    const MenuSetupHooks hooks={menu_setup_child,NULL,setup};
    begin_top_level_menu(&hooks);
}
int native_menu_resume(NativeMenuSetup *setup,unsigned ticks) {
    if(!setup->pending) return 1;
    if(ticks<setup->ready_tick) return 0;
    const MenuSetupHooks hooks={menu_setup_child,NULL,setup};
    finish_top_level_menu(&hooks);
    setup->pending=0;
    return 1;
}
