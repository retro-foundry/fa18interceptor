#include "graphics_setup.h"
#include "renderer_page_layout.h"
#include <string.h>

static int terminate_setup(const FA18NativeGraphicsSetupOps *ops,int32_t reason) {
    if(ops->terminate) ops->terminate(ops->context,reason);
    return 0;
}
static void initialize_bitmap(FA18NativeGraphicsBitmap *bitmap,uint8_t depth) {
    memset(bitmap,0,sizeof *bitmap); bitmap->row_bytes=40; bitmap->rows=200; bitmap->depth=depth;
}
static int build_pair(FA18NativeGraphicsSetup *s,const FA18NativeGraphicsSetupOps *ops,unsigned slot) {
    if(!ops->make_viewport || !ops->merge_view || !ops->make_viewport(ops->context,s) ||
       !ops->merge_view(ops->context,s)) return 0;
    s->pairs[slot].view=s->display->saved_pair.view;
    s->pairs[slot].display_list=s->display->saved_pair.display_list;
    return 1;
}
int fa18_initialize_native_graphics(FA18NativeGraphicsSetup *s,const FA18NativeGraphicsSetupOps *ops) {
    unsigned i;
    FA18NativeGraphicsPlane *plane;
    uint8_t *destination;
    if(!s || !s->display || !s->initial_palette || !ops || !ops->open_display ||
       !ops->allocate_plane || !ops->allocate_color_map || !ops->allocate_dynamic_palette) return 0;
    s->service=ops->open_display(ops->context,29);
    if(!s->service) return terminate_setup(ops,1);
    s->previous_view=s->service->active_view;
    /* Ordinary native equivalents of InitView, InitVPort, InitBitMap and
     * InitRastPort. These data operations use the proven host defaults. */
    s->display->saved_pair=(FA18NativeInputDisplayPair){NULL,NULL};
    s->short_view=NULL;
    s->view_x=129; s->view_y=44;
    s->width=s->height=s->modes=s->raster_x=s->raster_y=0;
    s->viewport_x=s->viewport_y=0; s->viewport_color_map=NULL;
    s->bitmaps[0]=&s->bitmap_storage[0]; s->bitmaps[1]=&s->bitmap_storage[1];
    initialize_bitmap(s->bitmaps[0],5); initialize_bitmap(s->bitmaps[1],4);
    s->drawing_bitmap[0]=s->bitmaps[0]; s->drawing_bitmap[1]=s->bitmaps[1];
    s->raster_bitmap=s->bitmaps[0]; s->raster_x=s->raster_y=0;
    s->width=320; s->height=200;
    for(i=0;i<4;++i) {
        plane=ops->allocate_plane(ops->context,8000,0x10002);
        if(!s->bitmaps[0]) return 0;
        s->bitmaps[0]->planes[i]=plane; s->source[i]=plane;
    }
    if(!s->source[0] || !s->source[1] || !s->source[2] || !plane)
        return terminate_setup(ops,-1000);
    plane=ops->allocate_plane(ops->context,8000,0x10002);
    if(!s->bitmaps[0]) return 0;
    s->bitmaps[0]->planes[4]=plane; s->source[4]=plane;
    if(!plane) return terminate_setup(ops,-1000);
    for(i=0;i<4;++i) s->source[5+i]=s->source[i];
    if(!s->bitmaps[1]) return 0;
    for(i=0;i<4;++i) s->bitmaps[1]->planes[i]=s->bitmaps[0]->planes[i];
    for(i=0;i<8;++i) s->table_a[i]=s->source[fa18_renderer_table_a_order[i]];
    for(i=0;i<10;++i) s->table_b[i]=s->source[fa18_renderer_table_b_order[i]];
    s->color_map=ops->allocate_color_map(ops->context,32);
    s->display->stable_palette=ops->allocate_dynamic_palette(ops->context,64,0x10002);
    if(!s->color_map || !s->color_map->bytes || s->color_map->byte_count<64) return 0;
    destination=s->color_map->bytes; /* Original local pointer, held throughout. */
    for(i=0;i<32;++i) {
        uint16_t word=s->initial_palette[i];
        destination[2*i]=(uint8_t)(word>>8); destination[2*i+1]=(uint8_t)word;
    }
    s->viewport_color_map=s->color_map;
    s->viewport_x=0; s->viewport_y=-2;
    return build_pair(s,ops,0);
}
int fa18_build_native_second_display(FA18NativeGraphicsSetup *s,const FA18NativeGraphicsSetupOps *ops) {
    if(!s || !s->display || !ops) return 0;
    s->display->saved_pair=(FA18NativeInputDisplayPair){NULL,NULL};
    s->raster_bitmap=s->bitmaps[1];
    if(!build_pair(s,ops,1)) return 0;
    s->display->draw_page=0;
    return 1;
}
