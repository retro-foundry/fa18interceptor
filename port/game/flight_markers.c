/* Complete source grid, scene-label and record-marker consumers.
 * Source bytes and actual projection/drawing children are the authority. */
#include "flight_markers.h"
#include <stdlib.h>
static void observe(const MarkerHooks *h,enum MarkerPhase p,enum MarkerValue f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static MarkerState consume(const MarkerHooks *h,enum MarkerChild child,MarkerState w) {
    if(h && h->consume_values) return h->consume_values(h->context,child,w);
    if(h && h->consume) return h->consume(h->context,child); abort();
}
static MarkerState restored(const MarkerHooks *h,MarkerState w) {
    return h && h->restored?h->restored(h->context):w;
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t low_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
static uint32_t sign_word(uint32_t v) { return (uint32_t)(int32_t)(int16_t)v; }
static uint32_t swapped(uint32_t v) { return (v<<16)|(v>>16); }
static uint32_t asr_long(uint32_t v,unsigned n) { n&=63; return (uint32_t)((int32_t)v>>(n<32?n:31)); }
static uint16_t asr_word(uint32_t v,unsigned n) { n&=63; return (uint16_t)((int16_t)v>>(n<16?n:15)); }
static gaddr indexed(gaddr base,uint32_t offset) { return base+sign_word(offset); }
static void byte(const MarkerHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,MM_STORE_BYTE,MM_OFFSET,v,0); }
static void word(const MarkerHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,MM_STORE_WORD,MM_OFFSET,v,0); }
static void longword(const MarkerHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,MM_STORE_LONG,MM_OFFSET,v,0); }
static int test_byte(const MarkerHooks *h,gaddr a) { uint8_t v=rd_u8(a); observe(h,MM_TEST_BYTE,MM_OFFSET,v,0); return v!=0; }
static int test_word(const MarkerHooks *h,gaddr a) { uint16_t v=rd_u16(a); observe(h,MM_TEST_WORD,MM_OFFSET,v,0); return v!=0; }
static int bit(const MarkerHooks *h,gaddr a,unsigned b) { uint8_t v=rd_u8(a); b&=7; observe(h,MM_BIT_TEST,MM_OFFSET,v,b); return (v&(1u<<b))!=0; }
static int change_bit(const MarkerHooks *h,gaddr a,unsigned b,int set) {
    uint8_t old=rd_u8(a); b&=7; wr_u8(a,set?(uint8_t)(old|(1u<<b)):(uint8_t)(old&~(1u<<b)));
    observe(h,set?MM_BIT_SET:MM_BIT_CLEAR,MM_OFFSET,old,b); return (old&(1u<<b))!=0;
}
static void and_byte(const MarkerHooks *h,gaddr a,uint8_t mask) { byte(h,a,rd_u8(a)&mask); }
static void and_word(const MarkerHooks *h,gaddr a,uint16_t mask) { word(h,a,rd_u16(a)&mask); }
static void or_byte(const MarkerHooks *h,gaddr a,uint8_t mask) { byte(h,a,rd_u8(a)|mask); }
static void or_word(const MarkerHooks *h,gaddr a,uint16_t mask) { word(h,a,rd_u16(a)|mask); }
static void or_long(const MarkerHooks *h,gaddr a,uint32_t mask) { longword(h,a,rd_u32(a)|mask); }
static void add_byte(const MarkerHooks *h,gaddr a,uint8_t v) { uint8_t old=rd_u8(a); wr_u8(a,(uint8_t)(old+v)); observe(h,MM_MEMORY_ADD_BYTE,MM_OFFSET,old,v); }
static void add_word(const MarkerHooks *h,gaddr a,uint16_t v) { uint16_t old=rd_u16(a); wr_u16(a,(uint16_t)(old+v)); observe(h,MM_MEMORY_ADD_WORD,MM_OFFSET,old,v); }
static void add_long(const MarkerHooks *h,gaddr a,uint32_t v) { uint32_t old=rd_u32(a); wr_u32(a,old+v); observe(h,MM_MEMORY_ADD_LONG,MM_OFFSET,old,v); }
static int64_t sub_word(const MarkerHooks *h,gaddr a,uint16_t v) { uint16_t old=rd_u16(a); wr_u16(a,(uint16_t)(old-v)); observe(h,MM_MEMORY_SUB_WORD,MM_OFFSET,old,v); return (int32_t)(int16_t)old-(int16_t)v; }
static void sub_long(const MarkerHooks *h,gaddr a,uint32_t v) { uint32_t old=rd_u32(a); wr_u32(a,old-v); observe(h,MM_MEMORY_SUB_LONG,MM_OFFSET,old,v); }
#define B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(v)); observe(h,MM_BYTE,id,w.f,0); } while(0)
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,MM_WORD,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,MM_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,MM_POINTER,id,w.f,0); } while(0)
#define AB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f+n_)); observe(h,MM_ADD_BYTE,id,n_,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,MM_ADD_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,MM_ADD_LONG,id,n_,0); } while(0)
#define SB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f-n_)); observe(h,MM_SUB_BYTE,id,n_,0); } while(0)
#define SW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f-n_)); observe(h,MM_SUB_WORD,id,n_,0); } while(0)
#define SL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f-=n_; observe(h,MM_SUB_LONG,id,n_,0); } while(0)
#define AND_B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(w.f&(v))); observe(h,MM_AND_BYTE,id,v,0); } while(0)
#define AND_W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(w.f&(v))); observe(h,MM_AND_WORD,id,v,0); } while(0)
#define AND_L(f,id,v) do { w.f&=(v); observe(h,MM_AND_LONG,id,v,0); } while(0)
#define OR_B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(w.f|(v))); observe(h,MM_OR_BYTE,id,v,0); } while(0)
#define EW(f,id) do { w.f=low_word(w.f,(uint16_t)(int16_t)(int8_t)w.f); observe(h,MM_EXT_WORD,id,0,0); } while(0)
#define EL(f,id) do { w.f=sign_word(w.f); observe(h,MM_EXT_LONG,id,0,0); } while(0)
#define SWAP(f,id) do { w.f=swapped(w.f); observe(h,MM_SWAP,id,0,0); } while(0)
#define ASW(f,id,n) do { unsigned n_=(unsigned)(n); w.f=low_word(w.f,asr_word(w.f,n_)); observe(h,MM_ASR_WORD,id,n_,0); } while(0)
#define ASL(f,id,n) do { unsigned n_=(unsigned)(n); w.f=asr_long(w.f,n_); observe(h,MM_ASR_LONG,id,n_,0); } while(0)
#define ALW(f,id,n) do { w.f=low_word(w.f,(uint16_t)(w.f<<(n))); observe(h,MM_ASL_WORD,id,n,0); } while(0)
#define ALL(f,id,n) do { w.f<<=(n); observe(h,MM_ASL_LONG,id,n,0); } while(0)
#define LSW(f,id,n) do { w.f=low_word(w.f,(uint16_t)w.f>>(n)); observe(h,MM_LSR_WORD,id,n,0); } while(0)
#define ROL(f,id,n) do { w.f=(w.f<<(n))|(w.f>>(32-(n))); observe(h,MM_ROL_LONG,id,n,0); } while(0)
#define NEGW(f,id) do { w.f=low_word(w.f,(uint16_t)(0u-w.f)); observe(h,MM_NEG_WORD,id,0,0); } while(0)
#define NEGL(f,id) do { w.f=0u-w.f; observe(h,MM_NEG_LONG,id,0,0); } while(0)
#define MUL(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=(uint32_t)((int32_t)(int16_t)w.f*(int32_t)(int16_t)n_); observe(h,MM_MULTIPLY,id,n_,0); } while(0)
#define CB(a,b) observe(h,MM_COMPARE_BYTE,MM_OFFSET,(uint8_t)(a),(uint8_t)(b))
#define CW(a,b) observe(h,MM_COMPARE_WORD,MM_OFFSET,(uint16_t)(a),(uint16_t)(b))
#define CL(a,b) observe(h,MM_COMPARE_LONG,MM_OFFSET,(uint32_t)(a),(uint32_t)(b))
static int decrement(uint32_t *v,enum MarkerValue id,const MarkerHooks *h) {
    *v=low_word(*v,(uint16_t)(*v-1)); observe(h,MM_DECREMENT,id,*v,0); return (uint16_t)*v!=0xffff;
}
static void load_words(MarkerState *w,gaddr a,uint16_t mask,int advance,const MarkerHooks *h) {
    uint32_t *values[]={&w->offset,&w->screen_y,&w->x,&w->y,&w->z,&w->row_x,&w->row_y,&w->row_z};
    unsigned i; gaddr next=a;
    for(i=0;i<8;++i) if(mask&(1u<<i)) { *values[i]=sign_word(rd_u16(next)); next+=2; }
    observe(h,MM_LOAD_WORDS,MM_OFFSET,a,mask);
    if(advance==2) w->geometry=next; else if(advance==4) w->origin=next;
    if(advance>=0) observe(h,MM_POINTER,(enum MarkerValue)(MM_RECORD+advance),next,0);
}
static void load_longs(MarkerState *w,gaddr a,uint16_t mask,const MarkerHooks *h) {
    uint32_t *values[]={&w->offset,&w->screen_y,&w->x,&w->y,&w->z,&w->row_x,&w->row_y,&w->row_z}; unsigned i; gaddr next=a;
    for(i=0;i<8;++i) if(mask&(1u<<i)) { *values[i]=rd_u32(next); next+=4; } observe(h,MM_LOAD_LONGS,MM_OFFSET,a,mask);
}
static void store_values(MarkerState w,gaddr a,uint16_t mask,int wide,const MarkerHooks *h) {
    uint32_t values[]={w.offset,w.screen_y,w.x,w.y,w.z,w.row_x,w.row_y,w.row_z}; unsigned i; gaddr next=a;
    for(i=0;i<8;++i) if(mask&(1u<<i)) { if(wide) wr_u32(next,values[i]); else wr_u16(next,(uint16_t)values[i]); next+=wide?4:2; }
    observe(h,wide?MM_STORE_LONGS:MM_STORE_WORDS,MM_OFFSET,a,mask);
}

