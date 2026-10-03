/* Complete shared point projection, packed readouts and bounded HUD sweeps.
 * Original projection modes, layouts, glyph tables and child calls are retained. */
#include "projection_readouts.h"
#include <stdlib.h>
static void observe(const ReadoutHooks *h,enum ReadoutPhase p,enum ReadoutValue f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static ReadoutState consume(const ReadoutHooks *h,enum ReadoutChild child) {
    if(h && h->consume) return h->consume(h->context,child); abort();
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t low_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
static uint32_t sign_word(uint32_t v) { return (uint32_t)(int32_t)(int16_t)v; }
static uint32_t swapped(uint32_t v) { return (v<<16)|(v>>16); }
static uint16_t asr_word(uint32_t v,unsigned n) { n&=63; return (uint16_t)((int16_t)v>>(n<16?n:15)); }
static gaddr indexed(gaddr base,uint32_t offset) { return base+sign_word(offset); }
static void word(const ReadoutHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,PR_STORE_WORD,PR_X,v,0); }
static void byte(const ReadoutHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,PR_STORE_BYTE,PR_X,v,0); }
static void longword(const ReadoutHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,PR_STORE_LONG,PR_X,v,0); }
static void add_word(const ReadoutHooks *h,gaddr a,uint16_t v) { uint16_t old=rd_u16(a); wr_u16(a,(uint16_t)(old+v)); observe(h,PR_MEMORY_ADD_WORD,PR_X,old,v); }
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,PR_WORD,id,w.f,0); } while(0)
#define B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(v)); observe(h,PR_BYTE,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,PR_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,PR_POINTER,id,w.f,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,PR_ADD_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,PR_ADD_LONG,id,n_,0); } while(0)
#define SW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f-n_)); observe(h,PR_SUB_WORD,id,n_,0); } while(0)
#define AND_W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(w.f&(v))); observe(h,PR_AND_WORD,id,v,0); } while(0)
#define OR_W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(w.f|(v))); observe(h,PR_OR_WORD,id,v,0); } while(0)
#define SWAP(f,id) do { w.f=swapped(w.f); observe(h,PR_SWAP,id,0,0); } while(0)
#define EL(f,id) do { w.f=sign_word(w.f); observe(h,PR_EXT_LONG,id,0,0); } while(0)
#define ASW(f,id,n) do { unsigned n_=(unsigned)(n); w.f=low_word(w.f,asr_word(w.f,n_)); observe(h,PR_ASR_WORD,id,n_,0); } while(0)
#define ALW(f,id,n) do { w.f=low_word(w.f,(uint16_t)(w.f<<(n))); observe(h,PR_ASL_WORD,id,n,0); } while(0)
#define LSL(f,id,n) do { w.f>>=(n); observe(h,PR_LSR_LONG,id,n,0); } while(0)
#define NEGW(f,id) do { w.f=low_word(w.f,(uint16_t)(0u-w.f)); observe(h,PR_NEG_WORD,id,0,0); } while(0)
#define MUL(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=(uint32_t)((int32_t)(int16_t)w.f*(int16_t)n_); observe(h,PR_MULTIPLY,id,n_,0); } while(0)
#define CW(a,b) observe(h,PR_COMPARE_WORD,PR_X,(uint16_t)(a),(uint16_t)(b))
#define CB(a,b) observe(h,PR_COMPARE_BYTE,PR_X,(uint8_t)(a),(uint8_t)(b))
static int decrement(uint32_t *v,enum ReadoutValue field,const ReadoutHooks *h) {
    *v=low_word(*v,(uint16_t)(*v-1)); observe(h,PR_DECREMENT,field,*v,0); return (uint16_t)*v!=0xffff;
}
static ReadoutState pop_word(ReadoutState w,enum ReadoutValue field,const ReadoutHooks *h) {
    observe(h,PR_POP_WORD,field,0,0); if(!h || !h->restored) abort(); return h->restored(h->context);
}
static void divide(uint32_t *v,int16_t divisor,enum ReadoutValue field,const ReadoutHooks *h) {
    int32_t dividend=(int32_t)*v,quotient,remainder;
    /* Original callers test positive depth before either division. */
    if(!divisor) abort();
    quotient=dividend/divisor; remainder=dividend%divisor;
    if(quotient==(int16_t)quotient) *v=((uint32_t)(uint16_t)remainder<<16)|(uint16_t)quotient;
    observe(h,PR_DIVIDE,field,(uint16_t)divisor,0);
}

