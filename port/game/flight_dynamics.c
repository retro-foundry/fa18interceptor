/* Complete indexed record dynamics, scene motion and region dispatch owners.
 * The original instructions and children, not a physics model, are authority. */
#include "flight_dynamics.h"
#include "flight_recorder.h"
#include "record_steering.h"
#include "fixed_math.h"
#include <stdlib.h>
void update_dynamics_record_matrix(const RecordMatrixInput *input, RecordMatrixResult *result,
                                   RecordMatrixSideHook side_hook, void *context) {
    update_record_matrix(input, result, side_hook, context);
}
void update_dynamics_record_input(gaddr record, uint32_t incoming) {
    update_flight_input(record, incoming);
}
void update_dynamics_selected_record(IndexedRecordWork *work) {
    update_indexed_record(work);
}
static void observe(const DynamicsHooks *h,enum DynamicsPhase p,enum DynamicsValue f,uint32_t v,uint32_t o) {
    if(h && h->observe) h->observe(h->context,p,f,v,o);
}
static DynamicsState consume(const DynamicsHooks *h,enum DynamicsChild child,DynamicsState work) {
    if(h && h->consume_values) return h->consume_values(h->context,child,work);
    if(h && h->consume) return h->consume(h->context,child); abort();
}
static DynamicsState restored(const DynamicsHooks *h,DynamicsState w) {
    return h && h->restored?h->restored(h->context):w;
}
static uint32_t low_word(uint32_t old,uint16_t v) { return (old&0xffff0000u)|v; }
static uint32_t low_byte(uint32_t old,uint8_t v) { return (old&0xffffff00u)|v; }
static uint32_t sign_word(uint32_t v) { return (uint32_t)(int32_t)(int16_t)v; }
static uint32_t swapped(uint32_t v) { return (v<<16)|(v>>16); }
static uint32_t asr_long(uint32_t v,unsigned n) { n&=63; return (uint32_t)((int32_t)v>>(n<32?n:31)); }
static uint16_t asr_word(uint32_t v,unsigned n) { n&=63; return (uint16_t)((int16_t)v>>(n<16?n:15)); }
static gaddr indexed(gaddr base,uint32_t offset) { return base+sign_word(offset); }
static void byte(const DynamicsHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,DY_STORE_BYTE,DY_PRIMARY,v,0); }
static void word(const DynamicsHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,DY_STORE_WORD,DY_PRIMARY,v,0); }
static void longword(const DynamicsHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,DY_STORE_LONG,DY_PRIMARY,v,0); }
static int test_byte(const DynamicsHooks *h,gaddr a) { uint8_t v=rd_u8(a); observe(h,DY_TEST_BYTE,DY_PRIMARY,v,0); return v!=0; }
static int test_word(const DynamicsHooks *h,gaddr a) { uint16_t v=rd_u16(a); observe(h,DY_TEST_WORD,DY_PRIMARY,v,0); return v!=0; }
static int bit(const DynamicsHooks *h,gaddr a,unsigned b) { uint8_t v=rd_u8(a); b&=7; observe(h,DY_BIT_TEST,DY_PRIMARY,v,b); return (v&(1u<<b))!=0; }
static int change_bit(const DynamicsHooks *h,gaddr a,unsigned b,int set) {
    uint8_t old=rd_u8(a); b&=7; wr_u8(a,set?(uint8_t)(old|(1u<<b)):(uint8_t)(old&~(1u<<b)));
    observe(h,set?DY_BIT_SET:DY_BIT_CLEAR,DY_PRIMARY,old,b); return (old&(1u<<b))!=0;
}
static void and_byte(const DynamicsHooks *h,gaddr a,uint8_t mask) { byte(h,a,rd_u8(a)&mask); }
static void and_word(const DynamicsHooks *h,gaddr a,uint16_t mask) { word(h,a,rd_u16(a)&mask); }
static void or_byte(const DynamicsHooks *h,gaddr a,uint8_t mask) { byte(h,a,rd_u8(a)|mask); }
static void or_word(const DynamicsHooks *h,gaddr a,uint16_t mask) { word(h,a,rd_u16(a)|mask); }
static void or_long(const DynamicsHooks *h,gaddr a,uint32_t mask) { longword(h,a,rd_u32(a)|mask); }
static void add_byte(const DynamicsHooks *h,gaddr a,uint8_t v) { uint8_t old=rd_u8(a); wr_u8(a,(uint8_t)(old+v)); observe(h,DY_MEMORY_ADD_BYTE,DY_PRIMARY,old,v); }
static void add_word(const DynamicsHooks *h,gaddr a,uint16_t v) { uint16_t old=rd_u16(a); wr_u16(a,(uint16_t)(old+v)); observe(h,DY_MEMORY_ADD_WORD,DY_PRIMARY,old,v); }
static void add_long(const DynamicsHooks *h,gaddr a,uint32_t v) { uint32_t old=rd_u32(a); wr_u32(a,old+v); observe(h,DY_MEMORY_ADD_LONG,DY_PRIMARY,old,v); }
static int64_t sub_word(const DynamicsHooks *h,gaddr a,uint16_t v) { uint16_t old=rd_u16(a); wr_u16(a,(uint16_t)(old-v)); observe(h,DY_MEMORY_SUB_WORD,DY_PRIMARY,old,v); return (int32_t)(int16_t)old-(int16_t)v; }
static void sub_long(const DynamicsHooks *h,gaddr a,uint32_t v) { uint32_t old=rd_u32(a); wr_u32(a,old-v); observe(h,DY_MEMORY_SUB_LONG,DY_PRIMARY,old,v); }
#define B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(v)); observe(h,DY_BYTE,id,w.f,0); } while(0)
#define W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(v)); observe(h,DY_WORD,id,w.f,0); } while(0)
#define L(f,id,v) do { w.f=(uint32_t)(v); observe(h,DY_LONG,id,w.f,0); } while(0)
#define P(f,id,v) do { w.f=(gaddr)(v); observe(h,DY_POINTER,id,w.f,0); } while(0)
#define AB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f+n_)); observe(h,DY_ADD_BYTE,id,n_,0); } while(0)
#define AW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f+n_)); observe(h,DY_ADD_WORD,id,n_,0); } while(0)
#define AL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f+=n_; observe(h,DY_ADD_LONG,id,n_,0); } while(0)
#define SB(f,id,v) do { uint8_t n_=(uint8_t)(v); w.f=low_byte(w.f,(uint8_t)(w.f-n_)); observe(h,DY_SUB_BYTE,id,n_,0); } while(0)
#define SW(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=low_word(w.f,(uint16_t)(w.f-n_)); observe(h,DY_SUB_WORD,id,n_,0); } while(0)
#define SL(f,id,v) do { uint32_t n_=(uint32_t)(v); w.f-=n_; observe(h,DY_SUB_LONG,id,n_,0); } while(0)
#define AND_B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(w.f&(v))); observe(h,DY_AND_BYTE,id,v,0); } while(0)
#define AND_W(f,id,v) do { w.f=low_word(w.f,(uint16_t)(w.f&(v))); observe(h,DY_AND_WORD,id,v,0); } while(0)
#define AND_L(f,id,v) do { w.f&=(v); observe(h,DY_AND_LONG,id,v,0); } while(0)
#define OR_B(f,id,v) do { w.f=low_byte(w.f,(uint8_t)(w.f|(v))); observe(h,DY_OR_BYTE,id,v,0); } while(0)
#define EW(f,id) do { w.f=low_word(w.f,(uint16_t)(int16_t)(int8_t)w.f); observe(h,DY_EXT_WORD,id,0,0); } while(0)
#define EL(f,id) do { w.f=sign_word(w.f); observe(h,DY_EXT_LONG,id,0,0); } while(0)
#define SWAP(f,id) do { w.f=swapped(w.f); observe(h,DY_SWAP,id,0,0); } while(0)
#define ASW(f,id,n) do { unsigned n_=(unsigned)(n); w.f=low_word(w.f,asr_word(w.f,n_)); observe(h,DY_ASR_WORD,id,n_,0); } while(0)
#define ASL(f,id,n) do { unsigned n_=(unsigned)(n); w.f=asr_long(w.f,n_); observe(h,DY_ASR_LONG,id,n_,0); } while(0)
#define ALW(f,id,n) do { w.f=low_word(w.f,(uint16_t)(w.f<<(n))); observe(h,DY_ASL_WORD,id,n,0); } while(0)
#define ALL(f,id,n) do { w.f<<=(n); observe(h,DY_ASL_LONG,id,n,0); } while(0)
#define LSW(f,id,n) do { w.f=low_word(w.f,(uint16_t)w.f>>(n)); observe(h,DY_LSR_WORD,id,n,0); } while(0)
#define ROL(f,id,n) do { w.f=(w.f<<(n))|(w.f>>(32-(n))); observe(h,DY_ROL_LONG,id,n,0); } while(0)
#define NEGW(f,id) do { w.f=low_word(w.f,(uint16_t)(0u-w.f)); observe(h,DY_NEG_WORD,id,0,0); } while(0)
#define NEGL(f,id) do { w.f=0u-w.f; observe(h,DY_NEG_LONG,id,0,0); } while(0)
#define MUL(f,id,v) do { uint16_t n_=(uint16_t)(v); w.f=(uint32_t)((int32_t)(int16_t)w.f*(int32_t)(int16_t)n_); observe(h,DY_MULTIPLY,id,n_,0); } while(0)
#define CB(a,b) observe(h,DY_COMPARE_BYTE,DY_PRIMARY,(uint8_t)(a),(uint8_t)(b))
#define CW(a,b) observe(h,DY_COMPARE_WORD,DY_PRIMARY,(uint16_t)(a),(uint16_t)(b))
#define CL(a,b) observe(h,DY_COMPARE_LONG,DY_PRIMARY,(uint32_t)(a),(uint32_t)(b))
static int decrement(uint32_t *v,enum DynamicsValue id,const DynamicsHooks *h) {
    *v=low_word(*v,(uint16_t)(*v-1)); observe(h,DY_DECREMENT,id,*v,0); return (uint16_t)*v!=0xffff;
}
static void load_words(DynamicsState *w,gaddr a,uint16_t mask,int advance,const DynamicsHooks *h) {
    uint32_t *values[]={&w->primary,&w->detail,&w->x,&w->y,&w->z,&w->rate_x,&w->rate_y,&w->rate_z};
    unsigned i; gaddr next=a;
    for(i=0;i<8;++i) if(mask&(1u<<i)) { *values[i]=sign_word(rd_u16(next)); next+=2; }
    observe(h,DY_LOAD_WORDS,DY_PRIMARY,a,mask);
    if(advance==2) w->geometry=next; else if(advance==4) w->table=next;
    if(advance>=0) observe(h,DY_POINTER,(enum DynamicsValue)(DY_ROOT+advance),next,0);
}
static void load_longs(DynamicsState *w,gaddr a,uint16_t mask,const DynamicsHooks *h) {
    uint32_t *values[]={&w->primary,&w->detail,&w->x,&w->y,&w->z,&w->rate_x,&w->rate_y,&w->rate_z}; unsigned i; gaddr next=a;
    for(i=0;i<8;++i) if(mask&(1u<<i)) { *values[i]=rd_u32(next); next+=4; } observe(h,DY_LOAD_LONGS,DY_PRIMARY,a,mask);
}
static void store_values(DynamicsState w,gaddr a,uint16_t mask,int wide,const DynamicsHooks *h) {
    uint32_t values[]={w.primary,w.detail,w.x,w.y,w.z,w.rate_x,w.rate_y,w.rate_z}; unsigned i; gaddr next=a;
    for(i=0;i<8;++i) if(mask&(1u<<i)) { if(wide) wr_u32(next,values[i]); else wr_u16(next,(uint16_t)values[i]); next+=wide?4:2; }
    observe(h,wide?DY_STORE_LONGS:DY_STORE_WORDS,DY_PRIMARY,a,mask);
}
static DynamicsState geometry_table(DynamicsState w,uint32_t offset,const DynamicsHooks *h) {
    P(table,DY_TABLE,0xc295e0u); P(table,DY_TABLE,indexed(w.table,rd_u16(indexed(w.table,offset)))); return w;
}
static DynamicsState restore_rates(DynamicsState w,DynamicsState saved,const DynamicsHooks *h) {
    observe(h,DY_RESTORE_RATES,DY_PRIMARY,0,0); w.rate_x=saved.rate_x; w.rate_y=saved.rate_y; w.rate_z=saved.rate_z; w.record=saved.record; return restored(h,w);
}
static DynamicsState publish_record_coordinates(DynamicsState w,const DynamicsHooks *h) {
    ASL(x,DY_X,8); ASL(y,DY_Y,8); ASL(z,DY_Z,8);
    word(h,w.record+12,(uint16_t)w.x); word(h,w.record+14,(uint16_t)w.z); longword(h,w.record+16,w.y); return w;
}
static DynamicsState record_cell(DynamicsState w,const DynamicsHooks *h) {
    SWAP(x,DY_X); ROL(x,DY_X,4); SWAP(z,DY_Z); ROL(z,DY_Z,4);
    SW(x,DY_X,3); NEGW(x,DY_X); SW(z,DY_Z,3); NEGW(z,DY_Z); AW(z,DY_Z,w.z); AW(z,DY_Z,w.z); AW(z,DY_Z,w.x); return w;
}
int advance_record_dynamics(RecordDynamicsFrame *f,const DynamicsHooks *h) {
#define DYNAMICS_WAIT(selected,next) do { f->work=w; f->saved=saved; f->saved_record=saved_record; f->saved_root=saved_root; f->child=selected; f->phase=next; return 0; } while(0)

    DynamicsState w=f->work,saved=f->saved; gaddr saved_record=f->saved_record,saved_root=f->saved_root; int64_t signed_value; uint8_t old_byte,code;
    switch(f->phase) {
    case DYNAMICS_BEGIN: break;
    case DYNAMICS_AFTER_CELL: goto resume_cell;
    case DYNAMICS_AFTER_ZONE: goto resume_zone;
    case DYNAMICS_AFTER_ACTION: goto resume_action;
    case DYNAMICS_AFTER_CONTROLS: goto resume_controls;
    case DYNAMICS_AFTER_DESCENT: goto resume_descent;
    case DYNAMICS_AFTER_ALERT: goto resume_alert;
    case DYNAMICS_AFTER_SELECTOR: goto resume_selector;
    case DYNAMICS_AFTER_MATRIX: goto resume_matrix;
    case DYNAMICS_AFTER_ROOT: goto resume_root;
    case DYNAMICS_AFTER_CANDIDATE: goto resume_candidate;
    case DYNAMICS_AFTER_SOUND: goto resume_sound;
    case DYNAMICS_AFTER_MESSAGE: goto resume_message;
    case DYNAMICS_AFTER_FAULT: goto resume_fault;
    case DYNAMICS_AFTER_PROJECTION: goto resume_projection;
    case DYNAMICS_AFTER_REGION: goto resume_region;
    case DYNAMICS_AFTER_SLOT: goto resume_slot;
    case DYNAMICS_AFTER_TIMER: goto resume_timer;
    case DYNAMICS_COMPLETE: return 1;
    default: abort();
    }
    L(primary,DY_PRIMARY,0);
    if(test_word(h,0xc459b4u)) {
        OR_B(primary,DY_PRIMARY,rd_u8(0xc45788u));
        if((uint8_t)w.primary) {
            if(!bit(h,w.record+1,6)) goto complete;
            load_words(&w,w.record+102,0xe0,-1,h); saved_record=w.record; saved_root=w.root; observe(h,DY_SAVE_CELL,DY_PRIMARY,0,0);
            DYNAMICS_WAIT(DY_CELL_MATRIX,DYNAMICS_AFTER_CELL);
resume_cell: observe(h,DY_RESTORE_CELL,DY_PRIMARY,0,0); w.record=saved_record; w.root=saved_root; w=restored(h,w);
            W(x,DY_X,rd_u16(w.record+12)); W(z,DY_Z,rd_u16(w.record+14)); EL(x,DY_X); EL(z,DY_Z); w=record_cell(w,h); byte(h,w.record+10,(uint8_t)w.z); goto complete;
        }
    }
    OR_B(primary,DY_PRIMARY,rd_u8(0xc457aeu)); if((uint8_t)w.primary) goto complete;
    if(test_word(h,0xc459b4u)) {
        W(primary,DY_PRIMARY,rd_u16(0xc458dau)); AND_W(primary,DY_PRIMARY,31); CW(w.primary,rd_u16(0xc459b4u));
        if((uint16_t)w.primary==rd_u16(0xc459b4u)) { DYNAMICS_WAIT(DY_SELECTED_RECORD,DYNAMICS_AFTER_ZONE); }
resume_zone:;
    } else if(!test_byte(h,0xc45785u)) goto control_gates;
    W(primary,DY_PRIMARY,rd_u16(w.record)); W(detail,DY_DETAIL,w.primary); AND_W(primary,DY_PRIMARY,0x400);
    if((uint16_t)w.primary) {
        uint16_t timer=rd_u16(w.record+76); observe(h,DY_TEST_WORD,DY_PRIMARY,timer,0);
        if((int16_t)timer>0) goto clear_expiry;
        and_word(h,w.record,0xfbff); B(x,DY_X,rd_u8(w.record+98)); CB(w.x,21); if((uint8_t)w.x==21) goto expire;
        AND_B(x,DY_X,240); CB(w.x,0); if(!(uint8_t)w.x) goto expire;
        CB(w.x,16); if((uint8_t)w.x==16 && !bit(h,w.record+1,3) && !bit(h,w.record+3,7)) add_byte(h,0xc458abu,1);
        if(test_word(h,0xc459b4u) && !bit(h,w.record+3,7) && !bit(h,w.record,7)) goto clear_expiry;
expire:
        and_word(h,w.record,0xffbf); word(h,w.record+76,20); change_bit(h,w.record,6,0); and_word(h,w.record,0xdfff); goto record_timer;
clear_expiry:
        and_word(h,w.record,0xdfff);
    }
    W(primary,DY_PRIMARY,w.detail); AND_W(detail,DY_DETAIL,0x1000); if((uint16_t)w.detail) goto control_gates;
    AND_W(primary,DY_PRIMARY,0x100); if(!(uint16_t)w.primary) goto zero_rates;
control_gates:
    if(bit(h,w.record+2,0) && !test_byte(h,w.record+5)) { change_bit(h,w.record+3,4,1); goto record_controls; }
    W(primary,DY_PRIMARY,rd_u16(0xc458ccu)); AND_W(primary,DY_PRIMARY,64); if(!(uint16_t)w.primary) goto matrix;
    if(test_word(h,0xc459b4u)) { DYNAMICS_WAIT(DY_RECORD_ACTION,DYNAMICS_AFTER_ACTION); }
resume_action:
record_controls:
    DYNAMICS_WAIT(DY_RECORD_CONTROLS,DYNAMICS_AFTER_CONTROLS);
resume_controls: if(test_word(h,0xc459b4u)) goto record_warning;
    L(primary,DY_PRIMARY,rd_u32(w.record+66));
    if((int32_t)w.primary<0) {
        NEGL(primary,DY_PRIMARY); if(bit(h,w.record+32,1)) goto action_warning;
        CW(rd_u16(w.record+102),0x3840); if(rd_s16(w.record+102)>0x3840) goto height_warning;
        CW(rd_u16(w.record+102),0x1e0); if(rd_s16(w.record+102)<0x1e0) goto height_warning;
        ALL(primary,DY_PRIMARY,4); CL(w.primary,rd_u32(w.record+24)); if((int32_t)w.primary<rd_s32(w.record+24)) goto height_warning;
warning_nine:
        if(bit(h,w.record+3,7)) goto record_warning;
        CW(rd_u16(0xc45ae0u),0x9c06); if(rd_u16(0xc45ae0u)==0x9c06) goto record_warning;
        or_long(h,0xc45b54u,128); W(primary,DY_PRIMARY,0x9c06); goto descent_alert;
    }
height_warning:
    CW(rd_u16(w.record+106),0x3840);
    if(rd_s16(w.record+106)>0x3840) { CW(rd_u16(w.record+106),0x6bd0); if(rd_s16(w.record+106)>0x6bd0) goto record_warning; }
    else { CW(rd_u16(w.record+106),0x4b0); if(rd_s16(w.record+106)<0x4b0) goto record_warning; }
    CL(rd_u32(w.record+24),0x1800); if(rd_s32(w.record+24)<=0x1800) goto warning_nine; goto record_warning;
action_warning:
    B(detail,DY_DETAIL,rd_u8(w.record+124)); AND_B(detail,DY_DETAIL,15); if((uint8_t)w.detail) goto record_warning;
    ALL(primary,DY_PRIMARY,6); CL(w.primary,rd_u32(w.record+24)); if((int32_t)w.primary<rd_s32(w.record+24)) goto record_warning;
    CW(rd_u16(0xc45ae0u),0xd02a); if(rd_u16(0xc45ae0u)==0xd02a) goto record_warning;
    or_long(h,0xc45b54u,64); W(primary,DY_PRIMARY,0xd02a);
descent_alert:
    DYNAMICS_WAIT(DY_DESCENT_ALERT,DYNAMICS_AFTER_DESCENT);
resume_descent: goto matrix;
record_warning:
    W(primary,DY_PRIMARY,rd_u16(0xc459b4u)); CW(w.primary,rd_u16(0xc458dcu));
    if((uint16_t)w.primary==rd_u16(0xc458dcu) && !test_byte(h,0xc45785u) && bit(h,w.record+3,0)) {
        CW(rd_u16(0xc45ae0u),0xd00a);
        if(rd_u16(0xc45ae0u)!=0xd00a) { W(primary,DY_PRIMARY,0xd00a); DYNAMICS_WAIT(DY_RECORD_ALERT,DYNAMICS_AFTER_ALERT);
resume_alert: or_long(h,0xc45b54u,128); }
    }
    if(!bit(h,w.record+32,1)) {
        B(primary,DY_PRIMARY,rd_u8(w.record+98)); AND_B(primary,DY_PRIMARY,240); CB(w.primary,16);
        if((uint8_t)w.primary==16 && bit(h,w.record,4)) {
            saved_record=w.record; observe(h,DY_SAVE_RECORD,DY_PRIMARY,0,0); DYNAMICS_WAIT(DY_RECORD_SELECTOR,DYNAMICS_AFTER_SELECTOR);
resume_selector:
            observe(h,DY_RESTORE_RECORD,DY_PRIMARY,0,0); w.record=saved_record; w=restored(h,w);
        }
    }
matrix:
    B(primary,DY_PRIMARY,rd_u8(w.record+98)); AND_B(primary,DY_PRIMARY,240); CB(w.primary,48);
    if((uint8_t)w.primary==48 || !bit(h,w.record,7)) {
        saved_record=w.record; observe(h,DY_SAVE_RECORD,DY_PRIMARY,0,0); DYNAMICS_WAIT(DY_RECORD_MATRIX,DYNAMICS_AFTER_MATRIX);
resume_matrix:
        observe(h,DY_RESTORE_RECORD,DY_PRIMARY,0,0); w.record=saved_record; w=restored(h,w);
    }
    if(!test_word(h,0xc459b4u)) goto root_flight;
    W(rate_x,DY_RATE_X,rd_u16(w.record+150)); W(rate_y,DY_RATE_Y,rd_u16(w.record+156)); W(rate_z,DY_RATE_Z,rd_u16(w.record+162)); ASW(rate_x,DY_RATE_X,2); ASW(rate_y,DY_RATE_Y,2); ASW(rate_z,DY_RATE_Z,2);
    W(primary,DY_PRIMARY,rd_u16(w.record)); AND_W(primary,DY_PRIMARY,256); if((uint16_t)w.primary) { and_word(h,w.record,0xfeff); goto zero_rates; }
    B(primary,DY_PRIMARY,rd_u8(w.record+98)); AND_B(primary,DY_PRIMARY,240); CB(w.primary,48); if((uint8_t)w.primary!=48) goto root_flight;
    L(z,DY_Z,rd_u32(w.record+24)); signed_value=(int64_t)(int32_t)w.z+rd_s32(w.record+66); AL(z,DY_Z,rd_u32(w.record+66));
    if(signed_value<=0 || !w.z) { change_bit(h,w.record,7,1); L(z,DY_Z,0); }
    longword(h,w.record+24,w.z); load_longs(&w,w.record+62,0xe0,h); if(bit(h,w.record,7)) goto record_timer; goto integrate;
zero_rates:
    L(rate_x,DY_RATE_X,0); L(rate_y,DY_RATE_Y,0); L(rate_z,DY_RATE_Z,0); goto publish;
root_flight:
    W(primary,DY_PRIMARY,rd_u16(0xc458ccu)); AND_W(primary,DY_PRIMARY,64);
    if(!(uint16_t)w.primary) { L(rate_x,DY_RATE_X,0); L(rate_y,DY_RATE_Y,0); L(rate_z,DY_RATE_Z,0); }
    else {
        saved_record=w.record; observe(h,DY_SAVE_RECORD,DY_PRIMARY,0,0); DYNAMICS_WAIT(DY_ROOT_FLIGHT,DYNAMICS_AFTER_ROOT);
resume_root:
        observe(h,DY_RESTORE_RECORD,DY_PRIMARY,0,0); w.record=saved_record; w=restored(h,w); load_longs(&w,w.record+62,0xe0,h);
    }
integrate:
    if(!bit(h,w.record,4)) { saved=w; observe(h,DY_SAVE_RATES,DY_PRIMARY,0,0); goto action_decay; }
    L(primary,DY_PRIMARY,0x1fffffffu); L(x,DY_X,rd_u32(w.record+20)); L(z,DY_Z,rd_u32(w.record+28));
    signed_value=(int64_t)(int32_t)w.x+(int32_t)w.rate_x; AL(x,DY_X,w.rate_x);
    if(signed_value<0) L(z,DY_Z,0); /* The source clears Z here, leaving the negative X value. */
    else { CL(w.x,w.primary); if((int32_t)w.x>(int32_t)w.primary) L(x,DY_X,w.primary); }
    signed_value=(int64_t)(int32_t)w.z+(int32_t)w.rate_z; AL(z,DY_Z,w.rate_z);
    if(signed_value<0) L(z,DY_Z,0); else { CL(w.z,w.primary); if((int32_t)w.z>(int32_t)w.primary) L(z,DY_Z,w.primary); }
    longword(h,w.record+20,w.x); longword(h,w.record+28,w.z); CB(rd_u8(w.record+98),21); if(rd_u8(w.record+98)==21) longword(h,w.record+24,0xb000);
publish:
    saved=w; observe(h,DY_SAVE_RATES,DY_PRIMARY,0,0); L(y,DY_Y,rd_u32(w.record+24)); L(rate_y,DY_RATE_Y,0);
    L(primary,DY_PRIMARY,rd_u32(w.record+20)); L(x,DY_X,w.primary); AND_L(x,DY_X,0x3fffff); L(detail,DY_DETAIL,rd_u32(w.record+28)); L(z,DY_Z,w.detail); AND_L(z,DY_Z,0x3fffff);
    SWAP(primary,DY_PRIMARY); ASW(primary,DY_PRIMARY,6); CW(w.primary,rd_u16(w.record+6)); if((uint16_t)w.primary!=rd_u16(w.record+6)) { word(h,w.record+6,(uint16_t)w.primary); L(rate_y,DY_RATE_Y,0xffffffffu); }
    SWAP(detail,DY_DETAIL); ASW(detail,DY_DETAIL,6); CW(w.detail,rd_u16(w.record+8)); if((uint16_t)w.detail!=rd_u16(w.record+8)) { word(h,w.record+8,(uint16_t)w.detail); L(rate_y,DY_RATE_Y,0xffffffffu); }
    observe(h,DY_TEST_BYTE,DY_PRIMARY,w.rate_y,0); if((uint8_t)w.rate_y) byte(h,0xc45858u,(uint8_t)w.rate_y);
    w=publish_record_coordinates(w,h); SWAP(x,DY_X); ROL(x,DY_X,4); SWAP(z,DY_Z); ROL(z,DY_Z,4); W(rate_x,DY_RATE_X,w.x); W(rate_z,DY_RATE_Z,w.z);
    SW(x,DY_X,3); NEGW(x,DY_X); SW(z,DY_Z,3); NEGW(z,DY_Z); AW(z,DY_Z,w.z); AW(z,DY_Z,w.z); AW(z,DY_Z,w.x);
    CB(w.z,rd_u8(w.record+10)); if((uint8_t)w.z==rd_u8(w.record+10)) goto action_decay; byte(h,w.record+10,(uint8_t)w.z);
    W(primary,DY_PRIMARY,rd_u16(w.record+6)); signed_value=(int16_t)w.primary-rd_s16(0xc4594cu); SW(primary,DY_PRIMARY,rd_u16(0xc4594cu)); if(signed_value<0) NEGW(primary,DY_PRIMARY);
    CW(w.primary,2); if((int16_t)w.primary>2) goto action_decay;
    W(primary,DY_PRIMARY,rd_u16(w.record+8)); signed_value=(int16_t)w.primary-rd_s16(0xc4594eu); SW(primary,DY_PRIMARY,rd_u16(0xc4594eu)); if(signed_value<0) NEGW(primary,DY_PRIMARY);
    CW(w.primary,2); if((int16_t)w.primary>2) goto action_decay;
    byte(h,0xc45858u,255); LSW(rate_x,DY_RATE_X,2); LSW(rate_z,DY_RATE_Z,2); SW(rate_x,DY_RATE_X,3); NEGW(rate_x,DY_RATE_X); SW(rate_z,DY_RATE_Z,3); NEGW(rate_z,DY_RATE_Z); AW(rate_z,DY_RATE_Z,w.rate_z); AW(rate_z,DY_RATE_Z,w.rate_z); AW(rate_z,DY_RATE_Z,w.rate_x); byte(h,w.record+11,(uint8_t)w.rate_z);
action_decay:
    B(primary,DY_PRIMARY,rd_u8(w.record+124)); B(detail,DY_DETAIL,w.primary); AND_B(detail,DY_DETAIL,240); AND_B(primary,DY_PRIMARY,15);
    if((uint8_t)w.primary) {
        CB(w.primary,6);
        if((int8_t)w.primary>6) { SB(primary,DY_PRIMARY,1); CB(w.primary,6); if((int8_t)w.primary>6) goto store_action; L(primary,DY_PRIMARY,0); }
        AB(primary,DY_PRIMARY,1); CB(w.primary,6);
        if((int8_t)w.primary>6) { if(!bit(h,w.record+3,5)) SB(primary,DY_PRIMARY,1); else { B(primary,DY_PRIMARY,0); and_word(h,w.record+2,0xffdf); } }
store_action:
        OR_B(primary,DY_PRIMARY,w.detail); byte(h,w.record+124,(uint8_t)w.primary);
    }
    B(primary,DY_PRIMARY,w.detail); AND_B(primary,DY_PRIMARY,15); observe(h,DY_TEST_BYTE,DY_PRIMARY,w.detail,0);
    if((int8_t)w.detail>0) SB(detail,DY_DETAIL,16);
    else if((int8_t)w.detail<0) {
        old_byte=(uint8_t)w.detail; w.detail&=~128u; observe(h,DY_REGISTER_BIT_CLEAR,DY_DETAIL,old_byte,7); CB(w.detail,96); if((int8_t)w.detail>=96) goto view_decay;
        AB(detail,DY_DETAIL,16); old_byte=(uint8_t)w.detail; w.detail|=128; observe(h,DY_REGISTER_BIT_SET,DY_DETAIL,old_byte,7);
    } else goto view_decay;
    OR_B(detail,DY_DETAIL,w.primary); byte(h,w.record+124,(uint8_t)w.detail); byte(h,0xc45845u,3);
view_decay:
    if(!test_word(h,0xc459b6u)) {
        B(detail,DY_DETAIL,rd_u8(0xc45847u));
        if((int8_t)w.detail>0) SB(detail,DY_DETAIL,1);
        else if((int8_t)w.detail<0) {
            old_byte=(uint8_t)w.detail; w.detail&=~128u; observe(h,DY_REGISTER_BIT_CLEAR,DY_DETAIL,old_byte,7); CB(w.detail,5); if((int8_t)w.detail>=5) goto candidate;
            AB(detail,DY_DETAIL,1); old_byte=(uint8_t)w.detail; w.detail|=128; observe(h,DY_REGISTER_BIT_SET,DY_DETAIL,old_byte,7);
        } else goto candidate;
        byte(h,0xc45847u,(uint8_t)w.detail);
    }
candidate:
    if(bit(h,w.record,2)) { w=restore_rates(w,saved,h); goto record_timer; }
    load_longs(&w,w.record+20,0x1c,h); observe(h,DY_LOAD_LONGS,DY_PRIMARY,0,0xe0); w.rate_x=saved.rate_x; w.rate_y=saved.rate_y; w.rate_z=saved.rate_z; w=restored(h,w);
    SL(x,DY_X,w.rate_x); SL(y,DY_Y,w.rate_y); SL(z,DY_Z,w.rate_z); DYNAMICS_WAIT(DY_MOTION_CANDIDATE,DYNAMICS_AFTER_CANDIDATE);
resume_candidate: w=restore_rates(w,saved,h);
    if(w.child_equal) goto no_collision;
    W(detail,DY_DETAIL,rd_u16(0xc459b4u)); observe(h,DY_BIT_TEST,DY_PRIMARY,w.primary,5);
    if(w.primary&32) { longword(h,w.record+86,0); word(h,w.record+90,0); }
    CW(w.primary,16); if((uint16_t)w.primary==16) goto collision_position; CW(w.primary,32); if((uint16_t)w.primary!=32) goto collision_position;
    B(x,DY_X,rd_u8(w.record+98)); AND_B(x,DY_X,240); CB(w.x,0); if(!(uint8_t)w.x) goto collision_position;
    B(x,DY_X,rd_u8(w.record+98)); AND_B(x,DY_X,240); CB(w.x,0);
    if(!(uint8_t)w.x) { L(x,DY_X,w.rate_x); L(y,DY_Y,w.rate_y); L(z,DY_Z,w.rate_z); ASL(x,DY_X,2); ASL(y,DY_Y,2); ASL(z,DY_Z,2); SL(rate_x,DY_RATE_X,w.x); SL(rate_y,DY_RATE_Y,w.y); SL(rate_z,DY_RATE_Z,w.z); }
    if(!bit(h,w.record+3,7)) sub_long(h,w.record+24,w.rate_y); sub_long(h,w.record+20,w.rate_x); sub_long(h,w.record+28,w.rate_z);
collision_position:
    load_longs(&w,w.record+20,0x1c,h); AND_L(x,DY_X,0x3fffff); AND_L(z,DY_Z,0x3fffff); w=publish_record_coordinates(w,h); W(x,DY_X,rd_u16(w.record+110));
    observe(h,DY_TEST_WORD,DY_PRIMARY,w.detail,0); if((uint16_t)w.detail) goto collision_class;
    B(y,DY_Y,rd_u8(w.record+124)); AND_B(y,DY_Y,15); if((uint8_t)w.y) goto collision_status;
    observe(h,DY_BIT_TEST,DY_PRIMARY,w.primary,4); if(w.primary&16) { change_bit(h,w.record+3,7,1); goto collision_speed; }
    if(!test_byte(h,0xc4578cu)) goto record_timer; CW(w.primary,32); if((int16_t)w.primary>32) goto collision_speed;
    if(change_bit(h,w.record,7,1)) goto collision_speed; CW(w.x,960); if((int16_t)w.x>960) goto collision_speed;
    observe(h,DY_BIT_TEST,DY_PRIMARY,w.primary,5); if(!(w.primary&32)) goto collision_speed;
    saved_record=w.record; observe(h,DY_SAVE_RECORD,DY_PRIMARY,0,0); observe(h,DY_SOUND_ARGUMENTS,DY_PRIMARY,0,0);
    DYNAMICS_WAIT(DY_COLLISION_SOUND,DYNAMICS_AFTER_SOUND);
resume_sound: observe(h,DY_RESTORE_RECORD,DY_PRIMARY,0,0); w.record=saved_record; w=restored(h,w);
collision_speed:
    change_bit(h,w.record,7,1); CW(w.x,960); if((int16_t)w.x<=960 || test_byte(h,0xc4589au)) goto record_timer;
    observe(h,DY_BIT_TEST,DY_PRIMARY,w.primary,6); if(!(w.primary&64)) goto collision_damage;
    if(!bit(h,0xc458dbu,0)) {
        B(detail,DY_DETAIL,rd_u8(0xc4584fu)); OR_B(detail,DY_DETAIL,rd_u8(0xc4584eu)); if((int8_t)w.detail>0) goto record_timer;
        change_bit(h,w.record+32,1,1); W(detail,DY_DETAIL,0x9014); or_word(h,0xc458ccu,256);
    } else { W(detail,DY_DETAIL,0xd00b); if(bit(h,0xc458dbu,1)) W(detail,DY_DETAIL,0xd00c); }
    SWAP(primary,DY_PRIMARY); W(primary,DY_PRIMARY,w.detail); DYNAMICS_WAIT(DY_COLLISION_MESSAGE,DYNAMICS_AFTER_MESSAGE);
resume_message: SWAP(primary,DY_PRIMARY); byte(h,0xc457c0u,1); goto record_timer;
collision_damage:
    or_word(h,w.record,0x200); observe(h,DY_BIT_TEST,DY_PRIMARY,w.primary,6);
    if(!(w.primary&64)) { P(root,DY_ROOT,rd_u32(0xc1ab74u)); add_word(h,w.root+70,1); byte(h,0xc457c5u,1); byte(h,0xc45798u,4); }
    if(!test_byte(h,0xc45785u)) goto record_timer;
collision_class:
    CW(w.primary,64); if((uint16_t)w.primary!=64) change_bit(h,w.record,7,1);
collision_status:
    if(bit(h,w.record,2)) { CW(rd_u16(w.record+76),15); if(rd_s16(w.record+76)<=15) goto record_timer; word(h,0xc4599eu,58); DYNAMICS_WAIT(DY_COLLISION_FAULT,DYNAMICS_AFTER_FAULT);
resume_fault: goto record_timer; }
    B(x,DY_X,rd_u8(w.record+98)); AND_B(x,DY_X,240); CB(w.x,48); if((uint8_t)w.x==48 && !bit(h,w.record,1)) goto record_timer;
    change_bit(h,w.record+32,1,1); CW(w.primary,64); if((uint16_t)w.primary!=64) and_word(h,w.record,0xefff);
    CB(w.x,0); if(!(uint8_t)w.x) goto ground_collision;
    CB(rd_u8(w.record+98),21); if(rd_u8(w.record+98)==21) goto mark_expiry;
    CW(w.primary,64); if((int16_t)w.primary<64) goto mark_expiry;
    word(h,0xc4fdd2u,rd_u16(0xc459b6u)); change_bit(h,w.record+2,4,1); change_bit(h,w.record+32,1,1); byte(h,w.record+5,1);
ground_collision:
    CW(w.primary,64); if((uint16_t)w.primary==64) { change_bit(h,w.record+1,6,0); byte(h,0xc45858u,12); goto complete; }
choose_slot:
    L(detail,DY_DETAIL,17); CW(w.primary,32); if((int16_t)w.primary>=32) goto motion_slot;
    DYNAMICS_WAIT(DY_GROUND_PROJECTION,DYNAMICS_AFTER_PROJECTION);
resume_projection: saved_record=w.record; observe(h,DY_SAVE_RECORD,DY_PRIMARY,0,0); DYNAMICS_WAIT(DY_REGION_PROBE,DYNAMICS_AFTER_REGION);
resume_region: observe(h,DY_RESTORE_RECORD,DY_PRIMARY,0,0); w.record=saved_record; w=restored(h,w);
    if(bit(h,w.record+4,1)) goto mark_expiry; L(detail,DY_DETAIL,0);
motion_slot:
    DYNAMICS_WAIT(DY_MOTION_SLOT,DYNAMICS_AFTER_SLOT);
resume_slot:
mark_expiry:
    or_word(h,w.record,0x400); word(h,w.record+76,15); change_bit(h,w.record,1,1); if(test_word(h,0xc459b4u)) change_bit(h,w.record,1,0); goto record_timer;
no_collision:
    if(bit(h,w.record+32,1) && bit(h,w.record+3,7) && !bit(h,w.record,2)) { and_word(h,w.record,0xefff); goto mark_expiry; }
    B(x,DY_X,rd_u8(w.record+98)); AND_B(x,DY_X,240); CB(w.x,48); if((uint8_t)w.x==48) goto record_timer;
    change_bit(h,w.record,7,0); CB(w.x,0);
    if(!(uint8_t)w.x) { L(primary,DY_PRIMARY,0); if(bit(h,w.record,2)) goto request_flag; if(bit(h,w.record+3,7)) goto choose_slot; }
    if(bit(h,w.record,1)) goto mark_expiry;
request_flag:
    if(test_byte(h,0xc4589fu)) { change_bit(h,w.record+3,1,1); goto record_timer; }
    if(!change_bit(h,w.record+3,1,0)) goto record_timer;
    B(primary,DY_PRIMARY,rd_u8(w.record+98)); AND_B(primary,DY_PRIMARY,240); CB(w.primary,0);
    if(!(uint8_t)w.primary) { change_bit(h,w.record+32,5,1); byte(h,0xc458b4u,0); }
record_timer:
    DYNAMICS_WAIT(DY_RECORD_TIMER,DYNAMICS_AFTER_TIMER);
resume_timer:

complete:
    f->work=w; f->phase=DYNAMICS_COMPLETE; return 1;
#undef DYNAMICS_WAIT
}
void advance_indexed_record_dynamics(DynamicsState w,const DynamicsHooks *h) {
    RecordDynamicsFrame frame={0}; frame.work=w; frame.phase=DYNAMICS_BEGIN;
    while(!advance_record_dynamics(&frame,h)) frame.work=consume(h,frame.child,frame.work);
}
static DynamicsState scene_reference_distance(DynamicsState w,gaddr frame,const DynamicsHooks *h) {
    int64_t difference;
    load_longs(&w,w.scene,0x43,h); ASL(primary,DY_PRIMARY,8);
    difference=(int16_t)w.primary-rd_s16(frame-26); SW(primary,DY_PRIMARY,rd_u16(frame-26)); if(difference<0) NEGW(primary,DY_PRIMARY);
    CW(w.primary,w.rate_z); if((int16_t)w.primary>=(int16_t)w.rate_z) { w.child_equal=0; return w; }
    ASL(detail,DY_DETAIL,8); difference=(int64_t)(int32_t)w.detail-rd_s32(frame-22); SL(detail,DY_DETAIL,rd_u32(frame-22)); if(difference<0) NEGL(detail,DY_DETAIL);
    CL(w.detail,w.rate_z); if((int32_t)w.detail>=(int32_t)w.rate_z) { w.child_equal=0; return w; }
    ASL(rate_y,DY_RATE_Y,8); difference=(int16_t)w.rate_y-rd_s16(frame-20); SW(rate_y,DY_RATE_Y,rd_u16(frame-20)); if(difference<0) NEGW(rate_y,DY_RATE_Y);
    CW(w.rate_y,w.rate_z); w.child_equal=(int16_t)w.rate_y<(int16_t)w.rate_z; return w;
}
static DynamicsState damp_scene_velocity(DynamicsState w,gaddr address,uint32_t gravity,int duplicate_x,const DynamicsHooks *h) {
    load_longs(&w,address,7,h); SL(detail,DY_DETAIL,gravity); L(z,DY_Z,6);
    if(!bit(h,w.scene+38,4)) {
        if(!bit(h,w.scene+38,5)) return w;
        AL(detail,DY_DETAIL,gravity); L(z,DY_Z,2); L(y,DY_Y,w.detail); ASL(y,DY_Y,w.z); SL(detail,DY_DETAIL,w.y);
    }
    L(y,DY_Y,w.primary); if(duplicate_x) L(y,DY_Y,w.primary);
    ASL(y,DY_Y,w.z); SL(primary,DY_PRIMARY,w.y); L(y,DY_Y,w.x); ASL(y,DY_Y,w.z); SL(x,DY_X,w.y); return w;
}
DynamicsState collide_scene_motion(DynamicsState w,const DynamicsHooks *h) {
    gaddr frame; int64_t difference,signed_sum; uint32_t saved_scan; int component;
    L(primary,DY_PRIMARY,0); B(detail,DY_DETAIL,rd_u8(0xc457bdu)); OR_B(detail,DY_DETAIL,rd_u8(0xc457aeu)); if((uint8_t)w.detail) return w;
    observe(h,DY_BEGIN_FRAME,DY_PRIMARY,92,0); if(!h || !h->frame) abort(); frame=h->frame(h->context);
    L(primary,DY_PRIMARY,rd_u32(frame+8)); ALW(primary,DY_PRIMARY,6); P(scene,DY_SCENE,0xc45c72u); P(scene,DY_SCENE,indexed(w.scene,w.primary));
    load_longs(&w,w.scene+12,7,h); ASL(primary,DY_PRIMARY,8); ASL(detail,DY_DETAIL,8); ASL(x,DY_X,8); store_values(w,0xc46178u,7,0,h);
    word(h,frame-18,rd_u16(0xc46180u)); load_words(&w,0xc46172u,7,-1,h); store_values(w,frame-16,7,0,h); word(h,0xc459b8u,0);
    P(root,DY_ROOT,0xc46184u); word(h,frame-26,rd_u16(w.root+12)); longword(h,frame-22,rd_u32(w.root+16)); word(h,frame-20,rd_u16(w.root+14));
    B(primary,DY_PRIMARY,rd_u8(0xc4585eu)); EW(primary,DY_PRIMARY); word(h,frame-6,(uint16_t)w.primary); longword(h,frame-10,0); L(rate_x,DY_RATE_X,0xffffffffu); P(face,DY_FACE,rd_u32(0xc459c6u));
scan_descriptor:
    word(h,frame-2,rd_u16(w.face)); P(face,DY_FACE,w.face-24); add_word(h,frame-8,1); if(bit(h,frame-1,6)) goto next_descriptor;
    if(!bit(h,frame-1,4)) goto map_descriptor;
    CW(rd_u16(frame-10),6); if(rd_s16(frame-10)>=6) goto next_descriptor; add_word(h,frame-10,1);
    B(primary,DY_PRIMARY,rd_u8(frame-2)); EW(primary,DY_PRIMARY); CW(w.primary,rd_u16(w.scene+46)); if((uint16_t)w.primary==rd_u16(w.scene+46)) goto next_descriptor;
    W(x,DY_X,w.primary); ALW(primary,DY_PRIMARY,8); AW(primary,DY_PRIMARY,w.primary); P(root,DY_ROOT,0xc46184u);
    W(detail,DY_DETAIL,rd_u16(indexed(w.root+6,w.primary))); CW(w.detail,rd_u16(w.scene+48)); if((uint16_t)w.detail!=rd_u16(w.scene+48)) goto next_descriptor;
    W(detail,DY_DETAIL,rd_u16(indexed(w.root+8,w.primary))); CW(w.detail,rd_u16(w.scene+50)); if((uint16_t)w.detail!=rd_u16(w.scene+50)) goto next_descriptor;
    P(record,DY_RECORD,0xc46184u); W(detail,DY_DETAIL,rd_u16(indexed(w.root,w.primary))); AND_W(detail,DY_DETAIL,64); if(!(uint16_t)w.detail) goto next_descriptor;
    word(h,0xc459b8u,(uint16_t)w.x); W(x,DY_X,rd_u16(indexed(w.root+12,w.primary))); L(y,DY_Y,rd_u32(indexed(w.root+16,w.primary))); W(z,DY_Z,rd_u16(indexed(w.root+14,w.primary)));
    P(root,DY_ROOT,rd_u32(w.face+26)); P(root,DY_ROOT,rd_u32(w.root+4)); W(rate_z,DY_RATE_Z,rd_u16(w.root)); if((int16_t)w.rate_z>=0) W(rate_z,DY_RATE_Z,rd_u16(w.root+4));
    AND_W(rate_z,DY_RATE_Z,0x3fff); B(rate_z,DY_RATE_Z,rd_u8(indexed(w.root+7,w.rate_z))); byte(h,frame-4,(uint8_t)w.rate_z); AND_B(rate_z,DY_RATE_Z,16);
    if((uint8_t)w.rate_z) W(rate_z,DY_RATE_Z,0x800); else W(rate_z,DY_RATE_Z,rd_u16(0xc459a6u)); EL(rate_z,DY_RATE_Z);
    load_longs(&w,w.scene,0x43,h); ASL(primary,DY_PRIMARY,8); difference=(int16_t)w.x-(int16_t)w.primary; SW(x,DY_X,w.primary); if(difference<0) NEGW(x,DY_X);
    CW(w.x,w.rate_z); if((int16_t)w.x>=(int16_t)w.rate_z) goto next_descriptor;
    ASL(detail,DY_DETAIL,8); difference=(int64_t)(int32_t)w.y-(int32_t)w.detail; SL(y,DY_Y,w.detail); if(difference<0) NEGL(y,DY_Y);
    CL(w.y,w.rate_z); if((int32_t)w.y>=(int32_t)w.rate_z) goto next_descriptor;
    ASL(rate_y,DY_RATE_Y,8); difference=(int16_t)w.z-(int16_t)w.rate_y; SW(z,DY_Z,w.rate_y); if(difference<0) NEGW(z,DY_Z);
    CW(w.z,w.rate_z); if((int16_t)w.z>=(int16_t)w.rate_z) goto next_descriptor; goto collision_candidate;
map_descriptor:
    CW(w.rate_x,10); if((int16_t)w.rate_x>=10) goto next_descriptor; AW(rate_x,DY_RATE_X,1);
    P(root,DY_ROOT,0xc4d790u); P(record,DY_RECORD,0xc4d7bcu); W(primary,DY_PRIMARY,w.rate_x); ALW(primary,DY_PRIMARY,2); L(detail,DY_DETAIL,rd_u32(indexed(w.root,w.primary))); if((int32_t)w.detail<0) goto next_descriptor;
    P(root,DY_ROOT,w.detail); ALW(primary,DY_PRIMARY,6); P(record,DY_RECORD,indexed(w.record,w.primary)); P(record,DY_RECORD,w.record+2); L(primary,DY_PRIMARY,0); W(primary,DY_PRIMARY,rd_u16(w.scene+48));
    B(x,DY_X,rd_u8(w.record)); P(record,DY_RECORD,w.record+1); EW(x,DY_X); SW(primary,DY_PRIMARY,w.x); W(x,DY_X,w.primary); if((int16_t)w.x<0) NEGW(x,DY_X);
    CW(w.x,1); if((int16_t)w.x>1) goto next_descriptor;
    W(detail,DY_DETAIL,rd_u16(w.scene+50)); B(x,DY_X,rd_u8(w.record)); P(record,DY_RECORD,w.record+1); EW(x,DY_X); SW(detail,DY_DETAIL,w.x); W(x,DY_X,w.detail); if((int16_t)w.x<0) NEGW(x,DY_X);
    CW(w.x,1); if((int16_t)w.x>1) goto next_descriptor;
    EW(primary,DY_PRIMARY); SWAP(primary,DY_PRIMARY); ASL(primary,DY_PRIMARY,2); EW(detail,DY_DETAIL); SWAP(detail,DY_DETAIL); ASL(detail,DY_DETAIL,2);
    longword(h,frame-56,w.primary); longword(h,frame-60,w.detail); load_longs(&w,w.scene,7,h); ASL(detail,DY_DETAIL,8); CL(w.detail,0x7fff); if((int32_t)w.detail>0x7fff) goto next_descriptor;
    CW(w.detail,rd_u16(w.record)); if((int16_t)w.detail>rd_s16(w.record)) goto next_descriptor;
    ASL(primary,DY_PRIMARY,8); AL(primary,DY_PRIMARY,rd_u32(frame-56)); W(rate_y,DY_RATE_Y,rd_u16(w.record+2)); EL(rate_y,DY_RATE_Y); CL(w.primary,w.rate_y); if((int32_t)w.primary<=(int32_t)w.rate_y) goto next_descriptor;
    W(rate_y,DY_RATE_Y,rd_u16(w.record+4)); EL(rate_y,DY_RATE_Y); CL(w.primary,w.rate_y); if((int32_t)w.primary>=(int32_t)w.rate_y) goto next_descriptor;
    ASL(x,DY_X,8); AL(x,DY_X,rd_u32(frame-60)); W(rate_y,DY_RATE_Y,rd_u16(w.record+6)); EL(rate_y,DY_RATE_Y); CL(w.x,w.rate_y); if((int32_t)w.x<=(int32_t)w.rate_y) goto next_descriptor;
    W(rate_y,DY_RATE_Y,rd_u16(w.record+8)); EL(rate_y,DY_RATE_Y); CL(w.x,w.rate_y); if((int32_t)w.x<(int32_t)w.rate_y) goto map_chain;
next_descriptor:
    W(primary,DY_PRIMARY,rd_u16(frame-6)); CW(w.primary,rd_u16(frame-8)); if((int16_t)w.primary<=rd_s16(frame-8)) goto fallback_motion;
    CW(w.rate_x,10); if((int16_t)w.rate_x>=10) goto fallback_motion; CW(rd_u16(frame-10),6); if(rd_s16(frame-10)<6) goto scan_descriptor;
fallback_motion:
    L(primary,DY_PRIMARY,rd_u32(frame+8)); CW(w.primary,20); if((int16_t)w.primary<20) goto ordinary_velocity;
    W(rate_z,DY_RATE_Z,rd_u16(0xc459a6u)); EL(rate_z,DY_RATE_Z); w=scene_reference_distance(w,frame,h); if(!w.child_equal) goto ordinary_velocity;
    L(rate_x,DY_RATE_X,8);
approach:
    w=damp_scene_velocity(w,w.scene+24,32,0,h); store_values(w,w.scene+24,7,1,h); add_long(h,w.scene,w.primary); add_long(h,w.scene+4,w.detail); add_long(h,w.scene+8,w.x);
    W(rate_z,DY_RATE_Z,rd_u16(0xc459a8u)); EL(rate_z,DY_RATE_Z); w=scene_reference_distance(w,frame,h); if(w.child_equal) goto hit;
    SB(rate_x,DY_RATE_X,1); if((int8_t)w.rate_x>0) goto approach; goto miss;
ordinary_velocity:
    w=damp_scene_velocity(w,w.scene+12,256,1,h); store_values(w,w.scene+12,7,1,h);
    if(!test_word(h,w.scene+36)) word(h,w.scene+36,1); else { add_long(h,w.scene,w.primary); add_long(h,w.scene+4,w.detail); add_long(h,w.scene+8,w.x); }
    goto miss;
map_chain:
    P(root,DY_ROOT,w.root+10); P(record,DY_RECORD,w.record+10);
chain_bounds:
    CW(w.detail,rd_u16(w.record)); if((int16_t)w.detail>rd_s16(w.record)) goto next_chain;
    W(rate_y,DY_RATE_Y,rd_u16(w.record+2)); EL(rate_y,DY_RATE_Y); CL(w.primary,w.rate_y); if((int32_t)w.primary<=(int32_t)w.rate_y) goto next_chain;
    W(rate_y,DY_RATE_Y,rd_u16(w.record+4)); EL(rate_y,DY_RATE_Y); CL(w.primary,w.rate_y); if((int32_t)w.primary>=(int32_t)w.rate_y) goto next_chain;
    W(rate_y,DY_RATE_Y,rd_u16(w.record+6)); EL(rate_y,DY_RATE_Y); CL(w.x,w.rate_y); if((int32_t)w.x<=(int32_t)w.rate_y) goto next_chain;
    W(rate_y,DY_RATE_Y,rd_u16(w.record+8)); EL(rate_y,DY_RATE_Y); CL(w.x,w.rate_y); if((int32_t)w.x<(int32_t)w.rate_y) goto collision_candidate;
next_chain:
    L(y,DY_Y,rd_u32(w.record+10)); if((int32_t)w.y<0) goto next_descriptor;
follow_chain:
    P(record,DY_RECORD,w.y); P(root,DY_ROOT,rd_u32(w.root+10)); goto chain_bounds;
collision_candidate:
    SWAP(rate_x,DY_RATE_X); if(!bit(h,frame-1,4)) goto convex_mesh;
    W(rate_x,DY_RATE_X,7);
component_retry:
    load_longs(&w,w.scene+24,7,h); add_long(h,w.scene,w.primary); add_long(h,w.scene+4,w.detail); add_long(h,w.scene+8,w.x);
    B(primary,DY_PRIMARY,rd_u8(frame-2)); EW(primary,DY_PRIMARY); word(h,0xc459b8u,(uint16_t)w.primary);
    saved_scan=w.rate_x; observe(h,DY_SAVE_SCAN,DY_PRIMARY,0,0); L(rate_z,DY_RATE_Z,0); B(primary,DY_PRIMARY,rd_u8(frame-4)); AND_B(primary,DY_PRIMARY,16); component=(uint8_t)w.primary!=0;
    w=consume(h,component?DY_COMPONENT_COLLISION:DY_FACE_COLLISION,w); observe(h,DY_RESTORE_SCAN,DY_PRIMARY,0,0); w.rate_x=saved_scan; w=restored(h,w);
    observe(h,DY_TEST_WORD,DY_PRIMARY,w.rate_z,0); if((uint16_t)w.rate_z) goto component_hit;
    if(decrement(&w.rate_x,DY_RATE_X,h)) goto component_retry;
    load_longs(&w,w.scene+12,7,h); sub_long(h,w.scene,w.primary); sub_long(h,w.scene+4,w.detail); sub_long(h,w.scene+8,w.x); goto next_descriptor;
component_hit:
    B(primary,DY_PRIMARY,rd_u8(frame-4)); AND_B(primary,DY_PRIMARY,16); if((uint8_t)w.primary) goto hit;
    P(record,DY_RECORD,rd_u32(0xc1ab74u)); add_word(h,w.record+60,1); W(primary,DY_PRIMARY,rd_u16(0xc459b8u)); ALW(primary,DY_PRIMARY,8); AW(primary,DY_PRIMARY,w.primary); W(rate_z,DY_RATE_Z,w.primary); SWAP(rate_z,DY_RATE_Z);
    P(record,DY_RECORD,0xc46184u); P(record,DY_RECORD,indexed(w.record,w.primary)); B(rate_z,DY_RATE_Z,rd_u8(w.record+60)); B(primary,DY_PRIMARY,w.rate_z); AND_B(rate_z,DY_RATE_Z,15); AB(rate_z,DY_RATE_Z,1); CB(w.rate_z,3);
    if((int8_t)w.rate_z<3) { AND_B(primary,DY_PRIMARY,240); OR_B(primary,DY_PRIMARY,w.rate_z); byte(h,w.record+60,(uint8_t)w.primary); goto hit; }
    or_word(h,w.record,0x200); B(primary,DY_PRIMARY,rd_u8(w.record+98)); CB(w.primary,21); if((uint8_t)w.primary==21) goto hit;
    AND_B(primary,DY_PRIMARY,240); CB(w.primary,0); if(!(uint8_t)w.primary) goto hit;
    change_bit(h,w.record+32,1,1); SWAP(rate_z,DY_RATE_Z); word(h,0xc4fdd2u,(uint16_t)w.rate_z); SWAP(rate_z,DY_RATE_Z); change_bit(h,w.record+2,4,1); byte(h,w.record+5,1); goto hit;
convex_mesh:
    word(h,frame-28,8); P(geometry,DY_GEOMETRY,w.root); P(table,DY_TABLE,w.record);
mesh_retry:
    load_longs(&w,w.scene+24,7,h); add_long(h,w.scene,w.primary); add_long(h,w.scene+4,w.detail); add_long(h,w.scene+8,w.x);
    P(root,DY_ROOT,w.geometry+14); P(record,DY_RECORD,w.table+14); word(h,frame-30,rd_u16(w.root)); P(root,DY_ROOT,w.root+2);
    load_longs(&w,w.scene,7,h); ASL(primary,DY_PRIMARY,8); ASL(detail,DY_DETAIL,8); ASL(x,DY_X,8); AL(primary,DY_PRIMARY,rd_u32(frame-56)); AL(x,DY_X,rd_u32(frame-60));
plane:
    P(root,DY_ROOT,w.root+6); L(y,DY_Y,w.primary); L(z,DY_Z,w.detail); L(rate_x,DY_RATE_X,w.x);
    W(rate_y,DY_RATE_Y,rd_u16(w.record)); P(record,DY_RECORD,w.record+2); EL(rate_y,DY_RATE_Y); SL(y,DY_Y,w.rate_y);
    SW(z,DY_Z,rd_u16(w.record)); P(record,DY_RECORD,w.record+2); W(rate_y,DY_RATE_Y,rd_u16(w.record)); P(record,DY_RECORD,w.record+2); EL(rate_y,DY_RATE_Y); SL(rate_x,DY_RATE_X,w.rate_y);
    load_words(&w,w.root,0xc0,-1,h); P(root,DY_ROOT,w.root+4); MUL(rate_y,DY_RATE_Y,w.y); MUL(rate_z,DY_RATE_Z,w.z); AL(rate_z,DY_RATE_Z,w.rate_y);
    W(rate_y,DY_RATE_Y,rd_u16(w.root)); P(root,DY_ROOT,w.root+2); MUL(rate_y,DY_RATE_Y,w.rate_x);
    signed_sum=(int64_t)(int32_t)w.rate_z+(int32_t)w.rate_y; AL(rate_z,DY_RATE_Z,w.rate_y);
    if(signed_sum<0) { difference=sub_word(h,frame-30,1); if(difference>0 && rd_u16(frame-30)) goto plane; goto hit; }
    difference=sub_word(h,frame-28,1); if(difference>0 && rd_u16(frame-28)) goto mesh_retry;
    P(record,DY_RECORD,w.table); L(y,DY_Y,rd_u32(w.record+10)); if((int32_t)w.y<=0) goto miss;
    P(root,DY_ROOT,w.geometry); load_longs(&w,w.scene,7,h); load_longs(&w,w.scene+12,0x70,h); SL(primary,DY_PRIMARY,w.z); SL(detail,DY_DETAIL,w.rate_x); SL(x,DY_X,w.rate_y);
    store_values(w,w.scene,7,1,h); ASL(primary,DY_PRIMARY,8); ASL(detail,DY_DETAIL,8); ASL(x,DY_X,8); goto follow_chain;
hit:
    L(primary,DY_PRIMARY,1); observe(h,DY_END_FRAME,DY_PRIMARY,0,0); return w;
miss:
    L(primary,DY_PRIMARY,0); observe(h,DY_END_FRAME,DY_PRIMARY,0,0); return w;
}
void update_scene_regions(DynamicsState w,const DynamicsHooks *h) {
    int occupied; uint8_t code;
    if(test_byte(h,0xc45790u)) return;
    P(scene,DY_SCENE,0xc29720u); L(rate_z,DY_RATE_Z,0);
    for(;;) {
        uint16_t marker=rd_u16(w.scene); observe(h,DY_TEST_WORD,DY_PRIMARY,marker,0); if((int16_t)marker<0) return;
        load_words(&w,0xc4618au,0x60,-1,h); occupied=bit(h,0xc4579du,w.rate_z);
        P(geometry,DY_GEOMETRY,rd_u32(w.scene)); load_words(&w,w.geometry,0x1e,2,h);
        CW(w.rate_x,w.detail); if((int16_t)w.rate_x<(int16_t)w.detail) goto outside;
        CW(w.rate_x,w.x); if((int16_t)w.rate_x>(int16_t)w.x) goto outside;
        CW(w.rate_y,w.y); if((int16_t)w.rate_y<(int16_t)w.y) goto outside;
        CW(w.rate_y,w.z); if((int16_t)w.rate_y>(int16_t)w.z) goto outside;
        if(!occupied) { change_bit(h,0xc4579du,w.rate_z,1); w=consume(h,DY_REGION_ENTER,w); }
        else w=consume(h,DY_REGION_ACTIVE,w);
        goto next_region;
outside:
        if(!occupied) goto next_region;
        change_bit(h,0xc4579du,w.rate_z,0); W(primary,DY_PRIMARY,rd_u16(w.geometry)); P(geometry,DY_GEOMETRY,w.geometry+2);
        { int32_t count=(int16_t)w.primary-1; SW(primary,DY_PRIMARY,1); if(count<0) goto next_region; }
        do {
            P(root,DY_ROOT,0xc46184u); P(geometry,DY_GEOMETRY,w.geometry+4);
            W(detail,DY_DETAIL,rd_u16(w.geometry)); P(geometry,DY_GEOMETRY,w.geometry+2);
            W(x,DY_X,rd_u16(w.geometry)); P(geometry,DY_GEOMETRY,w.geometry+4);
            w=geometry_table(w,w.x,h); load_words(&w,w.table,0x3c,4,h);
            W(rate_y,DY_RATE_Y,w.detail); AND_W(detail,DY_DETAIL,127); ALW(detail,DY_DETAIL,8); AW(detail,DY_DETAIL,w.detail); P(root,DY_ROOT,indexed(w.root,w.detail));
            B(detail,DY_DETAIL,rd_u8(w.root+56));
            if((int8_t)w.detail<0) { AND_B(detail,DY_DETAIL,127); if((uint8_t)w.detail) goto next_row; }
            W(detail,DY_DETAIL,w.rate_y); AND_W(detail,DY_DETAIL,0xff00);
            if(!(uint16_t)w.detail) goto release;
            observe(h,DY_TEST_WORD,DY_PRIMARY,w.rate_y,0);
            if((int16_t)w.rate_y<0) {
                AND_W(rate_y,DY_RATE_Y,0x7f00); if(!(uint16_t)w.rate_y) goto next_row;
                ASW(rate_y,DY_RATE_Y,7); w=geometry_table(w,w.rate_y,h); load_words(&w,w.table,0x7c,4,h);
                w=consume(h,DY_REGION_REPLACE,w); byte(h,w.root+56,255); and_word(h,w.root,0xfffe); byte(h,w.root+122,3); goto next_row;
            }
            P(record,DY_RECORD,0xc46184u); AND_W(rate_y,DY_RATE_Y,0xff00); W(rate_x,DY_RATE_X,w.rate_y); AW(rate_y,DY_RATE_Y,w.rate_y); P(record,DY_RECORD,indexed(w.record,w.rate_y));
            W(detail,DY_DETAIL,rd_u16(w.record)); AND_W(detail,DY_DETAIL,64); if(!(uint16_t)w.detail) goto release;
            word(h,w.root+44,rd_u16(w.record+6)); word(h,w.root+46,rd_u16(w.record+8)); word(h,w.root+48,rd_u16(w.record+12)); word(h,w.root+50,rd_u16(w.record+14)); longword(h,w.root+52,rd_u32(w.record+16));
            and_word(h,w.root,0xfffe); LSW(rate_x,DY_RATE_X,8); OR_B(rate_x,DY_RATE_X,128); byte(h,w.root+56,(uint8_t)w.rate_x); byte(h,w.root+122,3); goto next_row;
release:
            code=rd_u8(w.root+122); CB(code,3); if(code!=3) { CB(code,4); if(code!=4) goto release_position; }
            byte(h,w.root+122,5);
release_position:
            W(rate_y,DY_RATE_Y,rd_u16(w.table)); P(table,DY_TABLE,w.table+2); EL(rate_y,DY_RATE_Y);
            w=consume(h,DY_REGION_RELEASE,w); byte(h,w.root+56,128); and_word(h,w.root,0xfffe);
next_row:;
        } while(decrement(&w.primary,DY_PRIMARY,h));
next_region:
        P(scene,DY_SCENE,w.scene+4); AW(rate_z,DY_RATE_Z,1); CW(w.rate_z,8); if((int16_t)w.rate_z>=8) return;
    }
}
DynamicsState spawn_region_records(DynamicsState w,const DynamicsHooks *h) {
    B(primary,DY_PRIMARY,rd_u8(0xc458a6u)); CB(w.primary,2); if((int8_t)w.primary<=2) return w;
    CB(w.primary,125); if((uint8_t)w.primary==125) return w;
    P(geometry,DY_GEOMETRY,rd_u32(w.scene)); P(geometry,DY_GEOMETRY,w.geometry+8); W(primary,DY_PRIMARY,rd_u16(w.geometry)); P(geometry,DY_GEOMETRY,w.geometry+2);
    { int32_t count=(int16_t)w.primary-1; SW(primary,DY_PRIMARY,1); if(count<0) { L(rate_z,DY_RATE_Z,0xffffffffu); return w; } }
    return dispatch_region_records(w,h);
}
DynamicsState dispatch_region_records(DynamicsState w,const DynamicsHooks *h) {
    gaddr saved_root,saved_geometry; uint32_t saved_count;
    do {
        P(record,DY_RECORD,0xc22048u); P(record,DY_RECORD,indexed(w.record,rd_u16(w.geometry))); P(geometry,DY_GEOMETRY,w.geometry+2);
        SWAP(primary,DY_PRIMARY); W(primary,DY_PRIMARY,rd_u16(w.geometry)); P(geometry,DY_GEOMETRY,w.geometry+2);
        W(y,DY_Y,rd_u16(w.geometry+4)); AND_W(y,DY_Y,0xf00);
        if((uint16_t)w.y) { LSW(y,DY_Y,8); CB(w.y,rd_u8(0xc458a6u)); if((uint8_t)w.y!=rd_u8(0xc458a6u)) goto skipped; }
        W(y,DY_Y,rd_u16(w.geometry)); AND_W(y,DY_Y,127); W(detail,DY_DETAIL,w.y); P(table,DY_TABLE,0xc46184u); ALW(y,DY_Y,8); AW(y,DY_Y,w.y); P(table,DY_TABLE,indexed(w.table,w.y));
        if(bit(h,w.table+1,6)) goto skipped;
        W(y,DY_Y,w.detail); P(root,DY_ROOT,0xc22188u); AW(y,DY_Y,w.y); AW(y,DY_Y,w.y); W(z,DY_Z,w.y); AW(y,DY_Y,w.y); AW(y,DY_Y,w.y); AW(y,DY_Y,w.z); P(root,DY_ROOT,indexed(w.root,w.y));
        load_longs(&w,w.record,0x7c,h); store_values(w,w.root,0x7c,1,h); P(record,DY_RECORD,rd_u32(w.root+4)); W(x,DY_X,rd_u16(w.record));
        if((int16_t)w.x>=0) { AND_W(x,DY_X,0x4000); W(x,DY_X,rd_u16(w.record+((uint16_t)w.x?2:4))); }
        AND_W(x,DY_X,0xfff); B(z,DY_Z,rd_u8(indexed(w.record+6,w.x))); AND_B(z,DY_Z,15);
        P(root,DY_ROOT,0xc46184u); W(y,DY_Y,w.detail); ALW(y,DY_Y,8); AW(y,DY_Y,w.y); P(root,DY_ROOT,indexed(w.root,w.y));
        B(y,DY_Y,rd_u8(0xc458aau)); CB(w.y,rd_u8(0xc458a9u)); if((int8_t)w.y<=rd_s8(0xc458a9u)) goto skipped;
        CB(rd_u8(w.root+98),21); if(rd_u8(w.root+98)==21 && test_word(h,w.root+6)) goto skipped;
        if(bit(h,w.root+1,6)) goto skipped;
        P(root,DY_ROOT,w.table); L(y,DY_Y,40); P(record,DY_RECORD,w.root);
        do { longword(h,w.record,0); P(record,DY_RECORD,w.record+4); } while(decrement(&w.y,DY_Y,h));
        B(x,DY_X,rd_u8(w.root+125)); AND_B(x,DY_X,240); OR_B(x,DY_X,w.z); byte(h,w.root+125,(uint8_t)w.x);
        byte(h,w.root+98,(uint8_t)w.primary); SWAP(primary,DY_PRIMARY); byte(h,w.root+94,(uint8_t)w.detail);
        W(detail,DY_DETAIL,rd_u16(w.geometry)); P(geometry,DY_GEOMETRY,w.geometry+2); W(x,DY_X,w.detail); AND_W(x,DY_X,0x7f00); LSW(x,DY_X,8); byte(h,w.root+58,(uint8_t)w.x);
        W(x,DY_X,rd_u16(w.geometry)); P(geometry,DY_GEOMETRY,w.geometry+2); w=geometry_table(w,w.x,h);
        W(x,DY_X,rd_u16(w.geometry)); P(geometry,DY_GEOMETRY,w.geometry+2); W(y,DY_Y,w.x); AND_W(x,DY_X,0x8000);
        if((uint16_t)w.x) change_bit(h,w.root+1,3,1);
        else { change_bit(h,w.root+1,3,0); B(x,DY_X,rd_u8(w.root+98)); AND_B(x,DY_X,240); CB(w.x,16); if((uint8_t)w.x==16) add_byte(h,0xc458a9u,1); }
        W(x,DY_X,w.y); AND_W(y,DY_Y,0x4000); if((uint16_t)w.y) byte(h,w.root+5,8);
        SWAP(detail,DY_DETAIL); W(detail,DY_DETAIL,w.x); AND_W(x,DY_X,0x2000); if((uint16_t)w.x) change_bit(h,w.root+1,0,1);
        load_words(&w,w.table,0x7c,4,h); SWAP(primary,DY_PRIMARY); W(primary,DY_PRIMARY,w.detail); AND_W(detail,DY_DETAIL,0x1000);
        if((uint16_t)w.detail) {
            if(test_byte(h,0xc4582bu)) {
                SWAP(z,DY_Z); W(z,DY_Z,w.primary); W(primary,DY_PRIMARY,rd_u16(0xc45b18u)); W(detail,DY_DETAIL,rd_u16(0xc45b1au)); AND_W(z,DY_Z,128);
                if((uint16_t)w.z) { ASW(primary,DY_PRIMARY,1); ASW(detail,DY_DETAIL,1); }
                AW(x,DY_X,w.primary); AW(y,DY_Y,w.detail); SWAP(z,DY_Z);
            } else {
                W(detail,DY_DETAIL,rd_u16(0xc45af8u)); LSW(detail,DY_DETAIL,1); AND_W(detail,DY_DETAIL,3); if(bit(h,0xc45af9u,4)) NEGW(detail,DY_DETAIL);
                word(h,0xc45b18u,(uint16_t)w.detail); AND_W(primary,DY_PRIMARY,128); if((uint16_t)w.primary) ASW(detail,DY_DETAIL,1); AW(x,DY_X,w.detail);
                W(detail,DY_DETAIL,rd_u16(0xc45af8u)); LSW(detail,DY_DETAIL,2); AND_W(detail,DY_DETAIL,3); if(bit(h,0xc45af9u,5)) NEGW(detail,DY_DETAIL);
                word(h,0xc45b1au,(uint16_t)w.detail); observe(h,DY_TEST_WORD,DY_PRIMARY,w.primary,0); if((uint16_t)w.primary) ASW(detail,DY_DETAIL,1); AW(y,DY_Y,w.detail);
            }
        }
        SWAP(detail,DY_DETAIL); SWAP(primary,DY_PRIMARY); AB(rate_z,DY_RATE_Z,1); byte(h,w.root+93,(uint8_t)w.rate_z); byte(h,w.root+122,3);
        B(rate_z,DY_RATE_Z,rd_u8(w.root+98)); AND_B(rate_z,DY_RATE_Z,240); CB(w.rate_z,32); if((uint8_t)w.rate_z!=32) or_word(h,w.root,0x1080);
        observe(h,DY_TEST_WORD,DY_PRIMARY,w.detail,0);
        if((int16_t)w.detail<0) {
            AND_W(detail,DY_DETAIL,0x7f00);
            if((uint16_t)w.detail) {
                ASW(detail,DY_DETAIL,7); w=geometry_table(w,w.detail,h);
                word(h,w.root+44,rd_u16(w.table)); P(table,DY_TABLE,w.table+2); word(h,w.root+46,rd_u16(w.table)); P(table,DY_TABLE,w.table+2);
                word(h,w.root+48,rd_u16(w.table)); P(table,DY_TABLE,w.table+2); word(h,w.root+50,rd_u16(w.table)); P(table,DY_TABLE,w.table+2);
                W(detail,DY_DETAIL,rd_u16(w.table)); EL(detail,DY_DETAIL); longword(h,w.root+52,w.detail);
            }
            B(detail,DY_DETAIL,255);
        } else { w=consume(h,DY_PLACE_RECORD,w); LSW(detail,DY_DETAIL,8); OR_B(detail,DY_DETAIL,128); }
        byte(h,w.root+56,(uint8_t)w.detail); word(h,w.root+6,(uint16_t)w.x); word(h,w.root+8,(uint16_t)w.y); word(h,w.root+12,(uint16_t)w.z); word(h,w.root+14,(uint16_t)w.rate_x); longword(h,w.root+16,w.rate_y);
        EL(z,DY_Z); EL(rate_x,DY_RATE_X); ALL(z,DY_Z,8); ALL(rate_y,DY_RATE_Y,8); ALL(rate_x,DY_RATE_X,8);
        observe(h,DY_TEST_LONG,DY_PRIMARY,w.rate_y,0);
        if(w.rate_y) { change_bit(h,w.root+124,7,1); or_byte(h,w.root+124,96); word(h,w.root+108,0x2000); word(h,w.root+110,0x2000); }
        or_word(h,w.root,0x140); and_word(h,w.root,0x7fff); byte(h,w.root+95,68); word(h,w.root+96,500); longword(h,w.root+114,0x61a800);
        byte(h,w.root+99,61); byte(h,w.root+113,255); byte(h,w.root+100,6); byte(h,w.root+100,16);
        EL(x,DY_X); EL(y,DY_Y); SWAP(x,DY_X); SWAP(y,DY_Y); ALL(x,DY_X,6); ALL(y,DY_Y,6); AL(x,DY_X,w.z); AL(y,DY_Y,w.rate_x);
        longword(h,w.root+20,w.x); longword(h,w.root+24,w.rate_y); longword(h,w.root+28,w.y);
        L(z,DY_Z,0); L(rate_x,DY_RATE_X,0); L(rate_y,DY_RATE_Y,0); P(record,DY_RECORD,w.root);
        saved_count=w.primary; saved_root=w.root; saved_geometry=w.geometry; observe(h,DY_SAVE_ORIENTATION,DY_PRIMARY,0,0);
        w=consume(h,DY_ORIENT_RECORD,w); observe(h,DY_RESTORE_ORIENTATION,DY_PRIMARY,0,0); w.primary=saved_count; w.root=saved_root; w.geometry=saved_geometry; w=restored(h,w);
        L(rate_z,DY_RATE_Z,0); goto next;
skipped:
        P(geometry,DY_GEOMETRY,w.geometry+6); SWAP(primary,DY_PRIMARY); L(rate_z,DY_RATE_Z,0xffffffffu);
next:;
    } while(decrement(&w.primary,DY_PRIMARY,h));
    return w;
}

