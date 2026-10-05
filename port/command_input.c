/* Ordinary-state input policy from $C1AC28/$C1AD74. The reference-backed
 * implementation is game/command_selection.c; source oracle evidence is
 * recorded in analysis/routines/native_command_input.md. */
#include "command_input.h"

static int compare(uint8_t value,uint8_t limit) {
    return (int8_t)value-(int8_t)limit;
}
typedef struct { uint8_t code; enum CommandAction action; } KeyRoute;
static int key_route(CommandRequest *request,const KeyRoute *table,unsigned count) {
    unsigned i;
    for(i=0;i<count;++i) if((uint8_t)request->raw_event==table[i].code) {
        request->action=table[i].action; return 1;
    }
    return 0;
}
#define KEY_ROUTES(request,table) key_route(request,table,sizeof(table)/sizeof(table[0]))

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

static CommandRequest keyboard_request(FA18CommandInput *s,uint32_t raw) {
    CommandRequest request={COMMAND_QUEUE_ONLY,raw,0,0,0,0};
    uint8_t value,recorder;
    unsigned i;

    value=s->event_counter;

    if((int8_t)value<=0) {
        ++value; s->event_counter=value;

        if((int8_t)value<=0) { request.action=COMMAND_COUNTER_WAIT; return request; }

        request.raw_event&=~0x80u;
    }
    request.origin_mode=s->origin_mode;

    request.modifier=s->modifier;

    request.detail_state=s->indexed.origin_detail;

    if(!compare(request.detail_state,3)) goto nonzero_context;
    if(s->return_state) goto nonzero_context;
    if(s->indexed.pose_inhibit) goto nonzero_context;
    if(s->indexed.enable_gate) goto function_keys;
    if(!s->indexed.mode_gate) goto function_keys;
    if(!s->indexed.mode) goto function_keys;
    recorder=s->indexed.recorder_mode;
    if((int8_t)recorder<0) goto function_keys;
    if(!compare(recorder,3)) goto mode_key;

    if(request.detail_state) goto alternate_context;
    if(!compare(s->indexed.mode,2)) goto alternate_context;
    value=s->block_flags&0x0f;

    if(value) goto alternate_context;
    if(!compare(recorder,1)) goto alternate_context;
    if(!compare(recorder,2)) goto alternate_context;
    if(compare(recorder,5)>=0) goto mode_key;
    if(KEY_ROUTES(&request,direct_keys)) return request;

alternate_context:
    request.detail_state=s->indexed.origin_detail;

    if(request.detail_state) {
        if(compare(request.detail_state,6)) goto context_default;
    } else if(KEY_ROUTES(&request,alternate_keys)) return request;
    if(!compare((uint8_t)request.raw_event,0x43)) {
        request.action=COMMAND_CONTEXT_SPECIAL; return request;
    }
context_default:
    if(!compare((uint8_t)request.raw_event,0x45)) {
        request.action=COMMAND_SIGN_INPUT; return request;
    }
    if(!compare(s->indexed.mode,2)) goto fallback_keys;
    if(!compare(request.detail_state,5)) goto function_keys;
    if(!compare(request.detail_state,6)) goto function_keys;

    if((int8_t)request.detail_state>0) goto nonzero_context;
function_keys:
    value=(uint8_t)request.raw_event;
    if(compare(value,1)>=0) {
        if(compare(value,10)<=0) {
            request.action=COMMAND_LOW_INDEX; return request;
        }
        if(compare(value,0x50)>=0 &&
           compare(value,0x59)<=0) {
            request.action=COMMAND_FUNCTION_LEVEL; return request;
        }
    }
    for(i=0;i<sizeof indexed_keys;++i)
        if(!compare(value,indexed_keys[i])) {
            request.index=(int16_t)i; request.action=COMMAND_INDEXED;
             return request;
        }
fallback_keys:
    if(!compare((uint8_t)request.raw_event,0x1b)) {
        request.action=COMMAND_ZOOM_OUT; return request;
    }
    if(!compare((uint8_t)request.raw_event,0x1a)) {
        request.action=COMMAND_ZOOM_IN; return request;
    }
    if(!compare((uint8_t)request.raw_event,0x9b) ||
       !compare((uint8_t)request.raw_event,0x9a)) {
        request.action=COMMAND_FIRE_REQUEST; return request;
    }
mode_key:
    if(!compare((uint8_t)request.raw_event,0x19)) {
        request.action=COMMAND_CONTEXT_REQUEST; return request;
    }
nonzero_context:
    if(!compare(s->indexed.pose_inhibit,2) &&
       !compare((uint8_t)request.raw_event,0x43)) {
        request.action=COMMAND_CONTEXT_SPECIAL; return request;
    }
    if(KEY_ROUTES(&request,release_keys)) return request;
    value=(uint8_t)request.raw_event;
    if(!compare(value,0x60) ||
       !compare(value,0x61)) {
        s->modifier=1;
        request.action=COMMAND_FINISH_EVENT; return request;
    }
    if(!compare(value,0xe0) ||
       !compare(value,0xe1)) {
        request.action=COMMAND_FINISH_EVENT; return request;
    }
    if(!compare(value,0x66)) {
        s->indexed.function_modifier=1;
        request.action=COMMAND_FINISH_EVENT; return request;
    }
    if(!compare(value,0xe6)) {
        request.action=COMMAND_FINISH_EVENT; return request;
    }
    if(!compare(value,0x67)) {
        s->other_modifier=1;
        request.action=COMMAND_FINISH_EVENT; return request;
    }
    if(!compare(value,0xe7)) {
        request.action=COMMAND_FINISH_EVENT; return request;
    }
    if(compare(s->message_state,2) &&
       !compare(value,0x31)) {
        request.action=COMMAND_RESET_CONTEXT; return request;
    }
    if(!compare(value,0x46)) {
        request.action=COMMAND_RESET_CONTEXT; return request;
    }
    if(!compare(value,0x45)) {
        request.action=COMMAND_SIGN_INPUT; return request;
    }
    if(!compare(value,0x44)) {
        s->return_state=0;
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
static int consume_word(CommandRequest *request,uint16_t *word,
                        const enum CommandAction *actions,unsigned count) {
    unsigned i;
    if(!*word) return 0;
    /* Original byte order: high byte bit 0 first, then the low byte.
     * Retain priority and clear precisely the first set command bit. */
    for(i=0;i<count;++i) {
        uint16_t mask=(uint16_t)(1u<<(i<8?i+8:i-8));
        uint16_t old=*word;
        *word=(uint16_t)(old&~mask);
        if(old&mask) { request->action=actions[i]; return 1; }
    }
    return 0;
}
static CommandRequest pending_request(FA18CommandInput *s) {
    CommandRequest request={COMMAND_PENDING_EMPTY,0,0,0,0,0};
    uint16_t first=s->pending_a;


    if(first&0xf0) { request.action=COMMAND_INVALID_WORD; return request; }
    request.origin_mode=s->origin_mode;

    if(consume_word(&request,&s->pending_a,first_word,sizeof first_word/sizeof first_word[0]))
        return request;
    if(!compare(s->indexed.recorder_mode,2)) return request;
    if(!compare(s->indexed.recorder_mode,1)) return request;

    if(consume_word(&request,&s->pending_b,second_word,sizeof second_word/sizeof second_word[0]))
        return request;
    s->indexed.function_modifier=0;
    s->other_modifier=0;
    return request;
}
int fa18_select_keyboard_command(FA18CommandInput *s,uint32_t event,CommandRequest *request) {
    if(!s || !request) return 0;
    *request=keyboard_request(s,event); return 1;
}
int fa18_select_pending_command(FA18CommandInput *s,CommandRequest *request) {
    if(!s || !request) return 0;
    *request=pending_request(s); return 1;
}
int fa18_apply_selected_indexed_command(FA18CommandInput *s,const CommandRequest *request,
                                        int16_t carry,const FA18IndexedControlPoses *poses,
                                        FA18IndexedStatusTone status_tone,void *context,
                                        uint32_t *published_event) {
    FA18IndexedControlRequest indexed;
    if(!s || !request) return 0;
    switch(request->action) {
    case COMMAND_LOW_INDEX: indexed.kind=FA18_INDEXED_LOW_KEY; break;
    case COMMAND_FUNCTION_LEVEL: indexed.kind=FA18_INDEXED_FUNCTION_KEY; break;
    case COMMAND_INDEXED: indexed.kind=FA18_INDEXED_SELECTION; break;
    default: return 0;
    }
    indexed.event=request->raw_event; indexed.modifier=request->modifier;
    indexed.selection=request->index;
    return fa18_apply_indexed_control(&s->indexed,&indexed,carry,poses,status_tone,
                                      context,published_event);
}
