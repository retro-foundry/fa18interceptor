#include "input_palette.h"
#include <assert.h>
#include <string.h>

static unsigned word(const uint8_t *p) { return (unsigned)p[0]<<8|p[1]; }
int main(void) {
    uint8_t colors[64],ins_a[12]={0,0,1,0x80,0,0,0,0,1,0x9e,0,0},ins_b[12],hw_a[8]={0},hw_b[8]={0};
    uint16_t first[16],second[16],stable[16];
    const uint16_t *modes[]={first,second};
    AmigaRgb4Palette color_map={colors,sizeof colors};
    AmigaRgb4HardwareList a={hw_a,sizeof hw_a},b={hw_b,sizeof hw_b};
    AmigaRgb4CopperList list_a={ins_a,sizeof ins_a,2,amiga_rgb4_write_hardware,&a};
    AmigaRgb4CopperList list_b={ins_b,sizeof ins_b,2,amiga_rgb4_write_hardware,&b};
    FA18NativeInputDisplay display={0}; FA18NativeInputPalette service={0};
    FA18NativeInputDisplayPair pairs[2];
    FA18ViewportTransitionOps ops; FA18ViewportModeState mode={0,1,0,0};
    FA18CommandInput commands={0}; FA18CommandAudio audio={0};
    FA18NativeInputCallbackState input={0};
    unsigned i;
    memset(colors,0xa5,sizeof colors); memcpy(ins_b,ins_a,sizeof ins_a);
    for(i=0;i<16;++i) { first[i]=(uint16_t)(0x100+i); second[i]=(uint16_t)(0xf200+i); stable[i]=0; }
    pairs[0]=(FA18NativeInputDisplayPair){&a,&list_a};
    pairs[1]=(FA18NativeInputDisplayPair){&b,&list_b};
    display.saved_pair=pairs[0]; display.pairs=pairs; display.pair_count=2;
    display.mode_palettes=modes; display.mode_count=2; display.stable_palette=stable;
    assert(fa18_bind_native_input_palette(&service,&display,&color_map));
    assert(fa18_prepare_native_input_display(&display,&ops));
    input.commands=&commands; input.audio=&audio; input.viewport=&mode;
    input.min_x=input.min_y=-100; input.max_x=input.max_y=100;
    audio.volume_fading=1; audio.master_volume=0x8000;
    assert(fa18_advance_native_input_callback(&input,0,&ops));
    assert(audio.master_volume==0x4000 && input.ticks==1);
    /* Both loads share the viewport ColorMap. Each patches its selected DspIns. */
    assert(word(colors)==0xf200);
    assert(word(ins_a+4)==0x200 && word(ins_b+10)==0x20f);
    assert(word(hw_a+2)==0x200 && word(hw_b+6)==0x20f);
    assert(colors[32]==0xa5 && mode.state==3);
    assert(display.saved_pair.display_list==&list_b && display.saved_pair.view==&b);
    stable[0]=0xa345;
    assert(fa18_advance_native_input_callback(&input,0,&ops));
    assert(!audio.master_volume && input.ticks==2);
    assert(word(colors)==0xa345 && word(hw_b+2)==0x345 && word(hw_a+2)==0x200);
    /* A real backend failure propagates after the map and first hardware write. */
    b.byte_count=4; stable[0]=0xa456;
    audio.master_volume=0x4000;
    assert(!fa18_advance_native_input_callback(&input,0,&ops));
    assert(audio.master_volume==0x4000 && input.ticks==3);
    assert(word(colors)==0xa456 && word(hw_b+2)==0x456);
    display.saved_pair.display_list=NULL;
    assert(fa18_advance_native_viewport_transition(&mode,&ops));
    service.color_map=NULL;
    assert(!fa18_advance_native_viewport_transition(&mode,&ops));
    assert(!fa18_bind_native_input_palette(NULL,&display,&color_map));
    return 0;
}
