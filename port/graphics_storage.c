#include "graphics_storage.h"
#include "renderer_page_layout.h"
#include <string.h>

static FA18NativeDisplayService *open_display(void *context,unsigned version) {
    FA18NativeGraphicsStorage *s=context;
    return version==29?&s->service:NULL;
}
static FA18NativeGraphicsPlane *allocate_plane(void *context,size_t bytes,uint32_t flags) {
    FA18NativeGraphicsStorage *s=context;
    size_t index=s->allocated_planes;
    FA18NativeGraphicsPlane *p;
    if(index>=5 || bytes!=FA18_COPPER_PAGE_BYTES || flags!=0x10002) return NULL;
    p=&s->planes[index]; p->payload=(uint32_t)(index*bytes);
    p->bytes=s->renderer.chip_bytes+p->payload; p->byte_count=bytes;
    memset(p->bytes,0,bytes); ++s->allocated_planes;
    return p;
}
static AmigaRgb4Palette *allocate_map(void *context,size_t colors) {
    FA18NativeGraphicsStorage *s=context;
    if(s->map_allocated || colors!=32) return NULL;
    memset(s->colors,0,sizeof s->colors);
    s->color_map=(AmigaRgb4Palette){s->colors,sizeof s->colors}; s->map_allocated=1;
    return &s->color_map;
}
static uint16_t *allocate_dynamic(void *context,size_t bytes,uint32_t flags) {
    FA18NativeGraphicsStorage *s=context;
    if(s->dynamic_allocated || bytes!=sizeof s->dynamic_palette || flags!=0x10002) return NULL;
    memset(s->dynamic_palette,0,sizeof s->dynamic_palette); s->dynamic_allocated=1;
    return s->dynamic_palette;
}
static unsigned slot_for(FA18NativeGraphicsSetup *s) { return s->raster_bitmap==s->bitmaps[1]?1:0; }
static int make_viewport(void *context,FA18NativeGraphicsSetup *state) {
    FA18NativeGraphicsStorage *s=context;
    AmigaViewportListInput input={0};
    AmigaNativeViewportLists *lists=&s->lists[slot_for(state)];
    FA18NativeGraphicsBitmap *bitmap=state->raster_bitmap;
    size_t i;
    if(!bitmap || !bitmap->depth || bitmap->depth>6) { s->construction_failed=1; return 1; }
    input.width=state->width; input.height=state->height; input.modes=state->modes;
    input.view_x=state->view_x; input.view_y=state->view_y;
    input.viewport_x=state->viewport_x; input.viewport_y=state->viewport_y;
    input.raster_x=state->raster_x; input.raster_y=state->raster_y;
    input.row_bytes=bitmap->row_bytes; input.depth=bitmap->depth;
    for(i=0;i<bitmap->depth;++i) input.planes[i]=bitmap->planes[i]?bitmap->planes[i]->payload:0;
    lists->display_list=(AmigaRgb4CopperList){lists->records,sizeof lists->records,0,NULL,NULL};
    if(!amiga_build_viewport_base(&input,&lists->display_list) ||
       (state->viewport_color_map && !amiga_append_viewport_colors(&lists->display_list,
          state->viewport_color_map->bytes,state->viewport_color_map->byte_count,state->viewport_color_map->byte_count/2))) {
        s->construction_failed=1; return 1;
    }
    state->display->saved_pair.display_list=&lists->display_list;
    return 1;
}
static int merge_view(void *context,FA18NativeGraphicsSetup *state) {
    FA18NativeGraphicsStorage *s=context;
    AmigaNativeViewportLists *lists=&s->lists[slot_for(state)];
    AmigaRgb4CopperList *display=state->display->saved_pair.display_list;
    size_t at=0;
    lists->view_list=(AmigaRgb4HardwareList){lists->merged,sizeof lists->merged};
    if(!display || !amiga_merge_viewport_records(display,&lists->view_list,&at) ||
       !amiga_finish_viewport_records(&lists->view_list,at)) { s->construction_failed=1; return 1; }
    lists->view_list.byte_count=(at+1)*4;
    display->write_hardware=amiga_rgb4_write_hardware; display->context=&lists->view_list;
    state->display->saved_pair.view=&lists->view_list;
    state->short_view=&lists->view_list;
    return 1;
}
static void terminate(void *context,int32_t reason) {
    FA18NativeGraphicsStorage *s=context; s->termination_reason=reason;
}
int fa18_bind_native_graphics_storage(FA18NativeGraphicsStorage *s,FA18NativeGraphicsSetupOps *ops) {
    if(!s || !ops) return 0;
    *ops=(FA18NativeGraphicsSetupOps){open_display,allocate_plane,allocate_map,
        allocate_dynamic,make_viewport,merge_view,terminate,s};
    return 1;
}
int fa18_bind_graphics_renderer(FA18NativeGraphicsStorage *s,const FA18NativeGraphicsSetup *setup) {
    uint32_t offsets[5];
    unsigned i;
    if(!s || !setup || s->allocated_planes!=5) return 0;
    for(i=0;i<5;++i) {
        if(setup->source[i]!=&s->planes[i] || s->planes[i].byte_count!=FA18_COPPER_PAGE_BYTES) return 0;
        offsets[i]=s->planes[i].payload;
    }
    if(fa18_five_plane_chip_binding_init(&s->renderer.chip_binding,s->renderer.chip_bytes,
           sizeof s->renderer.chip_bytes,offsets)) return 0;
    fa18_five_plane_page_init(&s->renderer.page);
    for(i=0;i<9;++i) {
        if(!setup->source[i]) return 0;
        s->renderer.source[i]=setup->source[i]->payload;
    }
    for(i=0;i<8;++i) s->renderer.table_a[i]=s->renderer.source[fa18_renderer_table_a_order[i]];
    for(i=0;i<10;++i) s->renderer.table_b[i]=s->renderer.source[fa18_renderer_table_b_order[i]];
    return 1;
}