#define NEGB(f,id) do { w.f=low_byte(w.f,(uint8_t)(0u-w.f)); observe(h,MM_NEG_BYTE,id,0,0); } while(0)
#define ALB(f,id,n) do { w.f=low_byte(w.f,(uint8_t)(w.f<<(n))); observe(h,MM_ASL_BYTE,id,n,0); } while(0)
#define LSB(f,id,n) do { w.f=low_byte(w.f,(uint8_t)w.f>>(n)); observe(h,MM_LSR_BYTE,id,n,0); } while(0)
static MarkerState pop(MarkerState w,enum MarkerPhase phase,enum MarkerValue field,const MarkerHooks *h) {
    observe(h,phase,field,0,0); return restored(h,w);
}

/* C2AFFA: unpack the signed X/Z offset and transform one view-space point. */
void transform_marker_point(MarkerState w,const MarkerHooks *h) {
    unsigned row;
    L(row_x,MM_ROW_X,w.offset); SWAP(row_x,MM_ROW_X); W(row_y,MM_ROW_Y,w.offset);
    AW(x,MM_X,w.row_x); AW(z,MM_Z,w.row_y); P(matrix,MM_MATRIX,0xc45bd8u);
    for(row=0;row<2;++row) {
        W(row_x,MM_ROW_X,w.x); W(row_y,MM_ROW_Y,w.y); W(row_z,MM_ROW_Z,w.z);
        MUL(row_x,MM_ROW_X,rd_u16(w.matrix)); P(matrix,MM_MATRIX,w.matrix+2);
        MUL(row_y,MM_ROW_Y,rd_u16(w.matrix)); P(matrix,MM_MATRIX,w.matrix+2);
        MUL(row_z,MM_ROW_Z,rd_u16(w.matrix)); P(matrix,MM_MATRIX,w.matrix+2);
        AL(row_z,MM_ROW_Z,w.row_y); AL(row_z,MM_ROW_Z,w.row_x); ASL(row_z,MM_ROW_Z,8);
        word(h,w.points,(uint16_t)w.row_z); P(points,MM_POINTS,w.points+2);
    }
    MUL(x,MM_X,rd_u16(w.matrix)); P(matrix,MM_MATRIX,w.matrix+2);
    MUL(y,MM_Y,rd_u16(w.matrix)); P(matrix,MM_MATRIX,w.matrix+2);
    MUL(z,MM_Z,rd_u16(w.matrix)); P(matrix,MM_MATRIX,w.matrix+2);
    AL(z,MM_Z,w.y); AL(z,MM_Z,w.x); ASL(z,MM_Z,8);
    word(h,w.points,(uint16_t)w.z); P(points,MM_POINTS,w.points+2);
}

