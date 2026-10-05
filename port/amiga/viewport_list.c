#include "viewport_list.h"
#include <string.h>

static uint16_t viewport_word(const uint8_t *p) { return (uint16_t)((unsigned)p[0]<<8|p[1]); }
static void viewport_store(uint8_t *p,uint16_t v) { p[0]=(uint8_t)(v>>8); p[1]=(uint8_t)v; }
static int parameters_valid(const AmigaViewportListInput *p) {
    return p && p->depth && p->depth<=6 && p->width && p->width<=640 && p->height && p->height<=512;
}
static void record(AmigaRgb4CopperList *list,uint16_t op,uint16_t a,uint16_t b) {
    uint8_t *p=list->instructions+6*list->instruction_count++;
    viewport_store(p,op); viewport_store(p+2,a); viewport_store(p+4,b);
}
int amiga_build_viewport_base(const AmigaViewportListInput *p,AmigaRgb4CopperList *list) {
    unsigned fetch,i;
    int x,y;
    uint32_t offset;
    if(!parameters_valid(p) || !list || !list->instructions ||
       10u+2u*p->depth>list->byte_count/6) return 0;
    list->instruction_count=0;
    x=(int)p->view_x+p->viewport_x; y=(int)p->view_y+p->viewport_y;
    fetch=p->width/16; if(!fetch) fetch=1;
    record(list,1,(uint16_t)(y>0?y-1:0),0);
    record(list,0,0x08e,(uint16_t)((((unsigned)y&255)<<8)|((unsigned)x&255)));
    record(list,0,0x090,(uint16_t)((((unsigned)(y+p->height)&255)<<8)|((unsigned)(x+p->width)&255)));
    record(list,0,0x092,0x38); record(list,0,0x094,(uint16_t)(0x38+(fetch-1)*8));
    record(list,0,0x100,(uint16_t)((p->depth<<12)|0x200|(p->modes&0x8c04)));
    record(list,0,0x102,0); record(list,0,0x104,0x24);
    record(list,0,0x108,(uint16_t)(p->row_bytes-p->width/8));
    record(list,0,0x10a,(uint16_t)(p->row_bytes-p->width/8));
    offset=(uint32_t)p->raster_y*p->row_bytes+p->raster_x/8u;
    for(i=0;i<p->depth;++i) {
        uint32_t payload=p->planes[i]+offset;
        record(list,0,(uint16_t)(0xe0+i*4),(uint16_t)(payload>>16));
        record(list,0,(uint16_t)(0xe2+i*4),(uint16_t)payload);
    }
    return 1;
}
int amiga_append_viewport_colors(AmigaRgb4CopperList *list,const uint8_t *colors,
                                   size_t color_bytes,size_t count) {
    size_t i;
    if(count>32) count=32;
    if(!list || (count && (!colors || !list->instructions)) || count>color_bytes/2 ||
       list->instruction_count>list->byte_count/6 || count>list->byte_count/6-list->instruction_count) return 0;
    for(i=0;i<count;++i) {
        uint16_t value=(uint16_t)(viewport_word(colors+2*i)&0xfff);
        record(list,0,(uint16_t)(0x180+2*i),value);
    }
    return 1;
}
int amiga_merge_viewport_records(const AmigaRgb4CopperList *list,
                                   AmigaRgb4HardwareList *hw,size_t *at) {
    size_t i;
    if(!list || !hw || !at || list->instruction_count>list->byte_count/6 ||
       (list->instruction_count && !list->instructions) || !hw->bytes ||
       *at>hw->byte_count/4 || list->instruction_count>hw->byte_count/4-*at) return 0;
    for(i=0;i<list->instruction_count;++i) {
        const uint8_t *p=list->instructions+6*i;
        uint8_t *out=hw->bytes+4*(*at);
        uint16_t op=viewport_word(p),a=viewport_word(p+2),b=viewport_word(p+4);
        if(op==1) {
            viewport_store(out,(uint16_t)(((a&255)<<8)|(b&0xfe)|1)); viewport_store(out+2,0xfffe);
        } else if(!op) { viewport_store(out,a); viewport_store(out+2,b); }
        else return 0;
        ++*at;
    }
    return 1;
}
int amiga_finish_viewport_records(AmigaRgb4HardwareList *hw,size_t at) {
    if(!hw || !hw->bytes || at>=hw->byte_count/4) return 0;
    viewport_store(hw->bytes+4*at,0xffff); viewport_store(hw->bytes+4*at+2,0xfffe);
    return 1;
}
int amiga_build_native_viewport(AmigaNativeViewportLists *s,const AmigaViewportListInput *input,
                                  const uint8_t *colors,size_t color_bytes,size_t count) {
    size_t at=0;
    if(!s || !parameters_valid(input)) return 0;
    memset(s,0,sizeof *s);
    s->display_list=(AmigaRgb4CopperList){s->records,sizeof s->records,0,NULL,NULL};
    s->view_list=(AmigaRgb4HardwareList){s->merged,sizeof s->merged};
    if(!amiga_build_viewport_base(input,&s->display_list) ||
       !amiga_append_viewport_colors(&s->display_list,colors,color_bytes,count) ||
       !amiga_merge_viewport_records(&s->display_list,&s->view_list,&at) ||
       !amiga_finish_viewport_records(&s->view_list,at)) return 0;
    s->view_list.byte_count=(at+1)*4;
    s->display_list.write_hardware=amiga_rgb4_write_hardware;
    s->display_list.context=&s->view_list;
    return 1;
}
