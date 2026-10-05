/* Source: the 28 aircraft action paths of $C1AC28/$C1AD74.
 * See game/flight_commands.c and analysis/routines/native_flight_command_input.md.
 * State and child arguments are ordinary C values and pointers. */
#include "flight_command_input.h"

int fa18_is_flight_input_command(enum CommandAction action) {
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

static uint32_t swap_event(uint32_t event) { return (event<<16)|(event>>16); }
static uint32_t sound_word(uint32_t event,uint16_t sound) {
    return (event&0xffff0000u)|sound;
}
static int invoke(FA18FlightCommandState *s,const FA18FlightCommandOps *ops,
                  enum FlightCommandChild child,uint32_t *event) {
    FA18FlightCommandChildInput input={*event,0,{0,0}};
    FlightCommandResult result;
    if(!ops->consume(ops->context,s,child,&input,&result)) return 0;
    *event=result.event; return 1;
}
static int countermeasure(FA18FlightCommandState *s,const CommandRequest *r,
                           int16_t carry,const FA18FlightCommandOps *ops,
                           int flare,uint32_t *published_event) {
    uint8_t *count=flare?&s->flare_count:&s->chaff_count;
    uint8_t old=*count;
    FA18FlightCommandChildInput input={r->raw_event,carry,{0,0}};
    FlightCommandResult result;
    s->emitted_requests|=flare?2u:4u;
    *count=(uint8_t)(old-1);
    /* The branch follows signed subtraction with overflow: $80-1 takes
     * the empty-count path even though its wrapped result is $7F. */
    if((int8_t)old>1) {
        if(flare) s->flare_timer=0x1e; else s->chaff_timer=0x1e;
        input.restore_event_word=(int16_t)r->raw_event;
        input.event=sound_word(input.event,flare?0x4026:0x4028);
        s->mission_flags=flare?2:1;
    } else {
        *count=0;
        input.event=sound_word(input.event,flare?0x4027:0x4029);
    }
    if(!ops->consume(ops->context,s,flare?FLIGHT_FLARE_SOUND:FLIGHT_CHAFF_SOUND,
                     &input,&result)) return 0;
    *published_event=(result.event&0xffff0000u)|(uint16_t)result.carried_event_word;
    return 1;
}

int fa18_apply_flight_input_command(FA18FlightCommandState *s,
                                    const CommandRequest *r,int16_t carry,
                                    const FA18FlightCommandOps *ops,
                                    uint32_t *published_event) {
    uint32_t event;
    uint8_t value,input;
    uint16_t word;
    if(!s || !r || !published_event || !ops || !ops->consume || !s->commands ||
       !s->player || !s->viewed || !s->target || !s->spawn_slots[0] ||
       !s->spawn_slots[1] || !s->spawn_slots[2] || !fa18_is_flight_input_command(r->action)) return 0;
    event=r->raw_event;
#define CALL(child) do { if(!invoke(s,ops,child,&event)) return 0; } while(0)
    switch(r->action) {
    case COMMAND_EJECT:
        if(!r->modifier || s->commands->indexed.origin_gate_a) break;
        s->redraw_e=8; CALL(FLIGHT_EJECT_TOGGLE);
        s->emitted_requests|=0x2000; s->commands->block_flags|=0x0a; break;
    case COMMAND_NEXT_TARGET:
        CALL(FLIGHT_NEXT_TARGET); s->emitted_requests|=0x8000; s->next_target=1; break;
    case COMMAND_RADAR_RANGE:
        CALL(FLIGHT_RADAR_RANGE); s->emitted_requests|=0x4000;
        if(!s->viewed) return 0;
        value=s->viewed->weapon_radar&0x0f;
        value=value==9?13:value==11?9:11;
        s->viewed->weapon_radar&=0xf0;
        s->viewed->weapon_radar|=value; s->scale_redraws=3; break;
    case COMMAND_SPACE:
        if(!r->modifier) CALL(FLIGHT_SPACE_PRESS);
        break;
    case COMMAND_SPACE_RELEASE: CALL(FLIGHT_SPACE_RELEASE); break;
    case COMMAND_INFO_PAGE:
        word=(uint16_t)(s->info_page+1);
        if((int16_t)word>3) { word=1; event=1; }
        word|=0x8000; event=(event&0xffff0000u)|word;
        s->info_page=word; s->info_request=5; s->info_redraws=2; break;
    case COMMAND_HUD:
        value=(uint8_t)(s->hud_mode+1);
        if((int8_t)value>1) value=0;
        s->hud_mode=value; break;
    case COMMAND_Y_DOWN: CALL(FLIGHT_Y_DOWN); break;
    case COMMAND_Y_UP: CALL(FLIGHT_Y_UP); break;
    case COMMAND_Y_RELEASE: CALL(FLIGHT_Y_RELEASE); break;
    case COMMAND_X_RIGHT: CALL(FLIGHT_X_RIGHT); break;
    case COMMAND_X_LEFT: CALL(FLIGHT_X_LEFT); break;
    case COMMAND_X_RELEASE: CALL(FLIGHT_X_RELEASE); break;
    case COMMAND_TRIM_A: case COMMAND_TRIM_B: case COMMAND_TRIM_RELEASE:
        input=r->action==COMMAND_TRIM_A?0x80:r->action==COMMAND_TRIM_B?0x40:0;
        s->player->stick=(uint8_t)((s->player->stick&0x3f)|input);
        s->trim_input=input; break;
    case COMMAND_THROTTLE_UP: case COMMAND_THROTTLE_DOWN: case COMMAND_THROTTLE_RELEASE:
        input=r->action==COMMAND_THROTTLE_UP?1:r->action==COMMAND_THROTTLE_DOWN?2:0;
        s->player->stick=(uint8_t)((s->player->stick&0xfc)|input);
        CALL(FLIGHT_THROTTLE_RELEASE); break;
    case COMMAND_THROTTLE_MODE:
        CALL(FLIGHT_THROTTLE_MODE_RELEASE); CALL(FLIGHT_THROTTLE_MODE);
        s->emitted_requests|=1; s->redraw_b=3;
        s->player->flags^=0x0800; break;
    case COMMAND_HOOK:
        s->emitted_requests|=0x0200;
        if(s->player->equipment_kind!=0x11) break;
        CALL(FLIGHT_HOOK);
        s->player->secondary_flags^=0x8000; s->script_count^=0x80; s->redraw_d=3;
        event=swap_event(event);
        if(s->player->secondary_flags&0x8000) event=sound_word(event,0x4023);
        CALL(FLIGHT_HOOK_SOUND); event=swap_event(event); break;
    case COMMAND_WEAPON_MODE:
        if(s->commands->indexed.mode==2) goto enable_weapon;
        if(s->commands->indexed.mode==0x7d) {
            if((int8_t)s->weapon_pause<0) goto enable_weapon;
            event=sound_word(swap_event(event),0x401f);
            CALL(FLIGHT_WEAPON_SOUND); event=swap_event(event); break;
        }
        CALL(FLIGHT_WEAPON_MODE);
        if(s->commands->block_flags&0x0f) break;
        s->emitted_requests|=0x1000;
        value=(uint8_t)((s->player->weapon_radar&0xf0)-0x10);
        if((int8_t)value<0) value=0x30;
        s->player->weapon_radar&=0x0f; s->player->weapon_radar|=value;
        s->weapon_mode_redraws=3; s->weapon_redraws=3; s->shoot_cue=0; break;
    enable_weapon:
        CALL(FLIGHT_WEAPON_ENABLE); s->player->secondary_flags|=0x0800; break;
    case COMMAND_GEAR:
        s->emitted_requests|=0x0100;
        if(!(s->player->secondary_flags&0x0080)) {
            s->commands->block_flags^=0x80;
            if(s->gear_gate&0x40) { s->gear_message=0x83; event=r->raw_event; break; }
            event=r->raw_event;
        }
        CALL(FLIGHT_GEAR); break;
    case COMMAND_TARGET:
        if(s->commands->indexed.mode==0x7d) {
            CALL(FLIGHT_TARGET); s->target->secondary_flags^=0x1000;
        }
        break;
    case COMMAND_FLARE:
        if(!r->modifier) return countermeasure(s,r,carry,ops,1,published_event);
        if(s->commands->indexed.mode!=6 || s->spawn_gate) break;
        if((s->spawn_slots[0]->flags&0x40) && (s->spawn_slots[1]->flags&0x40) &&
           (s->spawn_slots[2]->flags&0x40)) break;
        s->commands->block_flags|=0x0f;
        {
            FA18FlightCommandChildInput args={0x1c,0,{0x1c,0x30}};
            FlightCommandResult result;
            if(!ops->consume(ops->context,s,FLIGHT_FLARE_SPAWN,&args,&result)) return 0;
            event=(result.event&0xffff0000u)|(r->raw_event&0xffffu);
        }
        break;
    case COMMAND_CHAFF: return countermeasure(s,r,carry,ops,0,published_event);
    case COMMAND_ECM:
        s->emitted_requests|=8; s->redraw_c=3;
        CALL(FLIGHT_ECM); s->ecm_enabled=s->ecm_enabled?0:1; break;
    case COMMAND_SIGN_INPUT: s->sequence_phase=r->modifier?0xff:1; break;
    default: return 0;
    }
#undef CALL
    *published_event=event; return 1;
}

int fa18_apply_flight_control_child(FA18FlightCommandState *s,
                                    enum FlightCommandChild child,
                                    const FA18FlightCommandChildInput *input,
                                    FlightCommandResult *result) {
    uint8_t field,mask;
    if(!s || !s->commands || !s->player || !input || !result) return 0;
    switch(child) {
    case FLIGHT_Y_DOWN: case FLIGHT_Y_UP: case FLIGHT_Y_RELEASE:
        field=child==FLIGHT_Y_DOWN?0x20:child==FLIGHT_Y_UP?0x10:0;
        s->stick_y=field; mask=0xcf; break;
    case FLIGHT_X_RIGHT: case FLIGHT_X_LEFT: case FLIGHT_X_RELEASE:
        field=child==FLIGHT_X_RIGHT?8:child==FLIGHT_X_LEFT?4:0;
        s->stick_x=field; mask=0xf3; break;
    case FLIGHT_THROTTLE_RELEASE: case FLIGHT_THROTTLE_MODE_RELEASE:
        s->commands->indexed.function_level=0;
        s->commands->indexed.throttle=0; s->commands->indexed.throttle_companion=0;
        goto finished;
    case FLIGHT_SPACE_RELEASE:
        s->emitted_requests|=0x0800; s->command_word&=0xfff7; goto finished;
    default: return 0;
    }
    if(s->pause || s->context_started)
        s->player->stick=(uint8_t)((s->player->stick&mask)|field);
finished:
    result->event=input->event; result->carried_event_word=input->restore_event_word;
    return 1;
}
