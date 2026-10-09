#include "raster.h"

void native_raster_line(gaddr plane,const LineSetup *line,PlaneOp op,int one_dot) {
    int x=line->x,y=line->row,marked=0;
    int16_t error=line->error;
    /* C2FAB2/C2FAD8 shift X logically before the signed word offset is
     * extended. Preserve that starting address, including negative-X spill,
     * rather than rebuilding it from signed screen coordinates. BLT pointers
     * address words; an odd setup byte offset selects the same aligned word. */
    const gaddr start=(plane+(gaddr)line->offset)&~1u;
    const int first_word=line->x>>4;
    int negative=(line->con1 & 0x40u)!=0;
    int direction=(line->con1 & (line->x_major?0x04u:0x08u))?-1:1;
    unsigned dots=line->size>>6;
    if(!dots) dots=1024;
    for(unsigned i=0;i<dots;++i) {
        if(!one_dot || !marked) {
            gaddr at=start+(gaddr)((y-line->row)*40+2*((x>>4)-first_word));
            uint16_t value=rd_u16(at),bit=(uint16_t)(0x8000u>>(x&15));
            wr_u16(at,op==PLANE_SET?(uint16_t)(value|bit):
                op==PLANE_CLEAR?(uint16_t)(value&~bit):(uint16_t)(value^bit));
        }
        marked=1;
        error=(int16_t)(error+(negative?line->step_minor:line->step_both));
        if(line->x_major) {
            x+=direction;
            if(!negative) { ++y; marked=0; }
        } else {
            if(!negative) x+=direction;
            ++y; marked=0;
        }
        negative=error<0;
    }
}

static unsigned width(uint16_t size) { return (size&63u)?size&63u:64u; }
static unsigned height(uint16_t size) { return (size>>6)?size>>6:1024u; }

void native_raster_panel_copy(gaddr image,gaddr dest,uint16_t size,int16_t modulo) {
    unsigned words=width(size);
    int stride=2*(int)words+(int16_t)((uint16_t)modulo&0xfffeu);
    for(unsigned y=0;y<height(size);++y)
        for(unsigned x=0;x<words;++x)
            wr_u16(dest+y*stride+2*x,rd_u16(image+y*stride+2*x));
}
void native_raster_panel_image(gaddr mask,gaddr image,gaddr dest,uint16_t size,int16_t source_modulo,int16_t dest_modulo) {
    unsigned words=width(size);
    int source_stride=2*(int)words+(int16_t)((uint16_t)source_modulo&0xfffeu);
    int dest_stride=2*(int)words+(int16_t)((uint16_t)dest_modulo&0xfffeu);
    for(unsigned y=0;y<height(size);++y)
        for(unsigned x=0;x<words;++x) {
            unsigned from=y*source_stride+2*x,to=y*dest_stride+2*x;
            uint16_t a=rd_u16(mask+from),b=rd_u16(image+from),c=rd_u16(dest+to);
            wr_u16(dest+to,(uint16_t)(b|(~a&c)));
        }
}
void native_raster_bar(gaddr dest,uint16_t size,int16_t modulo,uint16_t first,uint16_t last,int set) {
    unsigned words=width(size);
    int stride=2*(int)words+(int16_t)((uint16_t)modulo&0xfffeu);
    for(unsigned y=0;y<height(size);++y)
        for(unsigned x=0;x<words;++x) {
            uint16_t mask=0xffff;
            if(!x) mask&=first;if(x==words-1) mask&=last;
            gaddr at=dest+y*stride+2*x;uint16_t old=rd_u16(at);
            wr_u16(at,set?(uint16_t)(mask|old):(uint16_t)(~mask&old));
        }
}
void native_raster_panel_inverted(gaddr mask,gaddr pattern,gaddr dest,uint16_t size,int16_t source_modulo,int16_t dest_modulo) {
    unsigned words=width(size);
    int source_stride=2*(int)words+(int16_t)((uint16_t)source_modulo&0xfffeu);
    int dest_stride=2*(int)words+(int16_t)((uint16_t)dest_modulo&0xfffeu);
    for(unsigned y=0;y<height(size);++y)
        for(unsigned x=0;x<words;++x) {
            unsigned from=y*source_stride+2*x,to=y*dest_stride+2*x;
            uint16_t a=rd_u16(mask+from),b=rd_u16(pattern+from),c=rd_u16(dest+to);
            wr_u16(dest+to,(uint16_t)((a&~b)|(~a&c)));
        }
}
void native_raster_compass(gaddr image,gaddr dest,uint16_t size,int16_t source_modulo,int16_t dest_modulo,uint16_t first,uint16_t last,unsigned shift) {
    unsigned words=width(size);uint16_t previous=0;
    int source_stride=2*(int)words+(int16_t)((uint16_t)source_modulo&0xfffeu);
    int dest_stride=2*(int)words+(int16_t)((uint16_t)dest_modulo&0xfffeu);
    for(unsigned y=0;y<height(size);++y)
        for(unsigned x=0;x<words;++x) {
            uint16_t a=0xffff,b=rd_u16(image+y*source_stride+2*x);
            uint16_t shifted=(uint16_t)(((uint32_t)previous<<16|b)>>shift);previous=b;
            if(!x) a&=first;if(x==words-1) a&=last;
            gaddr at=dest+y*dest_stride+2*x;uint16_t c=rd_u16(at);
            wr_u16(at,(uint16_t)((a&~shifted)|(~a&c)));
        }
}
void native_raster_mark_clear(gaddr image,gaddr dest,uint16_t size,int16_t dest_modulo) {
    unsigned words=width(size);int dest_stride=2*(int)words+(int16_t)((uint16_t)dest_modulo&0xfffeu);
    for(unsigned y=0;y<height(size);++y)
        for(unsigned x=0;x<words;++x) {
            gaddr at=dest+y*dest_stride+2*x;
            wr_u16(at,(uint16_t)(~rd_u16(image+y*2*words+2*x)&rd_u16(at)));
        }
}