/* Original record autopilot. The action byte names maneuver phases; targets,
 * response tables and the forty-arm dispatch are original game data. Limits
 * live in this C frame across child calls rather than in a guest local frame. */
static DynamicsState autopilot_table_term(DynamicsState w,gaddr table,int term,
                                          const DynamicsHooks *h) {
    P(root,DY_ROOT,table); L(y,DY_Y,term);
    W(y,DY_Y,rd_u16(indexed(w.root,w.y))); return w;
}
static void autopilot_limits(AutopilotFrame *f,const DynamicsHooks *h) {
    const int offsets[]={-4,-8,-12,-20,-16,-24,-28,-32};
    const int32_t values[]={f->positive_roll,f->positive_pitch,f->hysteresis,
        f->negative_roll,f->negative_pitch,f->negative_hysteresis,
        f->direction_limit,f->steep_limit};
    unsigned i;
    for(i=0;i<8;++i) observe(h,DY_AUTOPILOT_LIMIT,DY_PRIMARY,(uint32_t)offsets[i],(uint32_t)values[i]);
}
int advance_record_autopilot(AutopilotFrame *f,const DynamicsHooks *h) {
    DynamicsState w=f->work;
    int64_t difference;
    uint32_t old; unsigned shift;
    static const gaddr action_targets[40]={
        0xc2c48a,0xc2c18e,0xc2c204,0xc2c208,0xc2c20c,0xc2c310,0xc2c1e6,0xc2c48a,
        0xc2c216,0xc2c28c,0xc2bd5a,0xc2bd72,0xc2bd88,0xc2bdd8,0xc2bdf0,0xc2be06,
        0xc2be50,0xc2be74,0xc2be8a,0xc2bedc,0xc2beee,0xc2bf04,0xc2bf42,0xc2bf5a,
        0xc2bf70,0xc2bfca,0xc2bfe2,0xc2bff8,0xc2c03e,0xc2c050,0xc2c066,0xc2c0d0,
        0xc2c0e2,0xc2c0f8,0xc2bcc8,0xc2bcf8,0xc2bd0c,0xc2bd4a,0xc2c16c,0xc2c178
    };
#define AP_WAIT(next) do { f->work=w; f->phase=(next); return 0; } while(0)
    switch(f->phase) {
    case AP_BEGIN: break;
    case AP_AFTER_FAULT: goto start_level;
    case AP_AFTER_NORMALIZE: goto choose_limits;
    case AP_AFTER_TURN: goto publish_turn;
    case AP_AFTER_PITCH: goto publish_pitch;
    case AP_AFTER_SIMPLE_ROLL: goto toggle_phase;
    case AP_AFTER_WAIT_PITCH: goto toggle_phase;
    case AP_AFTER_RATE_ROLL: goto toggle_phase;
    case AP_AFTER_RATE_NEUTRAL: goto toggle_phase;
    case AP_AFTER_LOOP_PITCH: goto toggle_phase;
    case AP_AFTER_LOOP_LEVEL: goto clear_loop_stick;
    case AP_AFTER_BANK_ROLL: goto toggle_phase;
    case AP_AFTER_PITCH_ARC: goto toggle_phase;
    case AP_AFTER_LEVEL_ROLL: goto toggle_phase;
    case AP_AFTER_REVERSE_PITCH: goto toggle_phase;
    case AP_AFTER_COMBINED_ROLL: goto combined_pitch;
    case AP_AFTER_COMBINED_PITCH: goto toggle_phase;
    case AP_AFTER_REVERSE_ROLL: goto toggle_phase;
    case AP_AFTER_DIVE_PITCH: goto toggle_phase;
    case AP_AFTER_DIVE_LEVEL: goto begin_climb;
    case AP_AFTER_CLIMB_PITCH: goto toggle_phase;
    case AP_AFTER_CLIMB_LEVEL: goto finish_climb;
    case AP_ORIGINAL_TRANSFER: return 0;
    case AP_COMPLETE: return 1;
    }
    observe(h,DY_BEGIN_FRAME,DY_PRIMARY,44,0);
    B(primary,DY_PRIMARY,rd_u8(w.record+98)); AND_B(primary,DY_PRIMARY,240); CB(w.primary,48);
    if((uint8_t)w.primary==48) goto clear_control_flags;
    word(h,w.record+126,0x1400); W(primary,DY_PRIMARY,rd_u16(w.record)); AND_W(primary,DY_PRIMARY,128);
    if(!(uint16_t)w.primary) goto clear_control_flags;
    B(primary,DY_PRIMARY,rd_u8(0xc457afu)); OR_B(primary,DY_PRIMARY,rd_u8(0xc457aeu));
    if((uint8_t)w.primary) goto finished;
redispatch:
    B(primary,DY_PRIMARY,rd_u8(w.record+5)); if(!(uint8_t)w.primary) goto active_guidance;
    CB(rd_u8(w.record+5),8); if(rd_u8(w.record+5)==8) goto dispatch;
    CB(rd_u8(w.record+5),1); if(rd_u8(w.record+5)==1) goto dispatch;
    if(bit(h,w.record+32,1) || bit(h,w.record+2,0)) goto dispatch;
    W(z,DY_Z,rd_u16(w.record+108)); EL(z,DY_Z); ALL(z,DY_Z,6);
    if(rd_s32(w.record+66)<0) {
        observe(h,DY_TEST_LONG,DY_PRIMARY,rd_u32(w.record+66),0); CL(w.z,rd_u32(w.record+24));
        if((int32_t)w.z<rd_s32(w.record+24)) goto dispatch;
        goto start_level;
    }
    observe(h,DY_TEST_LONG,DY_PRIMARY,rd_u32(w.record+66),0);
    NEGL(z,DY_Z); AL(z,DY_Z,0x800000); CL(w.z,rd_u32(w.record+24));
    if((int32_t)w.z<rd_s32(w.record+24)) goto start_level;
dispatch:
    CB(w.primary,8); if((uint8_t)w.primary==8) goto action_index;
    CB(rd_u8(0xc458a6u),5); if(rd_u8(0xc458a6u)!=5) goto action_index;
    if(!bit(h,w.record+1,3)) goto action_index;
    observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)>0) goto action_index;
    byte(h,w.record+5,8); L(primary,DY_PRIMARY,0); B(primary,DY_PRIMARY,rd_u8(w.record+58)); AW(primary,DY_PRIMARY,w.primary);
    w=geometry_table(w,w.primary,h); observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.table),0);
    if(rd_s16(w.table)<0) goto action_index;
    load_words(&w,w.table,31,-1,h); store_values(w,w.record+44,15,0,h); EL(z,DY_Z); longword(h,w.record+52,w.z); goto redispatch;
