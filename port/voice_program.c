#include "voice_program.h"

PortVoiceResult port_step_voice_program(PortVoice *v,PortVoice **slot,
                                        void (*ended)(void *context),void *context) {
    const PortVoiceProgram *p;
    size_t cursor;
    if(!v || !slot || !ended) return PORT_VOICE_INVALID_ARGUMENT;
    if(!v->delay) return PORT_VOICE_OK;
    --v->delay;
    if(v->delay) return PORT_VOICE_OK;
    p=v->program;
    if(!p || !p->operations || !p->data.values || !p->data.count ||
       p->data.count>UINT32_MAX/PORT_VOICE_INSTRUCTION_BYTES ||
       v->position%PORT_VOICE_INSTRUCTION_BYTES) return PORT_VOICE_INVALID_PROGRAM;
    cursor=v->position/PORT_VOICE_INSTRUCTION_BYTES;
    for(;;) {
        PortVoiceOperation operation;
        uint32_t value,*counter;
        if(cursor>=p->data.count) return PORT_VOICE_INVALID_PROGRAM;
        operation=p->operations[cursor]; value=p->data.values[cursor++];
        switch(operation) {
        case PORT_VOICE_SET_PERIOD: v->period=value; break;
        case PORT_VOICE_SET_VOLUME: v->volume=value; break;
        case PORT_VOICE_SET_PERIOD_SLIDE: v->period_slide=value; break;
        case PORT_VOICE_SET_VOLUME_SLIDE: v->volume_slide=value; break;
        case PORT_VOICE_SET_LOOP0: v->loop_counters[0]=value; break;
        case PORT_VOICE_SET_LOOP1: v->loop_counters[1]=value; break;
        case PORT_VOICE_WAIT:
            v->delay=value; v->position=(uint32_t)(cursor*PORT_VOICE_INSTRUCTION_BYTES);
            if(!value) { *slot=NULL; ended(context); }
            return PORT_VOICE_OK;
        case PORT_VOICE_LOOP0: case PORT_VOICE_LOOP1:
            counter=&v->loop_counters[operation==PORT_VOICE_LOOP0?0:1];
            if(*counter && --*counter==0) break;
            if(value%PORT_VOICE_INSTRUCTION_BYTES ||
               value/PORT_VOICE_INSTRUCTION_BYTES>=p->data.count) return PORT_VOICE_INVALID_PROGRAM;
            cursor=value/PORT_VOICE_INSTRUCTION_BYTES;
            break;
        default: return PORT_VOICE_INVALID_PROGRAM;
        }
    }
}

void port_advance_voice_slides(PortVoice *v) {
    v->period+=v->period_slide;
    v->volume+=v->volume_slide;
    if(v->period_ticks && --v->period_ticks==0) v->period_slide=0;
    if(v->volume_ticks && --v->volume_ticks==0) v->volume_slide=0;
}
