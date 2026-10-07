/* C1BC50-C1BEE4: indexed controls, function-key levels and scene modes.
 * C1BEE4's sealed unconditional branch goes straight to queue publication. */
#include "indexed_commands.h"
#include "globals.h"
#include <stdlib.h>

#define INDEXED_MODE_REQUEST 0xc45792u
#define INDEXED_ENABLE_SELECTION 0xc45849u

static void observe(const IndexedCommandHooks *h,enum IndexedCommandPhase phase,
                    uint32_t value,uint32_t limit,gaddr address) {
    if(h->observe) h->observe(h->context,phase,value,limit,address);
}
static uint8_t test(const IndexedCommandHooks *h,gaddr address) {
    uint8_t value=rd_u8(address); observe(h,INDEXED_BYTE_TEST,value,0,address); return value;
}
static void byte(const IndexedCommandHooks *h,gaddr address,uint8_t value) {
    wr_u8(address,value); observe(h,INDEXED_BYTE_STORE,value,0,address);
}
static void word(const IndexedCommandHooks *h,gaddr address,uint16_t value) {
    wr_u16(address,value); observe(h,INDEXED_WORD_STORE,value,0,address);
}
static int compare(const IndexedCommandHooks *h,int16_t index,int16_t limit) {
    observe(h,INDEXED_COMPARE_WORD,(uint16_t)index,(uint16_t)limit,0);
    return (int)index-limit;
}
static int compare_byte(const IndexedCommandHooks *h,uint8_t value,uint8_t limit) {
    observe(h,INDEXED_COMPARE_BYTE,value,limit,0); return (int8_t)value-(int8_t)limit;
}
static int16_t select_byte(const IndexedCommandHooks *h,int16_t index,uint8_t value) {
    observe(h,INDEXED_SELECT_BYTE,value,0,0); return (int16_t)(((uint16_t)index&0xff00u)|value);
}
static int16_t select_constant(const IndexedCommandHooks *h,int16_t value) {
    observe(h,INDEXED_SELECT_CONSTANT,(uint32_t)(int32_t)value,0,0); return value;
}
static int16_t add_index(const IndexedCommandHooks *h,int16_t index,int16_t amount) {
    observe(h,amount<0?INDEXED_SUBTRACT_WORD:INDEXED_ADD_WORD,(uint16_t)index,
            amount<0?(uint16_t)-amount:(uint16_t)amount,0);
    return (int16_t)((uint16_t)index+(uint16_t)amount);
}
int is_indexed_command(enum CommandAction action) {
    return action==COMMAND_LOW_INDEX || action==COMMAND_FUNCTION_LEVEL || action==COMMAND_INDEXED;
}

