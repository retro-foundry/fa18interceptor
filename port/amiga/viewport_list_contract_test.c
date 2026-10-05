#include "viewport_list.h"
#include <assert.h>
#include <string.h>
static unsigned word(const uint8_t *p) { return (unsigned)p[0]<<8|p[1]; }
int main(void) {
    AmigaViewportListInput p={0};
    AmigaNativeViewportLists lists;
    uint8_t colors[64],merged[32]={0};
    unsigned i;
    size_t at;
    AmigaRgb4HardwareList small={merged,sizeof merged};
    p.width=320; p.height=200; p.row_bytes=40; p.depth=5;
    p.view_x=129; p.view_y=44; p.viewport_y=-2;
    for(i=0;i<6;++i) p.planes[i]=8000*i;
    for(i=0;i<32;++i) { colors[2*i]=0xf0; colors[2*i+1]=(uint8_t)i; }
    assert(amiga_build_native_viewport(&lists,&p,colors,sizeof colors,32));
    assert(lists.display_list.instruction_count==52 && lists.view_list.byte_count==212);
    assert(word(lists.merged)==0x2901 && word(lists.merged+2)==0xfffe);
    assert(word(lists.records+10)==0x2a81 && word(lists.records+34)==0x5200);
    assert(word(lists.records+82)==8000 && word(lists.records+310)==31);
    assert(word(lists.merged+208)==0xffff && word(lists.merged+210)==0xfffe);
    assert(lists.display_list.context==&lists.view_list);
    assert(amiga_rgb4_write_hardware(lists.display_list.context,51,0xabc));
    assert(word(lists.merged+206)==0xabc);
    /* Invalid source geometry leaves a previously constructed owner intact. */
    p.depth=7;
    assert(!amiga_build_native_viewport(&lists,&p,colors,sizeof colors,32));
    assert(word(lists.merged+206)==0xabc);
    p.depth=5;
    assert(!amiga_build_native_viewport(&lists,&p,NULL,0,16));
    assert(lists.display_list.instruction_count==20);
    /* Unknown records reject the merge after preceding records were written. */
    lists.records[6]=0; lists.records[7]=2;
    lists.display_list.instruction_count=2; at=0;
    assert(!amiga_merge_viewport_records(&lists.display_list,&small,&at) && at==1);
    assert(word(merged)==0x2901 && !word(merged+4));
    assert(amiga_finish_viewport_records(&small,at) && word(merged+4)==0xffff);
    assert(!amiga_finish_viewport_records(&small,8));
    /* Source payload addition is unsigned 32-bit wrap. */
    p.planes[0]=0xfffffff0; p.raster_x=128;
    assert(amiga_build_native_viewport(&lists,&p,NULL,0,0));
    assert(word(lists.records+64)==0 && word(lists.records+70)==0);
    assert(!amiga_build_viewport_base(NULL,&lists.display_list));
    return 0;
}