/* C2ECEA: reflection, last-row acceptance and original plot-mode dispatch. */
static void finish_point_projection(ReadoutState w,const ReadoutHooks *h) {
    int32_t sum;
    SW(x,PR_X,319); NEGW(x,PR_X); SW(y,PR_Y,179); NEGW(y,PR_Y); AW(y,PR_Y,1);
    CW(w.y,rd_u16(0xc45984u)); if((int16_t)w.y>rd_s16(0xc45984u)) goto rejected;
    wr_u16(0xc45958u,(uint16_t)w.x); wr_u16(0xc4595au,(uint16_t)w.y); /* MOVEM leaves flags. */
    observe(h,PR_STORE_WORD,PR_X,w.mode,0);
    if((int16_t)w.mode>=0) {
        W(value,PR_VALUE,48); ASW(value,PR_VALUE,w.mode);
    }
    /* Nonnegative modes select the original size-dependent block/pair/pixel. */
    if((int16_t)w.mode>=0) {
        gaddr frame; if(!h || !h->frame) abort(); frame=h->frame(h->context);
        CW(w.value,rd_u16(frame-40)); if((int16_t)w.value>=rd_s16(frame-40)) { w=consume(h,PR_BLOCK); goto drawn; }
        W(value,PR_VALUE,80); ASW(value,PR_VALUE,w.mode); CW(w.value,rd_u16(frame-40));
        if((int16_t)w.value>=rd_s16(frame-40)) w=consume(h,PR_PAIR); else w=consume(h,PR_PIXEL);
        goto drawn;
    }
    sum=(int16_t)w.mode+1; AW(mode,PR_MODE,1); if(sum>=0) { w=consume(h,PR_PIXEL); goto drawn; }
    sum=(int16_t)w.mode+1; AW(mode,PR_MODE,1); if(sum>=0) { w=consume(h,PR_PAIR); goto drawn; }
    sum=(int16_t)w.mode+1; AW(mode,PR_MODE,1); if(sum>=0) { w=consume(h,PR_BLOCK); goto drawn; }
    sum=(int16_t)w.mode+1; AW(mode,PR_MODE,1); if(sum>=0) { w=consume(h,PR_CIRCLE); goto drawn; }
    W(x,PR_X,w.x); return;
drawn:
    L(x,PR_X,1); return;
rejected:
    L(x,PR_X,0); longword(h,0xc45958u,0xffffffffu);
}


/* C2ECE4: the upper Y clamp is shared with the complete projector. */
void finish_projected_y_limit(ReadoutState w,const ReadoutHooks *h) {
    CW(w.y,180); if((int16_t)w.y>=180) W(y,PR_Y,179);
    finish_point_projection(w,h);
}
static void scale_projected_y(ReadoutState w,const ReadoutHooks *h) {
    int32_t sum;
    MUL(y,PR_Y,90); divide(&w.y,(int16_t)w.value,PR_Y,h); sum=(int16_t)w.y+90; AW(y,PR_Y,90);
    if(sum<0) { W(y,PR_Y,0); finish_point_projection(w,h); }
    else finish_projected_y_limit(w,h);
}
/* C2ECD2: the upper X clamp precedes Y scaling. */
void finish_projected_x_limit(ReadoutState w,const ReadoutHooks *h) {
    CW(w.x,320); if((int16_t)w.x>=320) W(x,PR_X,319);
    scale_projected_y(w,h);
}
/* C2EC90/94/9C/A4/A8 share C2ECAA. One actual child performs the drawing. */
void project_and_plot_point(ReadoutState w,int selected_mode,const ReadoutHooks *h) {
    int32_t sum;
    if(selected_mode==1) W(mode,PR_MODE,rd_u16(0xc45ab8u));
    else L(mode,PR_MODE,selected_mode);
    CW(w.x,w.value); if((int16_t)w.x>=(int16_t)w.value) goto rejected;
    CW(w.y,w.value); if((int16_t)w.y>=(int16_t)w.value) goto rejected;
    W(shift,PR_SHIFT,w.x); NEGW(shift,PR_SHIFT); CW(w.shift,w.value); if((int16_t)w.shift>=(int16_t)w.value) goto rejected;
    W(shift,PR_SHIFT,w.y); NEGW(shift,PR_SHIFT); CW(w.shift,w.value); if((int16_t)w.shift>=(int16_t)w.value) goto rejected;
    observe(h,PR_STORE_WORD,PR_X,w.value,0);
    if((int16_t)w.value<=0) { word(h,0xc4599eu,27); w=consume(h,PR_FAULT); L(x,PR_X,0); return; }
    MUL(x,PR_X,160); divide(&w.x,(int16_t)w.value,PR_X,h); sum=(int16_t)w.x+160; AW(x,PR_X,160);
    if(sum<0) { W(x,PR_X,0); scale_projected_y(w,h); }
    else finish_projected_x_limit(w,h);
    return;
rejected:
    L(x,PR_X,0); longword(h,0xc45958u,0xffffffffu);
}

