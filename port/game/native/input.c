/* C0F3C4 -> C16EAE/C16C56/C1AD74 using host events and existing game owners.
 * Native SDL/replay events replace OS keyboard descriptors, not command logic.
 * No gameport descriptor is supplied by this keyboard-only host backend. */
#include "input.h"
#include "menu.h"
#include "../pending_input.h"
#include "../input_events.h"
#include "../player_input.h"
#include "../globals.h"
#include <stdio.h>
#include <stdlib.h>

enum { KEY_DESCRIPTOR=0x3140 };
void native_input_enqueue(NativeFrontend *game,int key,int down) {
    unsigned raw=native_menu_raw_key(key,down);
    if(raw==0xff) return;
    if(game->input_count==sizeof game->input_keys) {
        fputs("native host keyboard queue exhausted before source input poll\n",stderr);abort();
    }
    unsigned slot=(game->input_read+game->input_count)%sizeof game->input_keys;
    game->input_keys[slot]=(uint8_t)raw;
    ++game->input_count;
}
static uint32_t event_child(void *context,enum InputEventChild child) {
    NativeFrontend *game=context;
    switch(child) {
    case INPUT_EXTERNAL_READ: return 0;
    case INPUT_DIRECTION_REFRESH: latch_joystick_input(game->joystick_directions);return 0;
    case INPUT_KEYBOARD_READ:
        if(!game->input_count) return 0;
        wr_u32(KEYBOARD_INPUT_DESCRIPTOR,KEY_DESCRIPTOR);
        wr_u16(KEY_DESCRIPTOR+6,game->input_keys[game->input_read]);
        game->input_read=(game->input_read+1)%sizeof game->input_keys;
        --game->input_count;++game->input_events;
        return 1;
    case INPUT_KEYBOARD_RELEASE: return 0; /* host queue entry has been consumed */
    case INPUT_RAW_SOURCE: {
        const InputEventHooks hooks={event_child,NULL,game};
        return read_keyboard_event_source(&hooks);
    }
    default:
        fprintf(stderr,"native input event child unavailable: %u\n",(unsigned)child);abort();
    }
}
static uint32_t pending_child(void *context,enum PendingInputChild child,uint8_t key) {
    NativeFrontend *game=context;
    const InputEventHooks events={event_child,NULL,game};
    switch(child) {
    case PENDING_PREPARE: consume_external_input_event(&events);break;
    case PENDING_BUTTONS: return game->mouse_buttons;
    case PENDING_CHANGED: consume_changed_buttons(&events);break;
    case PENDING_POLL_KEY: return poll_raw_keyboard_event(&events);
    case PENDING_DISPATCH_KEY: native_menu_dispatch_raw(game,key);break;
    case PENDING_WAIT_ONE: case PENDING_WAIT_BOTH: native_menu_dispatch_pending(game);break;
    default: abort();
    }
    return 0;
}
void native_input_process(NativeFrontend *game) {
    const PendingInputHooks hooks={pending_child,NULL,game};
    process_pending_key_events(&hooks);
    ++game->input_passes;
}
