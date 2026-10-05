#include "command_effects.h"
#include "audio_selection.h"

static int valid_audio(const FA18CommandAudio *a) {
    return a && a->acknowledge && a->programmed.values && a->programmed.count>=10 &&
           a->sweep.values && a->sweep.count>=3;
}

static int release_voice(FA18CommandAudio *a,unsigned channel) {
    return fa18_release_native_audio_channel(a,channel);
}

static int play_voice(FA18CommandAudio *a,FA18CommandVoice *const *voice,unsigned channel) {
    return fa18_select_native_sound(a,voice,1,0,channel,0);
}

int fa18_release_native_command_voices(FA18CommandAudio *a,uint32_t *result) {
    unsigned channel;
    if(!valid_audio(a) || !result) return 0;
    for(channel=0;channel<4;++channel)
        if(!release_voice(a,channel)) return 0;
    *result=12;
    return 1;
}

int fa18_play_native_status_tone(FA18CommandAudio *a,uint32_t *result) {
    uint32_t *p,pitch;
    if(!valid_audio(a) || !result) return 0;
    /* $C3318E/$C3316A: the signed mute byte only suppresses positive values. */
    if(a->tone_mute==0 || a->tone_mute>=128) {
        if(a->programmed_voice) {
            if(!release_voice(a,3)) return 0;
            p=a->programmed.values;
            pitch=a->volume_fading?2u:4u;
            p[0]=2; p[1]=1; p[2]=300u<<16; p[3]=pitch<<16; p[4]=1;
            p[5]=300u<<16; p[6]=pitch<<16; p[7]=1;
            /* Instruction 8 is the loop jump, not the final delay. */
            p[9]=2;
            a->programmed_voice->position=0;
            a->programmed_voice->delay=1;
            if(!play_voice(a,&a->programmed_voice,3)) return 0;
        }
    }
    *result=2;
    return 1;
}

int fa18_post_native_command_message(FA18CommandEffects *e,uint32_t event,uint32_t *result) {
    uint16_t code=(uint16_t)event,kind=code&0xff00u;
    if(!e || !e->context || !e->context->view || !e->context->view->flight ||
       !e->context->view->flight->commands || !result) return 0;
    e->message_code=code;
    e->context->view->flight->commands->indexed.cockpit_low_byte|=1;
    if(kind&0x2000u) e->message_state=(e->message_state&0x7fffu)|0x2000u;
    else if(kind==0x4000 || kind==0x4800) e->message_state&=0xdfffu;
    *result=(event&0xffff0000u)|kind;
    return 1;
}

int fa18_press_native_space_command(FA18FlightCommandState *f) {
    uint8_t command;
    if(!f || !f->commands || !f->player || !f->target) return 0;
    command=f->commands->block_flags&15;
    if(command) return 1;
    if(f->commands->indexed.mode==0x7d) {
        if(f->weapon_pause>=128) f->target->secondary_flags|=0x0800;
        return 1;
    }
    f->emitted_requests|=0x0400;
    command=f->player->weapon_radar&0xf0;
    if(command==0x10) f->command_word|=8;
    else if(command) f->space_command_latch=1;
    return 1;
}

static uint32_t random_bits11(FA18CommandAudio *a) {
    unsigned i;
    uint32_t value=0;
    for(i=0;i<11;++i) {
        uint32_t bit=(a->random_seed^(a->random_seed>>3))&1;
        a->random_seed=(a->random_seed>>1)|(bit<<31);
        value=(value<<1)|bit;
    }
    return value;
}

int fa18_start_native_sound6(FA18CommandEffects *e,uint32_t event,
                             int32_t period,int32_t ticks,uint32_t *result) {
    FA18CommandAudio *a;
    uint32_t bits;
    int32_t numerator;
    if(!e || !e->context || !e->context->view || !valid_audio(e->audio) || !result) return 0;
    a=e->audio;
    if(!(a->effect_flags&1)) {
        e->context->view->fire_state=0xfa;
        a->sound6_mode=2;
        *result=event;
        return 1;
    }
    if(!a->sweep_voice) { *result=event; return 1; }
    if(!release_voice(a,2)) return 0;
    bits=(uint32_t)period<<16;
    a->sweep.values[0]=bits;
    bits=0u-bits;
    numerator=bits<0x80000000u?(int32_t)bits:(int32_t)((int64_t)bits-0x100000000LL);
    /* $C52EC8 is a software divider: zero divisor returns zero, and the
     * quotient wraps to 32 bits even for INT32_MIN / -1. */
    a->sweep.values[1]=ticks?(uint32_t)((int64_t)numerator/ticks):0;
    a->sweep.values[2]=(uint32_t)ticks;
    a->sweep_voice->position=0;
    a->sweep_voice->period=(random_bits11(a)*4u+0x231eu)<<16;
    a->sweep_voice->delay=1;
    if(!play_voice(a,&a->sweep_voice,2)) return 0;
    *result=8;
    return 1;
}

