#include "host_graphics.h"
#include "hunk.h"
#include "rgb4.h"
#include "viewport_list.h"
#include <string.h>
static uint8_t *range(AmigaHostCompat *c,uint32_t a,uint32_t n) { return amiga_guest_range(&c->memory,a,n); }
static void word(uint8_t *p,uint16_t v) { p[0]=(uint8_t)(v>>8); p[1]=(uint8_t)v; }
int amiga_host_init_view(AmigaHostCompat *c,uint32_t view) {
    uint8_t *v=range(c,view,18); if (!v) return 0;
    memset(v,0,18); word(v+12,44); word(v+14,129); return 1;
}
int amiga_host_free_copper(AmigaHostCompat *c,uint32_t allocation) {
    if (!allocation) return 1;
    for (size_t i=0;i<c->used_count;++i) if (c->used[i].base==allocation)
        return amiga_host_free(c,allocation,c->used[i].size);
    return 0;
}
int amiga_host_make_viewport(AmigaHostCompat *c,uint32_t view,uint32_t viewport) {
    uint8_t *v=range(c,view,18),*vp=range(c,viewport,40);
    if (!v || !vp) return 0;
    uint8_t *ri=range(c,amiga_be32(vp+36),12); if (!ri) return 0;
    uint8_t *bm=range(c,amiga_be32(ri+4),40); if (!bm || !bm[5] || bm[5]>6) return 0;
    unsigned width=amiga_be16(vp+24),height=amiga_be16(vp+26);
    if (!width || width>640 || !height || height>512) return 0;
    uint32_t old=amiga_be32(vp+8);
    if (old && !amiga_host_free_copper(c,old)) return 0;
    uint32_t descriptor=amiga_host_alloc(c,48+128*6,0x10004); if (!descriptor) return 0;
    uint8_t *cl=range(c,descriptor,48+128*6),*ins=cl+48;
    amiga_store_be32(cl+8,viewport); amiga_store_be32(cl+12,descriptor+48); word(cl+30,128);
    AmigaViewportListInput input={0};
    AmigaRgb4CopperList native={ins,128*6,0,NULL,NULL};
    input.width=(uint16_t)width; input.height=(uint16_t)height; input.depth=bm[5];
    input.view_x=(int16_t)amiga_be16(v+14); input.view_y=(int16_t)amiga_be16(v+12);
    input.viewport_x=(int16_t)amiga_be16(vp+28); input.viewport_y=(int16_t)amiga_be16(vp+30);
    input.modes=amiga_be16(vp+32); input.row_bytes=amiga_be16(bm);
    input.raster_x=amiga_be16(ri+8); input.raster_y=amiga_be16(ri+10);
    for(unsigned plane=0;plane<bm[5];++plane) input.planes[plane]=amiga_be32(bm+8+plane*4);
    if(!amiga_build_viewport_base(&input,&native)) { amiga_host_free_copper(c,descriptor); return 0; }
    uint32_t color_map=amiga_be32(vp+4);
    if (color_map) {
        uint8_t *cm=range(c,color_map,8); if (!cm) { amiga_host_free_copper(c,descriptor); return 0; }
        unsigned colors=amiga_be16(cm+2); if (colors>32) colors=32;
        uint8_t *rgb=range(c,amiga_be32(cm+4),colors*2);
        if (colors && !rgb) { amiga_host_free_copper(c,descriptor); return 0; }
        if(!amiga_append_viewport_colors(&native,rgb,colors*2u,colors)) { amiga_host_free_copper(c,descriptor); return 0; }
    }
    word(cl+28,(uint16_t)native.instruction_count); amiga_store_be32(cl+16,descriptor+48+(uint32_t)native.instruction_count*6);
    amiga_store_be32(vp+8,descriptor); return 1;
}
int amiga_host_merge_view(AmigaHostCompat *c,uint32_t view) {
    uint8_t *v=range(c,view,18); if (!v) return 0;
    uint32_t vp=amiga_be32(v),seen[16]; unsigned count=0,total=0;
    while (vp) {
        if (count==16) return 0;
        for (unsigned i=0;i<count;++i) if (seen[i]==vp) return 0;
        seen[count++]=vp; uint8_t *p=range(c,vp,40); if (!p) return 0;
        uint8_t *cl=range(c,amiga_be32(p+8),32); if (!cl || amiga_be16(cl+28)>128) return 0;
        total+=amiga_be16(cl+28); vp=amiga_be32(p);
    }
    uint32_t allocation=amiga_host_alloc(c,16+(total+1)*4,0x10002); if (!allocation) return 0;
    uint8_t *header=range(c,allocation,16+(total+1)*4),*hardware=header+16; size_t at=0;
    AmigaRgb4HardwareList merged={hardware,(total+1)*4u};
    for (unsigned n=0;n<count;++n) {
        uint8_t *p=range(c,seen[n],40),*cl=range(c,amiga_be32(p+8),32);
        unsigned instructions=amiga_be16(cl+28); uint8_t *ins=range(c,amiga_be32(cl+12),instructions*6);
        if (!ins) { amiga_host_free_copper(c,allocation); return 0; }
        amiga_store_be32(cl+20,allocation+16+(uint32_t)at*4); amiga_store_be32(cl+24,allocation+16+(uint32_t)at*4);
        AmigaRgb4CopperList source={ins,instructions*6u,instructions,NULL,NULL};
        if(!amiga_merge_viewport_records(&source,&merged,&at)) { amiga_host_free_copper(c,allocation); return 0; }
    }
    if(!amiga_finish_viewport_records(&merged,at)) { amiga_host_free_copper(c,allocation); return 0; }
    amiga_store_be32(header+4,allocation+16); word(header+8,(uint16_t)(at+1));
    amiga_store_be32(v+4,allocation); amiga_store_be32(v+8,allocation); return 1;
}
typedef struct { AmigaHostCompat *host; uint32_t address; } HostRgb4Hardware;
static int write_rgb4_hardware(void *context,size_t instruction,uint16_t value) {
    HostRgb4Hardware *h=context;
    uint8_t *p=range(h->host,h->address+(uint32_t)instruction*4u+2u,2);
    if(!p) return 0;
    word(p,value); return 1;
}
int amiga_host_load_rgb4(AmigaHostCompat *c,uint32_t viewport,uint32_t colors,unsigned count) {
    uint8_t *vp=range(c,viewport,40); if (!vp) return 0;
    uint8_t *cm=range(c,amiga_be32(vp+4),8); if (!cm) return 0;
    if (count>amiga_be16(cm+2)) count=amiga_be16(cm+2);
    if (!count) return 1;
    uint32_t destination=amiga_be32(cm+4);
    uint8_t *src=range(c,colors,count*2),*dst=range(c,destination,count*2);
    if (!src || !dst) return 0;
    AmigaRgb4Palette palette={dst,count*2u};
    if(!amiga_rgb4_copy(&palette,src,count*2u,count)) return 0;
    uint32_t list=amiga_be32(vp+8); if (!list) return 1;
    uint8_t *cl=range(c,list,32); if (!cl) return 0;
    unsigned entries=amiga_be16(cl+28); uint8_t *ins=range(c,amiga_be32(cl+12),entries*6);
    if (!ins) return 0;
    uint32_t hardware=amiga_be32(cl+20);
    /* Packed CopIns may contain an odd register offset. Preserve its original
     * byte read when the trailing byte exists in the same memory bank. */
    if(range(c,destination,count*2u+1u)) ++palette.byte_count;
    HostRgb4Hardware output={c,hardware};
    AmigaRgb4CopperList copper={ins,entries*6u,entries,hardware?write_rgb4_hardware:NULL,&output};
    return amiga_rgb4_patch(&palette,count,&copper);
}
