/* Complete source history, zone exit, candidate scan and triangle consumers.
 * The original instructions and children, not a physics model, are authority. */
#include "flight_geometry.h"
#include <stdlib.h>
static void observe(const GeometryHooks *h,enum GeometryPhase p,enum GeometryValue f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static GeometryState consume(const GeometryHooks *h,enum GeometryChild child) {
    if(h && h->consume) return h->consume(h->context,child); abort();
}
static GeometryState restored(const GeometryHooks *h,GeometryState w) {
    return h && h->restored?h->restored(h->context):w;
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t low_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
static uint32_t sign_word(uint32_t v) { return (uint32_t)(int32_t)(int16_t)v; }
static uint32_t swapped(uint32_t v) { return (v<<16)|(v>>16); }
static uint32_t asr_long(uint32_t v,unsigned n) { n&=63; return (uint32_t)((int32_t)v>>(n<32?n:31)); }
static uint16_t asr_word(uint32_t v,unsigned n) { n&=63; return (uint16_t)((int16_t)v>>(n<16?n:15)); }
static gaddr indexed(gaddr base,uint32_t offset) { return base+sign_word(offset); }
static void byte(const GeometryHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,FG_STORE_BYTE,FG_PRIMARY,v,0); }
static void word(const GeometryHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,FG_STORE_WORD,FG_PRIMARY,v,0); }
static void longword(const GeometryHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,FG_STORE_LONG,FG_PRIMARY,v,0); }
static int test_byte(const GeometryHooks *h,gaddr a) { uint8_t v=rd_u8(a); observe(h,FG_TEST_BYTE,FG_PRIMARY,v,0); return v!=0; }
static int test_word(const GeometryHooks *h,gaddr a) { uint16_t v=rd_u16(a); observe(h,FG_TEST_WORD,FG_PRIMARY,v,0); return v!=0; }
static int bit(const GeometryHooks *h,gaddr a,unsigned b) { uint8_t v=rd_u8(a); b&=7; observe(h,FG_BIT_TEST,FG_PRIMARY,v,b); return (v&(1u<<b))!=0; }
static int change_bit(const GeometryHooks *h,gaddr a,unsigned b,int set) {
    uint8_t old=rd_u8(a); b&=7; wr_u8(a,set?(uint8_t)(old|(1u<<b)):(uint8_t)(old&~(1u<<b)));
    observe(h,set?FG_BIT_SET:FG_BIT_CLEAR,FG_PRIMARY,old,b); return (old&(1u<<b))!=0;
}
static void and_byte(const GeometryHooks *h,gaddr a,uint8_t mask) { byte(h,a,rd_u8(a)&mask); }
static void and_word(const GeometryHooks *h,gaddr a,uint16_t mask) { word(h,a,rd_u16(a)&mask); }
static void or_byte(const GeometryHooks *h,gaddr a,uint8_t mask) { byte(h,a,rd_u8(a)|mask); }
static void or_word(const GeometryHooks *h,gaddr a,uint16_t mask) { word(h,a,rd_u16(a)|mask); }
static void or_long(const GeometryHooks *h,gaddr a,uint32_t mask) { longword(h,a,rd_u32(a)|mask); }
static void add_byte(const GeometryHooks *h,gaddr a,uint8_t v) { uint8_t old=rd_u8(a); wr_u8(a,(uint8_t)(old+v)); observe(h,FG_MEMORY_ADD_BYTE,FG_PRIMARY,old,v); }
static void add_word(const GeometryHooks *h,gaddr a,uint16_t v) { uint16_t old=rd_u16(a); wr_u16(a,(uint16_t)(old+v)); observe(h,FG_MEMORY_ADD_WORD,FG_PRIMARY,old,v); }
static void add_long(const GeometryHooks *h,gaddr a,uint32_t v) { uint32_t old=rd_u32(a); wr_u32(a,old+v); observe(h,FG_MEMORY_ADD_LONG,FG_PRIMARY,old,v); }
static int64_t sub_word(const GeometryHooks *h,gaddr a,uint16_t v) { uint16_t old=rd_u16(a); wr_u16(a,(uint16_t)(old-v)); observe(h,FG_MEMORY_SUB_WORD,FG_PRIMARY,old,v); return (int32_t)(int16_t)old-(int16_t)v; }
static void sub_long(const GeometryHooks *h,gaddr a,uint32_t v) { uint32_t old=rd_u32(a); wr_u32(a,old-v); observe(h,FG_MEMORY_SUB_LONG,FG_PRIMARY,old,v); }
#define B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(v)); observe(h,FG_BYTE,id,w.f,0); } while(0)
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,FG_WORD,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,FG_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,FG_POINTER,id,w.f,0); } while(0)
#define AB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f+n_)); observe(h,FG_ADD_BYTE,id,n_,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,FG_ADD_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,FG_ADD_LONG,id,n_,0); } while(0)
#define SB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f-n_)); observe(h,FG_SUB_BYTE,id,n_,0); } while(0)
#define SW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f-n_)); observe(h,FG_SUB_WORD,id,n_,0); } while(0)
#define SL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f-=n_; observe(h,FG_SUB_LONG,id,n_,0); } while(0)
#define AND_B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(w.f&(v))); observe(h,FG_AND_BYTE,id,v,0); } while(0)
#define AND_W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(w.f&(v))); observe(h,FG_AND_WORD,id,v,0); } while(0)
#define AND_L(f,id,v) do { w.f&=(v); observe(h,FG_AND_LONG,id,v,0); } while(0)
#define OR_B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(w.f|(v))); observe(h,FG_OR_BYTE,id,v,0); } while(0)
#define EW(f,id) do { w.f=low_word(w.f,(uint16_t)(int16_t)(int8_t)w.f); observe(h,FG_EXT_WORD,id,0,0); } while(0)
#define EL(f,id) do { w.f=sign_word(w.f); observe(h,FG_EXT_LONG,id,0,0); } while(0)
#define SWAP(f,id) do { w.f=swapped(w.f); observe(h,FG_SWAP,id,0,0); } while(0)
#define ASW(f,id,n) do { unsigned n_=(unsigned)(n); w.f=low_word(w.f,asr_word(w.f,n_)); observe(h,FG_ASR_WORD,id,n_,0); } while(0)
#define ASL(f,id,n) do { unsigned n_=(unsigned)(n); w.f=asr_long(w.f,n_); observe(h,FG_ASR_LONG,id,n_,0); } while(0)
#define ALW(f,id,n) do { w.f=low_word(w.f,(uint16_t)(w.f<<(n))); observe(h,FG_ASL_WORD,id,n,0); } while(0)
#define ALL(f,id,n) do { w.f<<=(n); observe(h,FG_ASL_LONG,id,n,0); } while(0)
#define LSW(f,id,n) do { w.f=low_word(w.f,(uint16_t)w.f>>(n)); observe(h,FG_LSR_WORD,id,n,0); } while(0)
#define ROL(f,id,n) do { w.f=(w.f<<(n))|(w.f>>(32-(n))); observe(h,FG_ROL_LONG,id,n,0); } while(0)
#define NEGW(f,id) do { w.f=low_word(w.f,(uint16_t)(0u-w.f)); observe(h,FG_NEG_WORD,id,0,0); } while(0)
#define NEGL(f,id) do { w.f=0u-w.f; observe(h,FG_NEG_LONG,id,0,0); } while(0)
#define MUL(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=(uint32_t)((int32_t)(int16_t)w.f*(int32_t)(int16_t)n_); observe(h,FG_MULTIPLY,id,n_,0); } while(0)
#define CB(a,b) observe(h,FG_COMPARE_BYTE,FG_PRIMARY,(uint8_t)(a),(uint8_t)(b))
#define CW(a,b) observe(h,FG_COMPARE_WORD,FG_PRIMARY,(uint16_t)(a),(uint16_t)(b))
#define CL(a,b) observe(h,FG_COMPARE_LONG,FG_PRIMARY,(uint32_t)(a),(uint32_t)(b))
static int decrement(uint32_t *v,enum GeometryValue id,const GeometryHooks *h) {
    *v=low_word(*v,(uint16_t)(*v-1)); observe(h,FG_DECREMENT,id,*v,0); return (uint16_t)*v!=0xffff;
}
static void load_words(GeometryState *w,gaddr a,uint16_t mask,int advance,const GeometryHooks *h) {
    uint32_t *values[]={&w->primary,&w->detail,&w->x,&w->y,&w->z,&w->rate_x,&w->rate_y,&w->rate_z};
    unsigned i; gaddr next=a;
    for(i=0;i<8;++i) if(mask&(1u<<i)) { *values[i]=sign_word(rd_u16(next)); next+=2; }
    observe(h,FG_LOAD_WORDS,FG_PRIMARY,a,mask);
    if(advance==2) w->geometry=next; else if(advance==4) w->table=next;
    if(advance>=0) observe(h,FG_POINTER,(enum GeometryValue)(FG_ROOT+advance),next,0);
}
static void load_longs(GeometryState *w,gaddr a,uint16_t mask,const GeometryHooks *h) {
    uint32_t *values[]={&w->primary,&w->detail,&w->x,&w->y,&w->z,&w->rate_x,&w->rate_y,&w->rate_z}; unsigned i; gaddr next=a;
    for(i=0;i<8;++i) if(mask&(1u<<i)) { *values[i]=rd_u32(next); next+=4; } observe(h,FG_LOAD_LONGS,FG_PRIMARY,a,mask);
}
static void store_values(GeometryState w,gaddr a,uint16_t mask,int wide,const GeometryHooks *h) {
    uint32_t values[]={w.primary,w.detail,w.x,w.y,w.z,w.rate_x,w.rate_y,w.rate_z}; unsigned i; gaddr next=a;
    for(i=0;i<8;++i) if(mask&(1u<<i)) { if(wide) wr_u32(next,values[i]); else wr_u16(next,(uint16_t)values[i]); next+=wide?4:2; }
    observe(h,wide?FG_STORE_LONGS:FG_STORE_WORDS,FG_PRIMARY,a,mask);
}

