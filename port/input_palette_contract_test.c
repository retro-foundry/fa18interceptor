#include "input_palette.h"
#include "amiga/viewport_list.h"
#include "renderer_page_setup.h"
#include <assert.h>
#include <string.h>

static unsigned word(const uint8_t *p) { return (unsigned)p[0]<<8|p[1]; }
static unsigned color(const AmigaRgb4HardwareList *list,unsigned reg) {
    size_t i;
    for(i=0;i<list->byte_count;i+=4) if(word(list->bytes+i)==reg) return word(list->bytes+i+2);
    assert(!"missing actual merged color record"); return 0;
}
int main(void) {
    uint8_t colors[64];
    uint16_t first[16],second[16],stable[16];
    const uint16_t *modes[]={first,second};
    AmigaRgb4Palette color_map={colors,sizeof colors};
    AmigaNativeViewportLists a,b;
    FA18RendererPageSetup renderer;
    AmigaViewportListInput geometry={0};
    FA18NativeInputDisplay display={0}; FA18NativeInputPalette service={0};
    FA18NativeInputDisplayPair pairs[2];
    FA18ViewportTransitionOps ops; FA18ViewportModeState mode={0,1,0,0};
    FA18CommandInput commands={0}; FA18CommandAudio audio={0};
    FA18NativeInputCallbackState input={0};
    unsigned i;
    memset(colors,0xa5,sizeof colors);
    assert(!fa18_initialize_renderer_page_setup(&renderer));
    geometry.width=FA18_WIDTH; geometry.height=FA18_HEIGHT;
    geometry.depth=5; geometry.row_bytes=FA18_COPPER_PAGE_ROW_BYTES;
    geometry.view_x=129; geometry.view_y=44; geometry.viewport_y=-2;
    for(i=0;i<5;++i) geometry.planes[i]=renderer.chip_binding.plane_pointers[i];
    assert(amiga_build_native_viewport(&a,&geometry,colors,sizeof colors,32));
    assert(amiga_build_native_viewport(&b,&geometry,colors,sizeof colors,32));
    for(i=0;i<16;++i) { first[i]=(uint16_t)(0x100+i); second[i]=(uint16_t)(0xf200+i); stable[i]=0; }
    pairs[0]=(FA18NativeInputDisplayPair){&a.view_list,&a.display_list};
    pairs[1]=(FA18NativeInputDisplayPair){&b.view_list,&b.display_list};
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
    assert(color(&a.view_list,0x180)==0x200 && color(&b.view_list,0x19e)==0x20f);
    assert(color(&b.view_list,0x1a0)==0x5a5);
    assert(colors[32]==0xa5 && mode.state==3);
    assert(display.saved_pair.display_list==&b.display_list && display.saved_pair.view==&b.view_list);
    /* The constructed view resolves to the actual native renderer buffers. */
    {
        FA18CopperInstructionStream stream={b.view_list.bytes,b.view_list.byte_count};
        FA18CopperPageState decoded;
        FA18CopperPlaneBuffer buffers[5]; FA18Video video={0};
        renderer.chip_bytes[renderer.chip_binding.plane_pointers[0]]=0x80;
        renderer.chip_bytes[renderer.chip_binding.plane_pointers[4]]=0x80;
        for(i=0;i<5;++i) buffers[i]=(FA18CopperPlaneBuffer){geometry.planes[i],
             renderer.chip_bytes+geometry.planes[i],FA18_COPPER_PAGE_BYTES};
        assert(!fa18_decode_copper_page_at_vpos(&stream,1,42,&decoded));
        assert(!fa18_present_copper_page(&decoded,buffers,5,&video));
        assert(video.pixels[0]==17 && !video.pixels[1] && video.palette[17]==0x5a5);
    }
    stable[0]=0xa345;
    assert(fa18_advance_native_input_callback(&input,0,&ops));
    assert(!audio.master_volume && input.ticks==2);
    assert(word(colors)==0xa345 && color(&b.view_list,0x180)==0x345 && color(&a.view_list,0x180)==0x200);
    /* A real backend failure propagates after the map and first hardware write. */
    b.view_list.byte_count=21*4; stable[0]=0xa456;
    audio.master_volume=0x4000;
    assert(!fa18_advance_native_input_callback(&input,0,&ops));
    assert(audio.master_volume==0x4000 && input.ticks==3);
    assert(word(colors)==0xa456 && color(&b.view_list,0x180)==0x456);
    display.saved_pair.display_list=NULL;
    assert(fa18_advance_native_viewport_transition(&mode,&ops));
    service.color_map=NULL;
    assert(!fa18_advance_native_viewport_transition(&mode,&ops));
    assert(!fa18_bind_native_input_palette(NULL,&display,&color_map));
    return 0;
}
