/* Native consumers for C1612C. C53F88 calls graphics.WaitBOVP (-402):
 * suspend until the next host PAL presentation boundary. Raster work is
 * synchronous, so C53F44 WaitBlit already has a completed host submission.
 * Amiga view/Copper objects are replaced by the selected host plane page;
 * the source owner still publishes its saved pair and swaps DRAW_PAGE. */
#include "display.h"
#include "../globals.h"
#include "../render_page.h"
#include <stdlib.h>

enum { DISPLAY_FRAME=0x30c0,PLANE_BYTES=40*256 };
static int is_wait(enum InputDisplayChild child) {
    switch(child) {
    case IDS_WAIT_PUBLICATION: case IDS_WAIT_ACTIVITY:
    case IDS_WAIT_STATIC_FIRST: case IDS_WAIT_STATIC_SECOND:
    case IDS_WAIT_DYNAMIC_FIRST: case IDS_WAIT_DYNAMIC_SECOND:
    case IDS_WAIT_CLEAR_PALETTE: return 1;
    default: return 0;
    }
}
static int await_child(void *context,enum InputDisplayChild child) {
    NativeFrontend *game=context;
    if(!is_wait(child)) return 1;
    if(!game->display_wait_pending) {
        game->display_wait_tick=game->ticks;
        game->display_wait_pending=1;
        return 0;
    }
    if(game->ticks==game->display_wait_tick) return 0;
    game->display_wait_pending=0;
    return 1;
}
static int32_t consume(void *context,enum InputDisplayChild child) {
    NativeFrontend *game=context;
    gaddr palette;
    if(is_wait(child) || child==IDS_WAIT_BLIT) return 0;
    switch(child) {
    case IDS_LOAD_VIEW:
        game->displayed_page=rd_u16(DISPLAY_FRAME-2);
        if(game->displayed_page>1) abort();
        ++game->display_publications;
        return 0;
    case IDS_LOAD_STATIC_PALETTE: palette=0xc084d0; break;
    case IDS_LOAD_DYNAMIC_PALETTE: case IDS_LOAD_CLEAR_PALETTE:
        palette=rd_u32(LONG_TABLE); break;
    default: abort();
    }
    for(unsigned i=0;i<32;++i) game->palette[i]=rd_u16(palette+2*i);
    return 0;
}
void native_display_begin_frame(NativeFrontend *game) {
    if(!game->display_drawing) {
        select_draw_page();
        game->display_drawing=1;
    }
}
void native_display_finish_frame(NativeFrontend *game) {
    if(!game->display_drawing || game->display_pending) abort();
    game->display_drawing=0;
    game->display_pending=1;
    game->display.phase=OUTER_WAIT_PUBLICATION;
    (void)native_display_resume(game);
}
int native_display_resume(NativeFrontend *game) {
    const InputDisplayHooks hooks={consume,NULL,game};
    if(!advance_outer_display(DISPLAY_FRAME,&hooks,&game->display,await_child)) {
        ++game->display_yields;
        return 0;
    }
    game->display_pending=0;
    return 1;
}
void native_display_read_pixels(NativeFrontend *game) {
    gaddr table=PAGE0_PLANE_TABLE+16*game->displayed_page;
    gaddr planes[4];
    for(unsigned p=0;p<4;++p) planes[p]=rd_u32(table+4*p);
    for(unsigned y=0;y<256;++y) for(unsigned x=0;x<320;++x) {
        uint8_t index=0;
        for(unsigned p=0;p<4;++p)
            if(rd_u8(planes[p]+y*40+x/8)&(0x80u>>(x&7)))
                index|=(uint8_t)(1u<<(3-p));
        game->indices[y*320+x]=index;
    }
}
