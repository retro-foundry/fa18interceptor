/* Aircraft controls and requests in the shared C1B126-C1C23C action body. */
#include "flight_commands.h"
#include "globals.h"
#include <stdlib.h>

static void observe(const FlightCommandHooks *h, enum FlightCommandPhase phase,
                    uint32_t value, uint32_t limit, gaddr address) {
    if(h->observe) h->observe(h->context,phase,value,limit,address);
}
static uint8_t test_byte(const FlightCommandHooks *h,gaddr address) {
    uint8_t value=rd_u8(address);
    observe(h,FLIGHT_BYTE_TEST,value,0,address); return value;
}
static void store_byte(const FlightCommandHooks *h,gaddr address,uint8_t value) {
    wr_u8(address,value); observe(h,FLIGHT_BYTE_STORE,value,0,address);
}
static void store_word(const FlightCommandHooks *h,gaddr address,uint16_t value) {
    wr_u16(address,value); observe(h,FLIGHT_WORD_STORE,value,0,address);
}
static uint8_t request_bit(const FlightCommandHooks *h,gaddr address,unsigned bit) {
    uint8_t old=rd_u8(address);
    wr_u8(address,(uint8_t)(old|(1u<<bit)));
    observe(h,FLIGHT_REQUEST_BIT,old,bit,address); return old;
}
static int test_bit(const FlightCommandHooks *h,gaddr address,unsigned bit) {
    uint8_t value=rd_u8(address)&(1u<<bit);
    observe(h,FLIGHT_TEST_BIT,value,bit,address); return value!=0;
}
static int compare_byte(const FlightCommandHooks *h,gaddr address,uint8_t limit) {
    uint8_t value=rd_u8(address);
    observe(h,FLIGHT_BYTE_COMPARE,value,limit,address);
    return (int8_t)value-(int8_t)limit;
}
static uint32_t swap_event(uint32_t event,const FlightCommandHooks *h) {
    event=(event<<16)|(event>>16);
    observe(h,FLIGHT_SOUND_SWAP,event,0,0); return event;
}
static uint32_t sound_word(uint32_t event,uint16_t sound,const FlightCommandHooks *h) {
    event=(event&0xffff0000u)|sound;
    observe(h,FLIGHT_SOUND_WORD,sound,0,0); return event;
}

int is_flight_command(enum CommandAction action) {
    switch(action) {
    case COMMAND_SPACE: case COMMAND_SPACE_RELEASE: case COMMAND_EJECT:
    case COMMAND_NEXT_TARGET: case COMMAND_RADAR_RANGE: case COMMAND_INFO_PAGE:
    case COMMAND_HUD: case COMMAND_Y_DOWN: case COMMAND_Y_UP: case COMMAND_Y_RELEASE:
    case COMMAND_X_RIGHT: case COMMAND_X_LEFT: case COMMAND_X_RELEASE:
    case COMMAND_TRIM_A: case COMMAND_TRIM_B: case COMMAND_TRIM_RELEASE:
    case COMMAND_THROTTLE_UP: case COMMAND_THROTTLE_DOWN: case COMMAND_THROTTLE_RELEASE:
    case COMMAND_THROTTLE_MODE: case COMMAND_HOOK: case COMMAND_WEAPON_MODE:
    case COMMAND_GEAR: case COMMAND_TARGET: case COMMAND_FLARE: case COMMAND_CHAFF:
    case COMMAND_ECM: case COMMAND_SIGN_INPUT: return 1;
    default: return 0;
    }
}

