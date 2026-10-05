#include "rgb4.h"
#include <string.h>

static uint16_t rgb4_word(const uint8_t *p) { return (uint16_t)((unsigned)p[0]<<8|p[1]); }
static void rgb4_store(uint8_t *p,uint16_t value) { p[0]=(uint8_t)(value>>8); p[1]=(uint8_t)value; }

int amiga_rgb4_copy(AmigaRgb4Palette *p,const uint8_t *source,size_t source_bytes,size_t count) {
    if(!p || count>p->byte_count/2 || count>source_bytes/2) return 0;
    if(!count) return 1;
    if(!p->bytes || !source) return 0;
    memmove(p->bytes,source,count*2);
    return 1;
}
int amiga_rgb4_patch(const AmigaRgb4Palette *p,size_t count,AmigaRgb4CopperList *list) {
    size_t i;
    if(!p || !list || count>p->byte_count/2 ||
       list->instruction_count>list->byte_count/6 ||
       (list->instruction_count && !list->instructions) || (count && !p->bytes)) return 0;
    for(i=0;i<list->instruction_count;++i) {
        uint8_t *ins=list->instructions+6*i;
        uint16_t reg=rgb4_word(ins+2),value;
        size_t color_offset;
        if(rgb4_word(ins) || reg<0x180) continue;
        color_offset=(size_t)(reg-0x180);
        if(color_offset/2>=count) continue;
        if(color_offset+2>p->byte_count) return 0;
        value=(uint16_t)(rgb4_word(p->bytes+color_offset)&0xfff);
        rgb4_store(ins+4,value);
        if(list->write_hardware && !list->write_hardware(list->context,i,value)) return 0;
    }
    return 1;
}
int amiga_rgb4_load(AmigaRgb4Palette *p,const uint8_t *source,size_t source_bytes,
                       size_t count,AmigaRgb4CopperList *list) {
    if(!p) return 0;
    if(count>p->byte_count/2) count=p->byte_count/2;
    if(!amiga_rgb4_copy(p,source,source_bytes,count)) return 0;
    return !count || !list || amiga_rgb4_patch(p,count,list);
}
int amiga_rgb4_write_hardware(void *context,size_t instruction,uint16_t value) {
    AmigaRgb4HardwareList *list=context;
    if(!list || !list->bytes || instruction>=list->byte_count/4) return 0;
    rgb4_store(list->bytes+4*instruction+2,value);
    return 1;
}