/* C2B3C2: project the 16-byte scene-position stream and number visible rows. */
void draw_scene_position_labels(MarkerState w,const MarkerHooks *h) {
    CB(rd_u8(0xc458a6u),1); if(rd_u8(0xc458a6u)!=1) return;
    CB(rd_u8(0xc458aeu),5); if(rd_s8(0xc458aeu)<5) return;
    CL(rd_u32(0xc45a66u),0xfe800000u); if(rd_s32(0xc45a66u)>=(int32_t)0xfe800000u) return;
    add_byte(h,0xc45883u,1);
    if(test_byte(h,0xc45785u)) P(origin,MM_ORIGIN,0xc45c3eu);
    else { P(origin,MM_ORIGIN,0xc46184u); W(x,MM_X,rd_u16(0xc458deu)); P(origin,MM_ORIGIN,indexed(w.origin+20,w.x)); }
    P(record,MM_RECORD,0xc42a02u); word(h,0xc459aau,0);
    for(;;) {
        unsigned row; CB(rd_u8(0xc458aeu),5);
        if(rd_s8(0xc458aeu)>5 && !test_byte(h,0xc457aeu)) {
            W(offset,MM_OFFSET,rd_u16(0xc459aau)); CB(w.offset,rd_u8(0xc45848u));
            if((uint8_t)w.offset!=rd_u8(0xc45848u)) { CB(rd_u8(0xc458aeu),6); if(rd_u8(0xc458aeu)==6) goto skip_row; }
            B(offset,MM_OFFSET,rd_u8(0xc45857u));
            if((uint8_t)w.offset) { B(offset,MM_OFFSET,rd_u8(0xc45883u)); AND_B(offset,MM_OFFSET,1); if(!(uint8_t)w.offset) goto skip_row; }
        }
        load_words(&w,w.record,31,-1,h); P(record,MM_RECORD,w.record+10);
        observe(h,MM_TEST_WORD,MM_OFFSET,w.offset,0);
        if((int16_t)w.offset<0) {
            CW(w.offset,0xffff); if((uint16_t)w.offset==0xffff) return;
            P(record,MM_RECORD,w.record+6); AND_W(offset,MM_OFFSET,0x7fff); ALW(offset,MM_OFFSET,8); AW(offset,MM_OFFSET,w.offset);
            P(matrix,MM_MATRIX,indexed(0xc46184u,w.offset)); if(!bit(h,w.matrix+1,6)) goto next_row;
            L(offset,MM_OFFSET,rd_u32(w.matrix+20)); L(screen_y,MM_SCREEN_Y,rd_u32(w.matrix+28));
        } else {
            SWAP(offset,MM_OFFSET); ALL(offset,MM_OFFSET,8); SWAP(screen_y,MM_SCREEN_Y); ALL(screen_y,MM_SCREEN_Y,8);
            ALW(x,MM_X,2); P(matrix,MM_MATRIX,0xc1d7e2u); load_words(&w,indexed(w.matrix,w.x),0x60,-1,h);
            ALL(row_x,MM_ROW_X,8); ALL(row_y,MM_ROW_Y,8); ALL(row_x,MM_ROW_X,2); ALL(row_y,MM_ROW_Y,2);
            AL(offset,MM_OFFSET,w.row_x); AL(screen_y,MM_SCREEN_Y,w.row_y);
            load_words(&w,w.record,0xe0,-1,h); P(record,MM_RECORD,w.record+6);
            ALL(row_x,MM_ROW_X,4); ALL(row_y,MM_ROW_Y,4); ALL(y,MM_Y,8); ALL(z,MM_Z,8); ALL(y,MM_Y,2); ALL(z,MM_Z,2);
            AL(y,MM_Y,w.row_x); AL(z,MM_Z,w.row_y); AL(offset,MM_OFFSET,w.y); AL(screen_y,MM_SCREEN_Y,w.z);
        }
        load_longs(&w,w.origin,0x1c,h); SL(x,MM_X,w.offset); SL(z,MM_Z,w.screen_y); NEGL(x,MM_X); NEGL(y,MM_Y); NEGL(z,MM_Z);
        L(row_x,MM_ROW_X,14); ASL(x,MM_X,w.row_x); ASL(y,MM_Y,w.row_x); ASL(z,MM_Z,w.row_x); P(matrix,MM_MATRIX,0xc45bd8u);
        for(row=0;row<2;++row) {
            W(row_x,MM_ROW_X,w.x); W(row_y,MM_ROW_Y,w.y);
            if(!row) W(offset,MM_OFFSET,w.z); else W(screen_y,MM_SCREEN_Y,w.z);
            MUL(row_x,MM_ROW_X,rd_u16(w.matrix)); P(matrix,MM_MATRIX,w.matrix+2);
            MUL(row_y,MM_ROW_Y,rd_u16(w.matrix)); P(matrix,MM_MATRIX,w.matrix+2);
            if(!row) { MUL(offset,MM_OFFSET,rd_u16(w.matrix)); AL(offset,MM_OFFSET,w.row_y); AL(offset,MM_OFFSET,w.row_x); ASL(offset,MM_OFFSET,8); }
            else { MUL(screen_y,MM_SCREEN_Y,rd_u16(w.matrix)); AL(screen_y,MM_SCREEN_Y,w.row_y); AL(screen_y,MM_SCREEN_Y,w.row_x); ASL(screen_y,MM_SCREEN_Y,8); }
            P(matrix,MM_MATRIX,w.matrix+2);
        }
        MUL(x,MM_X,rd_u16(w.matrix)); P(matrix,MM_MATRIX,w.matrix+2);
        MUL(y,MM_Y,rd_u16(w.matrix)); P(matrix,MM_MATRIX,w.matrix+2);
        MUL(z,MM_Z,rd_u16(w.matrix)); P(matrix,MM_MATRIX,w.matrix+2);
        AL(x,MM_X,w.y); AL(x,MM_X,w.z); ASL(x,MM_X,8);
        /* Native children carry ordinary values, so save the two source
         * cursors locally instead of requiring the reference CPU stack. */
        MarkerState saved=w;
        observe(h,MM_SAVE_SCENE,MM_OFFSET,0,0); word(h,0xc45954u,2); w=consume(h,MM_SCENE_PROJECT,w);
        W(x,MM_X,rd_u16(0xc459aau)); AW(x,MM_X,1); W(offset,MM_OFFSET,rd_u16(0xc45958u));
        if((int16_t)w.offset>=0) { W(screen_y,MM_SCREEN_Y,rd_u16(0xc4595au)); L(y,MM_Y,0); word(h,0xc45954u,9); w=consume(h,MM_SCENE_LABEL,w); }
        w=pop(w,MM_RESTORE_SCENE,MM_OFFSET,h);
        if(h && h->consume_values) { w.y=saved.y;w.record=saved.record; }
next_row:
        add_word(h,0xc459aau,1); continue;
skip_row:
        P(record,MM_RECORD,w.record+16); observe(h,MM_TEST_WORD,MM_OFFSET,rd_u16(w.record),0);
        if(rd_s16(w.record)<0) return; goto next_row;
    }
}

