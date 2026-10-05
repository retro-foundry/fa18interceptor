#include "input_callback.h"
#include <assert.h>

typedef struct {
    FA18NativeInputDisplay *display;
    FA18ViewportModeState *mode;
    unsigned count,fail;
    const uint16_t *captured;
} Palettes;
static int load(void *context,FA18ViewportPalettePhase phase,const uint16_t *words) {
    Palettes *p=context;
    ++p->count;
    if(phase==FA18_PALETTE_FIRST) {
        p->captured=words;
        p->display->draw_page=1;
    } else if(phase==FA18_PALETTE_SECOND) {
        assert(words==p->captured);
        assert(p->display->saved_pair.view==p->display->pairs[0].view);
        p->display->draw_page=0; /* second publication retains captured index */
    } else assert(words==p->display->stable_palette);
    return p->count!=p->fail;
}
int main(void) {
    FA18CommandInput commands={0}; FA18FlightCommandState flight={0}; FA18ViewCommandState view={0};
    FA18ContextCommandState context={0}; FA18CommandQueue queue={0};
    FA18CommandAudio audio={0}; FA18ViewportModeState mode={8,9,0,0};
    FA18NativeInputCallbackState callback={0}; FA18NativeInputDisplay display={0};
    FA18ViewportTransitionOps ops;
    uint16_t palette8[16]={0},palette9[16]={0},stable[32]={0};
    const uint16_t *palettes[]={palette8,palette9};
    int objects[4]={0};
    FA18NativeInputDisplayPair pairs[]={{&objects[0],&objects[1]},{&objects[2],&objects[3]}};
    Palettes calls={&display,&mode,0,0,NULL};
    uint8_t data[266]={0},keys[128]={0};
    uint32_t event;
    unsigned i;
    flight.commands=&commands; view.flight=&flight; context.view=&view;
    data[0x13]=0xff; data[0x14]=0xff; data[0x15]=0; data[0x16]=100;
    data[0x17]=0; data[0x18]=50;
    assert(fa18_initialize_command_queue(&queue,&context,data,sizeof data,keys,sizeof keys));
    assert(fa18_initialize_native_input_callback(&callback,&queue,&audio,&mode));
    assert(callback.ticks==-1 && callback.mouse_x==100 && commands.indexed.throttle==50);
    callback.counter_x=250; callback.counter_y=2;
    callback.min_x=callback.min_y=-200; callback.max_x=callback.max_y=200;
    display.draw_page=0; display.pairs=pairs; display.pair_count=2;
    display.mode_palettes=palettes; display.mode_count=2; display.first_mode=8;
    display.stable_palette=stable; display.load_palette=load; display.context=&calls;
    for(i=0;i<16;++i) { palette9[i]=(uint16_t)(0x9000+i); stable[i+16]=0x5555; }
    assert(fa18_prepare_native_input_display(&display,&ops));
    audio.volume_fading=1; audio.master_volume=0x4000; audio.master_volume_target=0;
    assert(fa18_advance_native_input_callback(&callback,0xff05,&ops));
    assert(callback.mouse_x==111 && commands.indexed.throttle==52 && callback.ticks==0);
    assert(callback.counter_x==5 && callback.counter_y==255 && !audio.master_volume);
    assert(mode.current==9 && mode.state==3 && calls.count==2);
    assert(display.saved_pair.view==pairs[0].view && display.saved_pair.display_list==pairs[0].display_list);
    for(i=0;i<16;++i) assert(stable[i]==palette9[i] && stable[i+16]==0x5555);
    assert(fa18_advance_native_input_callback(&callback,0xff05,&ops) && calls.count==3);

    /* Signed bounds preserve the source order even for inverted bounds. */
    callback.min_x=100; callback.max_x=-100; callback.mouse_x=0;
    assert(fa18_advance_native_input_callback(&callback,0xff05,&ops) && callback.mouse_x==100);
    /* Queued neighbor writes reach the actual counter/X/Y words. */
    queue.translated_index=(uint8_t)(0x15-138); queue.key_table[1]=0x80;
    assert(fa18_publish_native_command(&queue,1,&event) && callback.mouse_x<0);
    queue.taken=queue.count=0; queue.translated_index=(uint8_t)(0x17-138);
    assert(fa18_publish_native_command(&queue,1,&event) && commands.indexed.throttle<0);
    callback.ticks=(int16_t)0xabcd;
    assert(fa18_bind_command_queue_word(&queue,0x13,&callback.ticks) && (uint16_t)callback.ticks==0xabcd);
    assert(!fa18_bind_command_queue_word(&queue,265,&callback.ticks));

    mode.current=8; mode.target=9; mode.countdown=0; calls.count=0; calls.fail=2;
    audio.master_volume=0x4000; callback.min_x=-200; callback.max_x=200;
    assert(!fa18_advance_native_input_callback(&callback,0xff05,&ops));
    assert(mode.current==9 && calls.count==2 && audio.master_volume==0x4000);
    /* Missing palette data fails after the original mode step, without fade. */
    calls.fail=0; mode.current=9; mode.target=10; mode.countdown=0;
    assert(!fa18_advance_native_input_callback(&callback,0xff05,&ops) && mode.current==10);
    assert(!fa18_prepare_native_input_display(NULL,&ops));
    return 0;
}