action_index: {
        uint8_t action=(uint8_t)w.primary;
        SW(primary,DY_PRIMARY,1); EW(primary,DY_PRIMARY); AW(primary,DY_PRIMARY,w.primary); AW(primary,DY_PRIMARY,w.primary);
        P(root,DY_ROOT,0xc2baf8u); P(root,DY_ROOT,rd_u32(indexed(w.root,w.primary)));
        /* The source has no bounds clamp. Unknown or modified targets retain
         * its exact signed-byte index and original transfer at the boundary. */
        if(!action || action>40 || w.root!=action_targets[action-1]) {
            f->unresolved_target=w.root; AP_WAIT(AP_ORIGINAL_TRANSFER);
        }
        switch(action) {
        case 1: case 3: case 4: case 8: goto active_guidance;
        case 2: goto simple_roll;
        case 5: W(primary,DY_PRIMARY,0x460); W(rate_x,DY_RATE_X,0x1e0); goto rate_roll;
        case 6: goto countdown_guidance;
        case 7: goto wait_pitch;
        case 9: W(primary,DY_PRIMARY,0x280); W(rate_x,DY_RATE_X,0x140); goto rate_roll;
        case 10: goto level_countdown;
        case 11: goto begin_bank;
        case 12: goto bank_countdown;
        case 13: goto bank_arc;
        case 14: goto begin_pitch_arc;
        case 15: goto pitch_arc_countdown;
        case 16: goto pitch_arc;
        case 17: CL(rd_u32(w.record+24),0x100000); if(rd_s32(w.record+24)<=0x100000) goto reset_guidance; goto begin_level_roll;
        case 18: goto level_roll_countdown;
        case 19: goto level_roll;
        case 20: goto begin_reverse_pitch;
        case 21: goto reverse_pitch_countdown;
        case 22: goto reverse_pitch;
        case 23: goto begin_combined;
        case 24: goto combined_countdown;
        case 25: goto combined_roll;
        case 26: goto begin_reverse_roll;
        case 27: goto reverse_roll_countdown;
        case 28: goto reverse_roll;
        case 29: goto begin_dive;
        case 30: goto dive_countdown;
        case 31: goto dive;
        case 32: goto begin_climb;
        case 33: goto climb_countdown;
        case 34: goto climb;
        case 35: goto throttle_phase;
        case 36: goto loop_countdown;
        case 37: goto loop;
        case 38: goto height_phase;
        case 39: byte(h,w.record+5,40); word(h,w.record+76,15); goto final_countdown;
        case 40: goto final_countdown;
        }
    }