/* C32AD0: packed digits, optional leading blanks and the four original planes. */
static void draw_packed_digits(ReadoutState w,const ReadoutHooks *h) {
    unsigned plane; int32_t sum;
    L(shift,PR_SHIFT,rd_u32(0xc45b22u)); W(offset,PR_OFFSET,w.value);
    do {
        W(y,PR_Y,w.shift); AND_W(y,PR_Y,15); AW(y,PR_Y,48);
        P(output,PR_OUTPUT,w.output-1); byte(h,w.output,(uint8_t)w.y); LSL(shift,PR_SHIFT,4);
    } while(decrement(&w.value,PR_VALUE,h));
    observe(h,PR_TEST_BYTE,PR_X,w.glyph,0);
    if(!(uint8_t)w.glyph) {
        SW(offset,PR_OFFSET,1);
        for(;;) {
            uint8_t c=rd_u8(w.output); P(output,PR_OUTPUT,w.output+1); CB(c,48); if(c!=48) break;
            byte(h,w.output-1,32); sum=(int16_t)w.offset-1; SW(offset,PR_OFFSET,1); if(sum<0) break;
        }
    }
    P(destination,PR_DESTINATION,w.destination+w.mode); W(mode,PR_MODE,322); P(planes,PR_PLANES,rd_u32(0xc456b6u)); AW(stride,PR_STRIDE,w.stride);
    do {
        W(offset,PR_OFFSET,rd_u16(w.layout)); P(layout,PR_LAYOUT,w.layout+2); W(shift,PR_SHIFT,rd_u16(w.layout)); P(layout,PR_LAYOUT,w.layout+2);
        B(glyph,PR_GLYPH,rd_u8(w.text)); P(text,PR_TEXT,w.text+1); CB(w.glyph,32); if((uint8_t)w.glyph==32) goto next_glyph;
        SWAP(x,PR_X); W(value,PR_VALUE,w.x); SWAP(x,PR_X); AW(value,PR_VALUE,w.stride);
        sum=(int16_t)w.value+(int16_t)w.offset; AW(value,PR_VALUE,w.offset); if(sum<0) goto next_glyph;
        CW(w.value,40); if((int16_t)w.value>=40) goto next_glyph;
        AW(offset,PR_OFFSET,w.stride); EL(offset,PR_OFFSET); AL(offset,PR_OFFSET,w.destination);
        AND_W(glyph,PR_GLYPH,255); SW(glyph,PR_GLYPH,32); AW(glyph,PR_GLYPH,w.glyph); P(shape,PR_SHAPE,0xc3d790u); P(shape,PR_SHAPE,indexed(w.shape,rd_u16(indexed(w.shape,w.glyph))));
        L(glyph,PR_GLYPH,w.shape); L(y,PR_Y,rd_u32(w.planes)); AL(y,PR_Y,w.offset);
        observe(h,PR_BIT_TEST,PR_X,w.y,0); if(w.y&1) goto next_glyph;
        for(plane=0;plane<4;++plane) {
            if(plane) { L(y,PR_Y,rd_u32(w.planes+4*plane)); AL(y,PR_Y,w.offset); }
            W(value,PR_VALUE,0xb0a); observe(h,PR_BIT_TEST,PR_X,rd_u8(0xc45955u),3-plane);
            if(rd_u8(0xc45955u)&(1u<<(3-plane))) W(value,PR_VALUE,0xbfa);
            OR_W(value,PR_VALUE,w.shift); w=consume(h,(enum ReadoutChild)(PR_GLYPH_FIRST+plane));
        }
next_glyph:
        ;
    } while(decrement(&w.x,PR_X,h));
}

