#include "command_effects.h"
#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandAudio *audio;
    unsigned count, channels[16];
    FA18CommandVoice *slots[16];
} Acks;
static void ack(void *context,unsigned channel,uint16_t mask) {
    Acks *a=context;
    assert(a->count<16 && mask==(uint16_t)(0x80u<<channel));
    a->channels[a->count]=channel;
    a->slots[a->count++]=a->audio->slots[channel];
}

int main(void) {
    FA18CommandInput c={0}; FA18FlightCommandState f={0}; FA18ViewCommandState v={0};
    FA18ContextCommandState context={0}; FA18CommandQueue q={0};
    FA18FlightCommandRecord records[5]={{0}};
    FA18CommandVoice voice4={0},voice6={0};
    uint32_t p4[12],p6[3]={0},event;
    FA18CommandAudio audio={0}; FA18CommandEffects e={0};
    Acks calls={&audio,0,{0},{0}};
    uint8_t data[FA18_COMMAND_QUEUE_NEIGHBORS]={0},keys[FA18_COMMAND_KEY_TABLE_SIZE]={0};
    FA18NativeCommandDispatcher d={&context,&q,NULL,0};
    FA18NativeCommandOwners owners={0}; FA18NativeCommandOutcome outcome;
    FA18ViewSpanOffsets spans={{0}};
    CommandRequest request={0};
    FA18ContextCommandChildInput voice_input={0};
    FA18ContextCommandChildResult voice_result;
    unsigned i;
    for(i=0;i<12;++i) p4[i]=0xfeed0000u+i;
    f.commands=&c; f.player=f.viewed=&records[0]; f.target=&records[4];
    for(i=0;i<3;++i) f.spawn_slots[i]=&records[i+1];
    v.flight=&f; context.view=&v;
    data[0x36]=0x33; data[0x76]=0x80; data[0x59]=0x44;
    assert(fa18_initialize_command_queue(&q,&context,data,sizeof data,keys,sizeof keys));
    audio.programmed_voice=&voice4; audio.sweep_voice=&voice6;
    audio.programmed=(FA18CommandSoundProgram){p4,12}; audio.sweep=(FA18CommandSoundProgram){p6,3};
    audio.acknowledge=ack; audio.acknowledge_context=&calls;
    for(i=0;i<4;++i) audio.interrupt_masks[i]=(uint16_t)(0x80u<<i);
    assert(fa18_initialize_command_effects(&e,&context,&q,&audio));
    assert(audio.sound6_mode==0x33 && audio.volume_fading==0x80 && f.space_command_latch==0x44);
    owners.spans=&spans;
    assert(fa18_install_command_effect_owners(&e,&owners) && owners.spans==&spans);

    assert(fa18_play_native_status_tone(&audio,&event) && event==2 && calls.count==3);
    assert(!calls.slots[0] && !calls.slots[1] && calls.slots[2]==&voice4);
    assert(p4[3]==(2u<<16) && p4[6]==(2u<<16) && p4[8]==0xfeed0008 && p4[9]==2);
    assert(voice4.delay==1 && !voice4.position && !voice4.volume && audio.slots[3]==&voice4);
    audio.tone_mute=0x7f; calls.count=0;
    assert(fa18_play_native_status_tone(&audio,&event) && event==2 && !calls.count);
    audio.tone_mute=0x80; audio.volume_fading=0;
    assert(fa18_play_native_status_tone(&audio,&event) && calls.count==3 && p4[3]==(4u<<16));

    c.indexed.cockpit_high_byte=0xab; c.indexed.cockpit_low_byte=0x40; e.message_state=0xc123;
    assert(fa18_post_native_command_message(&e,0xbeef2012,&event) && event==0xbeef2000);
    assert(e.message_code==0x2012 && e.message_state==0x6123);
    assert(c.indexed.cockpit_high_byte==0xab && c.indexed.cockpit_low_byte==0x41);
    assert(fa18_post_native_command_message(&e,0xbeef4809,&event) && e.message_state==0x4123);
    assert(fa18_post_native_command_message(&e,0xbeef5001,&event) && e.message_state==0x4123);

    f.emitted_requests=f.command_word=0; c.block_flags=0; c.indexed.mode=1;
    records[0].weapon_radar=0x1b;
    assert(fa18_press_native_space_command(&f) && f.emitted_requests==0x400 && f.command_word==8);
    f.space_command_latch=0; records[0].weapon_radar=0x30;
    assert(fa18_press_native_space_command(&f) && f.space_command_latch==1);
    c.indexed.mode=0x7d; f.weapon_pause=0x80;
    assert(fa18_press_native_space_command(&f) && records[4].secondary_flags==0x800);
    records[4].secondary_flags=0; c.block_flags=1;
    assert(fa18_press_native_space_command(&f) && !records[4].secondary_flags);

    calls.count=0; audio.effect_flags=0;
    assert(fa18_start_native_sound6(&e,0x12345678,28,48,&event));
    assert(event==0x12345678 && audio.sound6_mode==2 && v.fire_state==0xfa && !calls.count);
    audio.effect_flags=1; audio.random_seed=0;
    assert(fa18_start_native_sound6(&e,0x12345678,28,48,&event));
    assert(event==8 && calls.count==3 && p6[0]==(28u<<16));
    assert(p6[1]==(uint32_t)-38229 && p6[2]==48 && voice6.period==0x231e0000u);
    assert(!voice6.position && voice6.delay==1 && !voice6.volume && audio.slots[2]==&voice6);
    calls.count=0; event=0xfeedface;
    assert(fa18_start_native_sound6(&e,0,28,0,&event));
    assert(event==8 && calls.count==3 && !p6[1] && !p6[2]);
    calls.count=0;
    assert(fa18_start_native_sound6(&e,0,0x8000,-1,&event));
    assert(p6[1]==0x80000000u && p6[2]==0xffffffffu && calls.count==3);

    /* Real parent -> message child -> publication, with no test child stub. */
    calls.count=0; c.event_counter=c.indexed.mode_gate=c.indexed.mode=1; c.block_flags=0;
    f.chaff_count=2; q.taken=q.count=q.write_index=0; q.key_table[0x33]=0x77;
    assert(fa18_dispatch_native_keyboard_command(&d,0x33,0,&owners,&outcome));
    assert(outcome.action==COMMAND_CHAFF && outcome.event==0x77 && f.chaff_count==1);
    assert(e.message_code==0x4028 && !calls.count);

    /* Compose status and message children across the hook parent's word swaps. */
    request.action=COMMAND_HOOK; request.raw_event=0xabcd005a;
    records[0].equipment_kind=0x11; records[0].secondary_flags=0;
    c.origin_mode=0; audio.tone_mute=1;
    assert(fa18_apply_flight_input_command(&f,&request,0,owners.flight,&event));
    assert(event==0x40000002u && e.message_code==0x4023 && !calls.count);
    records[0].secondary_flags=0; c.origin_mode=1;
    assert(fa18_apply_flight_input_command(&f,&request,0,owners.flight,&event));
    assert(event==0x4000005au && !calls.count);
    request.action=COMMAND_NEXT_TARGET;
    assert(fa18_apply_flight_input_command(&f,&request,0,owners.flight,&event));
    assert(event==request.raw_event && f.next_target==1 && !calls.count);
    c.origin_mode=0;
    assert(fa18_apply_flight_input_command(&f,&request,0,owners.flight,&event) && event==2);
    request.action=COMMAND_FLARE; request.modifier=1; c.indexed.mode=6;
    audio.effect_flags=1;
    assert(fa18_apply_flight_input_command(&f,&request,0,owners.flight,&event));
    assert(event==0x5a && calls.count==3 && (c.block_flags&15)==15 && p6[0]==(28u<<16) && p6[2]==48);
    calls.count=0;
    assert(owners.context->consume(owners.context->context,&context,CONTEXT_COMMAND_MAP_VOICES,
                                    &voice_input,&voice_result));
    assert(voice_result.event==12 && calls.count==4);
    calls.count=0;

    /* Queue byte writes update audio/space owners and read current word bytes. */
    q.taken=q.count=0; q.translated_index=(uint8_t)(0x76-138); q.key_table[1]=7;
    assert(fa18_publish_native_command(&q,1,&event) && audio.volume_fading==7);
    q.taken=q.count=0; q.translated_index=(uint8_t)(0x36-138);
    assert(fa18_publish_native_command(&q,1,&event) && audio.sound6_mode==7);
    q.taken=q.count=0; q.translated_index=(uint8_t)(0x59-138);
    assert(fa18_publish_native_command(&q,1,&event) && f.space_command_latch==7);
    c.indexed.throttle=(int16_t)0xabcd;
    assert(fa18_bind_command_queue_byte(&q,0x17,&data[0]) && data[0]==0xab);
    assert(!fa18_bind_command_queue_byte(&q,266,&data[0]));

    calls.count=0;
    assert(fa18_release_native_command_voices(&audio,&event) && event==12 && calls.count==4);
    for(i=0;i<4;++i) assert(calls.channels[i]==i && !audio.slots[i]);
    audio.acknowledge=NULL;
    assert(!fa18_initialize_command_effects(&e,&context,&q,&audio));
    assert(!fa18_install_command_effect_owners(&e,&owners));
    return 0;
}
