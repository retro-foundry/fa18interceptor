#include "command_selection.h"
#include "globals.h"

static void observe(const CommandSelectionHooks *h, enum CommandSelectionPhase phase,
                    uint32_t value, uint32_t limit) {
    if(h && h->observe) h->observe(h->context,phase,value,limit);
}
static uint8_t test_byte(const CommandSelectionHooks *h,gaddr address) {
    uint8_t value=rd_u8(address);
    observe(h,COMMAND_TEST_BYTE,value,0); return value;
}
static int compare(const CommandSelectionHooks *h,enum CommandSelectionPhase phase,
                   uint8_t value,uint8_t limit) {
    observe(h,phase,value,limit);
    return (int8_t)value-(int8_t)limit;
}
typedef struct { uint8_t code; enum CommandAction action; } KeyRoute;
static int key_route(CommandRequest *request,const CommandSelectionHooks *h,
                     const KeyRoute *table,unsigned count) {
    unsigned i;
    for(i=0;i<count;++i)
        if(!compare(h,COMMAND_COMPARE_KEY,(uint8_t)request->raw_event,table[i].code)) {
            request->action=table[i].action; return 1;
        }
    return 0;
}
#define KEY_ROUTES(request,h,table) key_route(request,h,table,sizeof(table)/sizeof(table[0]))

/* C1AE28, C1AEF4, C1AFBE and C1B04A: source comparison order matters to
 * the CPU adapter, even when all failed keys have the same native result. */
static const KeyRoute direct_keys[]={
    {0x40,COMMAND_SPACE}, {0x4c,COMMAND_Y_UP}, {0x4d,COMMAND_Y_DOWN},
    {0x4f,COMMAND_X_RIGHT}, {0x4e,COMMAND_X_LEFT}, {0x38,COMMAND_TRIM_A},
    {0x39,COMMAND_TRIM_B}, {0x0c,COMMAND_THROTTLE_UP},
    {0x0b,COMMAND_THROTTLE_DOWN}, {0x0d,COMMAND_THROTTLE_MODE},
    {0x41,COMMAND_THROTTLE_MODE}, {0x12,COMMAND_EJECT},
    {0x13,COMMAND_RADAR_RANGE}, {0x24,COMMAND_GEAR}, {0x20,COMMAND_HOOK},
    {0x23,COMMAND_FLARE}, {0x33,COMMAND_CHAFF}, {0x26,COMMAND_ECM},
    {0x25,COMMAND_HUD}, {0x21,COMMAND_TARGET}, {0x14,COMMAND_NEXT_TARGET},
    {0x15,COMMAND_INFO_PAGE}, {0x37,COMMAND_MAP}
};
static const KeyRoute alternate_keys[]={
    {0x44,COMMAND_WEAPON_MODE}, {0x3e,COMMAND_VIEW_ZERO},
    {0x1e,COMMAND_VIEW_ONE}, {0x2d,COMMAND_VIEW_DECREMENT},
    {0x2f,COMMAND_VIEW_INCREMENT}, {0x3d,COMMAND_VIEW_TWELVE},
    {0x1d,COMMAND_VIEW_THIRTEEN}, {0x2e,COMMAND_CONTEXT_VIEW_ALTERNATE},
    {0x3f,COMMAND_VIEW_TOGGLE}, {0x1f,COMMAND_VIEW_THREE}, {0x3c,COMMAND_VIEW_NINE},
    {0x9e,COMMAND_FIRE_REQUEST}, {0xad,COMMAND_FIRE_REQUEST},
    {0xaf,COMMAND_FIRE_REQUEST}, {0xbd,COMMAND_FIRE_REQUEST}, {0x9d,COMMAND_FIRE_REQUEST}
};
static const uint8_t indexed_keys[]={0x1d,0x1e,0x1f,0x2d,0x2e,0x2f,0x3d,0x3e};
static const KeyRoute release_keys[]={
    {0xcc,COMMAND_Y_RELEASE}, {0xcd,COMMAND_Y_RELEASE},
    {0xcf,COMMAND_X_RELEASE}, {0xce,COMMAND_X_RELEASE},
    {0xb8,COMMAND_TRIM_RELEASE}, {0xb9,COMMAND_TRIM_RELEASE},
    {0x8c,COMMAND_THROTTLE_RELEASE}, {0x8b,COMMAND_THROTTLE_RELEASE},
    {0xc0,COMMAND_SPACE_RELEASE}
};