start_level:
    byte(h,w.record+5,1); word(h,w.record+76,30); goto active_guidance;
start_bank:
    byte(h,w.record+5,8); word(h,w.record+76,30);
active_guidance:
    if(bit(h,w.record+32,1)) {
        CW(rd_u16(w.record+102),0x3840); if(rd_s16(w.record+102)>0x3840) goto aim_target;
        CW(rd_u16(w.record+102),0xaf0); if(rd_s16(w.record+102)<0xaf0) goto aim_target;
        goto set_countdown_phase;
    }
    CB(rd_u8(w.record+5),8); if(rd_u8(w.record+5)!=8) {
        if(bit(h,w.record+2,0)) goto aim_target;
        observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0);
        if(rd_s16(w.record+76)<0) goto reset_guidance; goto aim_target;
    }
    if(bit(h,w.record+2,0)) {
        observe(h,DY_TEST_BYTE,DY_PRIMARY,rd_u8(0xc4579au),0); if(rd_s8(0xc4579au)>=0) goto finished;
        observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0);
        if(rd_s16(w.record+76)<0) { word(h,w.record+76,11); goto set_countdown_phase; }
    }
    W(z,DY_Z,rd_u16(w.record+102)); CW(w.z,0xe10);
    if((int16_t)w.z>=0xe10) { CW(w.z,0x6270); if((int16_t)w.z<0x6270) goto aim_target; }
    W(z,DY_Z,rd_u16(w.record+106)); CW(w.z,0x1c20); if((int16_t)w.z<0x1c20) goto aim_target;
    CW(w.z,0x5460); if((int16_t)w.z<0x5460) { W(primary,DY_PRIMARY,37); word(h,w.record+76,(uint16_t)w.primary); goto set_countdown_phase; }
