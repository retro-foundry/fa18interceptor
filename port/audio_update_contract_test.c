#include "audio_update.h"
#include <assert.h>

typedef struct {
    FA18CommandAudio *audio;
    unsigned channel,outputs,acks,fail;
    uint16_t period,volume;
} Output;
static int output(void *context,FA18AudioParameter parameter,uint16_t value) {
    Output *o=context;
    assert(parameter==((o->outputs&1)?FA18_AUDIO_VOLUME:FA18_AUDIO_PERIOD));
    ++o->outputs;
    if(parameter==FA18_AUDIO_PERIOD) o->period=value; else o->volume=value;
    return o->outputs!=o->fail;
}
static void ack(void *context,unsigned channel,uint16_t mask) {
    Output *outputs=context;
    Output *o=&outputs[channel];
    assert(mask==(uint16_t)(0x80u<<channel));
    ++o->acks;
}
int main(void) {
    /* Exact sound-4 program shape. Command patches and audio playback share
     * the same data/voice, so a status tone runs to completion. */
    uint32_t selectors[]={0x24,0x28,8,12,0x2c,8,12,0x2c,0x44,0x2c,0x40,0x2c};
    uint32_t values[]={1,1,300u<<16,2u<<16,1,300u<<16,2u<<16,1,16,1,8,0};
    PortVoiceOperation operations[12];
    PortVoiceProgram program={operations,{values,12}};
    FA18CommandAudio audio={0}; FA18CommandVoice voice={0};
    FA18AudioUpdate update={0}; Output outputs[4]={{0}};
    uint32_t sweep_values[5]={0},event;
    unsigned i;
    assert(fa18_import_voice_operations(selectors,12,operations));
    voice.program=&program; voice.period_slide=1; voice.period_ticks=1;
    audio.master_volume=63u<<16;
    audio.programmed_voice=&voice; audio.programmed=program.data;
    audio.sweep=(FA18CommandSoundProgram){sweep_values,5};
    audio.acknowledge=ack; audio.acknowledge_context=outputs;
    update.audio=&audio;
    for(i=0;i<4;++i) {
        outputs[i].audio=&audio; outputs[i].channel=i;
        audio.interrupt_masks[i]=(uint16_t)(0x80u<<i);
        update.channels[i]=(FA18AudioUpdateChannel){&audio.slots[i],output,&outputs[i]};
    }
    assert(fa18_play_native_status_tone(&audio,&event) && event==2 && outputs[3].acks==3);
    assert(fa18_update_native_audio(&update) && outputs[3].period==300 && outputs[3].volume==4);
    assert(voice.period==(300u<<16)+1 && !voice.period_slide);
    for(i=0;i<20 && audio.slots[3];++i) assert(fa18_update_native_audio(&update));
    assert(!audio.slots[3] && outputs[3].acks==4 && voice.position==96);
    assert(outputs[3].outputs>2 && !outputs[0].outputs && !outputs[1].outputs && !outputs[2].outputs);

    /* Even a program-ending wait outputs and advances that voice once. */
    voice.position=88; voice.delay=1; voice.period=0x80000000u;
    voice.volume=0xffffffffu; voice.volume_slide=1; voice.volume_ticks=1;
    audio.slots[1]=&voice; audio.master_volume=0xffff0000u;
    assert(fa18_update_native_audio(&update));
    assert(!audio.slots[1] && outputs[1].period==124 && outputs[1].volume==0xffff);
    assert(!voice.volume && !voice.volume_slide && !voice.volume_ticks);

    /* Aliased descriptor slots retain source channel order: the first
     * termination leaves the next channel's shared slot empty. */
    voice.position=88; voice.delay=1; audio.slots[0]=&voice;
    update.channels[1].slot=&audio.slots[0];
    outputs[0].outputs=outputs[1].outputs=0;
    assert(fa18_update_native_audio(&update) && outputs[0].outputs==2 && !outputs[1].outputs);
    update.channels[1].slot=&audio.slots[1];

    audio.volume_fading=1; audio.master_volume=0x80000010u; audio.master_volume_target=0x80000000u;
    assert(fa18_fade_native_master_volume(&audio) && audio.master_volume==0);
    audio.master_volume=0x7ffff000u; audio.master_volume_target=0x7fffffffu;
    assert(fa18_fade_native_master_volume(&audio) && audio.master_volume==0x80003000u);
    audio.master_volume=0x3effffu; audio.master_volume_target=0x400000u;
    assert(fa18_fade_native_master_volume(&audio) && audio.master_volume==0x3f0000u);
    audio.volume_fading=0;
    assert(fa18_fade_native_master_volume(&audio) && audio.master_volume==0x3f0000u);

    voice.delay=0; audio.slots[2]=&voice; voice.period=123u<<16;
    outputs[2].outputs=0; outputs[2].fail=1; event=voice.volume;
    assert(!fa18_update_native_audio(&update) && outputs[2].period==124 && voice.volume==event);
    selectors[0]=0;
    assert(!fa18_import_voice_operations(selectors,12,operations));
    assert(!fa18_import_voice_operations(selectors,0,operations));
    assert(!fa18_update_native_audio(NULL));
    return 0;
}