CommandRequest select_keyboard_command(uint32_t raw,const CommandSelectionHooks *h) {
    CommandRequest request={COMMAND_QUEUE_ONLY,raw,0,0,0,0};
    uint8_t value,recorder;
    unsigned i;
    observe(h,COMMAND_READ_EVENT,raw,0);
    value=rd_u8(COMMAND_EVENT_COUNTER);
    observe(h,COMMAND_TEST_COUNTER,value,0);
    if((int8_t)value<=0) {
        uint8_t old=value;
        ++value; wr_u8(COMMAND_EVENT_COUNTER,value);
        observe(h,COMMAND_INCREMENT_COUNTER,old,0);
        if((int8_t)value<=0) { request.action=COMMAND_COUNTER_WAIT; return request; }
        observe(h,COMMAND_CLEAR_RELEASE,request.raw_event,0);
        request.raw_event&=~0x80u;
    }
    request.origin_mode=rd_u8(ORIGIN_ENABLE);
    observe(h,COMMAND_READ_ORIGIN,request.origin_mode,0);
    request.modifier=rd_u8(KEY_STATE);
    observe(h,COMMAND_READ_MODIFIER,request.modifier,0);
    request.detail_state=rd_u8(ORIGIN_DETAIL_MODE);
    observe(h,COMMAND_READ_DETAIL,request.detail_state,0);
    if(!compare(h,COMMAND_COMPARE_DETAIL,request.detail_state,3)) goto nonzero_context;
    if(test_byte(h,COMMAND_RETURN_STATE)) goto nonzero_context;
    if(test_byte(h,CONTEXT_GATE)) goto nonzero_context;
    if(test_byte(h,COMMAND_ENABLE_GATE)) goto function_keys;
    if(!test_byte(h,COMMAND_MODE_GATE)) goto function_keys;
    if(!test_byte(h,MODE_SELECT)) goto function_keys;
    recorder=rd_u8(RECORDER_MODE); observe(h,COMMAND_READ_RECORDER,recorder,0);
    if((int8_t)recorder<0) goto function_keys;
    if(!compare(h,COMMAND_COMPARE_RECORDER,recorder,3)) goto mode_key;
    observe(h,COMMAND_TEST_BYTE,request.detail_state,0);
    if(request.detail_state) goto alternate_context;
    if(!compare(h,COMMAND_COMPARE_MODE,rd_u8(MODE_SELECT),2)) goto alternate_context;
    value=rd_u8(COMMAND_BLOCK_FLAGS)&0x0f;
    observe(h,COMMAND_READ_BLOCK,value,0);
    if(value) goto alternate_context;
    if(!compare(h,COMMAND_COMPARE_RECORDER,recorder,1)) goto alternate_context;
    if(!compare(h,COMMAND_COMPARE_RECORDER,recorder,2)) goto alternate_context;
    if(compare(h,COMMAND_COMPARE_RECORDER,recorder,5)>=0) goto mode_key;
    if(KEY_ROUTES(&request,h,direct_keys)) return request;

alternate_context:
    request.detail_state=rd_u8(ORIGIN_DETAIL_MODE);
    observe(h,COMMAND_READ_DETAIL,request.detail_state,0);
    if(request.detail_state) {
        if(compare(h,COMMAND_COMPARE_DETAIL,request.detail_state,6)) goto context_default;
    } else if(KEY_ROUTES(&request,h,alternate_keys)) return request;
    if(!compare(h,COMMAND_COMPARE_KEY,(uint8_t)request.raw_event,0x43)) {
        request.action=COMMAND_CONTEXT_SPECIAL; return request;
    }
context_default:
    if(!compare(h,COMMAND_COMPARE_KEY,(uint8_t)request.raw_event,0x45)) {
        request.action=COMMAND_SIGN_INPUT; return request;
    }
    if(!compare(h,COMMAND_COMPARE_MODE,rd_u8(MODE_SELECT),2)) goto fallback_keys;
    if(!compare(h,COMMAND_COMPARE_DETAIL,request.detail_state,5)) goto function_keys;
    if(!compare(h,COMMAND_COMPARE_DETAIL,request.detail_state,6)) goto function_keys;
    observe(h,COMMAND_TEST_BYTE,request.detail_state,0);
    if((int8_t)request.detail_state>0) goto nonzero_context;
function_keys:
    value=(uint8_t)request.raw_event;
    if(compare(h,COMMAND_COMPARE_KEY,value,1)>=0) {
        if(compare(h,COMMAND_COMPARE_KEY,value,10)<=0) {
            request.action=COMMAND_LOW_INDEX; return request;
        }
        if(compare(h,COMMAND_COMPARE_KEY,value,0x50)>=0 &&
           compare(h,COMMAND_COMPARE_KEY,value,0x59)<=0) {
            request.action=COMMAND_FUNCTION_LEVEL; return request;
        }
    }
    for(i=0;i<sizeof indexed_keys;++i)
        if(!compare(h,COMMAND_COMPARE_KEY,value,indexed_keys[i])) {
            request.index=(int16_t)i; request.action=COMMAND_INDEXED;
            observe(h,COMMAND_SET_INDEX,i,0); return request;
        }
fallback_keys:
    if(!compare(h,COMMAND_COMPARE_KEY,(uint8_t)request.raw_event,0x1b)) {
        request.action=COMMAND_ZOOM_OUT; return request;
    }
    if(!compare(h,COMMAND_COMPARE_KEY,(uint8_t)request.raw_event,0x1a)) {
        request.action=COMMAND_ZOOM_IN; return request;
    }
    if(!compare(h,COMMAND_COMPARE_KEY,(uint8_t)request.raw_event,0x9b) ||
       !compare(h,COMMAND_COMPARE_KEY,(uint8_t)request.raw_event,0x9a)) {
        request.action=COMMAND_FIRE_REQUEST; return request;
    }
mode_key:
    if(!compare(h,COMMAND_COMPARE_KEY,(uint8_t)request.raw_event,0x19)) {
        request.action=COMMAND_CONTEXT_REQUEST; return request;
    }
nonzero_context:
    if(!compare(h,COMMAND_COMPARE_GATE,rd_u8(CONTEXT_GATE),2) &&
       !compare(h,COMMAND_COMPARE_KEY,(uint8_t)request.raw_event,0x43)) {
        request.action=COMMAND_CONTEXT_SPECIAL; return request;
    }
    if(KEY_ROUTES(&request,h,release_keys)) return request;
    value=(uint8_t)request.raw_event;
    if(!compare(h,COMMAND_COMPARE_KEY,value,0x60) ||
       !compare(h,COMMAND_COMPARE_KEY,value,0x61)) {
        wr_u8(KEY_STATE,1); observe(h,COMMAND_SET_LATCH,1,0);
        request.action=COMMAND_FINISH_EVENT; return request;
    }
    if(!compare(h,COMMAND_COMPARE_KEY,value,0xe0) ||
       !compare(h,COMMAND_COMPARE_KEY,value,0xe1)) {
        request.action=COMMAND_FINISH_EVENT; return request;
    }
    if(!compare(h,COMMAND_COMPARE_KEY,value,0x66)) {
        wr_u8(KEY_STATE+1,1); observe(h,COMMAND_SET_LATCH,1,1);
        request.action=COMMAND_FINISH_EVENT; return request;
    }
    if(!compare(h,COMMAND_COMPARE_KEY,value,0xe6)) {
        request.action=COMMAND_FINISH_EVENT; return request;
    }
    if(!compare(h,COMMAND_COMPARE_KEY,value,0x67)) {
        wr_u8(KEY_STATE+2,1); observe(h,COMMAND_SET_LATCH,1,2);
        request.action=COMMAND_FINISH_EVENT; return request;
    }
    if(!compare(h,COMMAND_COMPARE_KEY,value,0xe7)) {
        request.action=COMMAND_FINISH_EVENT; return request;
    }
    if(compare(h,COMMAND_COMPARE_MODE,rd_u8(MESSAGE_STATE_C),2) &&
       !compare(h,COMMAND_COMPARE_KEY,value,0x31)) {
        request.action=COMMAND_RESET_CONTEXT; return request;
    }
    if(!compare(h,COMMAND_COMPARE_KEY,value,0x46)) {
        request.action=COMMAND_RESET_CONTEXT; return request;
    }
    if(!compare(h,COMMAND_COMPARE_KEY,value,0x45)) {
        request.action=COMMAND_SIGN_INPUT; return request;
    }
    if(!compare(h,COMMAND_COMPARE_KEY,value,0x44)) {
        wr_u8(COMMAND_RETURN_STATE,0); observe(h,COMMAND_CLEAR_CONTEXT,0,0);
    }
    return request;
}