/* Shared C2BAA8 tail: signed byte triples form horizontal marker segments. */
static void draw_marker_stream(MarkerState w,const MarkerHooks *h) {
    int32_t sum;
    for(;;) {
        B(row_x,MM_ROW_X,rd_u8(w.matrix)); P(matrix,MM_MATRIX,w.matrix+1); CB(w.row_x,0x80); if((uint8_t)w.row_x==0x80) return;
        B(row_y,MM_ROW_Y,rd_u8(w.matrix)); P(matrix,MM_MATRIX,w.matrix+1); B(row_z,MM_ROW_Z,rd_u8(w.matrix)); P(matrix,MM_MATRIX,w.matrix+1);
        SB(row_z,MM_ROW_Z,1); EW(row_x,MM_ROW_X); EW(row_y,MM_ROW_Y); EW(row_z,MM_ROW_Z);
        observe(h,MM_PUSH_WORD,MM_OFFSET,w.offset,0); observe(h,MM_PUSH_WORD,MM_SCREEN_Y,w.screen_y,0);
        W(x,MM_X,w.offset); W(y,MM_Y,w.screen_y); sum=(int32_t)(int16_t)w.screen_y+(int16_t)w.row_x; AW(screen_y,MM_SCREEN_Y,w.row_x);
        if(sum<0) goto restore_point; CW(w.screen_y,179); if((int16_t)w.screen_y>179) goto restore_point;
        AW(y,MM_Y,w.row_x); sum=(int32_t)(int16_t)w.offset+(int16_t)w.row_y; AW(offset,MM_OFFSET,w.row_y);
        if(sum<0) goto restore_point; AW(x,MM_X,w.row_y); AW(x,MM_X,w.row_z); CW(w.x,319); if((int16_t)w.x>319) goto restore_point;
        observe(h,MM_PUSH_LONG,MM_MATRIX,w.matrix,0); w=consume(h,MM_MARKER_LINE,w); w=pop(w,MM_POP_LONG,MM_MATRIX,h);
restore_point:
        w=pop(w,MM_POP_WORD,MM_SCREEN_Y,h); w=pop(w,MM_POP_WORD,MM_OFFSET,h);
    }
}

