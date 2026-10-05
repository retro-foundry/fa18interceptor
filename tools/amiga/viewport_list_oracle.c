/* Frozen pre-extraction host construction versus shared viewport operations. */
#include "../../port/amiga/host_graphics.h"
#include "../../port/amiga/hunk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define amiga_host_init_view previous_init_view
#define amiga_host_free_copper previous_free_copper
#define amiga_host_make_viewport previous_make_viewport
#define amiga_host_merge_view previous_merge_view
#define amiga_host_load_rgb4 previous_load_rgb4
#include "../../build/recomp/viewport_list_previous.h"
#undef amiga_host_init_view
#undef amiga_host_free_copper
#undef amiga_host_make_viewport
#undef amiga_host_merge_view
#undef amiga_host_load_rgb4

enum { BASE=0x100000,SIZE=0x20000 };
static uint32_t random_state=0x5671d334;
static uint32_t next_value(void) { random_state^=random_state<<13; random_state^=random_state>>17; random_state^=random_state<<5; return random_state; }
static void fixture_host(AmigaHostCompat *c,uint8_t *bytes,const uint8_t *base) {
    memset(c,0,sizeof *c); memcpy(bytes,base,SIZE);
    c->banks[0]=(AmigaGuestBank){BASE,SIZE,0,bytes}; c->memory=(AmigaGuestMemory){c->banks,1};
    c->free[0]=(AmigaHostRegion){BASE+0x10000,0x4000,5};
    c->free[1]=(AmigaHostRegion){BASE+0x15000,0xa000,3}; c->free_count=2;
}
static uint32_t fixture_make(AmigaHostCompat *c,unsigned n) {
    static const uint16_t widths[]={1,15,16,17,319,320,639,640};
    static const uint16_t heights[]={1,2,199,200,255,256,511,512};
    static const uint16_t positions[]={0,1,44,129,0x7fff,0x8000,0xffff,0xfffe};
    static const uint16_t capacities[]={0,1,2,15,16,31,32,33,255};
    uint8_t *v=range(c,BASE+0x100,18),*vp=range(c,BASE+0x200,40);
    uint8_t *ri=range(c,BASE+0x300,12),*bm=range(c,BASE+0x400,40),*cm=range(c,BASE+0x500,8);
    unsigned i,profile=(n/128)%14;
    uint32_t address=BASE+0x200;
    amiga_store_be32(v,BASE+0x200); word(v+12,positions[(n/8)%8]); word(v+14,positions[n%8]);
    amiga_store_be32(vp+36,BASE+0x300); amiga_store_be32(ri+4,BASE+0x400);
    word(vp+24,widths[n%8]); word(vp+26,heights[(n/8)%8]);
    word(vp+28,positions[(n/16)%8]); word(vp+30,positions[(n/32)%8]); word(vp+32,(uint16_t)next_value());
    word(bm,(uint16_t)next_value()); bm[5]=(uint8_t)(1+(n/4)%6);
    word(ri+8,(uint16_t)next_value()); word(ri+10,(uint16_t)next_value());
    for(i=0;i<6;++i) amiga_store_be32(bm+8+4*i,next_value());
    amiga_store_be32(vp+4,BASE+0x500); word(cm+2,capacities[(n/16)%9]); amiga_store_be32(cm+4,BASE+0x600);
    for(i=0;i<32;++i) word(range(c,BASE+0x600+2*i,2),(uint16_t)next_value());
    amiga_store_be32(vp+8,0);
    if(n&1) amiga_store_be32(vp+8,amiga_host_alloc(c,816,0x10004));
    switch(profile) {
    case 1: address=BASE+SIZE; break;
    case 2: amiga_store_be32(vp+36,BASE+SIZE); break;
    case 3: amiga_store_be32(ri+4,BASE+SIZE); break;
    case 4: bm[5]=(uint8_t)((n&1)?7:0); break;
    case 5: word(vp+24,(uint16_t)((n&1)?641:0)); break;
    case 6: word(vp+26,(uint16_t)((n&1)?513:0)); break;
    case 7: amiga_store_be32(vp+4,BASE+SIZE); break;
    case 8: amiga_store_be32(cm+4,BASE+SIZE); break;
    case 9: amiga_store_be32(vp+8,BASE+SIZE); break;
    case 10: if(!(n&1)) c->used_count=1024; break;
    case 11: amiga_store_be32(vp+4,0); break;
    case 12: word(cm+2,0); amiga_store_be32(cm+4,BASE+SIZE+1); break;
    case 13: c->free_count=0; break;
    }
    return address;
}
static void fixture_merge(AmigaHostCompat *c,unsigned n) {
    static const unsigned counts[]={0,1,2,8,16,17};
    static const unsigned entries[]={0,1,2,16,127,128,129};
    unsigned count=counts[n%6],i,j,profile=(n/128)%8;
    uint8_t *view=range(c,BASE+0x100,18);
    amiga_store_be32(view,count?BASE+0x200:0);
    for(i=0;i<count;++i) {
        uint32_t vp=BASE+0x200+64*i,cl=BASE+0x1000+64*i,ins=BASE+0x6000+800*i;
        uint8_t *p=range(c,vp,40),*descriptor=range(c,cl,32);
        unsigned length=entries[(n/6+i)%7];
        amiga_store_be32(p,i+1<count?vp+64:0); amiga_store_be32(p+8,cl);
        amiga_store_be32(descriptor+12,ins); word(descriptor+28,(uint16_t)length);
        for(j=0;j<length;++j) {
            uint8_t *r=range(c,ins+6*j,6);
            word(r,(uint16_t)((j&3)?0:1)); word(r+2,(uint16_t)next_value()); word(r+4,(uint16_t)next_value());
        }
        if(profile==1 && i==0) amiga_store_be32(p+8,BASE+SIZE);
        if(profile==2 && i==count/2) amiga_store_be32(descriptor+12,BASE+SIZE);
        if(profile==3 && length>1 && i==count/2) word(range(c,ins+6*(length/2),2),2);
        if(profile==4 && i+1==count) amiga_store_be32(p,BASE+0x200);
        if(profile==5 && i+1==count) amiga_store_be32(p,BASE+SIZE);
        if(profile==6 && length) word(range(c,ins,2),0xffff);
    }
    if(profile==7) c->free_count=0;
}
int main(int argc,char **argv) {
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):16384,mode,n,i,success[2]={0},errors[2]={0};
    uint8_t *bytes=malloc(SIZE),*base=malloc(SIZE),*before=malloc(SIZE),*expected=malloc(SIZE);
    AmigaHostCompat *c=malloc(sizeof *c),*prior=malloc(sizeof *c),*state=malloc(sizeof *c);
    if(!bytes || !base || !before || !expected || !c || !prior || !state || !cases) return 1;
    for(i=0;i<SIZE;++i) base[i]=(uint8_t)next_value();
    for(mode=0;mode<2;++mode) for(n=0;n<cases;++n) {
        uint32_t vp=0;
        int old,now;
        fixture_host(c,bytes,base);
        if(mode) fixture_merge(c,n); else vp=fixture_make(c,n);
        memcpy(prior,c,sizeof *c); memcpy(before,bytes,SIZE);
        old=mode?previous_merge_view(c,BASE+0x100):previous_make_viewport(c,BASE+0x100,vp);
        memcpy(state,c,sizeof *c); memcpy(expected,bytes,SIZE);
        memcpy(c,prior,sizeof *c); memcpy(bytes,before,SIZE);
        now=mode?amiga_host_merge_view(c,BASE+0x100):amiga_host_make_viewport(c,BASE+0x100,vp);
        if(old!=now || memcmp(state,c,sizeof *c) || memcmp(expected,bytes,SIZE)) {
            fprintf(stderr,"viewport extraction mode %u case %u returns %d/%d\n",mode,n,old,now);
            for(i=0;i<SIZE;++i) if(expected[i]!=bytes[i]) { fprintf(stderr,"byte %06X %02X/%02X\n",BASE+i,expected[i],bytes[i]); break; }
            return 1;
        }
        if(now) ++success[mode]; else ++errors[mode];
    }
    printf("viewport extraction: %u make and %u merge calls match return, full buffers and allocator/host state; make %u/%u and merge %u/%u success/error paths\n",cases,cases,success[0],errors[0],success[1],errors[1]);
    free(state); free(prior); free(c); free(expected); free(before); free(base); free(bytes); return 0;
}