aim_target:
    W(primary,DY_PRIMARY,rd_u16(w.record+44)); if((int16_t)w.primary<0) goto clear_control_flags;
    B(detail,DY_DETAIL,rd_u8(w.record+98)); AND_B(detail,DY_DETAIL,240); CB(w.detail,0);
    if(!(uint8_t)w.detail) {
        sub_word(h,w.record+38,1); CW(rd_u16(w.record+38),10); if(rd_s16(w.record+38)>=10) goto clear_control_flags;
    }
    SW(primary,DY_PRIMARY,rd_u16(w.record+6)); EL(primary,DY_PRIMARY); L(rate_x,DY_RATE_X,14); ALL(primary,DY_PRIMARY,14);
    W(detail,DY_DETAIL,rd_u16(w.record+46)); SW(detail,DY_DETAIL,rd_u16(w.record+8)); EL(detail,DY_DETAIL); ALL(detail,DY_DETAIL,14);
    W(x,DY_X,rd_u16(w.record+12)); W(rate_y,DY_RATE_Y,rd_u16(w.record+14)); EL(x,DY_X); EL(rate_y,DY_RATE_Y);
    load_words(&w,w.record+48,40,-1,h);
    if(bit(h,w.record+32,1)) goto target_height;
    B(z,DY_Z,rd_u8(w.record+98)); AND_B(z,DY_Z,240); CB(w.z,0); if(!(uint8_t)w.z) goto target_height;
    CB(rd_u8(w.record+98),20); if(rd_u8(w.record+98)==20) goto target_height;
    CB(rd_u8(w.record+98),21); if(rd_u8(w.record+98)==21) goto target_height;
    W(z,DY_Z,rd_u16(w.record+108)); EL(z,DY_Z); CB(rd_u8(w.record+5),8);
    ALL(z,DY_Z,rd_u8(w.record+5)==8?6:7);
    observe(h,DY_TEST_LONG,DY_PRIMARY,rd_u32(w.record+66),0);
    if(rd_s32(w.record+66)<0) {
        CL(w.z,rd_u32(w.record+24)); if((int32_t)w.z<rd_s32(w.record+24)) goto target_height;
        CL(w.z,rd_u32(w.record+52)); if((int32_t)w.z<rd_s32(w.record+52)) goto target_height;
    } else {
        NEGL(z,DY_Z); AL(z,DY_Z,0x800000); CL(w.z,rd_u32(w.record+24)); if((int32_t)w.z>rd_s32(w.record+24)) goto target_height;
        CL(w.z,rd_u32(w.record+52)); if((int32_t)w.z>=(int32_t)rd_s32(w.record+52)) goto target_height;
    }
    goto displacement;