#define NEGB(f,id) do { w.f=low_byte(w.f,(uint8_t)(0u-w.f)); observe(h,FG_NEG_BYTE,id,0,0); } while(0)

void record_position_history_complete(GeometryState w,const GeometryHooks *h) {
    unsigned i;
    W(primary,FG_PRIMARY,rd_u16(0xc459b6u)); CW(w.primary,rd_u16(0xc4fdd2u));
    if((uint16_t)w.primary!=rd_u16(0xc4fdd2u)) return;
    P(record,FG_RECORD,indexed(0xc46184u,w.primary)); if(test_byte(h,0xc457aeu)) return;
    P(root,FG_ROOT,0xc4fdd4u); B(primary,FG_PRIMARY,rd_u8(0xc4fdd1u)); EW(primary,FG_PRIMARY);
    AW(primary,FG_PRIMARY,w.primary); AW(primary,FG_PRIMARY,w.primary); W(y,FG_Y,w.primary);
    AW(primary,FG_PRIMARY,w.primary); AW(primary,FG_PRIMARY,w.y); P(root,FG_ROOT,indexed(w.root,w.primary));
    for(i=0;i<3;++i) { longword(h,w.root,rd_u32(w.record+20+4*i)); P(root,FG_ROOT,w.root+4); }
    CB(rd_u8(0xc4fdd0u),5);
    if(rd_s8(0xc4fdd0u)<5) for(i=0;i<3;++i) { longword(h,w.root,rd_u32(w.record+20+4*i)); P(root,FG_ROOT,w.root+4); }
    add_byte(h,0xc4fdd1u,1); CB(rd_u8(0xc4fdd1u),5); if(rd_s8(0xc4fdd1u)>5) byte(h,0xc4fdd1u,0);
    CB(rd_u8(0xc4fdd0u),6); if(rd_s8(0xc4fdd0u)<6) add_byte(h,0xc4fdd0u,1);
    if(test_word(h,w.record+110) && bit(h,w.record+2,4)) {
        B(primary,FG_PRIMARY,rd_u8(0xc4fdd0u)); SB(primary,FG_PRIMARY,1); CB(w.primary,5);
        if((int8_t)w.primary>=5) B(primary,FG_PRIMARY,4);
    } else {
        B(primary,FG_PRIMARY,rd_u8(w.record+61));
        if((uint8_t)w.primary) {
            int32_t remaining=(int8_t)w.primary-1; SB(primary,FG_PRIMARY,1);
            if(remaining>0 && (uint8_t)w.primary) goto publish_history_count;
        }
        byte(h,0xc4fdd0u,0); byte(h,0xc4fdd1u,0);
    }
publish_history_count:
    byte(h,w.record+61,(uint8_t)w.primary);
}

int advance_record_zone_exit(ZoneExitFrame *f,const GeometryHooks *h) {
    GeometryState w=f->work;
    if(f->phase==ZONE_AFTER_FAULT || f->phase==ZONE_COMPLETE) goto finished;
    if(f->phase==ZONE_AFTER_PLACE) { byte(h,w.root+56,255); goto finished; }
    int outside;
    P(root,FG_ROOT,0xc46184u); W(primary,FG_PRIMARY,rd_u16(0xc459b4u)); ALW(primary,FG_PRIMARY,8); AW(primary,FG_PRIMARY,w.primary);
    if(!(uint16_t)w.primary) goto finished;
    P(root,FG_ROOT,indexed(w.root,w.primary)); B(rate_x,FG_RATE_X,rd_u8(w.root+98)); AND_B(rate_x,FG_RATE_X,240); CB(w.rate_x,16);
    if((uint8_t)w.rate_x!=16) goto finished; CB(rd_u8(w.root+5),8); if(rd_u8(w.root+5)==8) goto finished;
    B(primary,FG_PRIMARY,rd_u8(w.root+93)); if((int8_t)w.primary<0) goto finished;
    { int32_t zone=(int8_t)w.primary-1; SB(primary,FG_PRIMARY,1); if(zone<0) { word(h,0xc4599eu,30); f->work=w; f->phase=ZONE_AFTER_FAULT; return 0; } }
    EW(primary,FG_PRIMARY); AW(primary,FG_PRIMARY,w.primary); AW(primary,FG_PRIMARY,w.primary);
    W(rate_x,FG_RATE_X,rd_u16(w.root+6)); W(rate_y,FG_RATE_Y,rd_u16(w.root+8));
    P(scene,FG_SCENE,indexed(0xc29720u,w.primary)); P(scene,FG_SCENE,rd_u32(w.scene)); load_words(&w,w.scene,0x1e,-1,h); P(scene,FG_SCENE,w.scene+8);
    CW(w.rate_x,w.detail); outside=(int16_t)w.rate_x<(int16_t)w.detail;
    if(!outside) { CW(w.rate_x,w.x); outside=(int16_t)w.rate_x>(int16_t)w.x; }
    if(!outside) { CW(w.rate_y,w.y); outside=(int16_t)w.rate_y<(int16_t)w.y; }
    if(!outside) { CW(w.rate_y,w.z); outside=(int16_t)w.rate_y>(int16_t)w.z; }
    if(!outside) { CB(rd_u8(w.root+122),5); if(rd_u8(w.root+122)!=5) goto finished; }
    W(primary,FG_PRIMARY,rd_u16(w.scene)); P(scene,FG_SCENE,w.scene+2);
    { int32_t count=(int16_t)w.primary-1; SW(primary,FG_PRIMARY,1); if(count<0) goto finished; }
    do {
        P(scene,FG_SCENE,w.scene+4); W(detail,FG_DETAIL,rd_u16(w.scene)); P(scene,FG_SCENE,w.scene+2);
        W(x,FG_X,rd_u16(w.scene)); P(scene,FG_SCENE,w.scene+4);
        P(table,FG_TABLE,0xc295e0u); P(table,FG_TABLE,indexed(w.table,rd_u16(indexed(w.table,w.x))));
        load_words(&w,w.table,0x7c,4,h); AND_W(detail,FG_DETAIL,127); CW(w.detail,rd_u16(0xc459b4u));
        if((uint16_t)w.detail!=rd_u16(0xc459b4u)) continue;
        if(!outside) goto finished;
        CB(rd_u8(w.root+122),3);
        if(rd_u8(w.root+122)==3) byte(h,w.root+122,5);
        else { CB(rd_u8(w.root+122),4); if(rd_u8(w.root+122)==4) byte(h,w.root+122,5); }
        and_word(h,w.root,0xfffe); f->work=w; f->phase=ZONE_AFTER_PLACE; return 0;
    } while(decrement(&w.primary,FG_PRIMARY,h));
finished:
    f->work=w; f->phase=ZONE_COMPLETE; return 1;
}

