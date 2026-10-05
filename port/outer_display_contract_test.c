#include "outer_display.h"
#include "input_palette.h"
#include "amiga/viewport_list.h"
#include "renderer_page_setup.h"
#include <assert.h>
#include <string.h>

typedef struct {
    FA18NativeInputDisplay display;
    FA18ViewportModeState mode;
    FA18NativeOuterDisplay outer;
    FA18NativeInputDisplayPair pairs[2];
    FA18CommandInput commands;
    FA18CommandAudio audio;
    FA18NativeInputCallbackState input;
    FA18ViewportTransitionOps transition;
    FA18RendererPageSetup renderer;
    AmigaViewportListInput geometry;
    AmigaNativeViewportLists lists[2];
    uint8_t colors[64],activity;
    uint16_t status,static_palette[32],dynamic_palette[32];
    const uint16_t *modes[1];
    AmigaRgb4Palette color_map;
    FA18NativeInputPalette palette;
    FA18Video video;
    unsigned events[64],event_count,views,waits;
    int input_on_wait,wrap_activity,fail_wait;
} Fixture;
static unsigned read_word(const uint8_t *p) { return (unsigned)p[0]<<8|p[1]; }
static void log_event(Fixture *s,unsigned event) {
    assert(s->event_count<64); s->events[s->event_count++]=event;
}
static int wait_owner(void *context,FA18NativeDisplayWait phase) {
    Fixture *s=context;
    log_event(s,(unsigned)phase); ++s->waits;
    if(s->fail_wait==(int)phase) return 0;
    if(s->input_on_wait && phase==FA18_DISPLAY_WAIT_PUBLICATION)
        assert(fa18_advance_native_input_callback(&s->input,0x0101,&s->transition));
    if(s->wrap_activity && phase==FA18_DISPLAY_WAIT_DYNAMIC_SECOND) s->activity=0;
    return 1;
}
static int view_owner(void *context,FA18NativeInputDisplay *display) {
    Fixture *s=context;
    AmigaRgb4HardwareList *view=display->saved_pair.view;
    FA18CopperInstructionStream stream={view->bytes,view->byte_count};
    FA18CopperPageState decoded;
    FA18CopperPlaneBuffer buffers[5];
    unsigned i;
    assert(display==&s->display); log_event(s,8); ++s->views;
    for(i=0;i<5;++i) buffers[i]=(FA18CopperPlaneBuffer){s->geometry.planes[i],
        s->renderer.chip_bytes+s->geometry.planes[i],FA18_COPPER_PAGE_BYTES};
    assert(!fa18_decode_copper_page_at_vpos(&stream,1,42,&decoded));
    assert(!fa18_present_copper_page(&decoded,buffers,5,&s->video));
    return 1;
}
static int palette_owner(void *context,FA18NativeDisplayPalettePhase phase,const uint16_t *words,size_t count) {
    Fixture *s=context;
    log_event(s,9+(unsigned)phase); assert(count==32);
    return fa18_load_native_display_palette(&s->palette,words,count);
}
static void setup(Fixture *s) {
    unsigned i;
    memset(s,0,sizeof *s); s->fail_wait=-1;
    assert(!fa18_initialize_renderer_page_setup(&s->renderer));
    s->geometry.width=FA18_WIDTH; s->geometry.height=FA18_HEIGHT; s->geometry.depth=5;
    s->geometry.row_bytes=FA18_COPPER_PAGE_ROW_BYTES;
    s->geometry.view_x=129; s->geometry.view_y=44; s->geometry.viewport_y=-2;
    for(i=0;i<5;++i) s->geometry.planes[i]=s->renderer.chip_binding.plane_pointers[i];
    s->renderer.chip_bytes[s->geometry.planes[0]]=0x80;
    s->renderer.chip_bytes[s->geometry.planes[4]]=0x80;
    for(i=0;i<32;++i) { s->static_palette[i]=(uint16_t)(0xe100+i); s->dynamic_palette[i]=(uint16_t)(0xf200+i); }
    for(i=0;i<2;++i) {
        assert(amiga_build_native_viewport(&s->lists[i],&s->geometry,s->colors,64,32));
        s->pairs[i]=(FA18NativeInputDisplayPair){&s->lists[i].view_list,&s->lists[i].display_list};
    }
    s->display.pairs=s->pairs; s->display.pair_count=2; s->display.saved_pair=s->pairs[1];
    s->display.stable_palette=s->dynamic_palette; s->display.mode_palettes=s->modes; s->display.mode_count=1;
    s->modes[0]=s->static_palette;
    s->color_map=(AmigaRgb4Palette){s->colors,64};
    assert(fa18_bind_native_input_palette(&s->palette,&s->display,&s->color_map));
    assert(fa18_prepare_native_input_display(&s->display,&s->transition));
    s->input.commands=&s->commands; s->input.audio=&s->audio; s->input.viewport=&s->mode;
    s->input.min_x=s->input.min_y=-100; s->input.max_x=s->input.max_y=100;
    s->outer=(FA18NativeOuterDisplay){&s->display,&s->mode,&s->activity,&s->status,s->static_palette};
}
int main(void) {
    Fixture s;
    FA18NativeOuterDisplayOps ops={wait_owner,view_owner,palette_owner,&s};
    setup(&s); s.activity=2;
    assert(fa18_synchronize_native_outer_display(&s.outer,&ops));
    assert(!s.activity && s.display.draw_page==1 && s.views==1 && s.waits==11);
    assert(s.display.saved_pair.view==&s.lists[0].view_list && s.display.saved_pair.display_list==&s.lists[0].display_list);
    assert(read_word(s.colors)==0xf200 && read_word(s.colors+62)==0xf21f);
    assert(s.events[0]==FA18_DISPLAY_WAIT_PUBLICATION && s.events[1]==8 && s.events[4]==9 && s.events[7]==10);
    assert(s.video.pixels[0]==17 && !s.video.pixels[1]);
    /* The callback runs during the publication wait, on the SAME owners.
     * It patches the previously published list, then the outer clear path
     * loads all 32 words into the newly published list. */
    setup(&s); s.mode.state=3; s.input_on_wait=1;
    s.audio.volume_fading=1; s.audio.master_volume=0x8000;
    assert(fa18_synchronize_native_outer_display(&s.outer,&ops));
    assert(s.mode.state==2 && s.input.ticks==1 && s.audio.master_volume==0x4000);
    assert(s.input.mouse_x==1 && s.commands.indexed.throttle==-1);
    assert(s.display.draw_page==1 && s.events[2]==FA18_DISPLAY_WAIT_CLEAR && s.events[3]==11);
    assert(read_word(s.colors+62)==0xf21f);
    /* A service changes the activity byte to zero before the source's
     * decrement. It wraps to FF and exits the signed loop after one pass. */
    setup(&s); s.activity=3; s.wrap_activity=1;
    assert(fa18_synchronize_native_outer_display(&s.outer,&ops));
    assert(s.activity==255 && s.event_count==10);
    setup(&s); s.activity=128;
    assert(fa18_synchronize_native_outer_display(&s.outer,&ops));
    assert(s.activity==128 && s.event_count==4 && s.display.draw_page==1);
    /* Late failure preserves publication and mode decrement, without page
     * toggle. A short hardware buffer also retains the map and first write. */
    setup(&s); s.mode.state=3; s.fail_wait=FA18_DISPLAY_WAIT_CLEAR;
    assert(!fa18_synchronize_native_outer_display(&s.outer,&ops));
    assert(s.mode.state==2 && !s.display.draw_page && s.views==1);
    setup(&s); s.mode.state=3; s.lists[0].view_list.byte_count=21*4;
    assert(!fa18_synchronize_native_outer_display(&s.outer,&ops));
    assert(s.mode.state==2 && !s.display.draw_page && read_word(s.colors+62)==0xf21f);
    assert(read_word(s.lists[0].merged+20*4+2)==0x200);
    setup(&s); s.display.draw_page=0xffff;
    assert(!fa18_synchronize_native_outer_display(&s.outer,&ops));
    assert(!s.views && s.event_count==1 && s.display.saved_pair.view==s.pairs[1].view);
    assert(!fa18_load_native_display_palette(&s.palette,s.dynamic_palette,33));
    return 0;
}