static FlightCommandExecution countermeasure(const CommandRequest *request,int16_t carried,
                              const FlightCommandHooks *h,int flare) {
    gaddr count=flare?MISSION_LEVEL_B:MISSION_LEVEL_A;
    uint8_t old,value;
    uint32_t event=request->raw_event;
    FlightCommandResult result;
    request_bit(h,PENDING_COMMAND_WORD_A+1,flare?1:2);
    old=rd_u8(count); value=(uint8_t)(old-1); wr_u8(count,value);
    const int deployed=(int8_t)old>1;
    observe(h,FLIGHT_COUNTER_DECREMENT,old,0,count);
    /* BLE follows the subtraction flags, including overflow for $80-1;
     * comparing the wrapped result as a signed byte would choose wrongly. */
    if(deployed) {
        store_byte(h,flare?COMMAND_FLARE_TIMER:COMMAND_CHAFF_TIMER,0x1e);
        carried=(int16_t)event;
        observe(h,FLIGHT_SOUND_CARRY,(uint16_t)carried,0,0);
        event=sound_word(event,flare?0x4026:0x4028,h);
        store_byte(h,MISSION_FLAGS_A,flare?2:1);
    } else {
        store_byte(h,count,0);
        event=sound_word(event,flare?0x4027:0x4029,h);
    }
    result=h->consume(h->context,flare?FLIGHT_FLARE_SOUND:FLIGHT_CHAFF_SOUND);
    /* Even the empty-count path restores the carried word. The sound child
     * owns its returned value; it need not preserve the incoming word. */
    event=(result.event&0xffff0000u)|(uint16_t)result.carried_event_word;
    observe(h,FLIGHT_SOUND_RESTORE,(uint16_t)event,0,0);
    const FlightActionOutput output=deployed
        ?(FlightActionOutput){.kind=FLIGHT_ACTION_COUNTERMEASURE_EVENT,
            .countermeasure_event=(uint16_t)result.carried_event_word}
        :result.output;
    return (FlightCommandExecution){event,output};
}

