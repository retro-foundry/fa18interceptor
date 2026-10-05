/* Extracted RGB4 versus the pre-extraction host service from commit 9645f4de.
 * This frozen validation body is not linked into either game runtime. */
#include "../../port/amiga/host_graphics.h"
#include "../../port/amiga/hunk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint8_t *range(AmigaHostCompat *c,uint32_t a,uint32_t n) { return amiga_guest_range(&c->memory,a,n); }
static void word(uint8_t *p,uint16_t v) { p[0]=(uint8_t)(v>>8); p[1]=(uint8_t)v; }
static int original_rgb4(AmigaHostCompat *c,uint32_t viewport,uint32_t colors,unsigned count) {
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
static uint32_t rng=0x9645f4de;
static uint32_t random_value(void) { rng^=rng<<13; rng^=rng>>17; rng^=rng<<5; return rng; }
int main(int argc,char **argv) {
    enum { BASE=0x100000, SIZE=4096 };
    static const unsigned counts[]={0,1,2,15,16,31,32,33,255,65535};
    uint8_t before[SIZE],after[SIZE];
    AmigaGuestBank a={BASE,SIZE,0,before},b={BASE,SIZE,0,after};
    AmigaHostCompat *old=calloc(1,sizeof *old),*now=calloc(1,sizeof *now);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):16384,n,i,success=0,failures=0;
    if(!old || !now || !cases) return 1;
    old->memory=(AmigaGuestMemory){&a,1}; now->memory=(AmigaGuestMemory){&b,1};
    for(n=0;n<cases;++n) {
        uint32_t vp=BASE+0x100,cm=BASE+0x200,source=BASE+0x300,dest=BASE+0x400;
        uint32_t cl=BASE+0x500,ins=BASE+0x600,hw=BASE+0x800;
        unsigned count=counts[n%10],capacity=counts[(n/10)%9],entries=(n/32)%33;
        int expected,actual;
        for(i=0;i<SIZE;++i) before[i]=(uint8_t)random_value();
        amiga_store_be32(before+0x104,cm); amiga_store_be32(before+0x108,cl);
        word(before+0x202,(uint16_t)capacity); amiga_store_be32(before+0x204,dest);
        amiga_store_be32(before+0x50c,ins); amiga_store_be32(before+0x514,hw); word(before+0x51c,(uint16_t)entries);
        for(i=0;i<entries;++i) {
            unsigned reg=(n&256)?0x100+(random_value()%97)*2:0x180+(random_value()%32)*2;
            if(n&128) reg|=1; /* Byte offset reads, including the last trailing byte. */
            word(before+0x600+6*i,(uint16_t)((i%5==0)?1:0)); word(before+0x602+6*i,(uint16_t)reg);
        }
        switch((n/512)%10) {
        case 1: vp=BASE+SIZE; break;
        case 2: amiga_store_be32(before+0x104,BASE+SIZE); break;
        case 3: source=BASE+SIZE; break;
        case 4: amiga_store_be32(before+0x204,BASE+SIZE); break;
        case 5: amiga_store_be32(before+0x108,0); break;
        case 6: amiga_store_be32(before+0x108,BASE+SIZE); break;
        case 7: amiga_store_be32(before+0x50c,BASE+SIZE); break;
        case 8: amiga_store_be32(before+0x514,0); break;
        case 9: amiga_store_be32(before+0x514,BASE+SIZE-10); break;
        }
        memcpy(after,before,SIZE);
        expected=original_rgb4(old,vp,source,count); actual=amiga_host_load_rgb4(now,vp,source,count);
        if(expected!=actual || memcmp(before,after,SIZE)) {
            fprintf(stderr,"RGB4 compatibility differs case %u returns %d/%d\n",n,expected,actual);
            for(i=0;i<SIZE;++i) if(before[i]!=after[i]) { fprintf(stderr,"byte %04X %02X/%02X\n",i,before[i],after[i]); break; }
            return 1;
        }
        if(actual) ++success; else ++failures;
    }
    printf("RGB4 extraction: %u calls match frozen host return and all buffer bytes (%u success, %u partial/error paths)\n",cases,success,failures);
    free(now); free(old); return 0;
}