void check_record_zone_exit_complete(GeometryState w,const GeometryHooks *h) {
    ZoneExitFrame frame={w,ZONE_BEGIN};
    while(!advance_record_zone_exit(&frame,h))
        frame.work=consume(h,frame.phase==ZONE_AFTER_FAULT?FG_ZONE_FAULT:FG_ZONE_PLACE);
}

void test_candidate_faces_complete(GeometryState w,gaddr frame,const GeometryHooks *h) {
    int64_t signed_sum;
    for(;;) {
        L(x,FG_X,rd_u32(w.table)); P(table,FG_TABLE,w.table+4);
        if((int32_t)w.x<0) { P(table,FG_TABLE,w.table-2); L(rate_z,FG_RATE_Z,1); return; }
        P(face,FG_FACE,w.x); load_words(&w,w.face+2,0x1c,-1,h); AND_W(z,FG_Z,0xfff);
        AW(x,FG_X,164); AW(y,FG_Y,164); AW(z,FG_Z,164); P(face,FG_FACE,indexed(w.scene,w.x)); W(primary,FG_PRIMARY,w.z);
        load_words(&w,indexed(w.scene,w.y),0x1c,-1,h); SW(x,FG_X,rd_u16(w.face)); SW(y,FG_Y,rd_u16(w.face+2)); SW(z,FG_Z,rd_u16(w.face+4));
        load_words(&w,indexed(w.scene,w.primary),0xe0,-1,h); SW(rate_x,FG_RATE_X,rd_u16(w.face)); SW(rate_y,FG_RATE_Y,rd_u16(w.face+2)); SW(rate_z,FG_RATE_Z,rd_u16(w.face+4));
        W(primary,FG_PRIMARY,w.z); W(detail,FG_DETAIL,w.rate_z); MUL(rate_z,FG_RATE_Z,w.y); MUL(z,FG_Z,w.rate_y); SL(rate_z,FG_RATE_Z,w.z);
        L(z,FG_Z,7); AW(z,FG_Z,rd_u16(frame-90)); ASL(rate_z,FG_RATE_Z,w.z);
        MUL(primary,FG_PRIMARY,w.rate_x); MUL(detail,FG_DETAIL,w.x); SL(primary,FG_PRIMARY,w.detail); ASL(primary,FG_PRIMARY,w.z);
        MUL(rate_y,FG_RATE_Y,w.x); MUL(rate_x,FG_RATE_X,w.y); SL(rate_y,FG_RATE_Y,w.rate_x); ASL(rate_y,FG_RATE_Y,w.z);
        W(rate_x,FG_RATE_X,w.rate_z); W(rate_z,FG_RATE_Z,w.rate_y); W(rate_y,FG_RATE_Y,w.primary);
        load_words(&w,w.face,0x1c,-1,h); SWAP(rate_z,FG_RATE_Z); W(rate_z,FG_RATE_Z,rd_u16(frame-90));
        ASW(x,FG_X,w.rate_z); ASL(y,FG_Y,w.rate_z); ASW(z,FG_Z,w.rate_z); SWAP(rate_z,FG_RATE_Z);
        AW(x,FG_X,rd_u16(w.scene+12)); AL(y,FG_Y,rd_u32(w.scene+16)); AW(z,FG_Z,rd_u16(w.scene+14));
        SW(x,FG_X,w.geometry); SL(y,FG_Y,w.record); SW(z,FG_Z,rd_u16(frame-66));
        NEGW(x,FG_X); NEGW(y,FG_Y); NEGW(z,FG_Z); MUL(rate_x,FG_RATE_X,w.x); MUL(rate_y,FG_RATE_Y,w.y); MUL(rate_z,FG_RATE_Z,w.z);
        AL(rate_z,FG_RATE_Z,w.rate_x); signed_sum=(int64_t)(int32_t)w.rate_z+(int32_t)w.rate_y; AL(rate_z,FG_RATE_Z,w.rate_y);
        if(signed_sum<0) continue;
        do { uint32_t value=rd_u32(w.table); observe(h,FG_TEST_LONG,FG_PRIMARY,value,0); P(table,FG_TABLE,w.table+4); if((int32_t)value<0) break; } while(1);
        P(table,FG_TABLE,w.table-2); L(rate_z,FG_RATE_Z,0); return;
    }
}

/* Each cube component uses the source's SUB/BLT absolute-value route, including
 * signed overflow. Later components remain untouched when an earlier one exits. */
