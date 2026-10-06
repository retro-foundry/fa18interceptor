/* Native intro/menu owner. Source startup C0E2E8 loads pix/splsh, C0F812 /
 * C11446 select credits message 15; C11478 acknowledges; C115BA-C1175A
 * handle tour/name; C0FBE0 queues the main menu. C32CEE is reused directly.
 * Executable hunks supply only original tables, fonts, strings and BSS. */
#include "frontend.h"
#include "../memory.h"
#include "../main_loop_control_messages.h"
#include "../menu_setup.h"
#include "../stages.h"
#include "../globals.h"
#include "../player_input.h"
#include "menu.h"
#include "flight.h"
#include "clock.h"
#include "display.h"
#include "input.h"
#include "cockpit_assets.h"
#include "audio_assets.h"
#include "viewport.h"
#include "../audio.h"
#include "../text.h"
#include "../../romfree/placement.h"
#include "../../romfree/media.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#include <direct.h>
#define make_directory(path) _mkdir(path)
#else
#include <sys/stat.h>
#define make_directory(path) mkdir(path,0755)
#endif
/* Keep the original immutable image hunk at $012988 outside display pages. */
enum { PLANE_TABLE=0x1000,PLAYER_LOG=0x2000,PLANE_FIRST=0x34000,PLANE_SECOND=0x40000,PLANE_BYTES=40*256 };
static int fail(char *error,size_t cap,const char *why) { if(cap) snprintf(error,cap,"Native startup: %s",why); return 0; }
void native_frontend_clear_text(void) {
    memset(native_storage_range(PLANE_FIRST,4*PLANE_BYTES),0,4*PLANE_BYTES);
    memset(native_storage_range(PLANE_SECOND,4*PLANE_BYTES),0,4*PLANE_BYTES);
}
static void select_screen(NativeFrontend *game,enum NativeScreen screen,uint16_t message,unsigned mode) {
    native_frontend_clear_text(); reset_message_sequence(); game->screen=screen; game->screen_ticks=0;
    wr_u16(0xc4574au,message); wr_u8(0xc457e0u,(uint8_t)mode);
    for(unsigned i=0;i<32;++i) game->palette[i]=rd_u16(0xc08490u+2*i);
}
void native_frontend_start_menu(NativeFrontend *game) {
    select_screen(game,NATIVE_MENU,0,0);
    native_menu_begin(&game->menu_setup,game->ticks);
    wr_u8(MODE_SELECT,0);
}
void native_frontend_enlist(NativeFrontend *game) {
    select_screen(game,NATIVE_ENLISTMENT,rd_u16(PLAYER_LOG+4)?2:1,0);
    if(rd_u16(PLAYER_LOG+4)) for(unsigned i=0;i<24 && rd_u8(PLAYER_LOG+30+i);++i)
        wr_u8(0xc3f1edu+i,rd_u8(PLAYER_LOG+30+i));
    game->name_finished=0;
}
void native_frontend_save_log(NativeFrontend *game) {
    FILE *file=fopen(game->config_path,"wb");
    if(!file) { perror(game->config_path); abort(); }
    int written=fwrite(native_storage_range(PLAYER_LOG,78),1,78,file)==78;
    int closed=fclose(file)==0;
    if(!written || !closed) { fprintf(stderr,"cannot save native flight log\n"); abort(); }
    wr_u8(MODE_TABLE_CHANGED,0);
}
static MessageWorking child(void *context,enum MainControlChild which,MessageWorking w) {
    NativeFrontend *game=context;
    if(which>=MC_GLYPH_FIRST && which<=MC_GLYPH_FOURTH) {
        unsigned plane=(unsigned)(which-MC_GLYPH_FIRST);
        unsigned bits=(w.colour&(1u<<(3-plane))?0xbfa:0xb0a)|w.style;
        plot_glyph8(w.glyph,rd_u32(w.planes+4*plane)+w.offset,(bits>>12)&15,450>>6,(bits&0xf0)!=0);
        ++game->glyphs; return w;
    }
    if(which==MC_SEQUENCE_TONE || which==MC_SEQUENCE_RESTART_TONE) {
        play_tone(1,2); /* C3316A supplies kind 1 and D1 = 2. */
        return w;
    }
    if(which==MC_FINISH_SEQUENCE) {
        /* C1643A writes exactly the source's 78-byte flight log. Native saves
         * are an overlay; the supplied ADF is always read-only. */
        native_frontend_save_log(game);
        game->name_finished=1; return w;
    }
    if(which==MC_ACCEPT_TYPED_CODE) { check_typed_code(); return w; }
    fprintf(stderr,"native frontend child unavailable: %u\n",(unsigned)which); abort();
}
int native_frontend_open(NativeFrontend *game,const char *path,const char *save_dir,char *error,size_t cap) {
    AmigaOfs disk={0}; AmigaHunks hunks={0}; FA18MediaInfo media;
    uint8_t *exe=NULL,*bytes=NULL; size_t size=0; int ok=0;
    memset(game,0,sizeof *game); native_storage_bind(&game->storage);
    native_audio_bind(&game->audio);
    native_clock_set(0);
    if(!save_dir || !*save_dir || snprintf(game->config_path,sizeof game->config_path,"%s/config",save_dir)>=(int)sizeof game->config_path)
        return fail(error,cap,"invalid save directory");
    if(make_directory(save_dir) && errno!=EEXIST) return fail(error,cap,"cannot create save directory");
    if(!amiga_ofs_open(&disk,path)) return fail(error,cap,"cannot open OFS ADF");
    exe=amiga_ofs_read(&disk,"F-18 Interceptor",&size); fa18_media_inspect(&disk,exe,size,&media);
    if(!media.supported) { fail(error,cap,"unsupported executable version"); goto done; }
    if(!amiga_hunks_parse(&hunks,exe,size) || hunks.count!=sizeof fa18_placements/sizeof *fa18_placements) { fail(error,cap,"invalid executable hunks"); goto done; }
    for(unsigned i=0;i<hunks.count;++i) {
        AmigaHunkSegment *segment=&hunks.segments[i]; uint32_t base=fa18_placements[i].payload_base;
        uint8_t *destination=native_storage_range(base,segment->size);
        if(segment->kind!=AMIGA_HUNK_BSS) memcpy(destination,segment->data,segment->size);
        for(unsigned j=0;j<segment->reloc_count;++j) {
            AmigaHunkReloc *reloc=&segment->relocs[j];
            wr_u32(base+reloc->offset,rd_u32(base+reloc->offset)+fa18_placements[reloc->target].payload_base);
        }
    }
    bytes=amiga_ofs_read(&disk,"pix/splsh",&size);
    if(!bytes || !amiga_ilbm_decode(&game->splash,bytes,size,error,cap)) goto done;
    free(bytes); bytes=amiga_ofs_read(&disk,"config",&size);
    if(!bytes || size!=78) { fail(error,cap,"missing original 78-byte config"); goto done; }
    memcpy(native_storage_range(PLAYER_LOG,78),bytes,78);
    FILE *saved=fopen(game->config_path,"rb");
    if(saved) {
        int valid=fread(native_storage_range(PLAYER_LOG,78),1,78,saved)==78 && fgetc(saved)==EOF;
        if(fclose(saved)) valid=0;
        if(!valid) { fail(error,cap,"invalid saved 78-byte config"); goto done; }
    } else if(errno!=ENOENT) { fail(error,cap,"cannot read saved config"); goto done; }
    wr_u32(0xc1ab74u,PLAYER_LOG); wr_u32(0xc456b6u,PLANE_TABLE);
    for(unsigned i=0;i<4;++i) wr_u32(PLANE_TABLE+4*i,PLANE_FIRST+i*PLANE_BYTES);
    for(unsigned page=0;page<2;++page) for(unsigned i=0;i<4;++i)
        wr_u32(PAGE0_PLANE_TABLE+16*page+4*i,(page?PLANE_SECOND:PLANE_FIRST)+i*PLANE_BYTES);
    /* C2FD08 clears source work buffers of 2000 longs. Bank B's fifth
     * entry is POLY_MASK_PLANE, assigned below. The other work buffers
     * are separate from the two pages and recording buffers. */
    for(unsigned bank=0;bank<2;++bank) for(unsigned i=0;i<5;++i)
        wr_u32((bank?RENDER_BUFFERS_B:RENDER_BUFFERS_A)+4*i,0x50000+(bank*5+i)*RENDER_BUFFER_LONGS*4);
    wr_u32(POLY_MASK_PLANE,0x30000); /* Separate 40-byte rows, host-owned mask. */
    wr_u32(CIRCLE_SPANS_PTR,0x33000); /* 127-radius symmetric span workspace. */
    native_menu_initialize();
    /* C16518 reads the original flight-recorder byte and word buffers from
     * textply/textctl. Demonstration mode 3 consumes these through C1B27E;
     * they are disk assets, not RAM copied from a reference capture. Read's
     * requested lengths are capacity and four times capacity respectively. */
    for(unsigned buffer=0;buffer<2;++buffer) {
        free(bytes);bytes=amiga_ofs_read(&disk,buffer?"text/textctl":"text/textply",&size);
        if(!bytes) { fail(error,cap,"missing original flight-recorder data");goto done; }
        size_t limit=(size_t)rd_u32(RECORDER_SIZE)*(buffer?4u:1u);
        memcpy(native_storage_range(rd_u32(buffer?RECORDER_WORDS:RECORDER_START),limit),
            bytes,size<limit?size:limit);
    }
    if(!native_cockpit_load(&disk,error,cap)) goto done;
    if(!native_audio_load_resources(&disk,error,cap)) goto done;
    native_flight_initialize(game);
    game->screen=NATIVE_SPLASH; memcpy(game->palette,game->splash.palette,sizeof game->palette);
    for(unsigned y=0;y<game->splash.height;++y) memcpy(game->indices+y*320,game->splash.indices+y*game->splash.width,game->splash.width);
    ok=1;
done:
    free(exe); free(bytes); amiga_hunks_free(&hunks); amiga_ofs_close(&disk);
    if(!ok) native_frontend_close(game);
    return ok;
}
void native_frontend_close(NativeFrontend *game) { amiga_ilbm_free(&game->splash); native_audio_bind(NULL); native_storage_bind(NULL); }
const char *native_frontend_screen(const NativeFrontend *game) {
    static const char *names[]={"splash","credits","enlistment","callsign","menu","mode-intro","missions","pilot-log","scene-setup"}; return names[game->screen];
}
void native_frontend_key(NativeFrontend *game,int key) {
    native_storage_bind(&game->storage);
    if(key>='a' && key<='z') key-=32;
    if(game->menu_setup.pending) { native_input_enqueue(game,key,1);return; }
    if(native_flight_enabled(game)) {
        native_input_enqueue(game,key,1);return;
    }
    if(game->screen==NATIVE_CREDITS) {
        /* C11624 copies an existing callsign into message 2. */
        native_frontend_enlist(game);
    } else if(game->screen==NATIVE_CALLSIGN) {
        /* C32E1A consumes raw Amiga key events. Keep the original ring and
         * C331CE translation table; C32CEE owns editing and Return handling. */
        unsigned event=key=='\r'?0x44:key=='\b'?0x41:0xff;
        if(event==0xff) for(unsigned i=0;i<0x40;++i) if(rd_u8(0xc331ceu+i)==(uint8_t)key) { event=i; break; }
        if(event!=0xff && rd_u8(0xc457f9u)<10) {
            unsigned at=(rd_u8(0xc457f8u)+rd_u8(0xc457f9u))%10;
            wr_u8(0xc457e1u+at,(uint8_t)event); wr_u8(0xc457f9u,(uint8_t)(rd_u8(0xc457f9u)+1));
        }
    } else if(game->screen==NATIVE_MENU || game->screen==NATIVE_MISSIONS || game->screen==NATIVE_PILOT_LOG
        || game->screen==NATIVE_SCENE_SETUP || game->screen==NATIVE_MODE_INTRO)
        native_menu_key(game,key,1);
}
void native_frontend_event(NativeFrontend *game,int key,int down) {
    native_storage_bind(&game->storage);
    /* SDL1 replay codes; host maps SDL2 modifier keys at its input boundary. */
    if(key==303 || key==304) {
        unsigned mask=1u<<(key-303);
        if(down) game->shift_keys|=mask; else game->shift_keys&=~mask;
        wr_u8(KEY_STATE,(uint8_t)(game->shift_keys!=0)); return;
    }
    if(game->menu_setup.pending) { native_input_enqueue(game,key,down);return; }
    if(native_flight_enabled(game)) {
        native_input_enqueue(game,key,down);return;
    }
    if(down) native_frontend_key(game,key);
    else if(game->screen==NATIVE_MENU || game->screen==NATIVE_MISSIONS || game->screen==NATIVE_PILOT_LOG
        || game->screen==NATIVE_SCENE_SETUP || game->screen==NATIVE_MODE_INTRO)
        native_menu_key(game,key,0);
}
void native_frontend_raw_event(NativeFrontend *game,uint8_t raw,int down) {
    native_storage_bind(&game->storage);
    raw=(uint8_t)((raw&0x7f)|(down?0:0x80));
    if(game->menu_setup.pending) { native_input_enqueue_raw(game,raw);return; }
    if(native_flight_enabled(game)) native_input_enqueue_raw(game,raw);
    else if(game->screen==NATIVE_MENU || game->screen==NATIVE_MISSIONS || game->screen==NATIVE_PILOT_LOG)
        native_menu_dispatch_raw(game,raw);
    else { fprintf(stderr,"native raw replay event outside connected menu/flight screen: %u\n",game->screen);abort(); }
}
void native_frontend_tick(NativeFrontend *game) {
    native_storage_bind(&game->storage); ++game->ticks; ++game->screen_ticks;
    native_audio_bind(&game->audio);
    native_clock_set(game->ticks);
    native_viewport_tick(game);
    native_audio_tick(&game->audio);
    if(!native_menu_resume(&game->menu_setup,game->ticks)) return;
    if(game->display_pending && !native_display_resume(game)) {
        native_display_read_pixels(game); return;
    }
    if(game->screen==NATIVE_SPLASH) {
        /* C0E53C's $A000 busy-loop iterations (C0E78A), nominal 68000
         * instruction timing converted once to PAL ticks. No CPU executes.
         * DMA contention/loading and fade timing remain reference-only. */
        const unsigned ticks=(unsigned)((0xa000ull*66*50+7093790-1)/7093790);
        if(game->screen_ticks<ticks) return;
        select_screen(game,NATIVE_CREDITS,15,0);
    }
    /* One C0EFD4 entry. Its clock poll and C1612C display continuation keep
     * the same iteration; replay input must never advance while suspended. */
    if(!game->flight_timer_pending) {
        /* C0F920 publishes C0FBE0 for the following update. Re-enter the
         * existing native menu owner after its last flight display returns. */
        if(game->screen==NATIVE_SCENE_SETUP && rd_u32(STAGE_CALLBACK)==0xc0fbe0)
            native_frontend_start_menu(game);
        if(game->menu_setup.pending) return;
        ++game->update_iterations;
        if(game->begin_update) game->begin_update(game,game->update_context);
    }
    /* Keyboard events received during C0E78A wait for the next source input
     * poll. Menu publication must not overwrite a selection made mid-pause. */
    if(game->input_count && !native_flight_enabled(game)) native_input_process(game);
    MainControlHooks hooks={0}; hooks.context=game; hooks.consume_values=child;
    const int flight=native_flight_enabled(game);
    const int menu=game->screen==NATIVE_MENU || game->screen==NATIVE_MISSIONS || game->screen==NATIVE_PILOT_LOG;
    if(!flight && !menu) advance_main_loop_message_sequence((MessageWorking){0},&hooks);
    if(game->screen==NATIVE_ENLISTMENT && (int8_t)rd_u8(0xc457e0u)<0) {
        if(!rd_u16(PLAYER_LOG+4)) {
            select_screen(game,NATIVE_CALLSIGN,3,2); wr_u8(0xc457f5u,20); wr_u8(0xc457e0u,2);
        } else native_frontend_start_menu(game);
        wr_u16(PLAYER_LOG+4,(uint16_t)(rd_u16(PLAYER_LOG+4)+1));
    } else if(game->screen==NATIVE_CALLSIGN && game->name_finished) native_frontend_start_menu(game);
    if(game->menu_setup.pending) return;
    native_menu_tick(game);
    if(game->menu_setup.pending) return;
    if(native_flight_enabled(game))
        native_display_begin_frame(game);
    /* C0FCB4 can select flight here. C0F5F8 dispatches one stage per update:
     * continue this frame without ticking its newly published C0FECE. */
    const int complete=native_flight_tick(game,!flight);
    /* C32CEE is C0EFD4's final child, after the flight/HUD work. */
    if((flight || menu) && complete) advance_main_loop_message_sequence((MessageWorking){0},&hooks);
    if(game->display_drawing && complete) native_display_finish_frame(game);
    native_display_read_pixels(game);
}
