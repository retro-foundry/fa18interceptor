#include "host_graphics.h"
#include "hunk.h"
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
static void instruction(uint8_t *p,unsigned *count,uint16_t op,uint16_t a,uint16_t b) {
    p+=6*(*count)++; word(p,op); word(p+2,a); word(p+4,b);
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
    uint8_t *cl=range(c,descriptor,48+128*6),*ins=cl+48; unsigned count=0;
    amiga_store_be32(cl+8,viewport); amiga_store_be32(cl+12,descriptor+48); word(cl+30,128);
    int y=(int16_t)amiga_be16(v+12)+(int16_t)amiga_be16(vp+30);
    int x=(int16_t)amiga_be16(v+14)+(int16_t)amiga_be16(vp+28);
    unsigned fetch=width/16; if (!fetch) fetch=1;
    instruction(ins,&count,1,(uint16_t)(y>0?y-1:0),0);
    instruction(ins,&count,0,0x08E,(uint16_t)((((unsigned)y&255)<<8)|(x&255)));
    instruction(ins,&count,0,0x090,(uint16_t)((((unsigned)(y+(int)height)&255)<<8)|((x+(int)width)&255)));
    instruction(ins,&count,0,0x092,0x38); instruction(ins,&count,0,0x094,(uint16_t)(0x38+(fetch-1)*8));
    instruction(ins,&count,0,0x100,(uint16_t)((bm[5]<<12)|0x200|(amiga_be16(vp+32)&0x8C04)));
    instruction(ins,&count,0,0x102,0); instruction(ins,&count,0,0x104,0x24);
    int16_t modulo=(int16_t)(amiga_be16(bm)-(width/8));
    instruction(ins,&count,0,0x108,(uint16_t)modulo); instruction(ins,&count,0,0x10A,(uint16_t)modulo);
    for (unsigned plane=0;plane<bm[5];++plane) {
        uint32_t pointer=amiga_be32(bm+8+plane*4);
        pointer+=(uint32_t)amiga_be16(ri+10)*amiga_be16(bm)+(amiga_be16(ri+8)/8);
        instruction(ins,&count,0,(uint16_t)(0xE0+plane*4),(uint16_t)(pointer>>16));
        instruction(ins,&count,0,(uint16_t)(0xE2+plane*4),(uint16_t)pointer);
    }
    uint32_t color_map=amiga_be32(vp+4);
    if (color_map) {
        uint8_t *cm=range(c,color_map,8); if (!cm) { amiga_host_free_copper(c,descriptor); return 0; }
        unsigned colors=amiga_be16(cm+2); if (colors>32) colors=32;
        uint8_t *rgb=range(c,amiga_be32(cm+4),colors*2);
        if (colors && !rgb) { amiga_host_free_copper(c,descriptor); return 0; }
        for (unsigned i=0;i<colors;++i) instruction(ins,&count,0,(uint16_t)(0x180+i*2),amiga_be16(rgb+i*2)&0xFFF);
    }
    word(cl+28,(uint16_t)count); amiga_store_be32(cl+16,descriptor+48+count*6);
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
    uint8_t *header=range(c,allocation,16+(total+1)*4),*hardware=header+16; unsigned at=0;
    for (unsigned n=0;n<count;++n) {
        uint8_t *p=range(c,seen[n],40),*cl=range(c,amiga_be32(p+8),32);
        unsigned instructions=amiga_be16(cl+28); uint8_t *ins=range(c,amiga_be32(cl+12),instructions*6);
        if (!ins) { amiga_host_free_copper(c,allocation); return 0; }
        amiga_store_be32(cl+20,allocation+16+at*4); amiga_store_be32(cl+24,allocation+16+at*4);
        for (unsigned i=0;i<instructions;++i) {
            uint16_t op=amiga_be16(ins+i*6),a=amiga_be16(ins+i*6+2),b=amiga_be16(ins+i*6+4);
            if (op==1) { word(hardware+at*4,(uint16_t)(((a&255)<<8)|(b&0xFE)|1)); word(hardware+at*4+2,0xFFFE); }
            else if (!op) { word(hardware+at*4,a); word(hardware+at*4+2,b); }
            else { amiga_host_free_copper(c,allocation); return 0; }
            ++at;
        }
    }
    word(hardware+at*4,0xFFFF); word(hardware+at*4+2,0xFFFE);
    amiga_store_be32(header+4,allocation+16); word(header+8,(uint16_t)(at+1));
    amiga_store_be32(v+4,allocation); amiga_store_be32(v+8,allocation); return 1;
}
int amiga_host_load_rgb4(AmigaHostCompat *c,uint32_t viewport,uint32_t colors,unsigned count) {
    uint8_t *vp=range(c,viewport,40); if (!vp) return 0;
    uint8_t *cm=range(c,amiga_be32(vp+4),8); if (!cm) return 0;
    if (count>amiga_be16(cm+2)) count=amiga_be16(cm+2);
    if (!count) return 1;
    uint8_t *src=range(c,colors,count*2),*dst=range(c,amiga_be32(cm+4),count*2);
    if (!src || !dst) return 0;
    memcpy(dst,src,count*2);
    uint32_t list=amiga_be32(vp+8); if (!list) return 1;
    uint8_t *cl=range(c,list,32); if (!cl) return 0;
    unsigned entries=amiga_be16(cl+28); uint8_t *ins=range(c,amiga_be32(cl+12),entries*6);
    if (!ins) return 0;
    uint32_t hardware=amiga_be32(cl+20);
    for (unsigned i=0;i<entries;++i) {
        uint16_t reg=amiga_be16(ins+i*6+2);
        if (!amiga_be16(ins+i*6) && reg>=0x180 && reg<0x180+count*2) {
            uint16_t value=amiga_be16(dst+reg-0x180)&0xFFF; word(ins+i*6+4,value);
            if (hardware) { uint8_t *hw=range(c,hardware+i*4+2,2); if (!hw) return 0; word(hw,value); }
        }
    }
    return 1;
}