target_height: L(z,DY_Z,rd_u32(w.record+52));
displacement:
    EL(y,DY_Y); EL(rate_x,DY_RATE_X); observe(h,DY_TEST_BYTE,DY_PRIMARY,rd_u8(w.record+56),0);
    if(rd_s8(w.record+56)>=0) {
        B(rate_z,DY_RATE_Z,rd_u8(w.record+56)); EW(rate_z,DY_RATE_Z); shift=w.rate_z&63;
        if(shift>=32) { L(y,DY_Y,0); L(z,DY_Z,0); L(rate_x,DY_RATE_X,0); }
        else { ALL(y,DY_Y,shift); ALL(z,DY_Z,shift); ALL(rate_x,DY_RATE_X,shift); }
        AND_L(y,DY_Y,0x3fff); AND_L(rate_x,DY_RATE_X,0x3fff);
    }
    SL(y,DY_Y,w.x); SL(rate_x,DY_RATE_X,w.rate_y); AL(y,DY_Y,w.primary); AL(rate_x,DY_RATE_X,w.detail);
    L(rate_y,DY_RATE_Y,rd_u32(w.record+16)); SL(z,DY_Z,w.rate_y); L(rate_z,DY_RATE_Z,w.rate_x); L(rate_y,DY_RATE_Y,w.z); L(rate_x,DY_RATE_X,w.y);
    L(primary,DY_PRIMARY,w.rate_x); if((int32_t)w.primary<0) NEGL(primary,DY_PRIMARY);
    L(detail,DY_DETAIL,w.rate_y); if((int32_t)w.detail<0) NEGL(detail,DY_DETAIL);
    L(x,DY_X,w.rate_z); if((int32_t)w.x<0) NEGL(x,DY_X);
    CL(w.detail,w.primary);
    if((int32_t)w.detail>(int32_t)w.primary) { CL(w.x,w.detail); L(y,DY_Y,(int32_t)w.x>(int32_t)w.detail?w.x:w.detail); }
    else { CL(w.x,w.primary); L(y,DY_Y,(int32_t)w.x>(int32_t)w.primary?w.x:w.primary); }
    for(;;) { CL(w.y,0x4800); if((int32_t)w.y<0x4800) break;
        ASL(rate_x,DY_RATE_X,2); ASL(rate_y,DY_RATE_Y,2); ASL(rate_z,DY_RATE_Z,2); ASL(y,DY_Y,2); }
    W(primary,DY_PRIMARY,0xc0); AP_WAIT(AP_AFTER_NORMALIZE);
choose_limits:
    CB(rd_u8(w.record+5),8);
    if(rd_u8(w.record+5)==8) { f->positive_roll=0xc000; f->positive_pitch=0x10000; f->hysteresis=0x18000;
        f->negative_roll=-0xc000; f->negative_pitch=-0x10000; f->negative_hysteresis=-0x18000; f->direction_limit=0x30000; f->steep_limit=0x80000; }
    else {
        B(primary,DY_PRIMARY,rd_u8(w.record+98)); AND_B(primary,DY_PRIMARY,240); CB(w.primary,16);
        if((uint8_t)w.primary==16) { f->positive_roll=0x30000; f->positive_pitch=0x40000; f->hysteresis=0x50000;
            f->negative_roll=-0x30000; f->negative_pitch=-0x40000; f->negative_hysteresis=-0x50000; f->direction_limit=0x80000; f->steep_limit=0x80000; }
        else { f->positive_roll=0x10000; f->positive_pitch=0x10000; f->hysteresis=0x20000;
            f->negative_roll=-0x10000; f->negative_pitch=-0x10000; f->negative_hysteresis=-0x20000; f->direction_limit=0x20000; f->steep_limit=0x20000; }
    }
    autopilot_limits(f,h); and_byte(h,w.record+100,0x79);
    W(primary,DY_PRIMARY,rd_u16(w.record+146)); W(detail,DY_DETAIL,rd_u16(w.record+152)); W(x,DY_X,rd_u16(w.record+158));
    MUL(primary,DY_PRIMARY,w.rate_x); MUL(detail,DY_DETAIL,w.rate_y); MUL(x,DY_X,w.rate_z); AL(x,DY_X,w.primary); AL(x,DY_X,w.detail);
    L(z,DY_Z,w.x); if((int32_t)w.z<0) NEGL(z,DY_Z);
    W(primary,DY_PRIMARY,rd_u16(w.record+148)); W(detail,DY_DETAIL,rd_u16(w.record+154)); W(y,DY_Y,rd_u16(w.record+160));
    MUL(primary,DY_PRIMARY,w.rate_x); MUL(detail,DY_DETAIL,w.rate_y); MUL(y,DY_Y,w.rate_z); AL(y,DY_Y,w.primary); AL(y,DY_Y,w.detail);
    L(primary,DY_PRIMARY,w.y); if((int32_t)w.primary<0) NEGL(primary,DY_PRIMARY); CL(w.z,w.primary);
    if((int32_t)w.z<=(int32_t)w.primary) { or_byte(h,w.record+100,8); L(x,DY_X,w.y); }
    else { and_byte(h,w.record+100,0xf7); observe(h,DY_TEST_LONG,DY_PRIMARY,w.x,0); }
    L(y,DY_Y,w.x);
    if((int32_t)w.y<0) { and_byte(h,w.record+100,0xfe); int is_roll=bit(h,w.record+100,3);
        CL(w.x,is_roll?f->negative_roll:f->negative_pitch);
        if((int32_t)w.x>=(is_roll?f->negative_roll:f->negative_pitch)) goto aligned; }
    else { or_byte(h,w.record+100,1); int is_roll=bit(h,w.record+100,3);
        CL(w.x,is_roll?f->positive_roll:f->positive_pitch);
        if((int32_t)w.x<=(is_roll?f->positive_roll:f->positive_pitch)) goto aligned; }
    B(primary,DY_PRIMARY,rd_u8(w.record+100)); AND_B(primary,DY_PRIMARY,8); if((uint8_t)w.primary) goto pitch_control;
    L(y,DY_Y,w.x); if((int32_t)w.y<0) NEGL(y,DY_Y);
    B(primary,DY_PRIMARY,rd_u8(w.record+100)); AND_B(primary,DY_PRIMARY,2);
    if((uint8_t)w.primary) { CL(w.y,f->hysteresis); if((int32_t)w.y<f->hysteresis) goto finished; }
    CL(w.y,f->steep_limit); if((int32_t)w.y<f->steep_limit) or_byte(h,w.record+100,128);
    CL(w.y,f->direction_limit); if((int32_t)w.y>=f->direction_limit) goto negative_turn_direction;
    W(primary,DY_PRIMARY,rd_u16(w.record+150)); W(detail,DY_DETAIL,rd_u16(w.record+156)); W(z,DY_Z,rd_u16(w.record+162));
    MUL(primary,DY_PRIMARY,w.rate_x); MUL(detail,DY_DETAIL,w.rate_y); MUL(z,DY_Z,w.rate_z); AL(z,DY_Z,w.primary);
    difference=(int64_t)(int32_t)w.z+(int32_t)w.detail; AL(z,DY_Z,w.detail); if(difference<0) goto negative_turn_direction;
    or_byte(h,w.record+100,32); goto turn_direction_ready;
negative_turn_direction: and_byte(h,w.record+100,0xdf);
turn_direction_ready:
    and_byte(h,w.record+100,0xfd); SWAP(y,DY_Y); CW(w.y,10); if((int16_t)w.y>10) L(y,DY_Y,10);
    B(primary,DY_PRIMARY,rd_u8(w.record+98)); AND_B(primary,DY_PRIMARY,240); CB(w.primary,16);
    if((uint8_t)w.primary==16) goto aircraft_turn;
    P(root,DY_ROOT,0xc2cc2au); AW(y,DY_Y,w.y); W(y,DY_Y,rd_u16(indexed(w.root,w.y)));
    observe(h,DY_TEST_LONG,DY_PRIMARY,w.x,0); if((int32_t)w.x>=0) NEGW(y,DY_Y); goto choose_roll_input;