uint32_t execute_indexed_command(const CommandRequest *r,int16_t index,const IndexedCommandHooks *h) {
    return execute_indexed_command_result(r,index,h).event;
}
static IndexedCommandExecution selection_result(uint32_t event,int16_t selection) {
    return (IndexedCommandExecution){event,{.kind=INDEXED_ACTION_SELECTION,.selection=selection}};
}
IndexedCommandExecution execute_indexed_command_result(const CommandRequest *r,int16_t index,const IndexedCommandHooks *h) {
    uint32_t event=r->raw_event;
    uint8_t value,level,twice;
    uint16_t entry,offset;
    gaddr table;
    if(!h || !h->mode_changed || !is_indexed_command(r->action)) abort();
    if(r->action==COMMAND_FUNCTION_LEVEL) {
        observe(h,INDEXED_EVENT_COMPARE,(uint16_t)event,0x59,0);
        if((int16_t)event>0x59) goto throttle_level;
        if(test(h,MODE_SELECT) && !test(h,KEY_STATE+1)) goto throttle_level;
        index=(int16_t)event; observe(h,INDEXED_EVENT_COPY,(uint16_t)index,0,0);
        index=add_index(h,index,-0x50); index=add_index(h,index,10);
    } else if(r->action==COMMAND_LOW_INDEX) {
        index=(int16_t)event; observe(h,INDEXED_EVENT_COPY,(uint16_t)index,0,0);
        index=add_index(h,index,-1);
    } else index=r->index;
    if(test(h,COMMAND_ENABLE_GATE)) {
        if(compare(h,index,0)<0 || compare(h,index,1)>0) return selection_result(event,index);
        index=select_byte(h,index,index==1?0x10:0x11);
        byte(h,INDEXED_ENABLE_SELECTION,(uint8_t)index); byte(h,COMMAND_ENABLE_GATE,0); return selection_result(event,index);
    }
    if(!test(h,COMMAND_MODE_GATE)) {
        if(compare(h,index,0)<0 || compare(h,index,8)>0) return selection_result(event,index);
        index=add_index(h,index,1); byte(h,COMMAND_MODE_GATE,(uint8_t)index); return selection_result(event,index);
    }
    if(test(h,MODE_SELECT)) goto select_pose;
    if(compare_byte(h,(uint8_t)index,0)<0) return selection_result(event,index);
    observe(h,INDEXED_BYTE_TEST,(uint8_t)index,0,0);
    if(!(uint8_t)index) {
        uint32_t playback;
        byte(h,INDEXED_MODE_REQUEST,1);
        playback=rd_u32(PLAYBACK_BYTES); observe(h,INDEXED_LONG_TEST,playback,0,PLAYBACK_BYTES);
        if(!playback) return selection_result(event,index);
        index=select_constant(h,0x7f); goto store_mode;
    }
    if(!compare_byte(h,(uint8_t)index,1)) {
        observe(h,INDEXED_MODIFIER_TEST,r->modifier,0,0);
        if(r->modifier) byte(h,INDEXED_MODE_REQUEST,2);
        goto store_mode;
    }
    if(!compare_byte(h,(uint8_t)index,2)) goto store_mode;
    if(!compare_byte(h,(uint8_t)index,3)) { index=select_constant(h,0x7d); goto store_mode; }
    if(!compare_byte(h,(uint8_t)index,4)) { index=select_constant(h,9); goto store_mode; }
    if(!compare_byte(h,(uint8_t)index,5)) { index=select_constant(h,-1); goto store_mode; }
    if(!compare_byte(h,(uint8_t)index,7)) { index=select_constant(h,-2); goto store_mode; }
    table=rd_u32(MODE_TABLE); observe(h,INDEXED_MODE_TABLE,0,0,table);
    entry=rd_u16(table); observe(h,INDEXED_WORD_TEST,entry,0,table);
    if(!entry) return selection_result(h->mode_changed(h->context),index);
    if(!compare_byte(h,(uint8_t)index,6)) {
        byte(h,INDEXED_MODE_REQUEST,1);
        level=rd_u8(table+6); observe(h,INDEXED_TABLE_LEVEL_READ,level,0,0);
        observe(h,INDEXED_TABLE_LEVEL_INCREMENT,level,0,0); ++level;
        index=(int16_t)(((uint16_t)index&0xff00u)|level);
        if(compare_byte(h,level,3)<0 || compare_byte(h,level,8)>0) index=select_constant(h,3);
        goto store_mode;
    }
    observe(h,INDEXED_SUBTRACT_WORD,(uint16_t)index,10,1);
    index=(int16_t)(((uint16_t)index&0xff00u)|(uint8_t)((uint8_t)index-10));
    if(compare_byte(h,(uint8_t)index,0)<0 || compare_byte(h,(uint8_t)index,5)>0) return selection_result(event,index);
    observe(h,INDEXED_LEVEL_INCREMENT,(uint8_t)index,1,0);
    index=(int16_t)(((uint16_t)index&0xff00u)|(uint8_t)((uint8_t)index+1));
    observe(h,INDEXED_LEVEL_INCREMENT,(uint8_t)index,2,0);
    index=(int16_t)(((uint16_t)index&0xff00u)|(uint8_t)((uint8_t)index+2));
    if(!compare_byte(h,(uint8_t)index,3)) goto store_mode;
    table+=0x12; index=(int8_t)index;
    observe(h,INDEXED_TABLE_AVAILABILITY,(uint16_t)index,0,table);
    if(!test(h,table+(gaddr)(int32_t)index-1)) return selection_result(event,index);
store_mode:
    byte(h,MODE_SELECT,(uint8_t)index); return selection_result(h->mode_changed(h->context),index);

select_pose:
    value=rd_u8(COCKPIT_FLAGS); observe(h,INDEXED_POSE_TEST,value,3,COCKPIT_FLAGS);
    if(value&8) {
        value=rd_u8(RECORDER_MODE); observe(h,INDEXED_RECORDER_READ,value,0,0);
        if(!value) goto increment_and_queue;
        observe(h,INDEXED_RECORDER_COMPARE,value,2,0);
        if(value!=2) return selection_result(event,index);
    increment_and_queue:
        index=add_index(h,index,1); return selection_result(event,index);
    }
    table=SCENE_POSE_TABLE;
    offset=(uint16_t)((int16_t)(int8_t)index*16);
    observe(h,INDEXED_POSE_BEGIN,(uint16_t)index,offset,table);
    for(entry=0;;entry=(uint16_t)(entry+16)) {
        int16_t pose=rd_s16(table+(gaddr)(int32_t)(int16_t)entry);
        observe(h,INDEXED_POSE_VALUE,(uint16_t)pose,entry,table);
        if(pose<0) {
            observe(h,INDEXED_POSE_TERMINATOR,(uint16_t)pose,0,0);
            if(pose==-1) return selection_result(event,index);
            break;
        }
        observe(h,INDEXED_POSE_COMPARE,offset,entry,0);
        if((int16_t)offset<=(int16_t)entry) break;
        observe(h,INDEXED_POSE_ADVANCE,entry,0,0);
    }
    byte(h,SCENE_POSE_ENTRY,(uint8_t)index);
    if((int8_t)test(h,ORIGIN_DETAIL_MODE)<0 || test(h,0xc458adu)) return selection_result(event,index);
    observe(h,INDEXED_GATE_ADDRESS,0,0,ORIGIN_GATE_B);
    byte(h,ORIGIN_GATE_B,test(h,ORIGIN_GATE_B)?0:1); return selection_result(event,index);

throttle_level:
    value=rd_u8(RECORDER_MODE); observe(h,INDEXED_RECORDER_READ,value,0,0);
    if((int8_t)value>0) return (IndexedCommandExecution){event,{.kind=INDEXED_ACTION_PRESERVE}};
    observe(h,INDEXED_MODIFIER_TEST,r->modifier,0,0);
    observe(h,INDEXED_RECORDER_COMPARE,value,0xfd,0);
    if(value==0xfd) {
        const int16_t previous=index;
        index=add_index(h,index,-0x50); index=add_index(h,index,11);
        return (IndexedCommandExecution){event,{.kind=INDEXED_ACTION_RECORDER_LEVEL_CHANGE,
            .recorder_level_change=(int16_t)((uint16_t)index-(uint16_t)previous)}};
    }
    observe(h,INDEXED_RECORDER_TEST,value,0,0);
    if(value || test(h,ORIGIN_GATE_A)) return (IndexedCommandExecution){event,{.kind=INDEXED_ACTION_PRESERVE}};
    index=(int16_t)event; observe(h,INDEXED_EVENT_COPY,(uint16_t)index,0,0);
    index=add_index(h,index,-0x4f);
    if(!compare(h,index,1)) {
        value=test(h,FUNCTION_KEY_LEVEL);
        if(!value) {
            if(!compare_byte(h,rd_u8(CONTROL_RECORDS+0x2b),0x0c)) {
                byte(h,FUNCTION_KEY_LEVEL,0xff); return selection_result(event,index);
            }
        }
        if(!compare_byte(h,rd_u8(FUNCTION_KEY_LEVEL),0x0c)) {
            byte(h,FUNCTION_KEY_LEVEL,0xff); return selection_result(event,index);
        }
    }
    level=(uint8_t)index;
    observe(h,INDEXED_LEVEL_DOUBLE,level,0,0); level=(uint8_t)(level+level);
    twice=level; observe(h,INDEXED_LEVEL_SAVE,twice,0,0);
    observe(h,INDEXED_LEVEL_DOUBLE,level,0,0); level=(uint8_t)(level+level);
    observe(h,INDEXED_LEVEL_ADD,level,twice,0); level=(uint8_t)(level+twice);
    observe(h,INDEXED_LEVEL_DOUBLE,level,0,0); level=(uint8_t)(level+level);
    if(compare_byte(h,level,0x78)>=0) {
        observe(h,INDEXED_LEVEL_INCREMENT,level,1,0); ++level;
    }
    byte(h,FUNCTION_KEY_LEVEL,level);
    if(!test(h,PLAYER_READY)) return (IndexedCommandExecution){event,
        {.kind=INDEXED_ACTION_THROTTLE_LEVEL,.throttle_level=level}};
    index=(int8_t)level; observe(h,INDEXED_LEVEL_EXTEND,(uint16_t)index,0,0);
    observe(h,INDEXED_LEVEL_SCALE,(uint16_t)index,0,0); index=(int16_t)((uint16_t)index<<3);
    observe(h,INDEXED_LEVEL_COMPARE,(uint16_t)index,0x3c0,0);
    if(index>0x3c0) { index=0x3c0; observe(h,INDEXED_LEVEL_CLAMP,(uint16_t)index,0,0); }
    word(h,CONTROL_ACCUMULATOR_Y,(uint16_t)index);
    word(h,CONTROL_ACCUMULATOR_COMPANION,(uint16_t)index);
    return (IndexedCommandExecution){event,{.kind=INDEXED_ACTION_THROTTLE_ACCUMULATOR,
        .throttle_accumulator=index}};
}