uint32_t execute_flight_command(const CommandRequest *r,int16_t carried,
                               const FlightCommandHooks *h) {
    return execute_flight_command_result(r,carried,h).event;
}
FlightCommandExecution execute_flight_command_result(const CommandRequest *r,int16_t carried,
                               const FlightCommandHooks *h) {
    uint32_t event=r->raw_event;
    FlightActionOutput output={0};
    uint8_t value,old,input;
    uint16_t word;
    gaddr record;
    if(!h || !h->consume || !is_flight_command(r->action)) abort();
    switch(r->action) {
    case COMMAND_EJECT:
        output.kind=FLIGHT_ACTION_PRESERVE;
        observe(h,FLIGHT_MODIFIER_TEST,r->modifier,0,0);
        if(!r->modifier || test_byte(h,ORIGIN_GATE_A)) break;
        store_byte(h,BAR_REDRAWS_E,8);
        observe(h,FLIGHT_TOGGLE_ADDRESS,0,0,BAR_E_FLAG);
        {
            const FlightCommandResult child=h->consume(h->context,FLIGHT_EJECT_TOGGLE);
            event=child.event;output=child.output;
        }
        request_bit(h,PENDING_COMMAND_WORD_A,5);
        store_byte(h,COMMAND_BLOCK_FLAGS,(uint8_t)(rd_u8(COMMAND_BLOCK_FLAGS)|0x0a));
        break;
    case COMMAND_NEXT_TARGET:
        output.kind=FLIGHT_ACTION_PRESERVE; /* C33186 preserves this output. */
        event=h->consume(h->context,FLIGHT_NEXT_TARGET).event;
        request_bit(h,PENDING_COMMAND_WORD_A,7);
        store_byte(h,COMMAND_NEXT_TARGET_FLAG,1); break;
    case COMMAND_RADAR_RANGE:
        event=h->consume(h->context,FLIGHT_RADAR_RANGE).event;
        request_bit(h,PENDING_COMMAND_WORD_A,6);
        record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(VIEW_RECORD);
        observe(h,FLIGHT_RADAR_RECORD,0,0,record);
        value=rd_u8(record+0x63); observe(h,FLIGHT_RADAR_READ,value,0,0);
        value&=0x0f; observe(h,FLIGHT_RADAR_MASK,value,0,0);
        observe(h,FLIGHT_RADAR_COMPARE,value,9,0);
        if(value==9) value=13;
        else {
            observe(h,FLIGHT_RADAR_COMPARE,value,11,0);
            value=value==11?9:11;
        }
        observe(h,FLIGHT_RADAR_SELECT,value,0,0);
        output=(FlightActionOutput){.kind=FLIGHT_ACTION_RADAR_RANGE,.radar_range=value};
        store_byte(h,record+0x63,(uint8_t)(rd_u8(record+0x63)&0xf0));
        store_byte(h,record+0x63,(uint8_t)(rd_u8(record+0x63)|value));
        store_byte(h,SCALE_REDRAWS,3); break;
    case COMMAND_SPACE:
        output.kind=FLIGHT_ACTION_PRESERVE;
        observe(h,FLIGHT_MODIFIER_TEST,r->modifier,0,0);
        if(!r->modifier) {
            const FlightCommandResult child=h->consume(h->context,FLIGHT_SPACE_PRESS);
            event=child.event;output=child.output;
        }
        break;
    case COMMAND_SPACE_RELEASE:
        output.kind=FLIGHT_ACTION_PRESERVE; /* C08394 changes only memory. */
        event=h->consume(h->context,FLIGHT_SPACE_RELEASE).event; break;
    case COMMAND_INFO_PAGE:
        output.kind=FLIGHT_ACTION_PRESERVE; /* C1B236-C1B260 use the event only. */
        word=rd_u16(INFO_PAGE); observe(h,FLIGHT_INFO_READ,word,0,0);
        observe(h,FLIGHT_INFO_INCREMENT,word,0,0); ++word;
        observe(h,FLIGHT_INFO_COMPARE,word,3,0);
        if((int16_t)word>3) { word=1; event=1; observe(h,FLIGHT_INFO_WRAP,1,0,0); }
        word|=0x8000; observe(h,FLIGHT_INFO_FLAG,word,0,0);
        event=(event&0xffff0000u)|word;
        store_word(h,INFO_PAGE,word); store_byte(h,INFO_REQUEST,5);
        store_byte(h,INFO_REDRAWS,2); break;
    case COMMAND_HUD:
        value=rd_u8(POST_INPUT_EXPIRED); observe(h,FLIGHT_HUD_READ,value,0,0);
        observe(h,FLIGHT_HUD_INCREMENT,value,0,0); ++value;
        observe(h,FLIGHT_HUD_COMPARE,value,1,0);
        if((int8_t)value>1) { value=0; observe(h,FLIGHT_HUD_WRAP,0,0,0); }
        store_byte(h,POST_INPUT_EXPIRED,value);
        output=(FlightActionOutput){.kind=FLIGHT_ACTION_HUD_MODE,.hud_mode=value}; break;
    case COMMAND_Y_DOWN: output.kind=FLIGHT_ACTION_PRESERVE;event=h->consume(h->context,FLIGHT_Y_DOWN).event; break;
    case COMMAND_Y_UP: output.kind=FLIGHT_ACTION_PRESERVE;event=h->consume(h->context,FLIGHT_Y_UP).event; break;
    case COMMAND_Y_RELEASE: output.kind=FLIGHT_ACTION_PRESERVE;event=h->consume(h->context,FLIGHT_Y_RELEASE).event; break;
    case COMMAND_X_RIGHT: output.kind=FLIGHT_ACTION_PRESERVE;event=h->consume(h->context,FLIGHT_X_RIGHT).event; break;
    case COMMAND_X_LEFT: output.kind=FLIGHT_ACTION_PRESERVE;event=h->consume(h->context,FLIGHT_X_LEFT).event; break;
    case COMMAND_X_RELEASE: output.kind=FLIGHT_ACTION_PRESERVE;event=h->consume(h->context,FLIGHT_X_RELEASE).event; break;
    case COMMAND_TRIM_A: case COMMAND_TRIM_B: case COMMAND_TRIM_RELEASE:
    case COMMAND_THROTTLE_UP: case COMMAND_THROTTLE_DOWN: case COMMAND_THROTTLE_RELEASE:
        output.kind=FLIGHT_ACTION_PRESERVE; /* C1B58E-C1B5D8 and C1B602. */
        if(r->action==COMMAND_TRIM_A || r->action==COMMAND_TRIM_B || r->action==COMMAND_TRIM_RELEASE) {
            input=r->action==COMMAND_TRIM_A?0x80:r->action==COMMAND_TRIM_B?0x40:0;
            observe(h,input?FLIGHT_INPUT_VALUE:FLIGHT_INPUT_RELEASE,input,0,0);
            value=rd_u8(PLAYER_STICK); observe(h,FLIGHT_INPUT_READ,value,0,0);
            value&=0x3f; observe(h,FLIGHT_INPUT_MASK,value,0,0);
            value|=input; observe(h,FLIGHT_INPUT_COMBINE,value,0,0);
            store_byte(h,PLAYER_STICK,value); store_byte(h,COMMAND_TRIM_INPUT,input);
        } else {
            input=r->action==COMMAND_THROTTLE_UP?1:r->action==COMMAND_THROTTLE_DOWN?2:0;
            observe(h,FLIGHT_INPUT_RELEASE,input,0,0);
            value=rd_u8(PLAYER_STICK); observe(h,FLIGHT_INPUT_READ,value,0,0);
            value&=0xfc; observe(h,FLIGHT_INPUT_MASK,value,0,0);
            value|=input; observe(h,FLIGHT_INPUT_COMBINE,value,0,0);
            store_byte(h,PLAYER_STICK,value);
            event=h->consume(h->context,FLIGHT_THROTTLE_RELEASE).event;
        }
        break;
    case COMMAND_THROTTLE_MODE:
        output.kind=FLIGHT_ACTION_PRESERVE; /* C1B602/C33186 and memory toggles. */
        event=h->consume(h->context,FLIGHT_THROTTLE_MODE_RELEASE).event;
        event=h->consume(h->context,FLIGHT_THROTTLE_MODE).event;
        request_bit(h,PENDING_COMMAND_WORD_A+1,0); store_byte(h,BAR_REDRAWS_B,3);
        store_word(h,CONTROL_RECORDS,rd_u16(CONTROL_RECORDS)^0x0800); break;
    case COMMAND_HOOK:
        output.kind=FLIGHT_ACTION_PRESERVE; /* C25704 changes D0, preserves D4. */
        request_bit(h,PENDING_COMMAND_WORD_A,1);
        if(compare_byte(h,CONTROL_RECORDS+0x62,0x11)) break;
        event=h->consume(h->context,FLIGHT_HOOK).event;
        store_word(h,CONTROL_RECORDS+2,rd_u16(CONTROL_RECORDS+2)^0x8000);
        old=rd_u8(SCRIPT_COUNT); wr_u8(SCRIPT_COUNT,old^0x80);
        observe(h,FLIGHT_TOGGLE_BIT,old,7,SCRIPT_COUNT);
        store_byte(h,BAR_REDRAWS_D,3);
        event=swap_event(event,h);
        if(test_bit(h,CONTROL_RECORDS+2,7)) event=sound_word(event,0x4023,h);
        event=h->consume(h->context,FLIGHT_HOOK_SOUND).event;
        event=swap_event(event,h); break;
    case COMMAND_WEAPON_MODE:
        output.kind=FLIGHT_ACTION_PRESERVE; /* Enable/message arms assign no output. */
        if(!compare_byte(h,MODE_SELECT,2)) goto enable_weapon;
        if(!compare_byte(h,MODE_SELECT,0x7d)) {
            if((int8_t)test_byte(h,COMMAND_WEAPON_PAUSE)<0) goto enable_weapon;
            event=swap_event(event,h); event=sound_word(event,0x401f,h);
            event=h->consume(h->context,FLIGHT_WEAPON_SOUND).event;
            event=swap_event(event,h); break;
        }
        event=h->consume(h->context,FLIGHT_WEAPON_MODE).event;
        value=rd_u8(COMMAND_BLOCK_FLAGS); observe(h,FLIGHT_WEAPON_READ,value,0,0);
        value&=0x0f; observe(h,FLIGHT_WEAPON_MASK,value,0,0);
        output=(FlightActionOutput){.kind=FLIGHT_ACTION_WEAPON_BLOCK,.weapon_block=value};
        if(value) break;
        request_bit(h,PENDING_COMMAND_WORD_A,4);
        value=rd_u8(CONTROL_RECORDS+0x63); observe(h,FLIGHT_WEAPON_READ,value,0,0);
        value&=0xf0; observe(h,FLIGHT_WEAPON_MASK,value,0,0);
        observe(h,FLIGHT_WEAPON_DECREMENT,value,0,0);
        /* C1BBE0/C1BBE4 uses SUBI's signed overflow flags. $80-$10 is
         * negative before byte wrapping, so it also selects mode $30. */
        const int decreased=(int8_t)value-0x10;
        value=(uint8_t)decreased;
        if(decreased<0) { value=0x30; observe(h,FLIGHT_WEAPON_WRAP,value,0,0); }
        output=(FlightActionOutput){.kind=FLIGHT_ACTION_WEAPON_MODE,.weapon_mode=value};
        store_byte(h,CONTROL_RECORDS+0x63,rd_u8(CONTROL_RECORDS+0x63)&0x0f);
        store_byte(h,CONTROL_RECORDS+0x63,(uint8_t)(rd_u8(CONTROL_RECORDS+0x63)|value));
        store_byte(h,COMMAND_WEAPON_MODE_REDRAWS,3); store_byte(h,WEAPON_REDRAWS,3);
        store_byte(h,SHOOT_CUE,0); break;
    enable_weapon:
        event=h->consume(h->context,FLIGHT_WEAPON_ENABLE).event;
        request_bit(h,CONTROL_RECORDS+2,3); break;
    case COMMAND_GEAR:
        output.kind=FLIGHT_ACTION_PRESERVE;
        request_bit(h,PENDING_COMMAND_WORD_A,0);
        if(!test_bit(h,CONTROL_RECORDS+3,7)) {
            old=rd_u8(COMMAND_BLOCK_FLAGS); wr_u8(COMMAND_BLOCK_FLAGS,old^0x80);
            observe(h,FLIGHT_TOGGLE_BIT,old,7,COMMAND_BLOCK_FLAGS);
            event=rd_u32(COMMAND_GEAR_GATE); observe(h,FLIGHT_GEAR_READ,event,0,0);
            event&=0x40; observe(h,FLIGHT_GEAR_MASK,event,0,0);
            output=(FlightActionOutput){.kind=FLIGHT_ACTION_GEAR_GATE,.gear_gate=event};
            if(event) { store_byte(h,COMMAND_GEAR_MESSAGE,0x83); event=r->raw_event; break; }
            event=r->raw_event;
        }
        event=h->consume(h->context,FLIGHT_GEAR).event; break;
    case COMMAND_TARGET:
        output.kind=FLIGHT_ACTION_PRESERVE; /* Tone and target bit update only. */
        if(!compare_byte(h,MODE_SELECT,0x7d)) {
            event=h->consume(h->context,FLIGHT_TARGET).event;
            store_word(h,CONTROL_RECORDS+0x802,rd_u16(CONTROL_RECORDS+0x802)^0x1000);
        }
        break;
    case COMMAND_FLARE:
        output.kind=FLIGHT_ACTION_PRESERVE;
        observe(h,FLIGHT_MODIFIER_TEST,r->modifier,0,0);
        if(!r->modifier) return countermeasure(r,carried,h,1);
        if(compare_byte(h,MODE_SELECT,6)) break;
        word=rd_u16(COMMAND_SPAWN_GATE); observe(h,FLIGHT_WORD_TEST,word,0,0);
        if(word) break;
        if(test_bit(h,CONTROL_RECORDS+0x201,6) && test_bit(h,CONTROL_RECORDS+0x401,6) &&
           test_bit(h,CONTROL_RECORDS+0x601,6)) break;
        store_byte(h,COMMAND_BLOCK_FLAGS,rd_u8(COMMAND_BLOCK_FLAGS)|0x0f);
        observe(h,FLIGHT_SPAWN_SAVE,(uint16_t)event,0,0);
        observe(h,FLIGHT_SPAWN_ARGUMENT,0x30,0,0);
        observe(h,FLIGHT_SPAWN_ARGUMENT,0x1c,0,0);
        {
            const FlightCommandResult child=h->consume(h->context,FLIGHT_FLARE_SPAWN);
            event=child.event;output=child.output;
        }
        event=(event&0xffff0000u)|(r->raw_event&0xffffu);
        observe(h,FLIGHT_SPAWN_RESTORE,(uint16_t)event,0,0); break;
    case COMMAND_CHAFF: return countermeasure(r,carried,h,0);
    case COMMAND_ECM:
        output.kind=FLIGHT_ACTION_PRESERVE; /* Tone and C1C214's memory toggle. */
        observe(h,FLIGHT_ECM_BEGIN,0,0,UPDATE_MAP_OVERRIDE);
        request_bit(h,PENDING_COMMAND_WORD_A+1,3); store_byte(h,BAR_REDRAWS_C,3);
        event=h->consume(h->context,FLIGHT_ECM).event;
        observe(h,FLIGHT_TOGGLE_ADDRESS,0,0,PLAYER_FLAGS_G);
        value=test_byte(h,PLAYER_FLAGS_G)?0:1; store_byte(h,PLAYER_FLAGS_G,value); break;
    case COMMAND_SIGN_INPUT:
        output.kind=FLIGHT_ACTION_PRESERVE; /* C1C224 writes only SEQUENCE_PHASE. */
        observe(h,FLIGHT_MODIFIER_TEST,r->modifier,0,0);
        store_byte(h,SEQUENCE_PHASE,r->modifier?0xff:1); break;
    default: abort();
    }
    return (FlightCommandExecution){event,output};
}
