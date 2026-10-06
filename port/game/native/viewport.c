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
void native_viewport_tick(NativeFrontend *game) {
    const InputDeviceHooks hooks={palette_child,NULL,game};
    advance_viewport_palette(PALETTE_FRAME,&hooks);
}
