#include "voice_program.h"
#include <assert.h>

typedef struct { PortVoice **slot; unsigned count; } End;
static void ended(void *context) {
    End *end=context;
    assert(!*end->slot);
    ++end->count;
}

int main(void) {
    PortVoiceOperation operations[]={PORT_VOICE_SET_LOOP0,PORT_VOICE_SET_PERIOD,
                                     PORT_VOICE_WAIT,PORT_VOICE_LOOP0,PORT_VOICE_WAIT};
    uint32_t values[]={2,300u<<16,1,8,0};
    PortVoiceProgram program={operations,{values,5}};
    PortVoice voice={0},other={0},*slot=&voice;
    End end={&slot,0};
    voice.program=&program; voice.delay=1;
    assert(port_step_voice_program(&voice,&slot,ended,&end)==PORT_VOICE_OK);
    assert(voice.position==24 && voice.delay==1 && voice.loop_counters[0]==2 && !end.count);
    assert(port_step_voice_program(&voice,&slot,ended,&end)==PORT_VOICE_OK);
    assert(voice.position==24 && voice.loop_counters[0]==1 && slot==&voice);
    assert(port_step_voice_program(&voice,&slot,ended,&end)==PORT_VOICE_OK);
    assert(!voice.delay && voice.position==40 && !slot && end.count==1);

    /* A zero counter jumps, while a one counter falls through without
     * consulting a jump that would otherwise be an invalid asset cursor. */
    operations[0]=PORT_VOICE_LOOP1; values[0]=8; voice.position=0; voice.delay=1;
    slot=&other;
    assert(port_step_voice_program(&voice,&slot,ended,&end)==PORT_VOICE_OK && voice.position==24);
    voice.loop_counters[1]=1; values[0]=7; voice.position=0; voice.delay=1;
    assert(port_step_voice_program(&voice,&slot,ended,&end)==PORT_VOICE_OK);
    assert(!voice.loop_counters[1] && voice.position==24);
    voice.position=0; voice.delay=1;
    assert(port_step_voice_program(&voice,&slot,ended,&end)==PORT_VOICE_INVALID_PROGRAM);
    assert(!voice.delay && !voice.position);
    voice.position=1; voice.delay=1;
    assert(port_step_voice_program(&voice,&slot,ended,&end)==PORT_VOICE_INVALID_PROGRAM);
    voice.position=program.data.count*8; voice.delay=1;
    assert(port_step_voice_program(&voice,&slot,ended,&end)==PORT_VOICE_INVALID_PROGRAM);
    voice.program=NULL; voice.delay=2;
    assert(port_step_voice_program(&voice,&slot,ended,&end)==PORT_VOICE_OK && voice.delay==1);
    assert(port_step_voice_program(&voice,&slot,ended,&end)==PORT_VOICE_INVALID_PROGRAM);
    assert(port_step_voice_program(NULL,&slot,ended,&end)==PORT_VOICE_INVALID_ARGUMENT);

    voice.period=0xffffffffu; voice.period_slide=1; voice.period_ticks=1;
    voice.volume=0; voice.volume_slide=0xffffffffu; voice.volume_ticks=0;
    port_advance_voice_slides(&voice);
    assert(!voice.period && !voice.period_slide && !voice.period_ticks);
    assert(voice.volume==0xffffffffu && voice.volume_slide==0xffffffffu);
    return 0;
}
