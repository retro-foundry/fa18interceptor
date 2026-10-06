#include "model_strips.h"
#include "globals.h"

enum { STRIP_POINTS=0xC4AD90u };
static int16_t next_word(gaddr *input) {
    int16_t value=rd_s16(*input); *input+=2; return value;
}
static int16_t vertex_shift(int16_t value,int shift) {
    unsigned bits=(unsigned)shift&63u;
    return bits>=16?(value<0?-1:0):(int16_t)(value>>bits);
}
static void transform_point(gaddr *input,gaddr target,const int16_t origin[3],int shift) {
    int16_t point[3];
    for(int k=0;k<3;++k)
        point[k]=(int16_t)(vertex_shift(next_word(input),shift)+origin[k]);
    for(int row=0;row<3;++row) {
        uint32_t sum=0;
        for(int k=0;k<3;++k)
            sum+=(uint32_t)((int32_t)point[k]*rd_s16(VIEW_ANGLE_MATRIX+6*row+2*k));
        wr_u32(target+4*row,sum);
    }
}
static void store_point(gaddr output,gaddr point) {
    for(int k=0;k<3;++k) wr_s16(output+2*k,(int16_t)(rd_s32(point+4*k)>>8));
}
static int32_t divide_step(int32_t delta,int16_t divisions) {
    int32_t quotient=delta/divisions;
    /* DIVS overflow leaves the dividend intact; the following EXT.L uses
     * its low word in that case. A positive division count cannot be zero. */
    if(quotient<-32768 || quotient>32767) return (int16_t)delta;
    return quotient;
}
void transform_model_strips(gaddr input,gaddr output,const int16_t origin[3],int shift) {
    for(;;) {
        int16_t segments=next_word(&input),stride=12;
        gaddr next_lane=output;
        if(segments<0) return;
        transform_point(&input,STRIP_POINTS,origin,shift);
        store_point(output,STRIP_POINTS); output+=12;
        for(;;) {
            do {
                transform_point(&input,STRIP_POINTS+12,origin,shift);
                int16_t divisions=next_word(&input);
                if(divisions>1) {
                    int32_t position[3],step[3];
                    for(int k=0;k<3;++k) {
                        position[k]=rd_s32(STRIP_POINTS+4*k)>>4;
                        int32_t end=rd_s32(STRIP_POINTS+12+4*k)>>4;
                        step[k]=divide_step((int32_t)((uint32_t)end-(uint32_t)position[k]),divisions);
                    }
                    for(int i=1;i<divisions;++i) {
                        for(int k=0;k<3;++k) {
                            position[k]=(int32_t)((uint32_t)position[k]+(uint32_t)step[k]);
                            wr_s16(output+2*k,(int16_t)(position[k]>>4));
                        }
                        output+=(gaddr)(int32_t)stride;
                    }
                }
                for(int k=0;k<3;++k) wr_u32(STRIP_POINTS+4*k,rd_u32(STRIP_POINTS+12+4*k));
                store_point(output,STRIP_POINTS);
                output+=(gaddr)(int32_t)stride;
                segments=(int16_t)(segments-1);
            } while(segments>0);
            if(stride<0) break;
            stride=-12; next_lane=output;
            uint16_t reverse=(uint16_t)next_word(&input);
            if(!reverse) break;
            output-=reverse&0x8000u?6u:18u;
            segments=(int16_t)(reverse&0x7fffu);
        }
        if(rd_s16(input)<=0) return;
        output=next_lane;
    }
}