void draw_class_twenty_marker(MarkerState w,const MarkerHooks *h) {
    observe(h,MM_TEST_BYTE,MM_OFFSET,rd_u8(0xc45857u),0); if(rd_s8(0xc45857u)<0) return;
    load_words(&w,0xc4c592u,7,-1,h); w=consume(h,MM_CLASS20_PROJECT,w); if(w.child_negative) return;
    word(h,0xc45954u,3); P(matrix,MM_MATRIX,0xc2b91eu); draw_marker_stream(w,h);
}

void draw_record_position_marker(MarkerState w,gaddr frame,const MarkerHooks *h) {
    int32_t difference;
    load_words(&w,0xc4c592u,7,-1,h); L(x,MM_X,rd_u32(w.record+20)); L(z,MM_Z,rd_u32(w.record+28)); SWAP(x,MM_X);
    W(y,MM_Y,rd_u16(frame-2)); SWAP(z,MM_Z); w=consume(h,MM_MARKER_POINT,w); load_words(&w,0xc4c592u,7,-1,h); w=consume(h,MM_MARKER_PROJECT,w);
    if(w.child_negative) { byte(h,0xc458afu,0); return; }
    L(row_z,MM_ROW_Z,w.record); SL(row_z,MM_ROW_Z,0xc46184u); CW(w.row_z,rd_u16(0xc459bau));
    if((uint16_t)w.row_z==rd_u16(0xc459bau)) {
        observe(h,MM_TEST_BYTE,MM_OFFSET,rd_u8(0xc458afu),0);
        if(rd_s8(0xc458afu)<0) goto select_visible;
        CW(w.offset,20); if((int16_t)w.offset<20) goto marker_route;
        CW(w.offset,300); if((int16_t)w.offset>300) goto marker_route;
        CW(w.screen_y,14); if((int16_t)w.screen_y<14) goto marker_route;
        CW(w.screen_y,186); if((int16_t)w.screen_y>186) goto marker_route;
select_visible:
        byte(h,0xc458afu,1);
    }
marker_route:
    if(test_byte(h,0xc457b5u)) goto refresh_marker;
    W(row_z,MM_ROW_Z,rd_u16(0xc458dau)); B(row_y,MM_ROW_Y,rd_u8(0xc45857u)); if((int8_t)w.row_y<0) return;
    if((uint8_t)w.row_y) { EW(row_y,MM_ROW_Y); AND_W(row_z,MM_ROW_Z,w.row_y); if(!(uint16_t)w.row_z) goto refresh_marker; }
    observe(h,MM_TEST_BYTE,MM_OFFSET,rd_u8(w.record+113),0); if(rd_s8(w.record+113)<0) goto refresh_marker;
    L(row_z,MM_ROW_Z,0); B(row_z,MM_ROW_Z,rd_u8(w.record+112)); B(row_y,MM_ROW_Y,w.row_z); LSB(row_y,MM_ROW_Y,4); AND_B(row_z,MM_ROW_Z,15);
    B(x,MM_X,w.offset); AND_B(x,MM_X,15); difference=(int8_t)w.x-(int8_t)w.row_y; SB(x,MM_X,w.row_y);
    if(difference<0) { CB(w.x,0xf8); if((int8_t)w.x<=-8) AB(x,MM_X,16); }
    else { CB(w.x,8); if((int8_t)w.x>=8) SB(x,MM_X,16); }
    NEGB(x,MM_X); EW(x,MM_X); AW(offset,MM_OFFSET,w.x);
    B(y,MM_Y,w.screen_y); AND_B(y,MM_Y,15); difference=(int8_t)w.y-(int8_t)w.row_z; SB(y,MM_Y,w.row_z);
    if(difference<0) { CB(w.y,0xf8); if((int8_t)w.y<=-8) AB(y,MM_Y,16); }
    else { CB(w.y,8); if((int8_t)w.y>=8) SB(y,MM_Y,16); }
    NEGB(y,MM_Y); EW(y,MM_Y); AW(screen_y,MM_SCREEN_Y,w.y); B(z,MM_Z,rd_u8(w.record+113)); goto choose_marker;
refresh_marker:
    W(row_z,MM_ROW_Z,rd_u16(w.record+104)); W(row_y,MM_ROW_Y,900); L(z,MM_Z,0);
    difference=(int16_t)w.row_z-(int16_t)w.row_y; SW(row_z,MM_ROW_Z,w.row_y); if(difference<0) goto publish_marker;
    W(row_y,MM_ROW_Y,1800);
    for(;;) {
        AB(z,MM_Z,1); CB(w.z,15); if((int8_t)w.z>15) { L(z,MM_Z,0); break; }
        difference=(int16_t)w.row_z-(int16_t)w.row_y; SW(row_z,MM_ROW_Z,w.row_y); if(difference<0) break;
    }
publish_marker:
    byte(h,w.record+113,(uint8_t)w.z); W(row_y,MM_ROW_Y,w.offset); W(row_z,MM_ROW_Z,w.screen_y);
    AND_B(row_y,MM_ROW_Y,15); AND_B(row_z,MM_ROW_Z,15); ALB(row_y,MM_ROW_Y,4); OR_B(row_z,MM_ROW_Z,w.row_y); byte(h,w.record+112,(uint8_t)w.row_z);
    if(!test_byte(h,0xc457b5u)) return; if(!bit(h,0xc458dbu,0)) return;
choose_marker:
    EW(z,MM_Z); AW(z,MM_Z,w.z); P(matrix,MM_MATRIX,0xc2b7e4u); W(row_x,MM_ROW_X,rd_u16(indexed(w.matrix,w.z))); P(matrix,MM_MATRIX,indexed(w.matrix,w.row_x)); draw_marker_stream(w,h);
}

