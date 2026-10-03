/* Complete original game-side timer/readout owners and setup bounds table.
 * Clock acquisition remains the original child, including its polling loop. */
#include "main_loop_timers.h"
#include <stdlib.h>
#define SAMPLE_SECONDS 0xc45af2u
#define SAMPLE_FRACTION 0xc45af6u
#define POLL_SECONDS 0xc45afau
#define POLL_FRACTION 0xc45afeu
#define PREVIOUS_SECONDS 0xc45b02u
#define PREVIOUS_FRACTION 0xc45b06u
#define ELAPSED_SAMPLE 0xc45b0au
#define PARTIAL_TOTAL 0xc45b0eu
#define ELAPSED_TOTAL 0xc45b10u
#define NOTIFIED_TOTAL 0xc45b14u
#define TIMER_FLAGS 0xc458ceu
static void observe(const MainTimerHooks *h,enum MainTimerPhase p,uint32_t v,uint32_t other) {
    if(h && h->observe) h->observe(h->context,p,v,other);
}
static MainTimerBounds consume(const MainTimerHooks *h,enum MainTimerChild child) {
    if(h && h->consume) return h->consume(h->context,child);
    abort();
}
static void byte(const MainTimerHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,MT_BYTE_STORE,v,0); }
static void word(const MainTimerHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,MT_WORD_STORE,v,0); }
static void longword(const MainTimerHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,MT_LONG_STORE,v,0); }
static int test_byte(const MainTimerHooks *h,gaddr a) { uint8_t v=rd_u8(a); observe(h,MT_BYTE_TEST,v,0); return v!=0; }
static int16_t corner(int16_t width,int16_t height,int16_t x,int16_t y) {
    return (int16_t)(((int32_t)width*x+(int32_t)height*y)>>8);
}
void prepare_setup_bounds(const MainTimerHooks *h) {
    gaddr cursor=0xc44880u,record; MainTimerBounds scaled; int16_t offset,width,height,points[6]; unsigned i;
    observe(h,MT_BOUNDS_BEGIN,cursor,0);
    for(;;) {
        offset=rd_s16(cursor); cursor+=2; observe(h,MT_BOUNDS_OFFSET,(uint16_t)offset,cursor);
        if(offset<0) return;
        record=0xc44880u+(uint32_t)(int32_t)offset;
        for(;;) {
            observe(h,MT_BOUNDS_LOAD,record,0); observe(h,MT_BOUNDS_SENTINEL,rd_u16(record),0);
            if(rd_s16(record)==-1) break;
            width=(int16_t)(rd_u16(record+4)-rd_u16(record)); height=(int16_t)(rd_u16(record+6)-rd_u16(record+2));
            observe(h,MT_BOUNDS_VECTOR,(uint16_t)width,(uint16_t)height); scaled=consume(h,MT_SCALE_BOUNDS);
            cursor=scaled.cursor; record=scaled.record; width=scaled.width; height=scaled.height;
            points[0]=corner(width,height,-88,241); points[1]=corner(width,height,-241,-88);
            points[2]=corner(width,height,-88,-241); points[3]=corner(width,height,241,-88);
            points[4]=(int16_t)(0u-(uint16_t)points[0]); points[5]=(int16_t)(0u-(uint16_t)points[1]);
            points[0]=(int16_t)(points[0]+rd_s16(record)); points[1]=(int16_t)(points[1]+rd_s16(record+2));
            points[2]=(int16_t)(points[2]+rd_s16(record)); points[3]=(int16_t)(points[3]+rd_s16(record+2));
            points[4]=(int16_t)(points[4]+rd_s16(record+4)); points[5]=(int16_t)(points[5]+rd_s16(record+6));
            observe(h,MT_BOUNDS_CORNERS,record,0);
            for(i=0;i<6;++i) wr_u16(record+8+2*i,(uint16_t)points[i]);
            record+=20; observe(h,MT_BOUNDS_NEXT,record,0);
        }
    }
}
static uint32_t elapsed(const MainTimerHooks *h,uint32_t seconds,gaddr fraction) {
    uint32_t delta=seconds; int32_t quotient; uint32_t remainder;
    delta-=rd_u32(SAMPLE_SECONDS); observe(h,MT_SAMPLE_SUBTRACT,rd_u32(SAMPLE_SECONDS),0);
    delta=0u-delta; observe(h,MT_SAMPLE_NEGATE,0,0);
    delta=(uint16_t)delta*1000u; observe(h,MT_SAMPLE_MULTIPLY,1000,0);
    remainder=rd_u32(SAMPLE_FRACTION); observe(h,MT_FRACTION_LOAD,remainder,0);
    remainder-=rd_u32(fraction); observe(h,MT_FRACTION_SUBTRACT,rd_u32(fraction),0);
    quotient=(int32_t)remainder/1000; observe(h,MT_FRACTION_DIVIDE,1000,0);
    /* DIVS overflow preserves the dividend before EXT.L of its low word. */
    if(quotient>=-32768 && quotient<=32767) remainder=(uint16_t)quotient;
    quotient=(int16_t)remainder; observe(h,MT_FRACTION_EXTEND,(uint32_t)quotient,0);
    delta+=(uint32_t)quotient; observe(h,MT_SAMPLE_ADD,(uint32_t)quotient,0); return delta;
}
void advance_main_loop_timers(const MainTimerHooks *h) {
    uint32_t previous,delta,old,total,current; uint16_t flags,partial,divisor; uint8_t count,level,index;
    consume(h,MT_SAMPLE_BEGIN); previous=rd_u32(PREVIOUS_SECONDS); observe(h,MT_ACCUMULATOR_LOAD,previous,0);
    if((int32_t)previous>=0) {
        flags=rd_u16(TIMER_FLAGS); observe(h,MT_DIVISOR_LOAD,flags,0); observe(h,MT_FLAGS_CLEAR,flags,0);
        if(flags&0x100) {
            wr_u16(TIMER_FLAGS,(uint16_t)(flags&0xfebf)); observe(h,MT_WORD_STORE,flags&0xfebf,0);
            longword(h,ELAPSED_TOTAL,0); longword(h,NOTIFIED_TOTAL,0); goto poll;
        }
        delta=elapsed(h,previous,PREVIOUS_FRACTION);
        old=rd_u32(ELAPSED_TOTAL); wr_u32(ELAPSED_TOTAL,old+delta); observe(h,MT_TOTAL_ADD,old,delta);
        partial=rd_u16(PARTIAL_TOTAL); wr_u16(PARTIAL_TOTAL,(uint16_t)(partial+delta)); observe(h,MT_PARTIAL_ADD,partial,delta);
        partial=rd_u16(PARTIAL_TOTAL); observe(h,MT_PARTIAL_COMPARE,partial,125);
        if((int16_t)partial>=125) word(h,PARTIAL_TOTAL,0);
        total=rd_u32(ELAPSED_TOTAL); observe(h,MT_ACCUMULATOR_LOAD,total,0);
        total-=rd_u32(NOTIFIED_TOTAL); observe(h,MT_TOTAL_SUBTRACT,rd_u32(NOTIFIED_TOTAL),0);
        observe(h,MT_TOTAL_COMPARE,total,10000);
        if((int32_t)total>=10000) {
            flags=rd_u16(TIMER_FLAGS); wr_u16(TIMER_FLAGS,(uint16_t)(flags|0x40)); observe(h,MT_FLAGS_SET,flags|0x40,0);
            old=rd_u32(NOTIFIED_TOTAL); wr_u32(NOTIFIED_TOTAL,old+total); observe(h,MT_TOTAL_ADD,old,total);
        }
    }
    current=rd_u32(SAMPLE_SECONDS); observe(h,MT_FRACTION_LOAD,current,0); observe(h,MT_SAMPLE_COMPARE,rd_u32(PREVIOUS_SECONDS),0);
    if(current!=rd_u32(PREVIOUS_SECONDS)) {
        longword(h,PREVIOUS_SECONDS,current);
        count=rd_u8(0xc45884u); wr_u8(0xc45884u,(uint8_t)(count-1)); observe(h,MT_DECREMENT_PRIMARY,count,0);
        observe(h,MT_SECONDARY_CURSOR,0xc45885u,0);
        if(test_byte(h,0xc45885u)) {
            count=rd_u8(0xc45885u); wr_u8(0xc45885u,count&0x7f); observe(h,MT_SECONDARY_CLEAR_BIT,count,7);
            count=rd_u8(0xc45885u); wr_u8(0xc45885u,(uint8_t)(count-1)); observe(h,MT_SECONDARY_DECREMENT,count,0);
            if((int8_t)(count-1)<0) byte(h,0xc45885u,0);
        }
        observe(h,MT_COUNT_CURSOR,0xc45886u,0); consume(h,MT_COUNT_FIRST);
        observe(h,MT_COUNT_CURSOR,0xc45891u,0); consume(h,MT_COUNT_SECOND);
        observe(h,MT_COUNT_CURSOR,0xc4588au,0); consume(h,MT_COUNT_THIRD);
        level=rd_u8(0xc45889u); observe(h,MT_LEVEL_LOAD,level,0);
        if(test_byte(h,0xc45888u)) {
            observe(h,MT_LEVEL_ADD,1,0); level=(uint8_t)(level+1); observe(h,MT_LEVEL_COMPARE,level,15);
            if((int8_t)level>15) { level=15; observe(h,MT_LEVEL_MAXIMUM,15,0); }
        } else {
            observe(h,MT_BYTE_TEST,level,0); if((int8_t)level<0) goto save_fraction;
            observe(h,MT_LEVEL_DECREMENT,1,0); level=(uint8_t)(level-1);
        }
        byte(h,0xc45889u,level);
    }
save_fraction:
    longword(h,PREVIOUS_FRACTION,rd_u32(SAMPLE_FRACTION));
poll:
    for(;;) {
        consume(h,MT_SAMPLE_POLL); previous=rd_u32(POLL_SECONDS); observe(h,MT_ACCUMULATOR_LOAD,previous,0);
        if((int32_t)previous<0) return;
        delta=elapsed(h,previous,POLL_FRACTION); longword(h,ELAPSED_SAMPLE,delta); observe(h,MT_POLL_COMPARE,delta,32767);
        if((int32_t)delta>32767) return;
        divisor=rd_u16(0xc458dau); observe(h,MT_DIVISOR_LOAD,divisor,2); observe(h,MT_DIVISOR_MASK,7,0);
        divisor=(uint16_t)((divisor&7)+1); observe(h,MT_DIVISOR_INCREMENT,1,0); observe(h,MT_POLL_DIVIDE,divisor,0);
        /* DIVU overflow leaves the dividend unchanged. */
        if(delta/divisor<=65535) delta=delta/divisor;
        observe(h,MT_THRESHOLD_BASE,0xc2502eu,0); index=rd_u8(0xc458beu); observe(h,MT_THRESHOLD_INDEX,index,0);
        observe(h,MT_THRESHOLD_EXTEND,(uint16_t)(int16_t)(int8_t)index,0); observe(h,MT_THRESHOLD_SCALE,0,0);
        partial=rd_u16(0xc2502eu+(uint32_t)(int32_t)(int16_t)((int16_t)(int8_t)index*2)); observe(h,MT_THRESHOLD_COMPARE,partial,0);
        if((int16_t)delta>=(int16_t)partial) return;
    }
}
void sample_main_loop_readout(const MainTimerHooks *h) {
    uint32_t divisor,result,quotient; int16_t value;
    observe(h,MT_LONG_TEST,rd_u32(POLL_SECONDS),0);
    if(rd_s32(POLL_SECONDS)>=0) {
        divisor=rd_u32(ELAPSED_SAMPLE); observe(h,MT_ACCUMULATOR_LOAD,divisor,0); observe(h,MT_READOUT_LIMIT,divisor,32767);
        if((int32_t)divisor>32767) {
            result=9999; observe(h,MT_READOUT_SENTINEL,result,0);
            word(h,0xc45ae6u,(uint16_t)result); goto latch;
        }
        else {
            result=800000; observe(h,MT_READOUT_DIVIDEND,result,0);
            /* A zero divisor is an original exception boundary. The adapter
             * raises it; portable arithmetic must not invent a result. */
            if(!(uint16_t)divisor) {
                if(!h || !h->divide_zero) abort();
                result=h->divide_zero(h->context);
            } else {
                quotient=result/(uint16_t)divisor;
                if(quotient<=65535) result=((result%(uint16_t)divisor)<<16)|(uint16_t)quotient;
                observe(h,MT_READOUT_DIVIDE,(uint16_t)divisor,0);
            }
        }
        value=(int16_t)result; observe(h,MT_MINIMUM_COMPARE,rd_u16(0xc45ae8u),0);
        if(value<rd_s16(0xc45ae8u)) word(h,0xc45ae8u,(uint16_t)value);
        observe(h,MT_MAXIMUM_COMPARE,rd_u16(0xc458e0u),0);
        if(value>rd_s16(0xc458e0u)) word(h,0xc458e0u,(uint16_t)value);
        word(h,0xc45ae6u,(uint16_t)value);
    }
latch:
    longword(h,POLL_SECONDS,rd_u32(SAMPLE_SECONDS)); longword(h,POLL_FRACTION,rd_u32(SAMPLE_FRACTION));
}