void native_raster_fill(gaddr end,uint16_t size) {
    for(unsigned y=0;y<height(size);++y) {
        int inside=0;
        for(unsigned x=0;x<width(size);++x) {
            gaddr at=end-40*y-2*x;
            uint16_t original=rd_u16(at),filled=original;
            /* Inclusive fill walks right to left; each original edge bit
             * changes the carry after the pixel has been included. */
            for(unsigned bit=0;bit<16;++bit) {
                uint16_t mask=(uint16_t)(1u<<bit);
                if(inside) filled|=mask;
                if(original&mask) inside=!inside;
            }
            wr_u16(at,filled);
        }
    }
}
void native_raster_composite(gaddr mask,gaddr dest,uint16_t size,PlaneOp op) {
    for(unsigned y=0;y<height(size);++y)
        for(unsigned x=0;x<width(size);++x) {
            unsigned offset=40*y+2*x;
            uint16_t a=rd_u16(mask-offset),b=rd_u16(dest-offset);
            wr_u16(dest-offset,op==PLANE_SET?(uint16_t)(a|b):
                op==PLANE_CLEAR?(uint16_t)(~a&b):(uint16_t)(a^b));
        }
}
void native_raster_clear(gaddr end,uint16_t size) {
    for(unsigned y=0;y<height(size);++y)
        for(unsigned x=0;x<width(size);++x) wr_u16(end-40*y-2*x,0);
}
void native_raster_copy_masked(gaddr mask,gaddr from,gaddr to,uint16_t size) {
    for(unsigned y=0;y<height(size);++y)
        for(unsigned x=0;x<width(size);++x) {
            unsigned offset=40*y+2*x;
            /* C3040C retains the edge routine's 40-byte C modulo, while
             * A/B/D use the fill rectangle's 40-byte row stride. */
            unsigned source_offset=(40+2*width(size))*y+2*x;
            uint16_t a=rd_u16(mask-offset),b=rd_u16(from-offset),c=rd_u16(to-source_offset);
            wr_u16(to-offset,(uint16_t)((a&b)|(~a&c)));
        }
}
void native_raster_lane(gaddr mask,gaddr pattern,gaddr dest,uint16_t size,int16_t modulo,int set) {
    int stride=(int)(2*width(size))+(int16_t)((uint16_t)modulo&0xfffeu);
    for(unsigned y=0;y<height(size);++y)
        for(unsigned x=0;x<width(size);++x) {
            unsigned offset=40*y+2*x;
            uint16_t a=rd_u16(mask-offset),b=rd_u16(dest-offset);
            uint16_t c=rd_u16(pattern-(gaddr)((int)y*stride+2*(int)x));
            wr_u16(dest-offset,set?(uint16_t)((a&c)|b):(uint16_t)(~(a&c)&b));
        }
}