/* C2B564: two original grid runs followed by all sixteen control records. */
void draw_view_grid_and_markers(MarkerState w,const MarkerHooks *h) {
    gaddr frame; int64_t remaining; uint32_t old; unsigned axis;
    observe(h,MM_TEST_BYTE,MM_OFFSET,rd_u8(0xc457adu),0); if(rd_s8(0xc457adu)<=0) return;
    if(!test_byte(h,0xc45785u)) return;
    observe(h,MM_BEGIN_FRAME,MM_OFFSET,10,0); if(!h || !h->frame || !h->stack) abort(); frame=h->frame(h->context);
    longword(h,0xc456e6u,0xfffff); word(h,0xc45954u,8); load_longs(&w,0xc45c3eu,7,h);
    old=w.screen_y; w.screen_y=w.x; w.x=old; observe(h,MM_EXCHANGE,MM_SCREEN_Y,MM_X,0);
    SWAP(x,MM_X); NEGW(x,MM_X); word(h,frame-2,(uint16_t)w.x);
    SWAP(offset,MM_OFFSET); SWAP(screen_y,MM_SCREEN_Y); NEGW(offset,MM_OFFSET); NEGW(screen_y,MM_SCREEN_Y); SWAP(offset,MM_OFFSET); W(offset,MM_OFFSET,w.screen_y);
    observe(h,MM_PUSH_LONG,MM_OFFSET,w.offset,0); W(offset,MM_OFFSET,0xf660); word(h,frame-4,0xf680); word(h,frame-6,115); word(h,frame-8,24);
    for(axis=0;axis<2;++axis) {
        if(axis) {
            word(h,frame-4,0xfeb0); word(h,frame-6,34); word(h,frame-8,16);
            L(offset,MM_OFFSET,rd_u32(h->stack(h->context))); SWAP(offset,MM_OFFSET); W(offset,MM_OFFSET,192); SWAP(offset,MM_OFFSET);
        }
        do {
            P(points,MM_POINTS,0xc4c592u); W(x,MM_X,axis?5184:rd_u16(frame-4)); W(y,MM_Y,rd_u16(frame-2)); W(z,MM_Z,axis?rd_u16(frame-4):9184);
            w=consume(h,axis?MM_GRID_Z_FIRST:MM_GRID_X_FIRST,w);
            W(x,MM_X,axis?0xea40:rd_u16(frame-4)); W(y,MM_Y,rd_u16(frame-2)); W(z,MM_Z,axis?rd_u16(frame-4):0xef60);
            w=consume(h,axis?MM_GRID_Z_SECOND:MM_GRID_X_SECOND,w);
            observe(h,MM_SAVE_DRAW,MM_OFFSET,0,0); word(h,0xc45954u,8); w=consume(h,axis?MM_GRID_Z_LINE:MM_GRID_X_LINE,w);
            W(screen_y,MM_SCREEN_Y,w.offset); w=pop(w,MM_RESTORE_DRAW,MM_OFFSET,h);
            if(bit(h,frame-7,0)) {
                W(x,MM_X,rd_u16(frame-6)); observe(h,MM_TEST_WORD,MM_OFFSET,w.screen_y,0);
                if((uint16_t)w.screen_y) {
                    observe(h,MM_SAVE_DRAW,MM_OFFSET,0,0); L(y,MM_Y,axis?1:2); L(offset,MM_OFFSET,rd_u32(axis?0xc4b390u:0xc4b394u)); L(screen_y,MM_SCREEN_Y,w.offset); SWAP(offset,MM_OFFSET);
                    if(!axis) {
                        SW(offset,MM_OFFSET,12); SW(screen_y,MM_SCREEN_Y,5); CW(w.screen_y,8); if((int16_t)w.screen_y<8) goto label_finished;
                        CW(w.offset,8); if((int16_t)w.offset<8) goto label_finished; CW(w.offset,300); if((int16_t)w.offset>=300) W(offset,MM_OFFSET,300);
                    } else {
                        SW(screen_y,MM_SCREEN_Y,1); CW(w.screen_y,4); if((int16_t)w.screen_y<4) L(screen_y,MM_SCREEN_Y,4);
                        CW(w.screen_y,175); if((int16_t)w.screen_y>=175) W(screen_y,MM_SCREEN_Y,175);
                        observe(h,MM_TEST_WORD,MM_OFFSET,w.offset,0); if((int16_t)w.offset<0) L(offset,MM_OFFSET,0);
                    }
                    word(h,0xc45954u,2); w=consume(h,axis?MM_GRID_Z_LABEL:MM_GRID_X_LABEL,w);
label_finished:
                    w=pop(w,MM_RESTORE_DRAW,MM_OFFSET,h);
                }
            }
            add_word(h,frame-4,axis?560:448); if(bit(h,frame-7,0)) add_word(h,frame-6,1);
            remaining=sub_word(h,frame-8,1);
        } while(remaining>0 && rd_u16(frame-8));
    }
    w=pop(w,MM_POP_LONG,MM_OFFSET,h); L(row_y,MM_ROW_Y,0);
    do {
        P(record,MM_RECORD,indexed(0xc46184u,w.row_y)); W(row_z,MM_ROW_Z,rd_u16(w.record)); W(x,MM_X,w.row_z); AND_W(row_z,MM_ROW_Z,64); if(!(uint16_t)w.row_z) goto next_record;
        AND_W(x,MM_X,8); if((uint16_t)w.x && bit(h,w.record+3,7)) goto next_record;
        load_longs(&w,w.record+20,0x1c,h); SWAP(x,MM_X); SWAP(z,MM_Z); P(points,MM_POINTS,0xc4c592u);
        observe(h,MM_PUSH_LONG,MM_OFFSET,w.offset,0); observe(h,MM_PUSH_WORD,MM_ROW_Y,w.row_y,0); W(y,MM_Y,rd_u16(frame-2)); w=consume(h,MM_RECORD_POINT,w);
        observe(h,MM_TEST_WORD,MM_OFFSET,rd_u16(h->stack(h->context)),0);
        if(!rd_u16(h->stack(h->context))) goto player_marker;
        word(h,0xc45954u,3); B(row_z,MM_ROW_Z,rd_u8(w.record+98)); AND_B(row_z,MM_ROW_Z,240); CB(w.row_z,32);
        if((uint8_t)w.row_z==32) { w=consume(h,MM_CLASS20_MARKER,w); goto record_finished; }
        CB(rd_u8(0xc458a6u),3); if(rd_u8(0xc458a6u)==3 && !bit(h,w.record+32,6)) goto record_marker;
        W(row_z,MM_ROW_Z,rd_u16(w.record)); AND_W(row_z,MM_ROW_Z,8); if((uint16_t)w.row_z) goto player_marker;
        word(h,0xc45954u,1); goto record_marker;
player_marker:
        word(h,0xc45954u,13);
record_marker:
        w=pop(w,MM_POP_WORD,MM_ROW_Y,h); w=pop(w,MM_POP_LONG,MM_OFFSET,h);
        observe(h,MM_PUSH_LONG,MM_OFFSET,w.offset,0); observe(h,MM_PUSH_WORD,MM_ROW_Y,w.row_y,0); w=consume(h,MM_RECORD_MARKER,w);
record_finished:
        w=pop(w,MM_POP_WORD,MM_ROW_Y,h); w=pop(w,MM_POP_LONG,MM_OFFSET,h);
next_record:
        AW(row_y,MM_ROW_Y,512); CW(w.row_y,0x1e00);
    } while((int16_t)w.row_y<=0x1e00);
    observe(h,MM_END_FRAME,MM_OFFSET,0,0);
}
