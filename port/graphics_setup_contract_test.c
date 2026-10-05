#include "graphics_storage.h"
#include "input_palette.h"
#include "outer_display.h"
#include <assert.h>
#include <string.h>

static void present(FA18NativeGraphicsStorage *storage,unsigned slot,FA18Video *video) {
    AmigaRgb4HardwareList *view=&storage->lists[slot].view_list;
    FA18CopperInstructionStream stream={view->bytes,view->byte_count};
    FA18CopperPageState state;
    FA18CopperPlaneBuffer buffers[5];
    unsigned i;
    for(i=0;i<5;++i) buffers[i]=(FA18CopperPlaneBuffer){storage->planes[i].payload,
        storage->planes[i].bytes,storage->planes[i].byte_count};
    assert(!fa18_decode_copper_page_at_vpos(&stream,1,42,&state));
    assert(((state.bplcon0>>12)&7)==(slot?4u:5u));
    assert(!fa18_present_copper_page(&state,buffers,5,video));
}
int main(void) {
    FA18NativeGraphicsStorage storage={0};
    FA18NativeGraphicsSetup setup={0};
    FA18NativeGraphicsSetupOps ops;
    FA18NativeInputDisplay display={0};
    FA18NativeInputPalette palette;
    FA18NativeInputCallbackState input={0};
    FA18CommandInput commands={0}; FA18CommandAudio audio={0};
    FA18ViewportModeState mode={0,1,0,0};
    FA18ViewportTransitionOps transition;
    uint16_t initial[32],selected[16]; const uint16_t *modes[]={initial,selected};
    FA18Video video={0};
    unsigned i; int previous_view;
    for(i=0;i<32;++i) initial[i]=(uint16_t)(0xf100+i);
    for(i=0;i<16;++i) selected[i]=(uint16_t)(0xf200+i);
    setup.display=&display; setup.initial_palette=initial;
    display.pairs=setup.pairs; display.pair_count=2; display.mode_palettes=modes; display.mode_count=2;
    display.draw_page=0xffff; storage.service.active_view=&previous_view;
    assert(fa18_bind_native_graphics_storage(&storage,&ops));
    assert(fa18_initialize_native_graphics(&setup,&ops));
    assert(setup.previous_view==&previous_view && storage.allocated_planes==5);
    assert(setup.bitmaps[0]->depth==5 && setup.bitmaps[1]->depth==4 && setup.viewport_y==-2);
    assert(setup.pairs[0].display_list==&storage.lists[0].display_list && !setup.pairs[1].view);
    assert(display.saved_pair.view==&storage.lists[0].view_list && display.draw_page==0xffff);
    assert(display.stable_palette==storage.dynamic_palette);
    for(i=0;i<32;++i) assert(!storage.dynamic_palette[i]); /* Allocation, not seed. */
    for(i=0;i<4;++i) assert(setup.bitmaps[1]->planes[i]==setup.bitmaps[0]->planes[i] && setup.source[5+i]==setup.source[i]);
    assert(fa18_bind_graphics_renderer(&storage,&setup));
    assert(!fa18_five_plane_chip_binding_renderer_lane_pointers(&storage.renderer.chip_binding,storage.renderer.table_a));
    storage.planes[0].bytes[0]=0x80; storage.planes[4].bytes[0]=0x80;
    present(&storage,0,&video); assert(video.pixels[0]==17 && video.palette[17]==0x111);
    assert(fa18_build_native_second_display(&setup,&ops));
    assert(storage.allocated_planes==5 && !display.draw_page && setup.raster_bitmap==setup.bitmaps[1]);
    assert(setup.pairs[1].display_list==&storage.lists[1].display_list);
    present(&storage,1,&video); assert(video.pixels[0]==1 && video.palette[1]==0x101);
    /* Actual callback now uses the two constructed objects; both loads share
     * one ColorMap and publication selects the appropriate record count. */
    assert(fa18_bind_native_input_palette(&palette,&display,setup.viewport_color_map));
    assert(fa18_prepare_native_input_display(&display,&transition));
    input.commands=&commands; input.audio=&audio; input.viewport=&mode;
    input.min_x=input.min_y=-100; input.max_x=input.max_y=100;
    assert(fa18_advance_native_input_callback(&input,0,&transition));
    assert(mode.state==3 && display.saved_pair.display_list==setup.pairs[1].display_list);
    present(&storage,1,&video); assert(video.pixels[0]==1 && video.palette[1]==0x201 && video.palette[17]==0x111);
    /* Native construction operation failure is handled and its return is
     * ignored by the ORIGINAL parent: pair publication and page reset remain. */
    setup.width=0; display.draw_page=123;
    assert(fa18_build_native_second_display(&setup,&ops));
    assert(storage.construction_failed && !setup.pairs[1].view && !setup.pairs[1].display_list && !display.draw_page);
    assert(setup.short_view==&storage.lists[1].view_list);
    /* Re-entering a full bounded storage keeps prior allocations and follows
     * the original four-allocation check before terminating for exhaustion. */
    assert(!fa18_initialize_native_graphics(&setup,&ops));
    assert(storage.termination_reason==-1000 && storage.allocated_planes==5);
    assert(!setup.source[0] && !setup.source[3]);
    assert(!fa18_bind_native_graphics_storage(NULL,&ops));
    return 0;
}