aircraft_turn:
    CB(rd_u8(w.record+98),20);
    if(rd_u8(w.record+98)==20) P(root,DY_ROOT,0xc2cc82u);
    else { CB(rd_u8(w.record+5),8); P(root,DY_ROOT,rd_u8(w.record+5)==8?0xc2cc82u:0xc2cc56u); }
    AW(y,DY_Y,w.y); W(y,DY_Y,rd_u16(indexed(w.root,w.y))); CW(rd_u16(w.record+108),0x1800); if(rd_s16(w.record+108)>=0x1800) ASW(y,DY_Y,1);
    L(primary,DY_PRIMARY,w.x); if((int32_t)w.primary<0) { NEGW(y,DY_Y); NEGL(primary,DY_PRIMARY); }
    W(z,DY_Z,rd_u16(w.record+106)); CW(w.z,0x1770);
    if((int16_t)w.z>=0x1770) { CW(w.z,0x5910); if((int16_t)w.z<=0x5910) goto choose_roll_input; }
    W(detail,DY_DETAIL,rd_u16(w.record+102)); CW(w.detail,0x6a40);
    if((int16_t)w.detail<=0x6a40) { CW(w.detail,0x640); if((int16_t)w.detail>0x640) goto choose_roll_input; }
    MUL(rate_x,DY_RATE_X,rd_u16(w.record+150)); MUL(rate_y,DY_RATE_Y,rd_u16(w.record+156)); MUL(rate_z,DY_RATE_Z,rd_u16(w.record+162));
    AL(rate_z,DY_RATE_Z,w.rate_x); difference=(int64_t)(int32_t)w.rate_z+(int32_t)w.rate_y; AL(rate_z,DY_RATE_Z,w.rate_y);
    if(difference<0) { L(detail,DY_DETAIL,0x300000); L(rate_z,DY_RATE_Z,w.detail); SL(detail,DY_DETAIL,w.primary); AL(detail,DY_DETAIL,w.rate_z); L(primary,DY_PRIMARY,w.detail); }
    ASL(primary,DY_PRIMARY,8); CW(w.primary,0x3000);
    if((int16_t)w.primary>0x3000) W(primary,DY_PRIMARY,0x3000);
    else { CW(w.primary,32); if((int16_t)w.primary<=32) { CW(w.primary,0x1000); if((int16_t)w.primary<0x1000) W(primary,DY_PRIMARY,0x1000); }
        else { CW(w.primary,0x1800); if((int16_t)w.primary<0x1800) W(primary,DY_PRIMARY,0x1800); } }
    CB(rd_u8(w.record+122),3); if(rd_u8(w.record+122)!=3) { CB(rd_u8(w.record+122),4); if(rd_u8(w.record+122)!=4) goto apply_turn; }
    observe(h,DY_TEST_WORD,DY_PRIMARY,w.y,0);
    if((int16_t)w.y<0) {
        B(x,DY_X,rd_u8(w.record+100)); AND_B(x,DY_X,96); CB(w.x,96);
        if((uint8_t)w.x==96) { CW(w.z,0x3840); if((int16_t)w.z>=0x3840) { CW(w.z,0x6d60); if((int16_t)w.z<0x6d60) goto fixed_right_turn; } }
        CW(w.z,0x3840); if((int16_t)w.z<=0x3840) goto apply_turn;
        CW(w.z,0x68b0); if((int16_t)w.z>0x68b0) goto apply_turn;
        CW(w.z,0x6720); if((int16_t)w.z>0x6720) { L(y,DY_Y,0); goto apply_turn; }
fixed_right_turn: W(y,DY_Y,240); goto apply_turn;
    } else {
        B(x,DY_X,rd_u8(w.record+100)); AND_B(x,DY_X,96); CB(w.x,96);
        if((uint8_t)w.x==96) { CW(w.z,0x3840); if((int16_t)w.z<=0x3840) { CW(w.z,0x320); if((int16_t)w.z>0x320) goto fixed_left_turn; } }
        CW(w.z,0x3840); if((int16_t)w.z>=0x3840) goto apply_turn;
        CW(w.z,0x7d0); if((int16_t)w.z<0x7d0) goto apply_turn;
        CW(w.z,0x960); if((int16_t)w.z<0x960) { L(y,DY_Y,0); goto apply_turn; }
fixed_left_turn: W(y,DY_Y,(uint16_t)-240);
    }
apply_turn:
    B(x,DY_X,rd_u8(w.record+5)); if(!(uint8_t)w.x) goto turn_child;
    CB(w.x,1); if((uint8_t)w.x==1) goto turn_child;
    CB(w.x,8); if((uint8_t)w.x!=8) goto finished;
    W(x,DY_X,rd_u16(w.record+2)); AND_W(x,DY_X,128); if((uint16_t)w.x) goto finished;
turn_child: AP_WAIT(AP_AFTER_TURN);
publish_turn:
    B(detail,DY_DETAIL,rd_u8(w.record+98)); AND_B(detail,DY_DETAIL,240); CB(w.detail,16);
    if((uint8_t)w.detail!=16) { word(h,w.record+84,(uint16_t)w.y); word(h,w.record+82,0); word(h,w.record+80,0); }
    CB(rd_u8(w.record+5),8); if(rd_u8(w.record+5)==8) ASW(primary,DY_PRIMARY,1);
    word(h,w.record+126,(uint16_t)w.primary); goto finished;
choose_roll_input:
    observe(h,DY_TEST_WORD,DY_PRIMARY,w.y,0);
    if(!(uint16_t)w.y) L(x,DY_X,0);
    else if((int16_t)w.y>0) { CW(w.y,rd_u16(w.record+88)); if((int16_t)w.y>rd_s16(w.record+88)) B(x,DY_X,128); else L(x,DY_X,0); }
    else { CW(w.y,rd_u16(w.record+88)); if((int16_t)w.y<rd_s16(w.record+88)) B(x,DY_X,64); else L(x,DY_X,0); }
    B(detail,DY_DETAIL,rd_u8(w.record+101)); AND_B(detail,DY_DETAIL,3); OR_B(detail,DY_DETAIL,w.x); byte(h,w.record+101,(uint8_t)w.detail);
    B(primary,DY_PRIMARY,rd_u8(w.record+98)); AND_B(primary,DY_PRIMARY,240); CB(w.primary,16); if((uint8_t)w.primary==16) goto finished;
    word(h,w.record+82,(uint16_t)w.y); word(h,w.record+84,0); word(h,w.record+80,0); goto finished;
pitch_control:
    L(y,DY_Y,w.x); if((int32_t)w.y<0) NEGL(y,DY_Y);
    B(primary,DY_PRIMARY,rd_u8(w.record+100)); AND_B(primary,DY_PRIMARY,4);
    if((uint8_t)w.primary) { CL(w.y,f->hysteresis); if((int32_t)w.y<=f->hysteresis) goto finished; }
    CL(w.y,f->direction_limit); if((int32_t)w.y>=f->direction_limit) goto negative_pitch_direction;
    W(primary,DY_PRIMARY,rd_u16(w.record+150)); W(detail,DY_DETAIL,rd_u16(w.record+156)); W(z,DY_Z,rd_u16(w.record+162));
    MUL(primary,DY_PRIMARY,w.rate_x); MUL(detail,DY_DETAIL,w.rate_y); MUL(z,DY_Z,w.rate_z); AL(z,DY_Z,w.primary);
    difference=(int64_t)(int32_t)w.z+(int32_t)w.detail; AL(z,DY_Z,w.detail); if(difference<0) goto negative_pitch_direction;
    or_byte(h,w.record+100,64); goto pitch_direction_ready;
negative_pitch_direction: and_byte(h,w.record+100,0xbf);
pitch_direction_ready:
    and_byte(h,w.record+100,0xfb); SWAP(y,DY_Y); CW(w.y,10); if((int16_t)w.y>10) L(y,DY_Y,10);
    B(primary,DY_PRIMARY,rd_u8(w.record+98)); AND_B(primary,DY_PRIMARY,240); CB(w.primary,16);
    if((uint8_t)w.primary!=16) P(root,DY_ROOT,0xc2cc2au);
    else { CB(rd_u8(w.record+98),20); P(root,DY_ROOT,rd_u8(w.record+98)==20?0xc2cc6cu:0xc2cc40u); }
    AW(y,DY_Y,w.y); W(y,DY_Y,rd_u16(indexed(w.root,w.y))); observe(h,DY_TEST_LONG,DY_PRIMARY,w.x,0); if((int32_t)w.x>=0) NEGW(y,DY_Y);
    B(x,DY_X,rd_u8(w.record+5)); if(!(uint8_t)w.x) goto pitch_child;
    CB(w.x,1); if((uint8_t)w.x==1) goto pitch_child;
    CB(w.x,8); if((uint8_t)w.x!=8) goto finished;
    W(x,DY_X,rd_u16(w.record+2)); AND_W(x,DY_X,128); if((uint16_t)w.x) goto finished;
pitch_child: AP_WAIT(AP_AFTER_PITCH);
publish_pitch:
    B(detail,DY_DETAIL,rd_u8(w.record+98)); AND_B(detail,DY_DETAIL,240); CB(w.detail,16); if((uint8_t)w.detail==16) goto finished;
    word(h,w.record+80,(uint16_t)w.y); word(h,w.record+82,0); word(h,w.record+84,0); goto finished;
aligned:
    B(primary,DY_PRIMARY,rd_u8(w.record+98)); AND_B(primary,DY_PRIMARY,240); CB(w.primary,16);
    if((uint8_t)w.primary!=16) { word(h,w.record+80,0); word(h,w.record+82,0); word(h,w.record+84,0); }
    B(primary,DY_PRIMARY,rd_u8(w.record+100)); AND_B(primary,DY_PRIMARY,8);
    if(!(uint8_t)w.primary) { or_byte(h,w.record+100,2); or_byte(h,w.record+100,32); goto latch_phase; }
    W(primary,DY_PRIMARY,rd_u16(w.record+150)); W(detail,DY_DETAIL,rd_u16(w.record+156)); W(x,DY_X,rd_u16(w.record+162));
    MUL(primary,DY_PRIMARY,w.rate_x); MUL(detail,DY_DETAIL,w.rate_y); MUL(x,DY_X,w.rate_z); AL(x,DY_X,w.primary);
    difference=(int64_t)(int32_t)w.x+(int32_t)w.detail; AL(x,DY_X,w.detail);
    if(difference<0) { L(x,DY_X,w.y); goto pitch_direction_ready; }
    or_byte(h,w.record+100,4); or_byte(h,w.record+100,64);
latch_phase:
    old=rd_u8(w.record+100); wr_u8(w.record+100,(uint8_t)(old^1)); observe(h,DY_AUTOPILOT_TOGGLE,DY_PRIMARY,old,0);
toggle_phase:
    or_byte(h,w.record+100,16); goto finished;
/* Maneuver sequence arms. Shared countdown and attitude tests retain the
 * source's signed word comparisons and overlapping record-byte flags. */
throttle_phase:
    CB(rd_u8(w.record+43),96); if(rd_s8(w.record+43)<96) or_byte(h,w.record+101,1); else and_byte(h,w.record+101,0xfc);
    CW(rd_u16(w.record+110),0x840); if(rd_s16(w.record+110)>=0x840) { byte(h,w.record+5,36); word(h,w.record+76,60); } goto toggle_phase;
loop_countdown:
    observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)>=0) goto toggle_phase;
    change_bit(h,w.record+32,4,0); byte(h,w.record+5,37);
loop:
    P(root,DY_ROOT,0xc2cc40u); L(y,DY_Y,-40); W(z,DY_Z,rd_u16(w.record+102));
    if(!bit(h,w.record+32,4)) { CW(w.z,0x3840); if((int16_t)w.z<0x3840) AP_WAIT(AP_AFTER_LOOP_PITCH); change_bit(h,w.record+32,4,1); }
    CW(w.z,0x68b0); if((int16_t)w.z>=0x68b0) AP_WAIT(AP_AFTER_LOOP_PITCH);
    L(y,DY_Y,0); AP_WAIT(AP_AFTER_LOOP_LEVEL);
clear_loop_stick: byte(h,w.record+101,0); byte(h,w.record+5,38);
height_phase: CL(rd_u32(w.record+24),0x240000); if(rd_s32(w.record+24)>0x240000) goto reset_guidance; goto toggle_phase;
begin_bank: change_bit(h,w.record+32,3,0); change_bit(h,w.record+32,4,0); byte(h,w.record+5,12); word(h,w.record+76,15);
bank_countdown: observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)>0) goto hold_maneuver;
    byte(h,w.record+5,13);
bank_arc:
    w=autopilot_table_term(w,0xc2cc56u,16,h); W(z,DY_Z,rd_u16(w.record+106));
    if(bit(h,w.record+32,4)) goto bank_arc_exit;
    if(!bit(h,w.record+32,3)) { CW(w.z,0x3840); if((int16_t)w.z>=0x3840) AP_WAIT(AP_AFTER_BANK_ROLL); change_bit(h,w.record+32,3,1); }
    CW(w.z,0x3840); if((int16_t)w.z<0x3840) AP_WAIT(AP_AFTER_BANK_ROLL); change_bit(h,w.record+32,4,1);
bank_arc_exit:
    CW(w.z,0x320); if((int16_t)w.z<0x320) goto reset_guidance; CW(w.z,0x6e00); if((int16_t)w.z>=0x6e00) L(y,DY_Y,0); AP_WAIT(AP_AFTER_BANK_ROLL);
begin_pitch_arc: change_bit(h,w.record+32,3,0); change_bit(h,w.record+32,4,0); byte(h,w.record+5,15); word(h,w.record+76,15);
pitch_arc_countdown: observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)>0) goto hold_maneuver; byte(h,w.record+5,16);
pitch_arc:
    w=autopilot_table_term(w,0xc2cc40u,12,h); NEGW(y,DY_Y); W(z,DY_Z,rd_u16(w.record+102));
    if(bit(h,w.record+32,4)) goto pitch_arc_exit;
    if(!bit(h,w.record+32,3)) { CW(w.z,0x3840); if((int16_t)w.z<=0x3840) AP_WAIT(AP_AFTER_PITCH_ARC); change_bit(h,w.record+32,3,1); }
    CW(w.z,0x3840); if((int16_t)w.z>0x3840) AP_WAIT(AP_AFTER_PITCH_ARC); change_bit(h,w.record+32,4,1);
pitch_arc_exit: CW(w.z,0x3840); if((int16_t)w.z>0x3840) goto reset_guidance; AP_WAIT(AP_AFTER_PITCH_ARC);
begin_level_roll: change_bit(h,w.record+32,3,0); change_bit(h,w.record+32,4,0); byte(h,w.record+5,18); word(h,w.record+76,15);
level_roll_countdown: observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)>0) goto hold_maneuver; byte(h,w.record+5,19);
level_roll:
    w=autopilot_table_term(w,0xc2cc56u,16,h); W(z,DY_Z,rd_u16(w.record+106));
    if(!bit(h,w.record+32,3)) { CW(w.z,0x3840); if((int16_t)w.z>=0x3840) AP_WAIT(AP_AFTER_LEVEL_ROLL); change_bit(h,w.record+32,3,1); }
    CW(w.z,0x3840); if((int16_t)w.z==0x3840) { byte(h,w.record+5,15); word(h,w.record+76,2); goto pitch_arc_countdown; }
    CW(w.z,0x35c0); if((int16_t)w.z>=0x35c0) { change_bit(h,w.record+3,4,1); L(y,DY_Y,0); } AP_WAIT(AP_AFTER_LEVEL_ROLL);
begin_reverse_pitch: change_bit(h,w.record+32,3,0); byte(h,w.record+5,21); word(h,w.record+76,15);
reverse_pitch_countdown: observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)>0) goto hold_maneuver; byte(h,w.record+5,22);
reverse_pitch:
    w=autopilot_table_term(w,0xc2cc40u,12,h); NEGW(y,DY_Y); W(z,DY_Z,rd_u16(w.record+102));
    if(!bit(h,w.record+32,3)) { CW(w.z,0x3840); if((int16_t)w.z<=0x3840) AP_WAIT(AP_AFTER_REVERSE_PITCH); change_bit(h,w.record+32,3,1); }
    CW(w.z,0x3840); if((int16_t)w.z>0x3840) AP_WAIT(AP_AFTER_REVERSE_PITCH); byte(h,w.record+5,13); goto bank_arc;
