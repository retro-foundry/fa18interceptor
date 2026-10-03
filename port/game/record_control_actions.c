/* Complete C153FC/C15688/C159AE/C15AD4/C181A0/C15138 original owners.
 * Direction, collision, rendering and sound consumers retain real child calls. */
#include "record_control_actions.h"
#include <stdlib.h>
#define SELECTED_CONTROL 0xc18214u
#define CONTROL_VECTOR 0xc45a52u
static void observe(const RecordActionHooks *h,enum RecordActionPhase p,uint32_t v,uint32_t other) {
    if(h && h->observe) h->observe(h->context,p,v,other);
}
static uint32_t consume(const RecordActionHooks *h,enum RecordActionChild c) {
    if(h && h->consume) return h->consume(h->context,c);
    abort();
}
static gaddr lookup(const RecordActionHooks *h,gaddr a) { observe(h,RA_LOOKUP,a,0); return a; }
static uint16_t primary_word(const RecordActionHooks *h,gaddr a) { uint16_t v=rd_u16(a); observe(h,RA_PRIMARY_WORD,v,0); return v; }
static uint32_t primary_long(const RecordActionHooks *h,gaddr a) { uint32_t v=rd_u32(a); observe(h,RA_PRIMARY_LONG,v,0); return v; }
static void word(const RecordActionHooks *h,gaddr a,uint16_t v) { wr_u16(a,v); observe(h,RA_STORE_WORD,v,0); }
static void longword(const RecordActionHooks *h,gaddr a,uint32_t v) { wr_u32(a,v); observe(h,RA_STORE_LONG,v,0); }
static void byte(const RecordActionHooks *h,gaddr a,uint8_t v) { wr_u8(a,v); observe(h,RA_STORE_BYTE,v,0); }
static int bit(const RecordActionHooks *h,uint32_t v,unsigned index) { observe(h,RA_BIT_TEST,v,index); return (v&(1u<<index))!=0; }
static uint32_t extend(const RecordActionHooks *h,uint16_t v) { uint32_t x=(uint32_t)(int32_t)(int16_t)v; observe(h,RA_PRIMARY_EXT_LONG,x,0); return x; }
static uint32_t shifted(uint32_t v,unsigned n) { return (uint32_t)((int32_t)v>>n); }
static void add_word(const RecordActionHooks *h,gaddr a,uint16_t n) { uint16_t v=rd_u16(a); wr_u16(a,(uint16_t)(v+n)); observe(h,RA_MEMORY_ADD_WORD,v,n); }
static void add_long(const RecordActionHooks *h,gaddr a,uint32_t n) { uint32_t v=rd_u32(a); wr_u32(a,v+n); observe(h,RA_MEMORY_ADD_LONG,v,n); }
static void subtract_long(const RecordActionHooks *h,gaddr a,uint32_t n) { uint32_t v=rd_u32(a); wr_u32(a,v-n); observe(h,RA_MEMORY_SUB_LONG,v,n); }
static void prepare_index(const RecordActionHooks *h,gaddr frame) { extend(h,primary_word(h,frame+10)); }
static void wrap_control_axis(const RecordActionHooks *h,gaddr frame,gaddr saved,unsigned cell_offset) {
    gaddr a=lookup(h,rd_u32(saved)),record; uint32_t position=rd_u32(a); uint16_t cell;
    observe(h,RA_TEST_LONG,position,0);
    if((int32_t)position<0) {
        record=lookup(h,rd_u32(SELECTED_CONTROL)); cell=primary_word(h,record+cell_offset); observe(h,RA_PRIMARY_SUB_WORD,1,0); --cell;
        record=lookup(h,rd_u32(SELECTED_CONTROL)); word(h,record+cell_offset,cell); observe(h,RA_TEST_WORD,cell,0);
        if((int16_t)cell<0) word(h,record+cell_offset,127);
        a=lookup(h,rd_u32(saved)); add_long(h,a,0x400000);
    } else {
        a=lookup(h,rd_u32(saved)); observe(h,RA_COMPARE_LONG,rd_u32(a),0x400000);
        if((int32_t)rd_u32(a)<0x400000) return;
        record=lookup(h,rd_u32(SELECTED_CONTROL)); cell=primary_word(h,record+cell_offset); observe(h,RA_PRIMARY_ADD_WORD,1,0); ++cell;
        record=lookup(h,rd_u32(SELECTED_CONTROL)); word(h,record+cell_offset,cell); observe(h,RA_COMPARE_WORD,cell,127);
        if((int16_t)cell>127) word(h,record+cell_offset,0);
        a=lookup(h,rd_u32(saved)); subtract_long(h,a,0x400000);
    }
    (void)frame;
}
void advance_control_record_action(gaddr frame,const RecordActionHooks *h) {
    gaddr flags,record,height,x,z; uint16_t value; uint32_t result; unsigned i;
    flags=lookup(h,rd_u32(SELECTED_CONTROL)); flags=lookup(h,flags+38);
    record=rd_u32(SELECTED_CONTROL); observe(h,RA_OTHER_RECORD,record,0); value=primary_word(h,record+40); longword(h,frame-4,flags); observe(h,RA_TEST_WORD,value,0);
    if((int16_t)value<=0) goto expire_record;
    longword(h,frame-10,record); height=record+4; observe(h,RA_NEXT_OTHER_RECORD,height,0);
    z=rd_u32(SELECTED_CONTROL); observe(h,RA_HEIGHT_CURSOR,z,0); z+=8; observe(h,RA_HEIGHT_CURSOR,z,0);
    value=primary_word(h,flags); longword(h,frame-14,height); longword(h,frame-18,z);
    if(!bit(h,value,1)) {
        prepare_index(h,frame); result=consume(h,RA_CHECK_RECORD); observe(h,RA_TEST_LONG,result,0);
        if(result) {
            flags=lookup(h,rd_u32(frame-4)); value=primary_word(h,flags); observe(h,RA_PRIMARY_AND_WORD,0xfff7,0); value&=0xfff7;
            observe(h,RA_PRIMARY_OR_WORD,0x12,0); word(h,flags,value|0x12); record=lookup(h,rd_u32(SELECTED_CONTROL)); word(h,record+40,2); goto draw_record;
        }
        height=lookup(h,rd_u32(frame-14)); result=rd_u32(height); observe(h,RA_TEST_LONG,result,0);
        if((int32_t)result<0) {
            flags=lookup(h,rd_u32(frame-4)); value=primary_word(h,flags); observe(h,RA_PRIMARY_AND_WORD,0xffe7,0); value&=0xffe7;
            observe(h,RA_PRIMARY_OR_WORD,2,0); word(h,flags,value|2); record=lookup(h,rd_u32(SELECTED_CONTROL)); word(h,record+40,2);
            record=lookup(h,rd_u32(SELECTED_CONTROL)); result=primary_long(h,record+16); observe(h,RA_PRIMARY_ASR_LONG,8,0); result=shifted(result,8);
            word(h,0xc45ad0u,(uint16_t)result); observe(h,RA_TEST_WORD,result,0); if(!(uint16_t)result) goto draw_record;
            prepare_index(h,frame); result=consume(h,RA_CHECK_GROUND); observe(h,RA_TEST_LONG,result,0); if(!result) goto draw_record;
            flags=lookup(h,rd_u32(frame-4)); value=primary_word(h,flags); observe(h,RA_PRIMARY_OR_WORD,0x80,0); word(h,flags,value|0x80); goto draw_record;
        }
        wrap_control_axis(h,frame,frame-10,48); wrap_control_axis(h,frame,frame-18,50);
    } else {
        flags=lookup(h,rd_u32(frame-4)); value=primary_word(h,flags);
        if(bit(h,value,8)) for(i=0;i<3;++i) {
            result=primary_long(h,0xc461c2u+4*i); observe(h,RA_PRIMARY_ASR_LONG,2,0); result=shifted(result,2);
            x=lookup(h,rd_u32(i==0?frame-10:i==1?frame-14:frame-18)); add_long(h,x,result);
        }
    }
draw_record:
    flags=lookup(h,rd_u32(frame-4)); value=primary_word(h,flags); observe(h,RA_PRIMARY_AND_WORD,0x1002,0); value&=0x1002; observe(h,RA_TEST_WORD,value,0);
    if(value) { prepare_index(h,frame); consume(h,RA_DRAW_SPECIAL); return; }
    flags=lookup(h,rd_u32(frame-4)); value=primary_word(h,flags);
    if(bit(h,value,13)) { prepare_index(h,frame); consume(h,RA_DRAW_TRACKED); return; }
    prepare_index(h,frame); consume(h,RA_DRAW_STANDARD); word(h,frame-6,rd_u16(0xc45ab6u)); return;
expire_record:
    flags=lookup(h,rd_u32(frame-4)); value=primary_word(h,flags); observe(h,RA_PRIMARY_AND_WORD,0xfffe,0); word(h,flags,value&0xfffe);
    for(i=0;i<2;++i) {
        result=extend(h,primary_word(h,frame+10)); observe(h,RA_PRIMARY_ADD_LONG,1,0); ++result;
        value=rd_u8(0xc4588cu+i); observe(h,RA_SECONDARY_BYTE,value,0); observe(h,RA_SECONDARY_EXT_WORD,(uint16_t)(int16_t)(int8_t)value,0);
        observe(h,RA_SECONDARY_EXT_LONG,(uint32_t)(int32_t)(int8_t)value,0); observe(h,RA_COMPARE_LONG,(uint32_t)(int32_t)(int8_t)value,result);
        if((uint32_t)(int32_t)(int8_t)value==result) byte(h,0xc4588cu+i,0);
    }
}
static uint32_t position_component(gaddr coefficients,int16_t x,int16_t y,int16_t z,unsigned shift) {
    uint32_t v=(uint32_t)((int32_t)rd_s16(coefficients)*x);
    v+=(uint32_t)((int32_t)rd_s16(coefficients+2)*y); v+=(uint32_t)((int32_t)rd_s16(coefficients+4)*z); return shifted(v,shift);
}
void initialise_control_record_action(gaddr frame,const RecordActionHooks *h) {
    gaddr flags,record; uint16_t value; uint32_t v,mode; unsigned i;
    flags=lookup(h,rd_u32(SELECTED_CONTROL)); flags=lookup(h,flags+38); value=primary_word(h,flags); longword(h,frame-10,flags);
    if(bit(h,value,12)) {
        observe(h,RA_PRIMARY_LONG,0,0); word(h,frame-4,0xfff8); record=lookup(h,rd_u32(SELECTED_CONTROL)); word(h,record+40,30); word(h,frame-6,0); word(h,frame-2,0);
    } else {
        flags=lookup(h,rd_u32(frame-10)); value=primary_word(h,flags);
        if(bit(h,value,13)) {
            word(h,frame-2,0); word(h,frame-4,0xfff8); word(h,frame-6,0xffc8); record=lookup(h,rd_u32(SELECTED_CONTROL)); word(h,record+40,60);
        } else {
            value=rd_u8(0xc461e6u); observe(h,RA_PRIMARY_BYTE,value,0); observe(h,RA_COMPARE_BYTE,value,17);
            if(value==17) { observe(h,RA_PRIMARY_LONG,0,0); word(h,frame-6,56); word(h,frame-4,0); word(h,frame-2,0); }
            else { word(h,frame-2,6); word(h,frame-4,1); word(h,frame-6,24); }
            record=lookup(h,rd_u32(SELECTED_CONTROL)); word(h,record+40,20);
        }
    }
    observe(h,RA_LAUNCH_POSITION,frame,0);
    for(i=0;i<3;++i) wr_u32(CONTROL_VECTOR+4*i,position_component(0xc46216u+6*i,rd_s16(frame-2),rd_s16(frame-4),rd_s16(frame-6),6));
    record=lookup(h,rd_u32(SELECTED_CONTROL)); word(h,record+48,rd_u16(0xc4618au)); record=lookup(h,rd_u32(SELECTED_CONTROL)); word(h,record+50,rd_u16(0xc4618cu));
    v=primary_long(h,0xc46198u); observe(h,RA_PRIMARY_AND_LONG,0x3fffff,0); v&=0x3fffff;
    mode=rd_u32(CONTROL_VECTOR); observe(h,RA_SECONDARY_LONG,mode,0); observe(h,RA_SECONDARY_ADD_LONG,v,0);
    record=lookup(h,rd_u32(SELECTED_CONTROL)); longword(h,record,mode+v);
    v=primary_long(h,CONTROL_VECTOR+4); observe(h,RA_PRIMARY_ADD_LONG,rd_u32(0xc4619cu),0); v+=rd_u32(0xc4619cu);
    record=lookup(h,rd_u32(SELECTED_CONTROL)); longword(h,record+4,v);
    v=primary_long(h,0xc461a0u); observe(h,RA_PRIMARY_AND_LONG,0x3fffff,0); v&=0x3fffff;
    mode=rd_u32(CONTROL_VECTOR+8); observe(h,RA_SECONDARY_LONG,mode,0); observe(h,RA_SECONDARY_ADD_LONG,v,0);
    record=lookup(h,rd_u32(SELECTED_CONTROL)); longword(h,record+8,mode+v);
    flags=lookup(h,rd_u32(frame-10)); value=primary_word(h,flags); observe(h,RA_PRIMARY_AND_WORD,0x3000,0); value&=0x3000; observe(h,RA_TEST_WORD,value,0);
    if(value) { word(h,frame-4,0xffc0); word(h,frame-6,0); }
    else {
        word(h,frame-4,64); word(h,frame-6,1024); mode=extend(h,primary_word(h,0xc458dau)); observe(h,RA_PRIMARY_AND_LONG,3,0); mode&=3;
        observe(h,RA_COMPARE_LONG,mode,2); if(mode==2) add_word(h,frame-4,5);
        else { observe(h,RA_COMPARE_LONG,mode,1); if(mode==1) { uint16_t old=rd_u16(frame-4); wr_u16(frame-4,(uint16_t)(old-3)); observe(h,RA_MEMORY_SUB_WORD,old,3); }
            else { observe(h,RA_TEST_LONG,mode,0); if(!mode) add_word(h,frame-4,3); } }
    }
    observe(h,RA_LAUNCH_VELOCITY,frame,0);
    for(i=0;i<3;++i) wr_u32(CONTROL_VECTOR+4*i,position_component(0xc46216u+6*i,0,rd_s16(frame-4),rd_s16(frame-6),4));
    word(h,0xc459b8u,0); prepare_index(h,frame); consume(h,RA_INITIALISE_DIRECTION);
}
void aim_control_record_action(gaddr frame,const RecordActionHooks *h) {
    uint32_t mode,v; uint16_t heading,value; gaddr a,flags; unsigned i;
    mode=extend(h,primary_word(h,0xc458dau)); observe(h,RA_PRIMARY_AND_LONG,3,0); mode&=3;
    observe(h,RA_COMPARE_LONG,mode,2);
    if(mode==2) word(h,frame-4,0);
    else { observe(h,RA_COMPARE_LONG,mode,1); if(mode==1) word(h,frame-4,16);
        else { observe(h,RA_TEST_LONG,mode,0); word(h,frame-4,mode?0xfff0:0); } }
    value=primary_word(h,0xc459b8u); observe(h,RA_ORIENTATION_BASE,value,0); a=0xc46184u+((uint32_t)(int32_t)(int16_t)value<<9);
    heading=primary_word(h,a+108); observe(h,RA_PRIMARY_ASR_WORD,6,0); heading=(uint16_t)(rd_s16(a+108)>>6);
    flags=lookup(h,rd_u32(SELECTED_CONTROL)); value=rd_u16(flags+38); observe(h,RA_SECONDARY_WORD,value,0); word(h,frame-6,heading);
    if(bit(h,value,12)) word(h,frame-2,13);
    else { flags=lookup(h,rd_u32(SELECTED_CONTROL)); value=primary_word(h,flags+38);
        if(bit(h,value,13)) word(h,frame-2,5);
        else { value=primary_word(h,frame-4); observe(h,RA_PRIMARY_ADD_WORD,198,0); value+=198; observe(h,RA_PRIMARY_ADD_WORD,rd_u16(frame-6),0); word(h,frame-2,(uint16_t)(value+rd_u16(frame-6))); } }
    observe(h,RA_TARGET_RELATIVE,frame,0); a=rd_u32(SELECTED_CONTROL);
    for(i=0;i<3;++i) {
        v=rd_u32(0xc46198u+4*i); if(i!=1) v&=0x3fffff; v+=rd_u32(CONTROL_VECTOR+4*i); v-=rd_u32(a+4*i);
        wr_u32(frame-10-4*i,v); wr_u16(frame-20-2*i,(uint16_t)shifted(v,8));
    }
    consume(h,RA_TRANSFORM_DIRECTION);
}
void publish_control_record_direction(gaddr frame,const RecordActionHooks *h) {
    gaddr record; uint16_t value; uint32_t v; unsigned i;
    observe(h,RA_DIRECTION_ARGUMENTS,frame,0); consume(h,RA_PROJECT_DIRECTION);
    for(i=0;i<3;++i) {
        value=rd_u16(0xc45a4cu+2*i); observe(h,i==1?RA_SECONDARY_WORD:RA_PRIMARY_WORD,value,0);
        v=(uint32_t)(int32_t)(int16_t)value; observe(h,i==1?RA_SECONDARY_EXT_LONG:RA_PRIMARY_EXT_LONG,v,0); longword(h,frame-12+4*i,v);
        observe(h,i==1?RA_SECONDARY_ASL_LONG:RA_PRIMARY_ASL_LONG,3,0); record=lookup(h,rd_u32(SELECTED_CONTROL)); longword(h,record+12+4*i,v<<3);
    }
    for(i=0;i<3;++i) {
        v=rd_u32(frame-12+4*i); observe(h,i==1?RA_PRIMARY_LONG:RA_SECONDARY_LONG,v,0);
        record=lookup(h,rd_u32(SELECTED_CONTROL)); longword(h,record+24+4*i,v);
    }
    word(h,record+36,0); record=lookup(h,rd_u32(SELECTED_CONTROL)); value=primary_word(h,record+38); observe(h,RA_PRIMARY_AND_WORD,0x3000,0); value&=0x3000; observe(h,RA_TEST_WORD,value,0);
    if(!value) return;
    for(i=0;i<3;++i) { v=primary_long(h,0xc461c2u+4*i); if(i) record=lookup(h,rd_u32(SELECTED_CONTROL)); add_long(h,record+12+4*i,v); }
    for(i=0;i<3;++i) { v=primary_long(h,0xc461c2u+4*i); observe(h,RA_PRIMARY_ASR_LONG,3,0); record=lookup(h,rd_u32(SELECTED_CONTROL)); add_long(h,record+24+4*i,shifted(v,3)); }
}
void start_control_record_alert(gaddr frame,const RecordActionHooks *h) {
    gaddr alert; uint32_t value;
    value=rd_u32(0xc0a460u); observe(h,RA_TEST_LONG,value,0); if(!value) return;
    consume(h,RA_RESERVE_ALERT); alert=lookup(h,rd_u32(0xc0a460u)); observe(h,RA_PRIMARY_LONG,0,0); longword(h,alert+44,0);
    longword(h,0xc50c74u,rd_u32(frame+8)); observe(h,RA_PRIMARY_LONG,16,0); value=rd_u32(frame+12); observe(h,RA_SECONDARY_LONG,value,0); observe(h,RA_SECONDARY_ASL_LONG,16,0);
    longword(h,0xc50c7cu,value<<16); observe(h,RA_PRIMARY_LONG,0,0); longword(h,alert+52,0); observe(h,RA_PRIMARY_LONG,1,0); longword(h,alert+44,1); consume(h,RA_START_ALERT);
}
void attenuate_control_record_offset(gaddr frame,const RecordActionHooks *h) {
    uint16_t sum,size,other,value;
    sum=primary_word(h,frame+10); other=rd_u16(frame+14); observe(h,RA_PRIMARY_ADD_WORD,other,0); sum+=other;
    word(h,frame-2,sum); word(h,frame-4,sum); observe(h,RA_TEST_WORD,sum,0); size=sum;
    if((int16_t)sum<0) { size=(uint16_t)(0u-sum); wr_u16(frame-4,size); observe(h,RA_MEMORY_NEG_WORD,sum,0); }
    observe(h,RA_COMPARE_WORD,size,4);
    if((int16_t)size>4) { sum=primary_word(h,frame-2); observe(h,RA_PRIMARY_ASR_WORD,2,0); sum=(uint16_t)((int16_t)sum>>2); word(h,frame-2,sum); }
    else { observe(h,RA_COMPARE_WORD,size,2); if((int16_t)size>2) { sum=primary_word(h,frame-2); observe(h,RA_PRIMARY_ASR_WORD,1,0); sum=(uint16_t)((int16_t)sum>>1); word(h,frame-2,sum); } }
    value=primary_word(h,frame-2); observe(h,RA_PRIMARY_SUB_WORD,rd_u16(frame+14),0); value-=rd_u16(frame+14); word(h,frame+10,value); extend(h,value);
}
