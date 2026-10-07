/* Original motion projection, slot publication and collision helpers.
 * Source: sealed C26322/C26352/C26C72/C26CC0/C26D8A owners. */
#include "flight_motion_helpers.h"
#include "plane_tests.h"
#include <stdlib.h>
static void observe(const MotionHooks *h,enum MotionPhase p,enum MotionValue f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t low_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
static uint32_t sign_word(uint32_t v) { return (uint32_t)(int32_t)(int16_t)v; }
static uint32_t asr_long(uint32_t v,unsigned n) { n&=63; return (uint32_t)((int32_t)v>>(n<32?n:31)); }
static uint16_t asr_word(uint32_t v,unsigned n) { n&=63; return (uint16_t)((int16_t)v>>(n<16?n:15)); }
static gaddr indexed(gaddr base,uint32_t offset) { return base+sign_word(offset); }
static void store_byte(const MotionHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,MH_STORE_BYTE,MH_VALUE,v,0); }
static void store_word(const MotionHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,MH_STORE_WORD,MH_VALUE,v,0); }
static void store_long(const MotionHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,MH_STORE_LONG,MH_VALUE,v,0); }
static void add_position(const MotionHooks *h,gaddr a,uint32_t v) {
    uint32_t old=rd_u32(a); wr_u32(a,old+v); observe(h,MH_MEMORY_ADD_LONG,MH_VALUE,old,v);
}
static void load_position(MotionState *w,gaddr a,const MotionHooks *h) {
    w->x=rd_u32(a); w->y=rd_u32(a+4); w->z=rd_u32(a+8); observe(h,MH_LOAD_POSITION,MH_VALUE,a,0);
}
static void load_normal(MotionState *w,gaddr a,const MotionHooks *h) {
    w->x=sign_word(rd_u16(a)); w->y=sign_word(rd_u16(a+2)); w->z=sign_word(rd_u16(a+4));
    observe(h,MH_LOAD_NORMAL,MH_VALUE,a,0);
}
static void load_vector(MotionState *w,gaddr a,const MotionHooks *h) {
    w->nx=sign_word(rd_u16(a)); w->ny=sign_word(rd_u16(a+2)); w->nz=sign_word(rd_u16(a+4));
    observe(h,MH_LOAD_VECTOR,MH_VALUE,a,0);
}
#define B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(v)); observe(h,MH_BYTE,id,w.f,0); } while(0)
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,MH_WORD,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,MH_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,MH_POINTER,id,w.f,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,MH_ADD_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,MH_ADD_LONG,id,n_,0); } while(0)
#define SW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f-n_)); observe(h,MH_SUB_WORD,id,n_,0); } while(0)
#define SL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f-=n_; observe(h,MH_SUB_LONG,id,n_,0); } while(0)
#define ANDW(f,id,v) do { w.f=low_word(w.f,(uint16_t)(w.f&(v))); observe(h,MH_AND_WORD,id,v,0); } while(0)
#define EW(f,id) do { w.f=low_word(w.f,(uint16_t)(int16_t)(int8_t)w.f); observe(h,MH_EXT_WORD,id,0,0); } while(0)
#define EL(f,id) do { w.f=sign_word(w.f); observe(h,MH_EXT_LONG,id,0,0); } while(0)
#define ASW(f,id,n) do { unsigned n_=(unsigned)(n); w.f=low_word(w.f,asr_word(w.f,n_)); observe(h,MH_ASR_WORD,id,n_,0); } while(0)
#define ASL(f,id,n) do { w.f=asr_long(w.f,n); observe(h,MH_ASR_LONG,id,n,0); } while(0)
#define ALW(f,id,n) do { w.f=low_word(w.f,(uint16_t)(w.f<<(n))); observe(h,MH_ASL_WORD,id,n,0); } while(0)
#define ALL(f,id,n) do { w.f<<=(n); observe(h,MH_ASL_LONG,id,n,0); } while(0)
#define LSL(f,id,n) do { w.f>>=(n); observe(h,MH_LSR_LONG,id,n,0); } while(0)
#define NEG(f,id) do { w.f=low_word(w.f,(uint16_t)(0u-w.f)); observe(h,MH_NEG_WORD,id,0,0); } while(0)
#define MUL(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=(uint32_t)((int32_t)(int16_t)w.f*(int32_t)(int16_t)n_); observe(h,MH_MULTIPLY,id,n_,0); } while(0)
#define CW(a,b) observe(h,MH_COMPARE_WORD,MH_VALUE,(uint16_t)(a),(uint16_t)(b))
#define CL(a,b) observe(h,MH_COMPARE_LONG,MH_VALUE,(uint32_t)(a),(uint32_t)(b))
static uint32_t divided(uint32_t value,int16_t divisor) {
    int32_t quotient,remainder;
    if(value==0x80000000u && divisor==-1) return 0;
    quotient=(int32_t)value/divisor; remainder=(int32_t)value%divisor;
    return quotient==(int16_t)quotient?((uint32_t)(uint16_t)remainder<<16)|(uint16_t)quotient:value;
}
static MotionState divide_projection(MotionState w,unsigned first_site,const MotionHooks *h) {
    if(!(uint16_t)w.selector) {
        if(!h || !h->divide_exception) abort(); w=h->divide_exception(h->context,first_site);
    } else {
        uint16_t divisor=(uint16_t)w.selector; w.value=divided(w.value,(int16_t)divisor);
        observe(h,MH_DIVIDE,MH_VALUE,divisor,0);
    }
    if(!(uint16_t)w.selector) {
        if(!h || !h->divide_exception) abort(); w=h->divide_exception(h->context,first_site+1);
    } else {
        uint16_t divisor=(uint16_t)w.selector; w.x=divided(w.x,(int16_t)divisor);
        observe(h,MH_DIVIDE,MH_X,divisor,0);
    }
    MUL(value,MH_VALUE,w.y); MUL(x,MH_X,w.y); ASL(value,MH_VALUE,4); ASL(x,MH_X,4); return w;
}
MotionState project_record_motion(MotionState w,const MotionHooks *h) {
    L(value,MH_VALUE,w.nx); L(selector,MH_SELECTOR,w.ny); L(x,MH_X,w.nz); ASL(selector,MH_SELECTOR,8);
    if(!w.selector) return w;
    L(y,MH_Y,rd_u32(w.record+24)); ASL(y,MH_Y,8); NEG(y,MH_Y); ALL(value,MH_VALUE,4); ALL(x,MH_X,4);
    w=divide_projection(w,0,h);
    add_position(h,w.record+20,w.value); store_long(h,w.record+24,0); add_position(h,w.record+28,w.x);
    return w;
}
void project_scene_motion(MotionState w,gaddr frame,const MotionHooks *h) {
    L(value,MH_VALUE,rd_u32(frame+8)); ALW(value,MH_VALUE,6); P(scene,MH_SCENE,0xc45c72u); P(scene,MH_SCENE,indexed(w.scene,w.value));
    w.value=rd_u32(w.scene+12); w.selector=rd_u32(w.scene+16); w.x=rd_u32(w.scene+20);
    observe(h,MH_LOAD_FACE,MH_VALUE,w.scene+12,0); ASL(selector,MH_SELECTOR,8);
    if(w.selector) {
        L(y,MH_Y,rd_u32(w.scene+4)); ASL(y,MH_Y,8); NEG(y,MH_Y); ALL(value,MH_VALUE,4); ALL(x,MH_X,4);
        w=divide_projection(w,2,h);
        add_position(h,w.scene,w.value); store_long(h,w.scene+4,0); add_position(h,w.scene+8,w.x);
        ASL(value,MH_VALUE,8); ASL(x,MH_X,8);
    }
    L(value,MH_VALUE,0);
}
void publish_motion_slot(MotionState w,const MotionHooks *h) {
    unsigned slot;
    P(table,MH_TABLE,0xc48184u); L(value,MH_VALUE,15);
    for(slot=0;slot<16;++slot) {
        uint8_t flags=rd_u8(w.table+1); observe(h,MH_BIT_TEST,MH_VALUE,flags,6);
        if(!(flags&64)) {
            uint32_t values[8]; unsigned i;
            for(i=0;i<8;++i) values[i]=rd_u32(w.record+4*i);
            w.x=values[0]; w.y=values[1]; w.z=values[2]; w.nx=values[3]; w.ny=values[4]; w.nz=values[5]; w.root=values[6]; w.face=values[7];
            observe(h,MH_LOAD_SLOT,MH_VALUE,w.record,0);
            for(i=0;i<8;++i) wr_u32(w.table+4*i,values[i]);
            store_word(h,w.table,64); store_word(h,w.table+4,127);
            observe(h,MH_TEST_WORD,MH_VALUE,w.selector,0);
            if(!(uint16_t)w.selector) {
                L(selector,MH_SELECTOR,18); observe(h,MH_BIT_TEST,MH_VALUE,w.value,0);
                if(w.value&1) L(selector,MH_SELECTOR,19);
            }
            store_word(h,w.table+2,(uint16_t)w.selector); store_byte(h,0xc45858u,255); return;
        }
        P(table,MH_TABLE,w.table+32); w.value=low_word(w.value,(uint16_t)(w.value-1));
        observe(h,MH_SCAN_EXHAUSTED,MH_VALUE,w.value,0);
    }
}
static MotionState component_table(MotionState w,gaddr address,const MotionHooks *h) {
    P(table,MH_TABLE,address); P(face,MH_FACE,rd_u32(w.table)); P(table,MH_TABLE,w.table+4);
    W(nz,MH_NZ,rd_u16(w.face+2)); AW(nz,MH_NZ,164); return w;
}
static MotionState component_faces(MotionState w,gaddr frame,const MotionHooks *h,int upper) {
    if(h && h->consume_face) return h->consume_face(h->context,upper);
    /* C27456: consume the actual pointer stream and original plane-side
     * predicate. Only its result and stream cursor are live on return here. */
    w.nz=(uint32_t)faces_all_behind(&w.table,w.scene,rd_s16(frame-90),
                                  (int16_t)w.point_x,(int32_t)w.record,rd_s16(frame-66));
    w.child_equal=w.nz==0;return w;
}
MotionState test_component_motion(MotionState w,gaddr frame,const MotionHooks *h) {
    uint8_t code; int hit=0;
    const gaddr saved_scene=w.scene,saved_face=w.face;
    observe(h,MH_SAVE_CURSORS,MH_VALUE,0,0); store_byte(h,frame-32,0); load_position(&w,w.scene,h);
    LSL(x,MH_X,8); LSL(y,MH_Y,8); LSL(z,MH_Z,8);
    P(point_x,MH_POINT_X,sign_word(w.x)); P(record,MH_RECORD,w.y); store_word(h,frame-66,(uint16_t)w.z);
    P(scene,MH_SCENE,0xc46184u); W(value,MH_VALUE,rd_u16(0xc459b8u)); ALW(value,MH_VALUE,8); AW(value,MH_VALUE,w.value); P(scene,MH_SCENE,indexed(w.scene,w.value));
    code=rd_u8(w.scene+98); observe(h,MH_COMPARE_BYTE,MH_VALUE,code,32);
    w=component_table(w,code==32?0xc39168u:0xc39e48u,h);
    B(x,MH_X,rd_u8(w.scene+125)); ANDW(x,MH_X,15); store_word(h,frame-90,(uint16_t)w.x);
    W(z,MH_Z,rd_u16(indexed(w.scene+2,w.nz))); ASW(z,MH_Z,w.x); CW(w.y,w.z);
    if((int16_t)w.y<(int16_t)w.z) {
        for(;;) {
            uint16_t marker=rd_u16(w.table); observe(h,MH_TEST_WORD,MH_VALUE,marker,0); if((int16_t)marker<0) break;
            w=component_faces(w,frame,h,0);
            if(!w.child_equal) { hit=1; break; }
        }
    } else {
        code=rd_u8(w.scene+98); observe(h,MH_COMPARE_BYTE,MH_VALUE,code,32);
        w=component_table(w,code==32?0xc391e4u:0xc39e68u,h);
        W(z,MH_Z,rd_u16(indexed(w.scene+2,w.nz))); EL(z,MH_Z); CL(w.z,w.record);
        if((int32_t)w.z<(int32_t)w.record) {
            P(table,MH_TABLE,w.table+18); P(face,MH_FACE,rd_u32(w.table)); P(table,MH_TABLE,w.table+4);
            W(nz,MH_NZ,rd_u16(w.face+2)); AW(nz,MH_NZ,164);
            W(z,MH_Z,rd_u16(indexed(w.scene+2,w.nz))); EL(z,MH_Z); CL(w.z,w.record);
            if((int32_t)w.z<(int32_t)w.record) goto done;
        }
        w=component_faces(w,frame,h,1); hit=!w.child_equal;
    }
done:
    L(nz,MH_NZ,hit); observe(h,MH_RESTORE_CURSORS,MH_VALUE,0,0);
    w.scene=saved_scene;w.face=saved_face;return w;
}
MotionState test_face_motion(MotionState w,gaddr frame,const MotionHooks *h) {
    uint32_t old; int hit=0; int64_t signed_sum;
    const gaddr saved_scene=w.scene,saved_face=w.face;
    observe(h,MH_SAVE_CURSORS,MH_VALUE,0,0); store_byte(h,frame-32,0); load_position(&w,w.scene,h);
    LSL(x,MH_X,8); LSL(y,MH_Y,8); LSL(z,MH_Z,8);
    P(point_x,MH_POINT_X,sign_word(w.x)); P(record,MH_RECORD,w.y); P(table,MH_TABLE,sign_word(w.z));
    P(root,MH_ROOT,0xc46184u); W(value,MH_VALUE,rd_u16(0xc459b8u)); ALW(value,MH_VALUE,8); AW(value,MH_VALUE,w.value); P(root,MH_ROOT,indexed(w.root,w.value));
    P(scene,MH_SCENE,0xc38b34u); B(value,MH_VALUE,rd_u8(w.root+123)); if((int8_t)w.value<0) goto done;
    B(x,MH_X,rd_u8(w.root+125)); ANDW(x,MH_X,15);
    w.value=low_byte(w.value,(uint8_t)w.value>>4); observe(h,MH_LSR_BYTE,MH_VALUE,4,0); ANDW(value,MH_VALUE,15);
    old=w.x; SW(x,MH_X,w.value); if((int16_t)old<(int16_t)w.value) L(x,MH_X,0);
    store_word(h,frame-62,(uint16_t)w.x);
    B(value,MH_VALUE,rd_u8(w.root+123)); ANDW(value,MH_VALUE,15); EW(value,MH_VALUE); AW(value,MH_VALUE,w.value); AW(value,MH_VALUE,w.value);
    P(scene,MH_SCENE,rd_u32(indexed(w.scene,w.value)));
    for(;;) {
        gaddr a=w.scene; load_normal(&w,a,h); P(scene,MH_SCENE,a+6);
        old=w.x; w.x&=~0x8000u; observe(h,MH_BIT_CLEAR,MH_X,old,15);
        if(old&0x8000u) { hit=1; break; }
        W(value,MH_VALUE,164); AW(x,MH_X,w.value); AW(y,MH_Y,w.value); AW(z,MH_Z,w.value);
        old=w.x; w.x&=~0x4000u; observe(h,MH_BIT_CLEAR,MH_X,old,14);
        P(face,MH_FACE,indexed(w.root,w.x));
        if(!(old&0x4000u)) {
            load_vector(&w,indexed(w.root,w.z),h);
            SW(nx,MH_NX,rd_u16(indexed(w.root,w.y))); SW(ny,MH_NY,rd_u16(indexed(w.root+2,w.y))); SW(nz,MH_NZ,rd_u16(indexed(w.root+4,w.y)));
        } else {
            W(value,MH_VALUE,w.z); load_normal(&w,indexed(w.root,w.y),h);
            SW(x,MH_X,rd_u16(w.face)); SW(y,MH_Y,rd_u16(w.face+2)); SW(z,MH_Z,rd_u16(w.face+4));
            load_vector(&w,indexed(w.root,w.value),h);
            SW(nx,MH_NX,rd_u16(w.face)); SW(ny,MH_NY,rd_u16(w.face+2)); SW(nz,MH_NZ,rd_u16(w.face+4));
            W(value,MH_VALUE,w.z); W(selector,MH_SELECTOR,w.nz);
            MUL(nz,MH_NZ,w.y); MUL(z,MH_Z,w.ny); SL(nz,MH_NZ,w.z); LSL(nz,MH_NZ,4);
            MUL(value,MH_VALUE,w.nx); MUL(selector,MH_SELECTOR,w.x); SL(value,MH_VALUE,w.selector); LSL(value,MH_VALUE,4);
            MUL(ny,MH_NY,w.x); MUL(nx,MH_NX,w.y); SL(ny,MH_NY,w.nx); LSL(ny,MH_NY,4);
            W(nx,MH_NX,w.nz); W(nz,MH_NZ,w.ny); W(ny,MH_NY,w.value);
        }
        load_normal(&w,w.face,h); W(value,MH_VALUE,rd_u16(frame-62));
        ASW(x,MH_X,w.value); ASW(y,MH_Y,w.value); ASW(z,MH_Z,w.value); EL(y,MH_Y);
        AW(x,MH_X,rd_u16(w.root+12)); AL(y,MH_Y,rd_u32(w.root+16)); AW(z,MH_Z,rd_u16(w.root+14));
        SW(x,MH_X,w.point_x); SL(y,MH_Y,w.record); SW(z,MH_Z,w.table); NEG(x,MH_X); NEG(y,MH_Y); NEG(z,MH_Z);
        MUL(nx,MH_NX,w.x); MUL(ny,MH_NY,w.y); MUL(nz,MH_NZ,w.z); AL(nz,MH_NZ,w.nx);
        /* BGE consumes N xor V from the last signed ADD, including overflow. */
        signed_sum=(int64_t)(int32_t)w.nz+(int32_t)w.ny; AL(nz,MH_NZ,w.ny);
        if(signed_sum<0) break;
    }
done:
    L(nz,MH_NZ,hit); observe(h,MH_RESTORE_CURSORS,MH_VALUE,0,0);
    w.scene=saved_scene;w.face=saved_face;return w;
}