static int flight_effect(void *context,FA18FlightCommandState *f,
                          enum FlightCommandChild child,
                          const FA18FlightCommandChildInput *input,FlightCommandResult *result) {
    FA18CommandEffects *e=context;
    uint32_t event;
    if(!e || !e->context || !e->context->view || f!=e->context->view->flight ||
       !f->commands || !input || !result) return 0;
    event=input->event;
    switch(child) {
    case FLIGHT_SPACE_PRESS:
        if(!fa18_press_native_space_command(f)) return 0;
        break;
    case FLIGHT_HOOK_SOUND: case FLIGHT_WEAPON_SOUND:
    case FLIGHT_FLARE_SOUND: case FLIGHT_CHAFF_SOUND:
        if(!fa18_post_native_command_message(e,event,&event)) return 0;
        break;
    case FLIGHT_FLARE_SPAWN: /* Original identity: $C17F8C starts sound 6. */
        if(!fa18_start_native_sound6(e,event,input->arguments[0],input->arguments[1],&event)) return 0;
        break;
    case FLIGHT_NEXT_TARGET: case FLIGHT_RADAR_RANGE: case FLIGHT_THROTTLE_MODE:
    case FLIGHT_HOOK: case FLIGHT_WEAPON_ENABLE: case FLIGHT_WEAPON_MODE:
    case FLIGHT_GEAR: case FLIGHT_TARGET: case FLIGHT_ECM:
        if(!f->commands->origin_mode && !fa18_play_native_status_tone(e->audio,&event)) return 0;
        break;
    default: return 0;
    }
    result->event=event;
    result->carried_event_word=input->restore_event_word;
    return 1;
}

static int context_effect(void *context,FA18ContextCommandState *s,
                           enum ContextCommandChild child,
                           const FA18ContextCommandChildInput *input,
                           FA18ContextCommandChildResult *result) {
    FA18CommandEffects *e=context;
    if(!e || s!=e->context || !input || !result ||
       (child!=CONTEXT_COMMAND_MAP_VOICES && child!=CONTEXT_COMMAND_REQUEST_VOICES)) return 0;
    if(!fa18_release_native_command_voices(e->audio,&result->event)) return 0;
    result->position[0]=result->position[1]=result->position[2]=0;
    return 1;
}

static uint32_t indexed_status(void *context,FA18IndexedControls *state) {
    FA18CommandEffects *e=context;
    uint32_t result=0;
    (void)state; /* Shares the validated command owner installed below. */
    fa18_play_native_status_tone(e->audio,&result);
    return result;
}

int fa18_initialize_command_effects(FA18CommandEffects *e,FA18ContextCommandState *s,
                                    FA18CommandQueue *q,FA18CommandAudio *a) {
    if(!e || !s || !s->view || !s->view->flight || !q ||
       !s->view->flight->commands || q->commands!=s->view->flight->commands ||
       s->key_taken!=&q->taken || !valid_audio(a)) return 0;
    e->context=s; e->audio=a;
    e->flight_ops=(FA18FlightCommandOps){flight_effect,e};
    e->context_ops=(FA18ContextCommandOps){context_effect,e};
    /* Both fields are reachable through signed queue indices. */
    if(!fa18_bind_command_queue_byte(q,0x36,&a->sound6_mode) ||
       !fa18_bind_command_queue_byte(q,0x76,&a->volume_fading)) return 0;
    return 1;
}

int fa18_install_command_effect_owners(FA18CommandEffects *e,FA18NativeCommandOwners *owners) {
    if(!e || !e->context || !e->context->view || !e->context->view->flight ||
       !e->context->view->flight->commands || !valid_audio(e->audio) || !owners ||
       e->flight_ops.consume!=flight_effect || e->flight_ops.context!=e ||
       e->context_ops.consume!=context_effect || e->context_ops.context!=e) return 0;
    owners->status_tone=indexed_status; owners->status_context=e;
    owners->flight=&e->flight_ops; owners->context=&e->context_ops;
    return 1;
}
