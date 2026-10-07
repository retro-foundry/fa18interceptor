/* Shared context actions at C1B664-C1B778 and C1BF8C-C1C0C8. */
#include "context_commands.h"
#include "globals.h"
#include <stdlib.h>

#define CONTEXT_VIEW_REQUEST 0xc45833u
#define CONTEXT_MAP_MIDDLE_CACHE 0xc45664u
#define CONTEXT_POSITION_PRESETS 0xc42a54u

static void observe(const ContextCommandHooks *h,enum ContextCommandPhase phase,
                    uint32_t value,uint32_t limit,gaddr address) {
    if(h->observe) h->observe(h->context,phase,value,limit,address);
}
static uint8_t test(const ContextCommandHooks *h,gaddr address) {
    uint8_t value=rd_u8(address); observe(h,CONTEXT_BYTE_TEST,value,0,address); return value;
}
static void byte(const ContextCommandHooks *h,gaddr address,uint8_t value) {
    wr_u8(address,value); observe(h,CONTEXT_BYTE_STORE,value,0,address);
}
static void word(const ContextCommandHooks *h,gaddr address,uint16_t value) {
    wr_u16(address,value); observe(h,CONTEXT_WORD_STORE,value,0,address);
}
static void long_value(const ContextCommandHooks *h,gaddr address,uint32_t value) {
    wr_u32(address,value); observe(h,CONTEXT_LONG_STORE,value,0,address);
}
static void request(const ContextCommandHooks *h,unsigned bit) {
    uint8_t old=rd_u8(PENDING_COMMAND_WORD_B);
    wr_u8(PENDING_COMMAND_WORD_B,old|(1u<<bit));
    observe(h,CONTEXT_REQUEST_BIT,old,bit,PENDING_COMMAND_WORD_B);
}
static int16_t copy_record_angle(const ContextCommandHooks *h) {
    int16_t offset;
    observe(h,CONTEXT_RECORD_COPY_BEGIN,0,0,CONTROL_RECORDS);
    offset=rd_s16(VIEW_RECORD); observe(h,CONTEXT_RECORD_COPY_OFFSET,(uint16_t)offset,0,0);
    word(h,ORIGIN_ANGLE_HISTORY,rd_u16(CONTROL_RECORDS+(gaddr)(int32_t)offset+0x68));
    word(h,ORIGIN_STATUS_WORD,rd_u16(ORIGIN_STATUS_WORD)|2);
    byte(h,CONTEXT_VIEW_REQUEST,0xff);
    return offset;
}
static int16_t start_record_context(const ContextCommandHooks *h) {
    byte(h,ORIGIN_THRESHOLD_FLAG,1); byte(h,ORIGIN_GATE_MODE,0); return copy_record_angle(h);
}
static uint32_t swapped_scaled_word(int16_t word) {
    uint32_t value=(uint32_t)(int32_t)word;
    return ((value<<16)|(value>>16))<<6;
}
static ContextCommandExecution calculate_context(const ContextCommandHooks *h) {
    ContextCommandInput input={0,{0,0,0},{0,0,0}};
    ContextCommandResult result;
    ContextActionOutput output;
    int16_t offset,pose,v[6];
    unsigned i;
    request(h,2); byte(h,TRACK_STARTED,0xff);
    observe(h,CONTEXT_POSE_BEGIN,0,0,SCENE_POSE_TABLE);
    offset=(int16_t)((int16_t)rd_s8(SCENE_POSE_ENTRY)*16);
    observe(h,CONTEXT_POSE_INDEX,rd_u8(SCENE_POSE_ENTRY),0,0);
    pose=rd_s16(SCENE_POSE_TABLE+(gaddr)(int32_t)offset);
    observe(h,CONTEXT_POSE_VALUE,(uint16_t)pose,0,0);
    if(pose<0) {
        uint8_t kind;
        uint16_t displacement=(uint16_t)(((uint16_t)pose&0x7fffu)*512u);
        input.record=CONTROL_RECORDS+(gaddr)(int32_t)(int16_t)displacement;
        observe(h,CONTEXT_RECORD_SELECT,(uint16_t)pose,0,input.record);
        kind=rd_u8(input.record+0x62); observe(h,CONTEXT_RECORD_COMPARE,kind,0x20,0);
        input.local[0]=kind==0x20?-36:0;
        input.local[1]=kind==0x20?47:20;
        input.local[2]=kind==0x20?-48:-106;
        /* C091E0 preserves the selected local Y while transforming the point. */
        output=(ContextActionOutput){.kind=CONTEXT_ACTION_LOCAL_HEIGHT,.local_height=input.local[1]};
        observe(h,CONTEXT_LOCAL_POINT,kind,0,0);
        result=h->consume(h->context,CONTEXT_COMMAND_LOCAL_TO_WORLD,&input);
        for(i=0;i<3;++i) input.position[i]=result.position[i];
    } else {
        gaddr preset=CONTEXT_POSITION_PRESETS+(gaddr)(int32_t)offset;
        int16_t pair_offset;
        for(i=0;i<6;++i) v[i]=rd_s16(preset+2*i);
        pair_offset=(int16_t)((uint16_t)v[2]*4u);
        input.position[0]=(int32_t)(swapped_scaled_word(v[0])+
            ((uint32_t)(int32_t)rd_s16(GRID_ADJUST_WORDS+(gaddr)(int32_t)pair_offset)<<8)+
            ((uint32_t)(int32_t)v[4]<<8));
        input.position[2]=(int32_t)(swapped_scaled_word(v[1])+
            ((uint32_t)(int32_t)rd_s16(GRID_ADJUST_WORDS+(gaddr)(int32_t)pair_offset+2)<<8)+
            ((uint32_t)(int32_t)v[5]<<8));
        input.position[1]=(int32_t)((uint32_t)(int32_t)v[3]<<8);
        output=(ContextActionOutput){.kind=CONTEXT_ACTION_PRESET_X_DISPLACEMENT,
            .preset_x_displacement=(int32_t)((uint32_t)(int32_t)v[4]<<8)};
        observe(h,CONTEXT_PRESET_POSITION,0,0,preset);
    }
    result=h->consume(h->context,CONTEXT_COMMAND_SET_OBSERVER,&input);
    byte(h,ORIGIN_GATE_B,0);
    if(!test(h,ORIGIN_ENABLE)) output=(ContextActionOutput){.kind=CONTEXT_ACTION_VIEW_RECORD,
        .view_record=start_record_context(h)};
    else byte(h,TRACK_STARTED,0);
    return (ContextCommandExecution){result.event,output};
}
static uint32_t release_voices(const CommandRequest *r,const ContextCommandHooks *h,
                               enum ContextCommandChild child) {
    ContextCommandResult result;
    observe(h,CONTEXT_EVENT_SAVE,r->raw_event,0,0);
    result=h->consume(h->context,child,NULL);
    observe(h,CONTEXT_EVENT_RESTORE,r->raw_event,0,0);
    return (result.event&0xffff0000u)|(r->raw_event&0xffffu);
}
int is_context_command(enum CommandAction action) {
    return action==COMMAND_CONTEXT_SPECIAL || action==COMMAND_CONTEXT_REFRESH ||
           action==COMMAND_CONTEXT_CALCULATION || action==COMMAND_MAP || action==COMMAND_CONTEXT_REQUEST;
}
uint32_t execute_context_command(const CommandRequest *r,const ContextCommandHooks *h) {
    return execute_context_command_result(r,h).event;
}
ContextCommandExecution execute_context_command_result(const CommandRequest *r,const ContextCommandHooks *h) {
    uint32_t event=r->raw_event,middle,cursor;
    ContextActionOutput output={.kind=CONTEXT_ACTION_PRESERVE};
    if(!h || !h->consume || !is_context_command(r->action)) abort();
    switch(r->action) {
    case COMMAND_CONTEXT_SPECIAL: byte(h,KEY_TAKEN,2); /* fall through */
    case COMMAND_CONTEXT_REFRESH:
        observe(h,CONTEXT_MODIFIER_TEST,r->modifier,0,0);
        if(r->modifier) return calculate_context(h);
        request(h,1); observe(h,CONTEXT_ORIGIN_TEST,r->origin_mode,0,0);
        if(!r->origin_mode) byte(h,ORIGIN_DETAIL_INDEX,4);
        byte(h,ORIGIN_GATE_B,1);
        output=(ContextActionOutput){.kind=CONTEXT_ACTION_VIEW_RECORD,.view_record=start_record_context(h)};break;
    case COMMAND_CONTEXT_CALCULATION: return calculate_context(h);
    case COMMAND_MAP:
        if(test(h,ORIGIN_DETAIL_MODE)) break;
        request(h,0);
        if(test(h,ORIGIN_GATE_MODE)) {
            middle=rd_u32(SELECTOR_ORIGIN_MIDDLE);
            observe(h,CONTEXT_MAP_CACHED_POSITION,middle,0,0);
            long_value(h,CONTEXT_MAP_MIDDLE_CACHE,middle);
            byte(h,ORIGIN_GATE_B,1);
            output=(ContextActionOutput){.kind=CONTEXT_ACTION_VIEW_RECORD,.view_record=start_record_context(h)};break;
        }
        event=release_voices(r,h,CONTEXT_COMMAND_MAP_VOICES);
        middle=rd_u32(CONTEXT_MAP_MIDDLE_CACHE);
        observe(h,CONTEXT_MAP_POSITION,middle,0,0);
        long_value(h,SELECTOR_ORIGIN,0x10c00000);
        long_value(h,SELECTOR_ORIGIN_MIDDLE,middle);
        long_value(h,SELECTOR_ORIGIN_THIRD,0x11400000);
        observe(h,CONTEXT_MAP_NEGATE,middle,0,0);
        long_value(h,ORIGIN_NEGATED_COMPANION,0);
        long_value(h,ORIGIN_NEGATED_COMPANION+4,0u-middle);
        long_value(h,ORIGIN_NEGATED_COMPANION+8,0);
        long_value(h,ORIGIN_SMOOTHED_DELTA,0);
        long_value(h,ORIGIN_AUXILIARY_DELTA,0);
        long_value(h,ORIGIN_AUXILIARY_DELTA+4,0);
        word(h,VIEW_PAN,0x1c20); word(h,VIEW_ROTATE,0);
        byte(h,ORIGIN_THRESHOLD_FLAG,0); byte(h,ORIGIN_GATE_B,1); byte(h,ORIGIN_GATE_MODE,1);
        if(!test(h,ORIGIN_ENABLE)) output=(ContextActionOutput){.kind=CONTEXT_ACTION_VIEW_RECORD,
            .view_record=copy_record_angle(h)};
        else {
            word(h,LINE_LAST_ROW,0xb3);
            output=(ContextActionOutput){.kind=CONTEXT_ACTION_MAP_ORIGIN_X,
                .map_origin_x=(int32_t)(0u-(0x10c00000u&0x3fffffu))};
        }
        break;
    case COMMAND_CONTEXT_REQUEST:
        if((int8_t)test(h,ORIGIN_DETAIL_MODE)<0) break;
        if(test(h,ORIGIN_GATE_A)) {
            byte(h,FIRE_STATE,0xfe); observe(h,CONTEXT_GATE_ADDRESS,0,0,ORIGIN_GATE_A);
            byte(h,ORIGIN_GATE_A,test(h,ORIGIN_GATE_A)?0:1); break;
        }
        event=release_voices(r,h,CONTEXT_COMMAND_REQUEST_VOICES);
        if(!test(h,RECORDER_MODE)) {
            cursor=rd_u32(RECORDER_CURSOR); observe(h,CONTEXT_RECORDER_CURSOR,cursor,0,0);
            if((int32_t)cursor>0 && test(h,RECORDER_ON)) {
                unsigned i;
                observe(h,CONTEXT_RECORDER_CLEAR_BEGIN,0,0,cursor);
                for(i=0;i<4;++i) {
                    wr_u8(cursor+i,0xff); observe(h,CONTEXT_RECORDER_CLEAR_BYTE,0xff,0,cursor+i);
                }
            }
        }
        byte(h,ORIGIN_GATE_A,0xff); break;
    default: abort();
    }
    return (ContextCommandExecution){event,output};
}
