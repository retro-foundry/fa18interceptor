#include "viewport.h"
#include "../globals.h"
#include "../input_device_callbacks.h"
#include "../audio.h"
#include <stdio.h>
#include <stdlib.h>

enum { PALETTE_FRAME=0x3080 };
static int32_t palette_child(void *context,enum InputDeviceChild child) {
    NativeFrontend *game=context;gaddr source;
    switch(child) {
    case IDC_START_SERVER: {
        const InputDeviceHooks hooks={palette_child,NULL,game};
        install_input_device_callback(&hooks);return 0;
    }
    case IDC_ADD_SERVER: game->input_server_installed=1;return 0;
    case IDC_REMOVE_SERVER: game->input_server_installed=0;return 0;
    case IDC_PALETTE_FIRST: case IDC_PALETTE_SECOND:
        source=rd_u32(PALETTE_FRAME-16);break;
    case IDC_PALETTE_STABLE: source=rd_u32(LONG_TABLE);break;
    case IDC_FADE: fade_master_volume();return 0;
    default: fprintf(stderr,"native viewport child unavailable: %u\n",(unsigned)child);abort();
    }
    /* C53EC0 loads sixteen RGB4 colours into each view. The host presents
     * one selected page, while the source owner retains its view-pair stores. */
    for(unsigned i=0;i<16;++i) game->palette[i]=rd_u16(source+2*i);
    return 0;
}
void native_viewport_install_callback(NativeFrontend *game) {
    const InputDeviceHooks hooks={palette_child,NULL,game};
    install_input_device_callback(&hooks); /* C17456 */
}
void native_viewport_remove_callback(NativeFrontend *game) {
    const InputDeviceHooks hooks={palette_child,NULL,game};
    remove_input_device_callback(&hooks); /* C1748C */
}
void native_viewport_initialize(NativeFrontend *game) {
    const InputDeviceHooks hooks={palette_child,NULL,game};
    /* C0F51A-C0F534 supplies -960..960 on both axes to C17104. The
     * executable's initial 0..319/199 bounds are for the preceding UI. */
    wr_s16(PALETTE_FRAME+10,-960);wr_s16(PALETTE_FRAME+14,-960);
    wr_s16(PALETTE_FRAME+18,960);wr_s16(PALETTE_FRAME+22,960);
    set_input_device_bounds(PALETTE_FRAME,&hooks);
    initialise_input_device_counters_sample(PALETTE_FRAME,&hooks,
        (uint16_t)(game->mouse_y_counter<<8|game->mouse_x_counter));
}
void native_viewport_tick(NativeFrontend *game) {
    if(!game->input_server_installed) {
        fputs("Native input PAL callback has not been installed\n",stderr);abort();
    }
    const InputDeviceHooks hooks={palette_child,NULL,game};
    advance_input_device_callback_sample(PALETTE_FRAME,&hooks,
        (uint16_t)(game->mouse_y_counter<<8|game->mouse_x_counter));
}