static const enum CommandAction first_word[]={
    COMMAND_GEAR,COMMAND_HOOK,COMMAND_SPACE,COMMAND_SPACE_RELEASE,
    COMMAND_WEAPON_MODE,COMMAND_EJECT,COMMAND_RADAR_RANGE,COMMAND_NEXT_TARGET,
    COMMAND_THROTTLE_MODE,COMMAND_FLARE,COMMAND_CHAFF,COMMAND_ECM
};
static const enum CommandAction second_word[]={
    COMMAND_MAP,COMMAND_CONTEXT_REFRESH,COMMAND_CONTEXT_CALCULATION,
    COMMAND_CONTEXT_VIEW_DECREMENT,COMMAND_VIEW_ZERO,COMMAND_VIEW_TOGGLE,
    COMMAND_VIEW_INCREMENT,COMMAND_VIEW_THREE,COMMAND_VIEW_ONE,
    COMMAND_VIEW_THIRTEEN,COMMAND_VIEW_DECREMENT,COMMAND_VIEW_TWELVE,
    COMMAND_VIEW_EIGHT,COMMAND_VIEW_NINE,COMMAND_FIRE_REQUEST,COMMAND_CONTEXT_VIEW_INCREMENT
};
static int consume_word(CommandRequest *request,const CommandSelectionHooks *h,
                        gaddr word,const enum CommandAction *actions,unsigned count) {
    uint16_t value=rd_u16(word);
    unsigned i;
    observe(h,COMMAND_TEST_WORD,value,0);
    if(!value) return 0;
    for(i=0;i<count;++i) {
        gaddr address=word+i/8;
        uint8_t old=rd_u8(address),mask=(uint8_t)(1u<<(i%8));
        wr_u8(address,(uint8_t)(old&~mask));
        observe(h,COMMAND_CLEAR_WORD_BIT,old,i%8);
        if(old&mask) { request->action=actions[i]; return 1; }
    }
    return 0;
}
CommandRequest select_pending_command(const CommandSelectionHooks *h) {
    CommandRequest request={COMMAND_PENDING_EMPTY,0,0,0,0,0};
    uint16_t first=rd_u16(RECORD_WORD_A);
    observe(h,COMMAND_READ_WORD_A,first,0);
    observe(h,COMMAND_MASK_WORD_A,first&0xf0,0);
    if(first&0xf0) { request.action=COMMAND_INVALID_WORD; return request; }
    request.origin_mode=rd_u8(ORIGIN_ENABLE);
    observe(h,COMMAND_PENDING_BEGIN,request.origin_mode,0);
    if(consume_word(&request,h,RECORD_WORD_A,first_word,sizeof first_word/sizeof first_word[0]))
        return request;
    if(!compare(h,COMMAND_COMPARE_RECORDER,rd_u8(RECORDER_MODE),2)) return request;
    if(!compare(h,COMMAND_COMPARE_RECORDER,rd_u8(RECORDER_MODE),1)) return request;
    observe(h,COMMAND_SELECT_WORD_B,0,0);
    if(consume_word(&request,h,RECORD_WORD_B,second_word,sizeof second_word/sizeof second_word[0]))
        return request;
    wr_u8(KEY_STATE+1,0); observe(h,COMMAND_CLEAR_LATCH,0,1);
    wr_u8(KEY_STATE+2,0); observe(h,COMMAND_CLEAR_LATCH,0,2);
    return request;
}