static GeometryState candidate_cube(GeometryState w,uint32_t bound,const GeometryHooks *h) {
    int64_t difference;
    difference=(int64_t)(int32_t)w.rate_x-(int32_t)w.x; SL(rate_x,FG_RATE_X,w.x); if(difference<0) NEGL(rate_x,FG_RATE_X);
    CL(w.rate_x,bound); if((int32_t)w.rate_x>(int32_t)bound) { w.child_equal=0; return w; }
    difference=(int64_t)(int32_t)w.rate_y-(int32_t)w.y; SL(rate_y,FG_RATE_Y,w.y); if(difference<0) NEGL(rate_y,FG_RATE_Y);
    CL(w.rate_y,bound); if((int32_t)w.rate_y>(int32_t)bound) { w.child_equal=0; return w; }
    difference=(int64_t)(int32_t)w.rate_z-(int32_t)w.z; SL(rate_z,FG_RATE_Z,w.z); if(difference<0) NEGL(rate_z,FG_RATE_Z);
    CL(w.rate_z,bound); w.child_equal=(int32_t)w.rate_z<=(int32_t)bound; return w;
}
static GeometryState candidate_edge_line(GeometryState w,const GeometryHooks *h) {
    W(rate_y,FG_RATE_Y,rd_u16(indexed(w.scene,w.primary))); W(rate_z,FG_RATE_Z,rd_u16(indexed(w.scene+4,w.primary)));
    SW(rate_y,FG_RATE_Y,rd_u16(indexed(w.scene,w.detail))); SW(rate_z,FG_RATE_Z,rd_u16(indexed(w.scene+4,w.detail))); NEGW(rate_z,FG_RATE_Z);
    W(x,FG_X,w.rate_y); W(y,FG_Y,w.rate_z); return w;
}
static GeometryState candidate_relative_edge_point(GeometryState w,gaddr frame,const GeometryHooks *h) {
    W(detail,FG_DETAIL,rd_u16(frame-90)); ASW(z,FG_Z,w.detail); ASW(rate_x,FG_RATE_X,w.detail);
    AW(z,FG_Z,rd_u16(w.scene+12)); AW(rate_x,FG_RATE_X,rd_u16(w.scene+14));
    SW(z,FG_Z,w.geometry); SW(rate_x,FG_RATE_X,rd_u16(frame-66)); return w;
}
static GeometryState candidate_plane_dot(GeometryState w,const GeometryHooks *h) {
    int64_t sum;
    P(root,FG_ROOT,w.root+6); L(y,FG_Y,w.primary); L(z,FG_Z,w.detail); L(rate_x,FG_RATE_X,w.x);
    W(rate_y,FG_RATE_Y,rd_u16(w.record)); P(record,FG_RECORD,w.record+2); EL(rate_y,FG_RATE_Y); SL(y,FG_Y,w.rate_y);
    SW(z,FG_Z,rd_u16(w.record)); P(record,FG_RECORD,w.record+2); W(rate_y,FG_RATE_Y,rd_u16(w.record)); P(record,FG_RECORD,w.record+2); EL(rate_y,FG_RATE_Y); SL(rate_x,FG_RATE_X,w.rate_y);
    load_words(&w,w.root,0xc0,-1,h); P(root,FG_ROOT,w.root+4); MUL(rate_y,FG_RATE_Y,w.y); MUL(rate_z,FG_RATE_Z,w.z); AL(rate_z,FG_RATE_Z,w.rate_y);
    W(rate_y,FG_RATE_Y,rd_u16(w.root)); P(root,FG_ROOT,w.root+2); MUL(rate_y,FG_RATE_Y,w.rate_x);
    sum=(int64_t)(int32_t)w.rate_z+(int32_t)w.rate_y; AL(rate_z,FG_RATE_Z,w.rate_y); w.child_equal=sum>=0; return w;
}
static GeometryState candidate_plane_run(GeometryState w,gaddr frame,const GeometryHooks *h) {
    for(;;) {
        int64_t remaining;
        w=candidate_plane_dot(w,h); if(w.child_equal) return w;
        remaining=sub_word(h,frame-30,1); if(remaining<=0 || !rd_u16(frame-30)) return w;
    }
}
static GeometryState candidate_special_point(GeometryState w,gaddr frame,gaddr offset,const GeometryHooks *h) {
    P(root,FG_ROOT,w.geometry+14); P(record,FG_RECORD,w.table+14); word(h,frame-30,rd_u16(w.root)); P(root,FG_ROOT,w.root+2);
    B(y,FG_Y,rd_u8(w.scene+125)); AND_W(y,FG_Y,15);
    W(primary,FG_PRIMARY,rd_u16(w.scene+offset)); W(detail,FG_DETAIL,rd_u16(w.scene+offset+2)); W(x,FG_X,rd_u16(w.scene+offset+4));
    ASW(primary,FG_PRIMARY,w.y); ASW(detail,FG_DETAIL,w.y); ASW(x,FG_X,w.y); EL(detail,FG_DETAIL);
    AW(primary,FG_PRIMARY,rd_u16(w.scene+12)); AL(detail,FG_DETAIL,rd_u32(w.scene+16)); AW(x,FG_X,rd_u16(w.scene+14));
    EL(primary,FG_PRIMARY); EL(x,FG_X); AL(primary,FG_PRIMARY,rd_u32(frame-56)); AL(x,FG_X,rd_u32(frame-60)); return candidate_plane_run(w,frame,h);
}

