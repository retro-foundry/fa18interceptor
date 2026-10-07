#include "postflight_scheduler.h"
#include "globals.h"
#include <stdlib.h>

static void observe(const PostflightScheduleHooks *h, enum PostflightSchedulePhase p,
                    uint32_t value, uint32_t limit, gaddr address) {
    if (h && h->observe) h->observe(h->context,p,value,limit,address);
}
static PostflightScheduleResult consume(const PostflightScheduleHooks *h,
                                        enum PostflightScheduleChild child,gaddr record) {
    if(!h || !h->consume) abort();
    return h->consume(h->context,child,record);
}
static uint8_t test_byte(const PostflightScheduleHooks *h,gaddr a) {
    uint8_t v=rd_u8(a); observe(h,SCHEDULE_BYTE_TEST,v,0,a); return v;
}
static uint16_t test_word(const PostflightScheduleHooks *h,gaddr a) {
    uint16_t v=rd_u16(a); observe(h,SCHEDULE_WORD_TEST,v,0,a); return v;
}
static int bit(const PostflightScheduleHooks *h,gaddr a,unsigned n) {
    uint8_t v=rd_u8(a); observe(h,SCHEDULE_BIT_TEST,v,n,a); return (v>>n)&1;
}
static int compare_byte(const PostflightScheduleHooks *h,uint8_t v,uint8_t n) {
    observe(h,SCHEDULE_BYTE_COMPARE,v,n,0); return (int8_t)v-(int8_t)n;
}
static int compare_word(const PostflightScheduleHooks *h,uint16_t v,uint16_t n) {
    observe(h,SCHEDULE_WORD_COMPARE,v,n,0); return (int16_t)v-(int16_t)n;
}
static void store_byte(const PostflightScheduleHooks *h,gaddr a,uint8_t v) {
    wr_u8(a,v); observe(h,SCHEDULE_BYTE_STORE,v,0,a);
}
static void store_word(const PostflightScheduleHooks *h,gaddr a,uint16_t v) {
    wr_u16(a,v); observe(h,SCHEDULE_WORD_STORE,v,0,a);
}
static uint16_t event_byte(const PostflightScheduleHooks *h,uint16_t event,uint8_t v) {
    observe(h,SCHEDULE_EVENT_BYTE,v,0,0); return (event&0xff00u)|v;
}
static void phase_three(const PostflightScheduleHooks *h,uint16_t event) {
    store_word(h,PHASE_WORD,event); store_byte(h,SEQUENCE_PHASE,3);
    store_byte(h,SEQUENCE_STEP,4); store_byte(h,CONTEXT_GATE,1);
}
static void outcome(const PostflightScheduleHooks *h,uint8_t phase,unsigned event) {
    store_byte(h,PLAYER_PHASE,phase);
    observe(h,event==4?SCHEDULE_EVENT_FOUR:event==7?SCHEDULE_EVENT_SEVEN:SCHEDULE_EVENT_ZERO,event,0,0);
    phase_three(h,(uint16_t)event);
}
static void existing_phase(const PostflightScheduleHooks *h,uint16_t event) {
    uint8_t value=rd_u8(PLAYER_PHASE);
    PostflightScheduleResult result;
    event=event_byte(h,event,value);
    if(compare_byte(h,value,0xff)) {
        observe(h,SCHEDULE_EVENT_DECREMENT,value,0,0);
        if((uint8_t)(value-1)) return;
    }
    result=consume(h,SCHEDULE_READY_GATE,0);
    if(result.zero) outcome(h,0xfc,0);
}
static int start_or_continue(const PostflightScheduleHooks *h,uint16_t event) {
    if(!test_byte(h,PLAYER_PHASE)) return 1;
    existing_phase(h,event); return 0;
}
static uint8_t countdown(const PostflightScheduleHooks *h,unsigned initial) {
    uint8_t old=test_byte(h,POST_INPUT_EVENT);
    if(!old) { old=(uint8_t)initial; store_byte(h,POST_INPUT_EVENT,old); }
    wr_u8(POST_INPUT_EVENT,(uint8_t)(old-1));
    observe(h,SCHEDULE_BYTE_DECREMENT,old,0,POST_INPUT_EVENT);
    return (uint8_t)(old-1);
}
static void save_view(const PostflightScheduleHooks *h,gaddr record,uint8_t refresh) {
    uint8_t old;
    if(compare_byte(h,rd_u8(POST_INPUT_EVENT),1)>0 || bit(h,SCHEDULE_SAVED_VIEW,7)) return;
    store_byte(h,SCHEDULE_SAVED_VIEW,rd_u8(VIEW_SIDE));
    old=rd_u8(SCHEDULE_SAVED_VIEW); wr_u8(SCHEDULE_SAVED_VIEW,old|0x80);
    observe(h,SCHEDULE_BIT_SET,old,7,SCHEDULE_SAVED_VIEW);
    store_byte(h,0xc45834u,rd_u8(CONTEXT_SELECT));
    if(!bit(h,record+1,6)) return;
    store_byte(h,VIEW_SIDE,7); store_byte(h,CONTEXT_SMOOTH,1);
    store_byte(h,CONTEXT_STARTED,1); store_byte(h,0xc457adu,0);
    if(test_byte(h,CONTEXT_SELECT)) return;
    store_word(h,0xc4592au,rd_u16(record+0x68));
    { uint16_t word=rd_u16(COMMAND_WORD); wr_u16(COMMAND_WORD,word|2);
      observe(h,SCHEDULE_WORD_OR,word,2,COMMAND_WORD); }
    store_byte(h,0xc45833u,refresh);
}
static void prepare_index(const PostflightScheduleHooks *h,enum PostflightScheduleChild child) {
    observe(h,SCHEDULE_PREPARE_INDEX,rd_u16(STREAM_MODE),0,0);
    consume(h,child,0);
}
static void record_mode(const PostflightScheduleHooks *h,uint16_t event,int seven) {
    gaddr record=CONTROL_RECORDS+0x800u;
    uint16_t value;
    uint8_t remaining;
    if(!start_or_continue(h,event)) return;
    observe(h,SCHEDULE_RECORD,0,0,record); store_word(h,STREAM_MODE,4);
    value=rd_u16(record+6); observe(h,SCHEDULE_EVENT_WORD,value,0,0);
    value|=rd_u16(record+12); observe(h,SCHEDULE_WORD_OR,value,0,0);
    if(!value) return;
    if(bit(h,record+1,6)) {
        if(seven) { if(bit(h,record+1,7)) return; }
        else {
            if(!bit(h,record+3,7) || compare_word(h,rd_u16(record+0x6c),0x320)>0 ||
               bit(h,CONTROL_RECORDS+0x1001,6) || bit(h,CONTROL_RECORDS+0x1401,6)) return;
        }
    }
    remaining=countdown(h,seven?2:5);
    if(remaining) { save_view(h,record,seven?3:0xff); return; }
    if(seven) {
        if(bit(h,record+0x20,1)) {
            if(compare_byte(h,rd_u8(SEQUENCE_PHASE),3)) outcome(h,0xff,0);
        } else {
            prepare_index(h,SCHEDULE_PREPARE_SEVEN);
            if(compare_byte(h,rd_u8(SEQUENCE_PHASE),3)) outcome(h,0xfe,7);
        }
    } else if(bit(h,record+1,6)) {
        prepare_index(h,SCHEDULE_PREPARE_FOUR);
        if(compare_byte(h,rd_u8(SEQUENCE_PHASE),3)) outcome(h,0xff,4);
    } else if(compare_byte(h,rd_u8(SEQUENCE_PHASE),3)) {
        outcome(h,bit(h,record+0x20,7)?0xfd:0xfe,0);
    }
}
static void generic_mode(const PostflightScheduleHooks *h,uint16_t event) {
    uint8_t value=rd_u8(SCENE_DISPATCH_ADMITTED);
    event=event_byte(h,event,value);
    if(compare_byte(h,value,rd_u8(SCENE_DISPATCH_AUX))>0) return;
    if(countdown(h,2) || !compare_byte(h,rd_u8(SEQUENCE_PHASE),3)) return;
    /* Unlike the other modes C0A39E does not clear the event word. */
    store_byte(h,PLAYER_PHASE,0xff); phase_three(h,event);
}
static void mode_five(const PostflightScheduleHooks *h,uint16_t event) {
    gaddr first=CONTROL_RECORDS+0x800,second=CONTROL_RECORDS+0xc00;
    uint32_t position[3],reference[3],limit,delta;
    unsigned i;
    uint16_t gate;
    if(!start_or_continue(h,event)) return;
    observe(h,SCHEDULE_PAIR,0,0,first);
    if(!bit(h,first+1,6)) {
        if(!bit(h,second+1,6)) goto admit;
        if(!bit(h,second+1,3)) goto gate_test;
        { gaddr swap=first; first=second; second=swap; }
        observe(h,SCHEDULE_PAIR_SWAP,0,0,0);
    } else if(!bit(h,first+1,3)) goto gate_test;
    if(compare_word(h,rd_u16(first+6),0x70)>=0) {
        if(compare_byte(h,rd_u8(SEQUENCE_PHASE),3)) outcome(h,0xfe,0);
        return;
    }
    for(i=0;i<3;++i) position[i]=rd_u32(first+0x14+4*i);
    observe(h,SCHEDULE_POSITION_LOAD,0,0,first+0x14);
    for(i=0;i<3;++i) reference[i]=rd_u32(CONTROL_RECORDS+0x14+4*i);
    observe(h,SCHEDULE_REFERENCE_LOAD,0,0,CONTROL_RECORDS+0x14);
    if(compare_byte(h,rd_u8(0xc458a7u),3)>=0) limit=0x18000;
    else if(compare_byte(h,rd_u8(0xc458a7u),2)>=0) limit=0x24000;
    else limit=0x30000;
    observe(h,SCHEDULE_DISTANCE_LIMIT,limit,0,0);
    for(i=0;i<3;++i) {
        delta=reference[i]-position[i];
        observe(h,SCHEDULE_DISTANCE_SUBTRACT,position[i],i,0);
        /* SUB.L/BGE tests the unwrapped signed difference, not delta's sign. */
        if((int64_t)(int32_t)reference[i]-(int32_t)position[i]<0) {
            delta=0u-delta; observe(h,SCHEDULE_DISTANCE_NEGATE,0,i,0);
        }
        observe(h,SCHEDULE_DISTANCE_COMPARE,delta,limit,i);
        if((int32_t)delta>(int32_t)limit) goto reset_gate;
    }
    gate=rd_u16(SCENE_DISPATCH_GATE); observe(h,SCHEDULE_EVENT_WORD,gate,0,0);
    if((int16_t)gate>=0) {
        wr_u16(SCENE_DISPATCH_GATE,(uint16_t)(gate-1));
        observe(h,SCHEDULE_WORD_DECREMENT,gate,0,SCENE_DISPATCH_GATE); goto gate_test;
    }
    observe(h,SCHEDULE_RESTORE_RECORD_SELECT,0,0,first);
    consume(h,SCHEDULE_RESTORE_FIRST,first);
    observe(h,SCHEDULE_RESTORE_RECORD_SELECT,0,0,second);
    consume(h,SCHEDULE_RESTORE_SECOND,second);
reset_gate:
    if((int16_t)test_word(h,SCENE_DISPATCH_GATE)>=0) store_word(h,SCENE_DISPATCH_GATE,0xc8);
gate_test:
    if((int16_t)test_word(h,SCENE_DISPATCH_GATE)>=0) return;
admit:
    event=event_byte(h,event,rd_u8(SCENE_DISPATCH_ADMITTED));
    if(compare_byte(h,(uint8_t)event,rd_u8(SCENE_DISPATCH_AUX))>0) return;
    if(countdown(h,2) || !compare_byte(h,rd_u8(SEQUENCE_PHASE),3)) return;
    outcome(h,0xff,0);
}
static void mode_six(const PostflightScheduleHooks *h,uint16_t event) {
    uint16_t offset;
    uint32_t x,z,px,pz,delta;
    gaddr record;
    if(!start_or_continue(h,event)) return;
    offset=rd_u16(SCHEDULE_TARGET); observe(h,SCHEDULE_EVENT_WORD,offset,0,0);
    if(!offset) return;
    record=CONTROL_RECORDS+(gaddr)(int32_t)(int16_t)offset;
    observe(h,SCHEDULE_TARGET_RECORD,offset,0,record);
    if(!bit(h,record,7) || (int16_t)test_word(h,record+0x4c)>0) return;
    x=rd_u32(CONTROL_RECORDS+0x1614); z=rd_u32(CONTROL_RECORDS+0x161c);
    observe(h,SCHEDULE_TARGET_POSITION,0,0,CONTROL_RECORDS+0x1614);
    px=rd_u32(record+0x14); pz=rd_u32(record+0x1c);
    delta=x-px; observe(h,SCHEDULE_TARGET_SUBTRACT_X,px,0,0);
    if((int64_t)(int32_t)x-(int32_t)px<0) { delta=0u-delta; observe(h,SCHEDULE_TARGET_NEGATE_X,0,0,0); }
    x=delta;
    delta=z-pz; observe(h,SCHEDULE_TARGET_SUBTRACT_Z,pz,0,0);
    if((int64_t)(int32_t)z-(int32_t)pz<0) { delta=0u-delta; observe(h,SCHEDULE_TARGET_NEGATE_Z,0,0,0); }
    observe(h,SCHEDULE_TARGET_COMPARE_X,x,0x10000,0);
    if((int32_t)x>0x10000) goto outside;
    observe(h,SCHEDULE_TARGET_COMPARE_Z,delta,0x10000,0);
    if((int32_t)delta>0x10000) goto outside;
    if(compare_byte(h,rd_u8(SEQUENCE_PHASE),3)) outcome(h,0xff,0);
    return;
outside:
    if(compare_byte(h,rd_u8(SEQUENCE_PHASE),3)) outcome(h,0xfe,0);
}
PostflightScheduleResult postflight_player_readiness(const PostflightScheduleHooks *h) {
    const gaddr record=CONTROL_RECORDS;
    uint8_t value;
    observe(h,SCHEDULE_PLAYER_RECORD,0,0,record);
    if(!bit(h,record+0x21,0) || !bit(h,record+3,7)) goto not_ready;
    if(compare_byte(h,rd_u8(0xc45848u),3)) {
        if(!bit(h,record+4,2)) goto not_ready;
    } else {
        value=rd_u8(record+4); observe(h,SCHEDULE_EVENT_BYTE,value,0,0);
        value&=0xc0; observe(h,SCHEDULE_EVENT_MASK_BYTE,value,0,0);
        if(!value) goto not_ready;
    }
    if(test_word(h,record+0x6e)) goto not_ready;
    observe(h,SCHEDULE_EVENT_ZERO,0,0,0); return (PostflightScheduleResult){0,1};
not_ready:
    observe(h,SCHEDULE_EVENT_ONE,1,0,0); return (PostflightScheduleResult){1,0};
}
void schedule_postflight(enum PostflightSchedule mode,uint16_t event,gaddr record,
                        const PostflightScheduleHooks *h) {
    uint8_t value;
    uint16_t word;
    switch(mode) {
    case POSTFLIGHT_DISPATCH:
        store_byte(h,SPACE_COMMAND_LATCH,0); store_byte(h,SPACE_COMMAND_LATCH+1,0);
        store_byte(h,PAIR_OVERRIDE,0); consume(h,SCHEDULE_SELECTION_GATE,0);
        if(!bit(h,SCHEDULE_STATUS,6) || test_byte(h,SCHEDULE_BLOCKED)) return;
        value=rd_u8(MODE_SELECT); event=event_byte(h,event,value);
        if(compare_byte(h,value,2)<=0) return;
        { static const uint8_t modes[]={3,4,5,6,7,9,125}; unsigned i;
          for(i=0;i<7;++i) if(!compare_byte(h,value,modes[i])) {
              consume(h,(enum PostflightScheduleChild)(SCHEDULE_THREE+i),0); return;
          }
          consume(h,SCHEDULE_OTHER,0); }
        return;
    case POSTFLIGHT_MODE_THREE:
        if(!start_or_continue(h,event) || (int8_t)test_byte(h,PLAYER_FLAGS_F)>=0 ||
           !compare_byte(h,rd_u8(SEQUENCE_PHASE),3)) return;
        outcome(h,0xff,0); return;
    case POSTFLIGHT_MODE_FOUR: record_mode(h,event,0); return;
    case POSTFLIGHT_MODE_FIVE: mode_five(h,event); return;
    case POSTFLIGHT_MODE_SIX: mode_six(h,event); return;
    case POSTFLIGHT_MODE_SEVEN: record_mode(h,event,1); return;
    case POSTFLIGHT_MODE_NINE:
        if(test_byte(h,PLAYER_PHASE)) return;
        record=CONTROL_RECORDS; observe(h,SCHEDULE_RECORD,0,0,record);
        if(!bit(h,record+1,6)) return;
        word=rd_u16(record+2); observe(h,SCHEDULE_EVENT_WORD,word,0,0);
        word&=0xc080; observe(h,SCHEDULE_EVENT_MASK_WORD,word,0,0);
        if(compare_word(h,word,0xc080) || test_word(h,record+0x6e) ||
           !compare_byte(h,rd_u8(SEQUENCE_PHASE),3)) return;
        outcome(h,0xff,0); return;
    case POSTFLIGHT_MODE_125:
        if(test_byte(h,PLAYER_PHASE)) return;
        record=CONTROL_RECORDS+0x800; observe(h,SCHEDULE_RECORD,0,0,record);
        if(bit(h,record+1,6) || !compare_byte(h,rd_u8(SEQUENCE_PHASE),3)) return;
        outcome(h,0xfe,0); return;
    case POSTFLIGHT_MODE_OTHER:
        if(start_or_continue(h,event)) generic_mode(h,event);
        return;
    case POSTFLIGHT_RESTORE_RECORD:
        observe(h,SCHEDULE_EVENT_ZERO,0,0,0);
        value=rd_u8(record+0x3a); observe(h,SCHEDULE_RESTORE_KIND,value,0,0);
        { gaddr table=VIEW_PARAMETER_TABLE;
          unsigned i;
          int16_t words[5];
          observe(h,SCHEDULE_RESTORE_TABLE,value,0,table);
          table+=(gaddr)(int32_t)rd_s16(table+2u*value);
          for(i=0;i<5;++i) words[i]=rd_s16(table+2*i);
          observe(h,SCHEDULE_RESTORE_VALUES,0,0,table);
          for(i=0;i<4;++i) store_word(h,record+0x2c+2*i,(uint16_t)words[i]);
          wr_u32(record+0x34,(uint32_t)(int32_t)words[4]);
          observe(h,SCHEDULE_LONG_STORE,(uint32_t)(int32_t)words[4],0,record+0x34); }
        return;
    case POSTFLIGHT_PLAYER_READY:
        (void)postflight_player_readiness(h); return;
    }
    abort();
}