void draw_scene_numeric_label(ReadoutState w,const ReadoutHooks *h) {
    EL(value,PR_VALUE); longword(h,0xc45b1eu,w.value); w=consume(h,PR_TO_BCD);
    P(layout,PR_LAYOUT,0xc3198cu); P(text,PR_TEXT,0xc457fau); W(value,PR_VALUE,w.shift); P(output,PR_OUTPUT,indexed(w.text+1,w.value));
    W(shift,PR_SHIFT,w.x); AND_W(shift,PR_SHIFT,15); ASW(shift,PR_SHIFT,2); AW(shift,PR_SHIFT,2); AW(shift,PR_SHIFT,w.shift); AW(shift,PR_SHIFT,w.shift); P(layout,PR_LAYOUT,indexed(w.layout,w.shift));
    ASW(x,PR_X,3); EL(x,PR_X); ALW(y,PR_Y,3); W(shift,PR_SHIFT,w.y); AW(shift,PR_SHIFT,w.shift); AW(shift,PR_SHIFT,w.shift); AW(y,PR_Y,w.shift); EL(y,PR_Y);
    P(destination,PR_DESTINATION,w.y); observe(h,PR_BIT_CLEAR,PR_X,w.x,0); w.x&=~1u;
    P(destination,PR_DESTINATION,w.destination+w.x); SWAP(x,PR_X); W(x,PR_X,w.value); L(stride,PR_STRIDE,0); L(mode,PR_MODE,0); L(glyph,PR_GLYPH,1); draw_packed_digits(w,h);
}
void draw_packed_numeric_readout(ReadoutState w,const ReadoutHooks *h) {
    L(stride,PR_STRIDE,0); L(mode,PR_MODE,0); W(value,PR_VALUE,w.x); L(glyph,PR_GLYPH,0); draw_packed_digits(w,h);
}
static void draw_readout_tick(ReadoutState w,int altitude,const ReadoutHooks *h) {
    L(glyph,PR_GLYPH,altitude?1:0); P(layout,PR_LAYOUT,altitude?0xc33258u:0xc33264u); W(x,PR_X,altitude?26:10);
    L(stride,PR_STRIDE,2); L(mode,PR_MODE,2); P(text,PR_TEXT,0xc457fau); P(output,PR_OUTPUT,w.text+3);
    W(value,PR_VALUE,w.mode); P(destination,PR_DESTINATION,w.offset); SWAP(x,PR_X); W(x,PR_X,w.stride); (void)consume(h,PR_TICK);
}
void draw_speed_readout_tick(ReadoutState w,const ReadoutHooks *h) { draw_readout_tick(w,0,h); }
void draw_altitude_readout_tick(ReadoutState w,const ReadoutHooks *h) { draw_readout_tick(w,1,h); }

void draw_bounded_readout_sweep(ReadoutState w,const ReadoutHooks *h) {
    gaddr frame; int32_t sum=(int16_t)w.x+rd_s16(0xc45988u); unsigned pass;
    AW(x,PR_X,rd_u16(0xc45988u)); if(sum<0 || !(uint16_t)w.x) return;
    CW(w.x,319); if((int16_t)w.x>=319) return;
    observe(h,PR_BEGIN_FRAME,PR_X,8,0); if(!h || !h->frame) abort(); frame=h->frame(h->context);
    word(h,frame-4,(uint16_t)w.x); word(h,frame-6,(uint16_t)w.y); AW(x,PR_X,w.y); W(y,PR_Y,rd_u16(0xc4598cu));
    observe(h,PR_PUSH_WORD,PR_Y,w.y,0); w=consume(h,PR_SWEEP_CENTRE); w=pop_word(w,PR_Y,h); word(h,frame-2,0);
    for(pass=0;pass<2;++pass) {
        if(pass) { W(y,PR_Y,rd_u16(0xc4598cu)); word(h,frame-2,0); }
        for(;;) {
            W(x,PR_X,rd_u16(frame-4)); if(pass) AW(y,PR_Y,3); else SW(y,PR_Y,3);
            CW(w.y,pass?112:71); if(pass?(int16_t)w.y>=112:(int16_t)w.y<=71) break;
            observe(h,PR_PUSH_WORD,PR_Y,w.y,0); CW(rd_u16(frame-2),4);
            if(rd_u16(frame-2)!=4) { CW(rd_u16(frame-2),9); if(rd_u16(frame-2)!=9) { w=consume(h,pass?PR_SWEEP_DOWN_PIXEL:PR_SWEEP_UP_PIXEL); goto restore_y; } }
            AW(x,PR_X,rd_u16(frame-6)); w=consume(h,pass?PR_SWEEP_DOWN_PAIR:PR_SWEEP_UP_PAIR);
restore_y:
            w=pop_word(w,PR_Y,h); add_word(h,frame-2,1);
        }
    }
    observe(h,PR_END_FRAME,PR_X,0,0);
}
