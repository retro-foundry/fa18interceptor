/* C12950's complete control/sound-action selector. Source contract:
 * control_readouts.c and analysis/data/control_readouts_source_scope.json.
 * No CPU working state, source stack, instruction helper or frame adapter. */
#include "control_actions.h"
#include "globals.h"
#include "audio.h"
#include "fixed_math.h"
#include <stdlib.h>

#define ACTION_VIEW 0xc45785u
#define VIEW_FACTOR 0xc457b5u
#define ACTION_ENABLED 0xc45795u
#define SCRIPT_COUNTDOWN 0xc45885u
#define PENDING_SOUNDS 0xc45b54u
#define ACTION_DIVISOR 0xc45b42u

void consume_control_sound(const ControlSoundCall *call) {
    const int32_t *a=call->argument;
    switch(call->kind) {
    case CONTROL_SCRIPT: play_scripted_sound(a[0]); break;
    case CONTROL_STOP_SCRIPT: stop_channel_2(); break;
    case CONTROL_PROGRAM: play_programmed_sound(a); break;
    case CONTROL_FREE_VOICE: free_voice(a[0]); break;
    case CONTROL_EVENT_SIX: start_sound_6(a[0],a[1]); break;
    case CONTROL_EVENT_DISPATCH: dispatch_event_sound((int16_t)a[0],(int16_t)a[1]); break;
    case CONTROL_ENGINE: play_engine(a[0],a[1]); break;
    case CONTROL_MAIN_ENGINE: play_main_engine(a[0],a[1]); break;
    case CONTROL_ENGINE_SLIDE: slide_engine(a[0],a[1],a[2]); break;
    case CONTROL_MAIN_ENGINE_SLIDE: slide_main_engine(a[0],a[1],a[2]); break;
    case CONTROL_NOISE: play_noise(a[0]); break;
    default: abort();
    }
}
static void sound(ControlSoundSink sink,void *context,enum ControlSoundKind kind,
                  unsigned count,int32_t a,int32_t b,int32_t c) {
    const ControlSoundCall call={kind,count,{a,b,c}};
    if(sink) sink(context,&call); else consume_control_sound(&call);
}
static int16_t divided_amount(int16_t value,int16_t divisor) {
    int32_t remainder;
    return (int16_t)long_divide((int32_t)value*32,divisor,&remainder);
}
int16_t control_sound_magnitude(gaddr record,int16_t value) {
    /* C131BE's zero-input return bypasses all view-factor adjustments. */
    if(!value) return (rd_u16(record+2)&0x80) && rd_u16(record+0x6e)?12:0;
    if(rd_u8(FIRE_ALERT_COUNTDOWN)) value=63;
    else if(value>8) value=(int16_t)(63-((int16_t)(120-value)>>2));
    else value=(int16_t)(60-((int16_t)(120-value)>>1));
    if(!rd_u8(ACTION_VIEW)) {
        if(rd_s8(0xc45889u)>5) value=(int16_t)(value>>1);
        return value;
    }
    if(!value) return 0;
    if(rd_u8(VIEW_FACTOR)) {
        int16_t factor=rd_s8(0xc458b2u);
        if(factor>4) { factor=(int16_t)(8-factor); if(factor<0) factor=0; }
        switch(factor) {
        case 0: case 1: value=five_eighths(value); break;
        case 2: value=(int16_t)(value-(value>>2)); break;
        case 3: value=(int16_t)(value-(value>>3)); break;
        }
        switch((int)rd_u8(ACTION_VIEW)-1) {
        case 0: value=(int16_t)(value-(value>>3)); break;
        case 1: value=(int16_t)(value-(value>>2)); break;
        case 2: value=five_eighths(value); break;
        case 3: value=(int16_t)(value>>1); break;
        }
    } else if(rd_u16(ACTION_DIVISOR)) {
        value=divided_amount(value,rd_s16(ACTION_DIVISOR));
        if(value>84) value=84;
    }
    if(!(rd_u16(record+2)&8)) value=(int16_t)(value-(value>>2));
    return value<=0?1:value;
}
int16_t control_sound_step(gaddr record) {
    if((rd_u8(record+0x62)&0xf0)==0x30) return 16;
    int16_t value=rd_s16(record+0x6e);
    if(value<0) value=(int16_t)(0u-(uint16_t)value); /* -32768 remains negative. */
    value=(int16_t)(value>>9);
    if(value>63) value=63;
    if(!rd_u8(ACTION_VIEW)) value=(int16_t)(value>>1);
    return value?value:1;
}
static void pending_sound(gaddr record,ControlSoundSink sink,void *context) {
    /* Priority and nine-long programs are the original C12A56-C12C62 pushes,
     * in ordinary call argument order rather than reverse stack order. */
    static const int32_t programs[][9]={
        {160,16,8,200,16,5,3,4,4}, {324,10,8,200,10,5,3,1,4},
        {124,16,8,180,16,5,3,3,4}, {229,3,5,229,3,5,1,0,1},
        {192,4,9,200,0,3,1,0,1}, {500,15,5,500,15,5,3,1,5},
        {500,10,5,130,0,1,3,3,5}, {400,8,5,330,0,1,3,2,5},
        {500,10,5,130,0,1,3,1,5}
    };
    uint32_t flags=rd_u32(PENDING_SOUNDS);
    int index=-1;
    if(flags&0x40) index=0;
    else if(!(rd_u8(record+0x7c)&15)) {
        if(flags&0x80) index=1;
        else if(flags&1) index=2;
        else if(flags&4) index=3;
        else if(flags&0x100) index=4;
        else if(flags&0x10) index=5;
        else if(flags&2) index=6;
        else if(flags&0x20) index=7;
        else if(flags&0x800) index=8;
        else if(flags&8) sound(sink,context,CONTROL_FREE_VOICE,1,3,0,0);
    }
    if(index>=0) {
        ControlSoundCall call={CONTROL_PROGRAM,9,{0}};
        for(unsigned i=0;i<9;++i) call.argument[i]=programs[index][i];
        if(sink) sink(context,&call); else consume_control_sound(&call);
        if(index==2) wr_u8(TONE_MUTE,3);
    }
    wr_u32(PENDING_SOUNDS,0);
}
static int16_t absolute_word(int16_t value) { return value<0?(int16_t)(0u-(uint16_t)value):value; }
void update_control_actions(ControlSoundSink sink,void *context) {
    const gaddr record=CONTROL_RECORDS+((uint32_t)(int32_t)rd_s16(TARGET_RECORD)<<9);
    const gaddr header=record+2;
    const uint8_t action=rd_u8(FIRE_STATE);
    wr_u32(CURRENT_RECORD,record);
    if(rd_u8(CONTEXT_STATE) || rd_u8(PAUSE_A) || rd_u8(POST_INPUT_EVENT) || !rd_u8(ACTION_ENABLED)) return;
    if(rd_s8(SCRIPT_COUNTDOWN)<0) {
        if(rd_u8(ACTION_VIEW)) {
            if(rd_u8(VIEW_FACTOR)) sound(sink,context,CONTROL_SCRIPT,1,5,0,0);
        } else sound(sink,context,CONTROL_SCRIPT,1,10,0,0);
        wr_u8(SCRIPT_COUNTDOWN,rd_u8(SCRIPT_COUNTDOWN)&0x7f);
    } else if((uint8_t)(rd_u8(SCRIPT_COUNTDOWN)-1)==0) {
        sound(sink,context,CONTROL_STOP_SCRIPT,0,0,0,0); wr_u8(SCRIPT_COUNTDOWN,0);
    }
    if(rd_u32(PENDING_SOUNDS) && rd_s8(TONE_MUTE)<=0) pending_sound(record,sink,context);
    if(rd_u8(0xc457b8u)) {
        uint8_t count=(uint8_t)(rd_u8(0xc457b8u)-1); wr_u8(0xc457b8u,count);
        if((int8_t)count<=0) {
            int32_t period=rd_s8(0xc4586au),ticks=rd_s8(0xc45869u);
            if(!rd_u8(ACTION_VIEW)) period>>=1;
            sound(sink,context,CONTROL_EVENT_SIX,2,period,ticks,0);wr_u8(0xc457b8u,0);
        }
    }
    if(!action && (rd_u8(0xc457c1u)&3)) goto countdown;
    if((rd_u8(record+0x62)&0xf0)==0x30) {
        sound(sink,context,CONTROL_NOISE,1,control_sound_step(record),0,0);goto consumed;
    }
    int16_t phase=absolute_word(rd_s8(record+0x2b));
    int16_t maximum=61,magnitude=control_sound_magnitude(record,phase);
    if(!magnitude) {
        sound(sink,context,CONTROL_NOISE,1,control_sound_step(record),0,0);goto consumed;
    }
    int16_t bank=absolute_word((int16_t)(rd_s16(record+0x5a)>>4));
    int16_t pitch=absolute_word((int16_t)(rd_s16(record+0x56)>>4));
    if(bank<pitch) bank=(int16_t)(bank+(rd_s16(record+0x56)>>4));
    if(phase>=120) phase=(int16_t)(phase-bank);
    else { phase=(int16_t)(phase+bank); if(phase>120) phase=120; }
    if(rd_u8(ACTION_VIEW)) {
        magnitude=(int16_t)(magnitude-(magnitude>>2));
        if(!rd_u8(VIEW_FACTOR) && rd_u16(ACTION_DIVISOR)) {
            maximum=divided_amount(maximum,rd_s16(ACTION_DIVISOR)); if(maximum>63) maximum=63;
        }
        const int16_t period=(int16_t)(800-(int16_t)(phase*2)-(phase>>1)-(phase>>2));
        switch(action) {
        case 0xfe: sound(sink,context,CONTROL_ENGINE,2,period,magnitude,0); break;
        case 0xfd: sound(sink,context,CONTROL_ENGINE,2,period+32,maximum,0); break;
        case 0xfc: sound(sink,context,CONTROL_ENGINE,2,period+32,magnitude,0); break;
        case 0xfb: sound(sink,context,CONTROL_EVENT_DISPATCH,2,period,maximum,0); break;
        case 0xfa: {
            int16_t twice=(int16_t)(maximum*2); if(twice>63) twice=63;
            sound(sink,context,CONTROL_ENGINE,2,period+22,twice,0);break;
        }
        default: sound(sink,context,CONTROL_ENGINE_SLIDE,3,period,magnitude,32); break;
        }
    } else {
        if(magnitude<40) magnitude=40;
        const int16_t period=(int16_t)(784-(int16_t)(phase*2)-(phase>>2));
        switch(action) {
        case 0xfe:
            if(!(rd_u16(header)&8)) sound(sink,context,CONTROL_MAIN_ENGINE,2,period,magnitude,0);
            else sound(sink,context,CONTROL_ENGINE,2,period,magnitude>>2,0);
            break;
        case 0xfd: sound(sink,context,CONTROL_ENGINE,2,period+35,maximum>>1,0); break;
        case 0xfc: sound(sink,context,CONTROL_MAIN_ENGINE,2,period+32,magnitude,0); break;
        case 0xfb: sound(sink,context,CONTROL_EVENT_DISPATCH,2,period,maximum,0); break;
        case 0xfa: {
            int16_t twice=(int16_t)(maximum*2); if(twice>40) twice=40;
            sound(sink,context,CONTROL_ENGINE,2,period+22,twice,0);break;
        }
        default:
            if(rd_u8(FIRE_ALERT_COUNTDOWN)) magnitude=(rd_u8(0xc45b5bu)&1)?10:40;
            else if((rd_u16(header)&8) && (rd_u8(0xc45b5bu)&1))
                magnitude=(int16_t)((magnitude>>2)+(magnitude>>3));
            sound(sink,context,CONTROL_MAIN_ENGINE_SLIDE,3,period,magnitude,32);break;
        }
    }
consumed:
    wr_u8(FIRE_STATE,0);
countdown:
    if(rd_s8(FIRE_ALERT_COUNTDOWN)>0) {
        uint8_t count=(uint8_t)(rd_u8(FIRE_ALERT_COUNTDOWN)-1);wr_u8(FIRE_ALERT_COUNTDOWN,count);
        if(!count && !rd_u8(ACTION_VIEW) && !(rd_u16(header)&8)) wr_u8(FIRE_STATE,0xfe);
    }
}