void update_candidate_record_complete(GeometryState w,const GeometryHooks *h) {
    gaddr frame; int64_t signed_sum,remaining; uint32_t old; int outside;
    byte(h,0xc4589fu,0); L(primary,FG_PRIMARY,0); observe(h,FG_BEGIN_FRAME,FG_PRIMARY,92,0);
    if(!h || !h->frame || !h->stack) abort(); frame=h->frame(h->context);
    P(root,FG_ROOT,0xc46184u); W(primary,FG_PRIMARY,rd_u16(0xc459b6u));
    byte(h,frame-26,rd_u8(indexed(w.root+94,w.primary))); and_byte(h,indexed(w.root+4,w.primary),63);
    word(h,frame-20,rd_u16(indexed(w.root,w.primary))); word(h,frame-22,rd_u16(indexed(w.root+2,w.primary)));
    B(rate_y,FG_RATE_Y,rd_u8(indexed(w.root+98,w.primary))); AND_B(rate_y,FG_RATE_Y,240); CB(w.rate_y,32); if((uint8_t)w.rate_y==32) goto zero;
    byte(h,frame-34,(uint8_t)w.rate_y);
next_candidate:
    AW(primary,FG_PRIMARY,512); CW(w.primary,0x1e00); if((int16_t)w.primary>0x1e00) goto select_level;
    W(detail,FG_DETAIL,rd_u16(indexed(w.root,w.primary))); W(rate_x,FG_RATE_X,w.detail); AND_W(detail,FG_DETAIL,64); if(!(uint16_t)w.detail) goto next_candidate;
    AND_W(rate_x,FG_RATE_X,0x600); if((uint16_t)w.rate_x) goto next_candidate;
    B(rate_x,FG_RATE_X,rd_u8(indexed(w.root+94,w.primary))); CB(w.rate_x,rd_u8(frame-26)); if((uint8_t)w.rate_x==rd_u8(frame-26)) goto next_candidate;
    if(!bit(h,frame-19,3) && !bit(h,indexed(w.root+1,w.primary),3)) goto next_candidate;
    B(rate_z,FG_RATE_Z,rd_u8(indexed(w.root+98,w.primary))); CB(w.rate_z,21);
    if((uint8_t)w.rate_z==21) { P(record,FG_RECORD,0x5000); P(geometry,FG_GEOMETRY,0x1000); goto far_cube; }
    AND_B(rate_z,FG_RATE_Z,240); CB(w.rate_z,48); if((uint8_t)w.rate_z==48) goto next_candidate;
    CB(w.rate_z,32); if((uint8_t)w.rate_z==32) { P(record,FG_RECORD,0xa0000); goto far_cube; }
    B(detail,FG_DETAIL,rd_u8(frame-34)); CB(w.detail,0);
    if((uint8_t)w.detail) { CB(w.detail,w.rate_z); if((uint8_t)w.detail==(uint8_t)w.rate_z) goto same_class; }
    P(record,FG_RECORD,0x10000); P(geometry,FG_GEOMETRY,0x5000); goto far_cube;
same_class:
    if(bit(h,frame-22,0) || bit(h,indexed(w.root+2,w.primary),0)) { P(record,FG_RECORD,0x6000); P(geometry,FG_GEOMETRY,0x800); }
    else { P(record,FG_RECORD,0x7000); P(geometry,FG_GEOMETRY,0x1000); }
far_cube:
    load_longs(&w,indexed(w.root+20,w.primary),0xe0,h); w=candidate_cube(w,w.record,h); if(!w.child_equal) goto next_candidate;
    byte(h,0xc4589fu,1); or_word(h,indexed(w.root+2,w.primary),1); B(rate_z,FG_RATE_Z,rd_u8(indexed(w.root+98,w.primary))); AND_B(rate_z,FG_RATE_Z,240); CB(w.rate_z,32);
    if((uint8_t)w.rate_z==32) goto class_twenty;
    load_longs(&w,indexed(w.root+20,w.primary),0xe0,h); word(h,frame-64,(uint16_t)w.geometry); w=candidate_cube(w,w.geometry,h); if(w.child_equal) goto side_result;
    load_longs(&w,indexed(w.root+20,w.primary),0xe0,h);
    L(detail,FG_DETAIL,rd_u32(indexed(w.root+62,w.primary))); ASL(detail,FG_DETAIL,1); SL(rate_x,FG_RATE_X,w.detail);
    L(detail,FG_DETAIL,rd_u32(indexed(w.root+66,w.primary))); ASL(detail,FG_DETAIL,1); SL(rate_y,FG_RATE_Y,w.detail);
    L(detail,FG_DETAIL,rd_u32(indexed(w.root+70,w.primary))); ASL(detail,FG_DETAIL,1); SL(rate_z,FG_RATE_Z,w.detail);
    W(detail,FG_DETAIL,rd_u16(frame-64)); EL(detail,FG_DETAIL); w=candidate_cube(w,w.detail,h); if(!w.child_equal) goto next_candidate;
side_result:
    P(root,FG_ROOT,indexed(w.root,w.primary)); W(detail,FG_DETAIL,rd_u16(w.root)); AND_W(detail,FG_DETAIL,0x1000); if(!(uint16_t)w.detail) goto side_statistics;
    or_word(h,w.root,0x200); B(detail,FG_DETAIL,rd_u8(w.root+98)); AND_B(detail,FG_DETAIL,240); CB(w.detail,0); if(!(uint8_t)w.detail) goto side_statistics;
    change_bit(h,w.root+32,1,1); CB(w.detail,21); if((uint8_t)w.detail==21) goto side_statistics;
    word(h,0xc4fdd2u,(uint16_t)w.primary); change_bit(h,w.root+2,4,1); byte(h,w.root+5,1); goto side_statistics;
class_twenty:
    observe(h,FG_PUSH_INDEX,FG_PRIMARY,0,0); word(h,frame-68,0); goto save_relative;
next_pass:
    W(primary,FG_PRIMARY,rd_u16(h->stack(h->context))); W(rate_z,FG_RATE_Z,rd_u16(0xc458deu)); CW(w.rate_z,rd_u16(0xc459b6u));
    if((uint16_t)w.rate_z!=rd_u16(0xc459b6u)) goto advance_scan;
    if(test_word(h,frame-68) && rd_s16(frame-68)>0) goto advance_scan;
    add_word(h,frame-68,1); load_longs(&w,frame-46,0x1c,h); goto relative_point;
save_relative:
    store_values(w,frame-46,0x1c,1,h);
relative_point:
    AND_L(x,FG_X,0x3fffff); AND_L(z,FG_Z,0x3fffff); ASL(x,FG_X,8); ASL(y,FG_Y,8); ASL(z,FG_Z,8);
    W(rate_z,FG_RATE_Z,rd_u16(0xc458deu)); CW(w.rate_z,rd_u16(0xc459b6u));
    if((uint16_t)w.rate_z==rd_u16(0xc459b6u)) {
        P(scene,FG_SCENE,indexed(0xc46184u,w.rate_z)); B(rate_x,FG_RATE_X,rd_u8(w.scene+125)); AND_W(rate_x,FG_RATE_X,15);
        observe(h,FG_TEST_WORD,FG_PRIMARY,rd_u16(frame-68),0); P(scene,FG_SCENE,w.scene+(rd_s16(frame-68)>0?182:164));
        W(rate_z,FG_RATE_Z,rd_u16(w.scene)); ASW(rate_z,FG_RATE_Z,w.rate_x); AW(x,FG_X,w.rate_z);
        W(rate_z,FG_RATE_Z,rd_u16(w.scene+2)); ASW(rate_z,FG_RATE_Z,w.rate_x); EL(rate_z,FG_RATE_Z); AL(y,FG_Y,w.rate_z);
        W(rate_z,FG_RATE_Z,rd_u16(w.scene+4)); ASW(rate_z,FG_RATE_Z,w.rate_x); AW(z,FG_Z,w.rate_z);
    }
    P(geometry,FG_GEOMETRY,sign_word(w.x)); P(record,FG_RECORD,w.y); word(h,frame-66,(uint16_t)w.z); P(scene,FG_SCENE,indexed(w.root,w.primary));
    CB(rd_u8(w.scene+98),32); P(table,FG_TABLE,rd_u8(w.scene+98)==32?0xc39168u:0xc39e48u);
    P(face,FG_FACE,rd_u32(w.table)); P(table,FG_TABLE,w.table+4); W(rate_z,FG_RATE_Z,rd_u16(w.face)); AW(rate_z,FG_RATE_Z,164);
    B(x,FG_X,rd_u8(w.scene+125)); AND_W(x,FG_X,15); word(h,frame-90,(uint16_t)w.x);
    W(z,FG_Z,rd_u16(indexed(w.scene+2,w.rate_z))); ASW(z,FG_Z,w.x); CW(w.y,w.z); if((int16_t)w.y>=(int16_t)w.z) goto edge_polygons;
lower_faces:
    observe(h,FG_TEST_WORD,FG_PRIMARY,rd_u16(w.table),0); if(rd_s16(w.table)<0) goto advance_scan;
    w=consume(h,FG_LOWER_FACES); if(w.child_equal) goto lower_faces;
    observe(h,FG_POP_INDEX,FG_PRIMARY,0,0); w=restored(h,w); goto collision;
advance_scan:
    load_longs(&w,frame-46,0x1c,h); observe(h,FG_POP_INDEX,FG_PRIMARY,0,0); w=restored(h,w); goto next_candidate;
edge_polygons:
    W(detail,FG_DETAIL,rd_u16(indexed(w.scene+2,w.rate_z))); W(primary,FG_PRIMARY,rd_u16(frame-90)); ASW(detail,FG_DETAIL,w.primary); word(h,frame-52,(uint16_t)w.detail); P(table,FG_TABLE,w.face);
edge_polygon:
    word(h,frame-48,0);
edge_start:
    word(h,frame-50,rd_u16(w.table)); if(rd_s16(w.table)<0) goto polygons_finished;
edge:
    W(primary,FG_PRIMARY,rd_u16(w.table)); P(table,FG_TABLE,w.table+2);
    if((int16_t)w.primary<0) { AND_W(primary,FG_PRIMARY,0xfff); W(detail,FG_DETAIL,rd_u16(frame-50)); add_word(h,frame-48,1); }
    else { W(detail,FG_DETAIL,rd_u16(w.table)); AND_W(detail,FG_DETAIL,0xfff); }
    AW(primary,FG_PRIMARY,164); AW(detail,FG_DETAIL,164); P(face,FG_FACE,indexed(w.scene,w.primary)); load_words(&w,indexed(w.scene,w.detail),0xe0,-1,h);
    SW(rate_x,FG_RATE_X,rd_u16(w.face)); SW(rate_z,FG_RATE_Z,rd_u16(w.face+4)); NEGW(rate_z,FG_RATE_Z);
    old=w.rate_x; w.rate_x=w.rate_z; w.rate_z=old; observe(h,FG_EXCHANGE,FG_RATE_X,FG_RATE_Z,0);
    W(x,FG_X,rd_u16(w.face)); W(z,FG_Z,rd_u16(w.face+4)); W(primary,FG_PRIMARY,rd_u16(frame-90)); ASW(x,FG_X,w.primary); ASW(z,FG_Z,w.primary);
    AW(x,FG_X,rd_u16(w.scene+12)); AW(z,FG_Z,rd_u16(w.scene+14)); SW(x,FG_X,w.geometry); SW(z,FG_Z,rd_u16(frame-66)); NEGW(x,FG_X); NEGW(z,FG_Z);
    MUL(rate_x,FG_RATE_X,w.x); MUL(rate_z,FG_RATE_Z,w.z); signed_sum=(int64_t)(int32_t)w.rate_z+(int32_t)w.rate_x; AL(rate_z,FG_RATE_Z,w.rate_x);
    if(signed_sum>=0) goto rejected_edge;
    if(!test_word(h,frame-48)) goto edge;
    CB(rd_u8(w.scene+98),32);
    word(h,frame-70,rd_u8(w.scene+98)==32?488:428); word(h,frame-72,rd_u8(w.scene+98)==32?494:434); word(h,frame-74,rd_u8(w.scene+98)==32?500:440);
    P(table,FG_TABLE,indexed(0xc46184u,rd_u16(0xc459b6u))); if(!test_word(h,frame-68)) goto detail_height;
    W(x,FG_X,rd_u16(w.table+2)); AND_W(x,FG_X,128); if(!(uint16_t)w.x) goto clear_edge_acceptance;
    W(primary,FG_PRIMARY,rd_u16(frame-70)); W(detail,FG_DETAIL,rd_u16(frame-74)); w=candidate_edge_line(w,h);
    W(z,FG_Z,rd_u16(indexed(w.scene,w.detail))); W(rate_x,FG_RATE_X,rd_u16(indexed(w.scene+4,w.detail))); w=candidate_relative_edge_point(w,frame,h);
    MUL(rate_z,FG_RATE_Z,w.z); MUL(rate_y,FG_RATE_Y,w.rate_x); signed_sum=(int64_t)(int32_t)w.rate_y+(int32_t)w.rate_z; AL(rate_y,FG_RATE_Y,w.rate_z); if(signed_sum<0) goto clear_edge_acceptance;
    W(detail,FG_DETAIL,rd_u16(frame-72)); W(z,FG_Z,rd_u16(indexed(w.scene,w.detail))); W(rate_x,FG_RATE_X,rd_u16(indexed(w.scene+4,w.detail))); w=candidate_relative_edge_point(w,frame,h);
    MUL(y,FG_Y,w.z); MUL(x,FG_X,w.rate_x); signed_sum=(int64_t)(int32_t)w.x+(int32_t)w.y; AL(x,FG_X,w.y); if(signed_sum>=0) goto clear_edge_acceptance;
    W(primary,FG_PRIMARY,rd_u16(frame-72)); W(detail,FG_DETAIL,rd_u16(frame-74)); w=candidate_edge_line(w,h);
    W(z,FG_Z,rd_u16(indexed(w.scene,w.detail))); W(rate_x,FG_RATE_X,rd_u16(indexed(w.scene+4,w.detail))); w=candidate_relative_edge_point(w,frame,h);
    MUL(rate_z,FG_RATE_Z,w.z); MUL(rate_y,FG_RATE_Y,w.rate_x); signed_sum=(int64_t)(int32_t)w.rate_y+(int32_t)w.rate_z; AL(rate_y,FG_RATE_Y,w.rate_z); if(signed_sum>=0) goto clear_edge_acceptance;
    W(primary,FG_PRIMARY,rd_u16(frame-70)); W(detail,FG_DETAIL,rd_u16(frame-74)); P(face,FG_FACE,indexed(w.scene,w.detail));
    W(rate_y,FG_RATE_Y,rd_u16(indexed(w.scene,w.primary))); W(rate_z,FG_RATE_Z,rd_u16(indexed(w.scene+4,w.primary))); W(z,FG_Z,w.rate_y); W(rate_x,FG_RATE_X,w.rate_z);
    SW(rate_y,FG_RATE_Y,rd_u16(indexed(w.scene,w.detail))); SW(rate_z,FG_RATE_Z,rd_u16(indexed(w.scene+4,w.detail))); ASW(rate_y,FG_RATE_Y,1); ASW(rate_z,FG_RATE_Z,1); AW(z,FG_Z,w.rate_y); AW(rate_x,FG_RATE_X,w.rate_z);
    w=candidate_relative_edge_point(w,frame,h); MUL(y,FG_Y,w.z); MUL(x,FG_X,w.rate_x); signed_sum=(int64_t)(int32_t)w.x+(int32_t)w.y; AL(x,FG_X,w.y); if(signed_sum<0) goto clear_edge_acceptance;
    W(primary,FG_PRIMARY,146); W(detail,FG_DETAIL,rd_u16(indexed(w.table+4,w.primary))); W(x,FG_X,rd_u16(indexed(w.table+16,w.primary)));
    MUL(detail,FG_DETAIL,rd_u16(indexed(w.scene+4,w.primary))); MUL(x,FG_X,rd_u16(indexed(w.scene+16,w.primary))); AL(x,FG_X,w.detail); CL(w.x,0xe000000);
    if((int32_t)w.x<0xe000000) goto clear_edge_acceptance;
    or_word(h,w.table+2,0x4000); goto detail_height;
clear_edge_acceptance:
    W(y,FG_Y,rd_u16(w.table+2)); W(rate_z,FG_RATE_Z,w.y); AND_W(y,FG_Y,0x4000);
    if((uint16_t)w.y) { AND_W(rate_z,FG_RATE_Z,0x8000); if((uint16_t)w.rate_z) goto detail_height; }
    and_word(h,w.table+2,0xbfff);
detail_height:
    if(!test_word(h,frame-68)) or_byte(h,w.table+4,64); else or_byte(h,w.table+4,128);
    word(h,w.table+78,rd_u16(frame-52)); L(y,FG_Y,rd_u32(w.table+16)); CB(rd_u8(w.scene+98),32); P(table,FG_TABLE,rd_u8(w.scene+98)==32?0xc391e4u:0xc39e68u);
    P(face,FG_FACE,rd_u32(w.table)); P(table,FG_TABLE,w.table+4); W(rate_z,FG_RATE_Z,rd_u16(w.face+2)); AW(rate_z,FG_RATE_Z,164);
    W(z,FG_Z,rd_u16(indexed(w.scene+2,w.rate_z))); W(rate_z,FG_RATE_Z,rd_u16(frame-90)); ASW(z,FG_Z,w.rate_z); EL(z,FG_Z); CL(w.y,w.z);
    if((int32_t)w.y<(int32_t)w.z) goto detail_faces;
    P(table,FG_TABLE,w.table+18); P(face,FG_FACE,rd_u32(w.table)); P(table,FG_TABLE,w.table+4); W(rate_z,FG_RATE_Z,rd_u16(w.face+2)); AW(rate_z,FG_RATE_Z,164);
    W(z,FG_Z,rd_u16(indexed(w.scene+2,w.rate_z))); W(rate_z,FG_RATE_Z,rd_u16(frame-90)); ASW(z,FG_Z,w.rate_z); AW(z,FG_Z,7); EL(z,FG_Z); CL(w.y,w.z); if((int32_t)w.y>=(int32_t)w.z) goto next_pass;
detail_faces:
    w=consume(h,FG_DETAIL_FACES); if(w.child_equal) goto next_pass;
    observe(h,FG_POP_INDEX,FG_PRIMARY,0,0); w=restored(h,w); goto collision;
rejected_edge:
    if(test_word(h,frame-48)) goto edge_polygon;
    do { W(primary,FG_PRIMARY,rd_u16(w.table)); P(table,FG_TABLE,w.table+2); } while((int16_t)w.primary>=0);
    goto edge_polygon;
polygons_finished:
    if(test_word(h,frame-68)) goto next_pass;
    P(table,FG_TABLE,0xc46184u); W(primary,FG_PRIMARY,rd_u16(0xc459b6u)); B(x,FG_X,rd_u8(indexed(w.table+4,w.primary))); AND_B(x,FG_X,64);
    if((uint8_t)w.x) longword(h,indexed(w.table+66,w.primary),0); goto next_pass;
select_level:
    P(scene,FG_SCENE,indexed(0xc46184u,rd_u16(0xc459b6u))); CL(rd_u32(w.scene+16),0x7fff); if(rd_s32(w.scene+16)>0x7fff) goto zero;
    B(primary,FG_PRIMARY,rd_u8(0xc4585eu)); EW(primary,FG_PRIMARY); word(h,frame-6,(uint16_t)w.primary); longword(h,frame-10,0); L(rate_x,FG_RATE_X,0xffffffffu); P(face,FG_FACE,rd_u32(0xc459c6u));
level:
    word(h,frame-2,rd_u16(w.face)); P(face,FG_FACE,w.face-24); add_word(h,frame-8,1); if(bit(h,frame-1,4) || bit(h,frame-1,6)) goto next_level;
    CW(w.rate_x,10); if((int16_t)w.rate_x>=10) goto next_level;
    AW(rate_x,FG_RATE_X,1); P(root,FG_ROOT,0xc4d790u); P(record,FG_RECORD,0xc4d7bcu); W(primary,FG_PRIMARY,w.rate_x); ALW(primary,FG_PRIMARY,2); L(detail,FG_DETAIL,rd_u32(indexed(w.root,w.primary))); if((int32_t)w.detail<0) goto next_level;
    P(root,FG_ROOT,w.detail); ALW(primary,FG_PRIMARY,6); P(record,FG_RECORD,indexed(w.record,w.primary)); P(record,FG_RECORD,w.record+2);
    L(primary,FG_PRIMARY,0); B(primary,FG_PRIMARY,rd_u8(w.scene+7)); SB(primary,FG_PRIMARY,rd_u8(w.record)); P(record,FG_RECORD,w.record+1); B(x,FG_X,w.primary); if((int8_t)w.x<0) NEGB(x,FG_X);
    CB(w.x,1); if((int8_t)w.x>1) goto next_level;
    B(detail,FG_DETAIL,rd_u8(w.scene+9)); SB(detail,FG_DETAIL,rd_u8(w.record)); P(record,FG_RECORD,w.record+1); B(x,FG_X,w.detail); if((int8_t)w.x<0) NEGB(x,FG_X);
    CB(w.x,1); if((int8_t)w.x>1) goto next_level;
    EW(primary,FG_PRIMARY); SWAP(primary,FG_PRIMARY); ASL(primary,FG_PRIMARY,2); EW(detail,FG_DETAIL); SWAP(detail,FG_DETAIL); ASL(detail,FG_DETAIL,2);
    longword(h,frame-56,w.primary); longword(h,frame-60,w.detail); W(primary,FG_PRIMARY,rd_u16(w.scene+12)); EL(primary,FG_PRIMARY); AL(primary,FG_PRIMARY,rd_u32(frame-56));
    L(detail,FG_DETAIL,rd_u32(w.scene+16)); W(x,FG_X,rd_u16(w.scene+14)); EL(x,FG_X); AL(x,FG_X,rd_u32(frame-60));
    CW(w.detail,rd_u16(w.record)); if((int16_t)w.detail>rd_s16(w.record)) goto next_level;
    W(rate_y,FG_RATE_Y,rd_u16(w.record+2)); EL(rate_y,FG_RATE_Y); CL(w.primary,w.rate_y); if((int32_t)w.primary<=(int32_t)w.rate_y) goto next_level;
    W(rate_y,FG_RATE_Y,rd_u16(w.record+4)); EL(rate_y,FG_RATE_Y); CL(w.primary,w.rate_y); if((int32_t)w.primary>=(int32_t)w.rate_y) goto next_level;
    W(rate_y,FG_RATE_Y,rd_u16(w.record+6)); EL(rate_y,FG_RATE_Y); CL(w.x,w.rate_y); if((int32_t)w.x<=(int32_t)w.rate_y) goto next_level;
    W(rate_y,FG_RATE_Y,rd_u16(w.record+8)); EL(rate_y,FG_RATE_Y); CL(w.x,w.rate_y); if((int32_t)w.x<(int32_t)w.rate_y) goto level_chain;
next_level:
    W(primary,FG_PRIMARY,rd_u16(frame-6)); CW(w.primary,rd_u16(frame-8)); if((int16_t)w.primary<=rd_s16(frame-8)) goto terminal;
    CW(w.rate_x,10); if((int16_t)w.rate_x>=10) goto terminal; goto level;
level_chain:
    byte(h,0xc4589fu,1); P(root,FG_ROOT,w.root+10); P(record,FG_RECORD,w.record+10);
chain:
    CW(w.detail,rd_u16(w.record)); if((int16_t)w.detail>rd_s16(w.record)) goto chain_next;
    W(rate_y,FG_RATE_Y,rd_u16(w.record+2)); EL(rate_y,FG_RATE_Y); CL(w.primary,w.rate_y); if((int32_t)w.primary<=(int32_t)w.rate_y) goto chain_next;
    W(rate_y,FG_RATE_Y,rd_u16(w.record+4)); EL(rate_y,FG_RATE_Y); CL(w.primary,w.rate_y); if((int32_t)w.primary>=(int32_t)w.rate_y) goto chain_next;
    W(rate_y,FG_RATE_Y,rd_u16(w.record+6)); EL(rate_y,FG_RATE_Y); CL(w.x,w.rate_y); if((int32_t)w.x<=(int32_t)w.rate_y) goto chain_next;
    W(rate_y,FG_RATE_Y,rd_u16(w.record+8)); EL(rate_y,FG_RATE_Y); CL(w.x,w.rate_y); if((int32_t)w.x<(int32_t)w.rate_y) goto level_planes;
chain_next:
    L(y,FG_Y,rd_u32(w.record+10)); if((int32_t)w.y<0) goto next_level;
    P(record,FG_RECORD,w.y); P(root,FG_ROOT,rd_u32(w.root+10)); goto chain;
level_planes:
    W(rate_y,FG_RATE_Y,rd_u16(w.scene+2)); AND_W(rate_y,FG_RATE_Y,2); if(!(uint16_t)w.rate_y) goto zero;
    SWAP(rate_x,FG_RATE_X); P(geometry,FG_GEOMETRY,w.root); P(table,FG_TABLE,w.record); P(root,FG_ROOT,w.geometry+14); P(record,FG_RECORD,w.table+14);
    word(h,frame-30,rd_u16(w.root)); P(root,FG_ROOT,w.root+2); B(y,FG_Y,rd_u8(w.scene+125)); W(rate_y,FG_RATE_Y,rd_u16(w.scene+164)); AND_W(y,FG_Y,15); ASW(rate_y,FG_RATE_Y,w.y); EL(rate_y,FG_RATE_Y); AL(primary,FG_PRIMARY,w.rate_y);
    W(z,FG_Z,rd_u16(w.scene+166)); ASW(z,FG_Z,w.y); AW(detail,FG_DETAIL,w.z); W(rate_y,FG_RATE_Y,rd_u16(w.scene+168)); ASW(rate_y,FG_RATE_Y,w.y); EL(rate_y,FG_RATE_Y); AL(x,FG_X,w.rate_y); EL(detail,FG_DETAIL);
    B(y,FG_Y,rd_u8(w.scene+98)); AND_B(y,FG_Y,240); CB(w.y,0); if((uint8_t)w.y) goto plane_run;
    word(h,frame-76,8); load_longs(&w,w.scene+62,0x38,h); ASL(y,FG_Y,3); ASL(z,FG_Z,3); ASL(rate_x,FG_RATE_X,3); NEGL(y,FG_Y); NEGL(z,FG_Z); NEGL(rate_x,FG_RATE_X); store_values(w,frame-88,0x38,1,h);
    NEGL(y,FG_Y); NEGL(z,FG_Z); NEGL(rate_x,FG_RATE_X); ASL(y,FG_Y,5); ASL(z,FG_Z,5); ASL(rate_x,FG_RATE_X,5); SL(primary,FG_PRIMARY,w.y); SL(detail,FG_DETAIL,w.z); SL(x,FG_X,w.rate_x);
plane_run:
    w=candidate_plane_run(w,frame,h); if(w.child_equal) goto plane_outside;
    B(y,FG_Y,rd_u8(w.scene+98)); AND_B(y,FG_Y,240); CB(w.y,0); if((uint8_t)w.y) goto collision;
    load_longs(&w,w.scene+62,0x1c,h); SL(x,FG_X,rd_u32(frame-88)); SL(y,FG_Y,rd_u32(frame-84)); SL(z,FG_Z,rd_u32(frame-80));
    P(face,FG_FACE,w.face+24); W(rate_z,FG_RATE_Z,rd_u16(w.face)); P(face,FG_FACE,w.face+2); P(record,FG_RECORD,rd_u32(w.face)); P(face,FG_FACE,w.face+14);
    observe(h,FG_BIT_TEST,FG_PRIMARY,w.rate_z,4); if(w.rate_z&16) goto settle_position;
    observe(h,FG_BIT_TEST,FG_PRIMARY,w.rate_z,6); if(w.rate_z&64) goto settle_position;
    L(detail,FG_DETAIL,rd_u32(w.record+4)); if((int32_t)w.detail<=0) goto settle_position;
    P(root,FG_ROOT,w.detail); W(detail,FG_DETAIL,rd_u16(w.root));
    if((int16_t)w.detail>=0) { AND_W(detail,FG_DETAIL,0x4000); W(detail,FG_DETAIL,rd_u16(w.root+((uint16_t)w.detail?2:4))); }
    CW(w.detail,0xffff); if((uint16_t)w.detail==0xffff) goto settle_position;
    AND_W(detail,FG_DETAIL,0xfff); B(detail,FG_DETAIL,rd_u8(indexed(w.root+7,w.detail))); AND_B(detail,FG_DETAIL,16); if(!(uint8_t)w.detail) goto settle_position;
    load_longs(&w,w.scene+62,0xe0,h); ASL(rate_x,FG_RATE_X,1); ASL(rate_y,FG_RATE_Y,1); ASL(rate_z,FG_RATE_Z,1); AL(x,FG_X,w.rate_x); AL(y,FG_Y,w.rate_y); AL(z,FG_Z,w.rate_z);
settle_position:
    sub_long(h,w.scene+20,w.x); sub_long(h,w.scene+24,w.y); sub_long(h,w.scene+28,w.z); goto collision;
plane_outside:
    B(y,FG_Y,rd_u8(w.scene+98)); AND_B(y,FG_Y,240); CB(w.y,16); if((uint8_t)w.y==16) goto special_points;
    CB(w.y,0); if((uint8_t)w.y) goto terminal_height;
    remaining=sub_word(h,frame-76,1); if(remaining<0) goto terminal_height;
    load_longs(&w,w.scene+62,0x38,h); ASL(y,FG_Y,3); ASL(z,FG_Z,3); ASL(rate_x,FG_RATE_X,3); add_long(h,frame-88,w.y); add_long(h,frame-84,w.z); add_long(h,frame-80,w.rate_x);
    ASL(y,FG_Y,8); ASL(z,FG_Z,8); ASL(rate_x,FG_RATE_X,8); AL(primary,FG_PRIMARY,w.y); AL(detail,FG_DETAIL,w.z); AL(x,FG_X,w.rate_x);
    P(root,FG_ROOT,w.geometry+14); P(record,FG_RECORD,w.table+14); word(h,frame-30,rd_u16(w.root)); P(root,FG_ROOT,w.root+2); goto plane_run;
special_points:
    w=candidate_special_point(w,frame,170,h); if(!w.child_equal) goto collision;
    w=candidate_special_point(w,frame,176,h); if(!w.child_equal) goto collision; goto terminal;
side_statistics:
    P(record,FG_RECORD,indexed(0xc46184u,rd_u16(0xc459b6u)));
    if(!bit(h,w.record+1,3)) goto clear_side_flags;
    if(!test_byte(h,w.record+94)) {
        P(geometry,FG_GEOMETRY,rd_u32(0xc1ab74u)); CB(rd_u8(w.record+98),0);
        if(rd_u8(w.record+98)==0) { if(!bit(h,w.root+1,3)) { add_word(h,w.geometry+68,1); goto side_pair; } }
        else { CB(rd_u8(w.record+98),1); if(rd_u8(w.record+98)==1 && !bit(h,w.root+1,3)) add_word(h,w.geometry+64,1); }
    }
side_pair:
    if(bit(h,w.root+1,3)) { change_bit(h,w.root+32,7,1); change_bit(h,w.record+32,7,1); goto contact; }
clear_side_flags:
    change_bit(h,w.root+32,7,0); change_bit(h,w.record+32,7,0);
    if(test_word(h,0xc459b6u)) goto contact;
    B(primary,FG_PRIMARY,rd_u8(w.root+98)); AND_B(primary,FG_PRIMARY,240); CB(w.primary,0); if((uint8_t)w.primary) goto contact;
    P(root,FG_ROOT,rd_u32(0xc1ab74u)); add_word(h,w.root+72,1);
contact:
    L(primary,FG_PRIMARY,64); observe(h,FG_END_FRAME,FG_PRIMARY,0,0); return;
terminal:
    L(detail,FG_DETAIL,rd_u32(w.scene+16)); L(x,FG_X,w.detail); B(primary,FG_PRIMARY,rd_u8(w.scene+123)); if((int8_t)w.primary<0) goto final_height;
    B(rate_z,FG_RATE_Z,rd_u8(w.scene+98)); AND_B(rate_z,FG_RATE_Z,240); CB(w.rate_z,16); if((uint8_t)w.rate_z!=16) goto final_height;
    AND_W(primary,FG_PRIMARY,15); if((uint16_t)w.primary) goto final_height;
    W(detail,FG_DETAIL,rd_u16(w.scene+166)); B(y,FG_Y,rd_u8(w.scene+125)); AND_W(y,FG_Y,15); ASW(detail,FG_DETAIL,w.y); EL(detail,FG_DETAIL);
    signed_sum=(int64_t)(int32_t)w.detail+(int32_t)w.x; AL(detail,FG_DETAIL,w.x); if(signed_sum<0) goto ground;
    W(detail,FG_DETAIL,rd_u16(w.scene+172)); ASW(detail,FG_DETAIL,w.y); EL(detail,FG_DETAIL); signed_sum=(int64_t)(int32_t)w.detail+(int32_t)w.x; AL(detail,FG_DETAIL,w.x); if(signed_sum<0) goto ground;
    W(detail,FG_DETAIL,rd_u16(w.scene+178)); ASW(detail,FG_DETAIL,w.y); EL(detail,FG_DETAIL); AL(detail,FG_DETAIL,w.x); goto final_height;
terminal_height:
    L(detail,FG_DETAIL,rd_u32(w.scene+16));
final_height:
    observe(h,FG_TEST_LONG,FG_PRIMARY,w.detail,0); if((int32_t)w.detail<0) goto ground;
zero:
    L(primary,FG_PRIMARY,0); observe(h,FG_END_FRAME,FG_PRIMARY,0,0); return;
ground:
    L(primary,FG_PRIMARY,16); observe(h,FG_END_FRAME,FG_PRIMARY,0,0); return;
collision:
    L(primary,FG_PRIMARY,32); observe(h,FG_END_FRAME,FG_PRIMARY,0,0);
}