begin_combined: change_bit(h,w.record+32,3,0); change_bit(h,w.record+32,4,0); byte(h,w.record+5,24); word(h,w.record+76,15);
combined_countdown: observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)>0) goto hold_maneuver; byte(h,w.record+5,25);
combined_roll:
    w=autopilot_table_term(w,0xc2cc56u,14,h); W(z,DY_Z,rd_u16(w.record+106));
    if(bit(h,w.record+32,4)) goto combined_exit;
    if(!bit(h,w.record+32,3)) { CW(w.z,0x3840); if((int16_t)w.z>=0x3840) AP_WAIT(AP_AFTER_COMBINED_ROLL); change_bit(h,w.record+32,3,1); }
    CW(w.z,0x3840); if((int16_t)w.z<0x3840) AP_WAIT(AP_AFTER_COMBINED_ROLL); change_bit(h,w.record+32,4,1);
combined_exit: CW(w.z,0x3840); if((int16_t)w.z<0x3840) goto reset_guidance; AP_WAIT(AP_AFTER_COMBINED_ROLL);
combined_pitch: w=autopilot_table_term(w,0xc2cc40u,14,h); NEGW(y,DY_Y); AP_WAIT(AP_AFTER_COMBINED_PITCH);
begin_reverse_roll: change_bit(h,w.record+32,3,0); change_bit(h,w.record+32,4,0); byte(h,w.record+5,27); word(h,w.record+76,15);
reverse_roll_countdown: observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)>0) goto hold_maneuver; byte(h,w.record+5,28);
reverse_roll:
    w=autopilot_table_term(w,0xc2cc56u,16,h); W(z,DY_Z,rd_u16(w.record+106));
    if(!bit(h,w.record+32,3)) { CW(w.z,0x3840); if((int16_t)w.z>=0x3840) AP_WAIT(AP_AFTER_REVERSE_ROLL); change_bit(h,w.record+32,3,1); }
    CW(w.z,0x3840); if((int16_t)w.z==0x3840) goto begin_dive;
    CW(w.z,0x35c0); if((int16_t)w.z>=0x35c0) { change_bit(h,w.record+3,4,1); L(y,DY_Y,0); } AP_WAIT(AP_AFTER_REVERSE_ROLL);
begin_dive: change_bit(h,w.record+32,4,0); byte(h,w.record+5,30); word(h,w.record+76,10);
dive_countdown: observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)>0) goto hold_maneuver; byte(h,w.record+5,31);
dive:
    w=autopilot_table_term(w,0xc2cc40u,8,h); NEGW(y,DY_Y); W(z,DY_Z,rd_u16(w.record+102)); W(x,DY_X,rd_u16(w.record+106));
    CW(w.x,0x5460); if((int16_t)w.x>0x5460) goto dive_upright; CW(w.x,0x1c20); if((int16_t)w.x<0x1c20) goto dive_upright;
    if(!bit(h,w.record+32,4)) { CW(w.z,0x3840); if((int16_t)w.z>0x3840) AP_WAIT(AP_AFTER_DIVE_PITCH); change_bit(h,w.record+32,4,1); }
    CW(w.z,0x960); if((int16_t)w.z<0x960) AP_WAIT(AP_AFTER_DIVE_PITCH); goto dive_level;
dive_upright:
    if(!bit(h,w.record+32,4)) { CW(w.z,0x3840); if((int16_t)w.z<0x3840) AP_WAIT(AP_AFTER_DIVE_PITCH); change_bit(h,w.record+32,4,1); }
    CW(w.z,0x6720); if((int16_t)w.z>=0x6720) AP_WAIT(AP_AFTER_DIVE_PITCH);
dive_level: L(y,DY_Y,0); AP_WAIT(AP_AFTER_DIVE_LEVEL);
begin_climb: change_bit(h,w.record+32,4,0); byte(h,w.record+5,33); word(h,w.record+76,25);
climb_countdown: observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)>0) goto hold_maneuver; byte(h,w.record+5,34);
climb:
    w=autopilot_table_term(w,0xc2cc40u,8,h); W(z,DY_Z,rd_u16(w.record+102)); W(x,DY_X,rd_u16(w.record+106));
    CW(w.x,0x5460); if((int16_t)w.x>0x5460) goto climb_upright; CW(w.x,0x1c20); if((int16_t)w.x<0x1c20) goto climb_upright;
    if(!bit(h,w.record+32,4)) { CW(w.z,0x3840); if((int16_t)w.z<0x3840) AP_WAIT(AP_AFTER_CLIMB_PITCH); change_bit(h,w.record+32,4,1); }
    CW(w.z,0x6720); if((int16_t)w.z>0x6720) AP_WAIT(AP_AFTER_CLIMB_PITCH); goto climb_level;
climb_upright:
    if(!bit(h,w.record+32,4)) { CW(w.z,0x3840); if((int16_t)w.z>0x3840) AP_WAIT(AP_AFTER_CLIMB_PITCH); change_bit(h,w.record+32,4,1); }
    CW(w.z,0x960); if((int16_t)w.z<=0x960) AP_WAIT(AP_AFTER_CLIMB_PITCH);
climb_level: L(y,DY_Y,0); AP_WAIT(AP_AFTER_CLIMB_LEVEL);
finish_climb: change_bit(h,w.record+32,3,1); byte(h,w.record+5,22); goto reverse_pitch;
final_countdown: observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)>0) goto hold_maneuver; byte(h,w.record+5,2);
simple_roll:
    L(x,DY_X,0); w=autopilot_table_term(w,0xc2cc56u,16,h); W(primary,DY_PRIMARY,0x1c20); W(detail,DY_DETAIL,0x5460); W(z,DY_Z,rd_u16(w.record+106));
    CW(w.z,0x3840); if((int16_t)w.z>=0x3840) W(primary,DY_PRIMARY,w.detail);
    CW(w.z,w.primary); if((int16_t)w.z>=(int16_t)w.primary) AW(x,DY_X,1);
    difference=(int16_t)w.z-(int16_t)w.primary; SW(z,DY_Z,w.primary); if(difference<0) NEGW(z,DY_Z);
    CW(w.z,0x4b0); if((int16_t)w.z<0x4b0) { byte(h,w.record+5,7); word(h,w.record+76,32); and_byte(h,w.record+101,3); goto toggle_phase; }
    observe(h,DY_TEST_WORD,DY_PRIMARY,w.x,0); if((uint16_t)w.x) NEGW(y,DY_Y); AP_WAIT(AP_AFTER_SIMPLE_ROLL);
wait_pitch:
    observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)<0) goto reset_guidance;
    w=autopilot_table_term(w,0xc2cc40u,8,h); NEGW(y,DY_Y); AP_WAIT(AP_AFTER_WAIT_PITCH);
rate_roll:
    W(detail,DY_DETAIL,0x7080); SW(detail,DY_DETAIL,w.primary);
    observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)<0) goto release_guidance;
    L(x,DY_X,0); w=autopilot_table_term(w,0xc2cc56u,4,h); W(z,DY_Z,rd_u16(w.record+106));
    CW(w.z,0x3840); if((int16_t)w.z>=0x3840) W(primary,DY_PRIMARY,w.detail);
    CW(w.z,w.primary); if((int16_t)w.z>=(int16_t)w.primary) AW(x,DY_X,1);
    difference=(int16_t)w.z-(int16_t)w.primary; SW(z,DY_Z,w.primary); if(difference<0) NEGW(z,DY_Z);
    W(primary,DY_PRIMARY,0x1000); CW(w.z,w.rate_x); if((int16_t)w.z<(int16_t)w.rate_x) AP_WAIT(AP_AFTER_RATE_NEUTRAL);
    observe(h,DY_TEST_WORD,DY_PRIMARY,w.x,0); if((uint16_t)w.x) NEGW(y,DY_Y); AP_WAIT(AP_AFTER_RATE_ROLL);
hold_maneuver: change_bit(h,w.record+3,4,1); goto toggle_phase;
reset_guidance:
    if(bit(h,w.record+2,0)) { observe(h,DY_TEST_BYTE,DY_PRIMARY,rd_u8(0xc4579au),0); if(rd_s8(0xc4579au)>=0) goto begin_level_countdown; goto default_countdown; }
    observe(h,DY_TEST_BYTE,DY_PRIMARY,rd_u8(w.record+56),0); if(rd_s8(w.record+56)>=0) goto default_countdown;
    CB(rd_u8(w.record+56),255); if(rd_u8(w.record+56)==255) goto default_countdown;
    CB(rd_u8(0xc458a7u),2); if(rd_s8(0xc458a7u)>2) { B(primary,DY_PRIMARY,rd_u8(w.record+100)); AND_B(primary,DY_PRIMARY,96); CB(w.primary,96); if((uint8_t)w.primary!=96) goto default_countdown; }
    B(primary,DY_PRIMARY,rd_u8(0xc458a7u));
    if((int8_t)w.primary<=0) W(primary,DY_PRIMARY,75);
    else { SB(primary,DY_PRIMARY,1); if(!(uint8_t)w.primary) W(primary,DY_PRIMARY,37);
        else { SB(primary,DY_PRIMARY,1); W(primary,DY_PRIMARY,!(uint8_t)w.primary?18:4); } }
    word(h,w.record+76,(uint16_t)w.primary); goto set_countdown_phase;
default_countdown: word(h,w.record+76,10);
set_countdown_phase: byte(h,w.record+5,6);
countdown_guidance:
    if(bit(h,w.record+32,1)) goto finished;
    if(bit(h,w.record+2,0)) {
        observe(h,DY_TEST_BYTE,DY_PRIMARY,rd_u8(0xc4579au),0); if(rd_s8(0xc4579au)<0) goto finished;
        observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)<0) goto start_bank; goto finished;
    }
    observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)<0) goto start_level;
    W(z,DY_Z,rd_u16(w.record+108)); EL(z,DY_Z); ALL(z,DY_Z,6); observe(h,DY_TEST_LONG,DY_PRIMARY,rd_u32(w.record+66),0);
    if(rd_s32(w.record+66)<0) { CL(w.z,rd_u32(w.record+24)); if((int32_t)w.z<rd_s32(w.record+24)) goto finished;
        word(h,0xc4599eu,60); AP_WAIT(AP_AFTER_FAULT); }
    NEGL(z,DY_Z); AL(z,DY_Z,0x800000); CL(w.z,rd_u32(w.record+24)); if((int32_t)w.z<rd_s32(w.record+24)) goto start_level; goto finished;
begin_level_countdown: byte(h,w.record+5,10); observe(h,DY_TEST_BYTE,DY_PRIMARY,rd_u8(0xc45793u),0); word(h,w.record+76,rd_u8(0xc45793u)?36:60);
level_countdown: observe(h,DY_TEST_WORD,DY_PRIMARY,rd_u16(w.record+76),0); if(rd_s16(w.record+76)<0) goto release_guidance; goto finished;
release_guidance: byte(h,w.record+5,0); and_byte(h,w.record+101,3); goto finished;
clear_control_flags: byte(h,w.record+100,0);
finished:
    observe(h,DY_END_FRAME,DY_PRIMARY,0,0); f->work=w; f->phase=AP_COMPLETE; return 1;
#undef AP_WAIT
}
static void steering_outputs(void *context,enum SteeringPhase phase,enum SteeringField field,
                             uint32_t value,uint32_t operand) {
    const DynamicsHooks *hooks=(const DynamicsHooks *)context;
    static const enum DynamicsPhase phases[]={DY_BYTE,DY_WORD,DY_LONG,DY_AND_BYTE,DY_OR_BYTE,
        DY_TEST_WORD,DY_COMPARE_BYTE,DY_COMPARE_WORD,DY_BIT_TEST,DY_STORE_BYTE};
    static const enum DynamicsValue fields[]={DY_PRIMARY,DY_DETAIL,DY_X,DY_Y};
    observe(hooks,phases[phase],fields[field],value,operand);
}
/* The maneuver phase selects a game function. Results return as C values;
 * the optional observer only publishes temporary caller compatibility state. */
static int update_autopilot_steering(AutopilotFrame *frame,const DynamicsHooks *hooks) {
    RecordSteeringState state={frame->work.detail,frame->work.x,frame->work.y,frame->work.record};
    RecordSteeringHooks output={steering_outputs,(void *)hooks};
    switch(frame->phase) {
    case AP_AFTER_FAULT: case AP_AFTER_NORMALIZE: case AP_ORIGINAL_TRANSFER:
        return 0;
    case AP_AFTER_TURN:
        observe(hooks,DY_STEERING_ENTER,DY_PRIMARY,frame->phase,0);
        state=select_record_turn(state,&output); break;
    case AP_AFTER_PITCH: case AP_AFTER_WAIT_PITCH:
        observe(hooks,DY_STEERING_ENTER,DY_PRIMARY,frame->phase,0);
        state=select_record_pitch(state,&output); break;
    case AP_AFTER_SIMPLE_ROLL: case AP_AFTER_RATE_ROLL: case AP_AFTER_BANK_ROLL:
    case AP_AFTER_LEVEL_ROLL: case AP_AFTER_COMBINED_ROLL: case AP_AFTER_REVERSE_ROLL:
        observe(hooks,DY_STEERING_ENTER,DY_PRIMARY,frame->phase,0);
        state=select_record_roll(state,&output); break;
    case AP_AFTER_RATE_NEUTRAL:
        observe(hooks,DY_STEERING_ENTER,DY_PRIMARY,frame->phase,0);
        state=select_record_neutral(state,&output); break;
    case AP_AFTER_COMBINED_PITCH:
        observe(hooks,DY_STEERING_ENTER,DY_PRIMARY,frame->phase,0);
        state=select_record_pitch_preserving_controls(state,&output); break;
    case AP_AFTER_LOOP_PITCH: case AP_AFTER_LOOP_LEVEL: case AP_AFTER_PITCH_ARC:
    case AP_AFTER_REVERSE_PITCH: case AP_AFTER_DIVE_PITCH: case AP_AFTER_DIVE_LEVEL:
    case AP_AFTER_CLIMB_PITCH: case AP_AFTER_CLIMB_LEVEL:
        observe(hooks,DY_STEERING_ENTER,DY_PRIMARY,frame->phase,0);
        state=select_record_pitch_branch(state,&output); break;
    default: abort();
    }
    observe(hooks,DY_STEERING_LEAVE,DY_PRIMARY,frame->phase,0);
    frame->work.detail=state.controls; frame->work.x=state.selection; frame->work.y=state.turn;
    return 1;
}
static void normalization_outputs(void *context,enum FixedNormalizationPhase phase,const NormalizedVectorState *s) {
    const DynamicsHooks *hooks=(const DynamicsHooks *)context;
    if(phase==FIXED_NORMALIZE_ENTER) observe(hooks,DY_NORMALIZE_ENTER,DY_PRIMARY,s->scale,0);
    else if(phase==FIXED_MAGNITUDE_ENTER) observe(hooks,DY_MAGNITUDE_ENTER,DY_PRIMARY,s->x,0);
    else {
        observe(hooks,DY_LONG,DY_PRIMARY,s->scale,0); observe(hooks,DY_LONG,DY_DETAIL,s->length,0);
        observe(hooks,DY_LONG,DY_X,s->shift,0); observe(hooks,DY_LONG,DY_Y,s->planar_factor,0);
        observe(hooks,DY_LONG,DY_Z,s->height_ratio,0); observe(hooks,DY_LONG,DY_RATE_X,s->x,0);
        observe(hooks,DY_LONG,DY_RATE_Y,s->y,0); observe(hooks,DY_LONG,DY_RATE_Z,s->z,0);
        if(s->rounding_valid) observe(hooks,DY_NORMALIZE_EXTENSION,DY_PRIMARY,s->rounding_bit,0);
        observe(hooks,DY_NORMALIZE_LEAVE,DY_PRIMARY,0,0);
    }
}
static void update_autopilot_direction(AutopilotFrame *frame,const DynamicsHooks *hooks) {
    DynamicsState *w=&frame->work;
    NormalizedVectorState state={w->primary,w->detail,w->x,w->y,w->z,w->rate_x,w->rate_y,w->rate_z,0,0};
    state=normalize_record_vector(state,normalization_outputs,(void *)hooks);
    w->primary=state.scale; w->detail=state.length; w->x=state.shift;
    w->y=state.planar_factor; w->z=state.height_ratio; w->rate_x=state.x; w->rate_y=state.y; w->rate_z=state.z;
}
int update_dynamics_record_action(AutopilotFrame *frame,const DynamicsHooks *hooks) {
    while(!advance_record_autopilot(frame,hooks)) {
        if(frame->phase==AP_AFTER_NORMALIZE) update_autopilot_direction(frame,hooks);
        else if(!update_autopilot_steering(frame,hooks)) return 0;
    }
    return 1;
}
int update_dynamics_record_zone_exit(ZoneExitFrame *frame,const GeometryHooks *hooks) {
    return advance_record_zone_exit(frame,hooks);
}
