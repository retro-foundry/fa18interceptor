#include "context_command_input.h"
#include <assert.h>
#include <string.h>

typedef struct { unsigned calls; uint32_t event; } Audio;
/* Explicit voice-release result used only by this contract test. */
static int voices(void *context,FA18ContextCommandState *s,enum ContextCommandChild child,
                   const FA18ContextCommandChildInput *input,FA18ContextCommandChildResult *result) {
    Audio *audio=context;
    assert(child==CONTEXT_COMMAND_MAP_VOICES || child==CONTEXT_COMMAND_REQUEST_VOICES);
    ++audio->calls; audio->event=input->event;
    if(child==CONTEXT_COMMAND_MAP_VOICES) s->map_middle_cache=0x12345678;
    else s->recorder_on=1;
    memset(result,0,sizeof *result); result->event=0xabcd1234; return 1;
}
int main(void) {
    FA18CommandInput commands;
    FA18FlightCommandState flight;
    FA18ViewCommandState view;
    FA18FlightCommandRecord aircraft;
    FA18ContextCommandRecord record;
    FA18ContextCommandState s;
    FA18ContextCommandPose pose;
    FA18ContextCommandPoses poses={&pose,1,-1};
    Audio audio={0,0};
    FA18ContextCommandOps ops={voices,&audio};
    CommandRequest request;
    uint8_t taken=0,recording[4]={1,2,3,4};
    uint32_t event;
    memset(&commands,0,sizeof commands); memset(&flight,0,sizeof flight);
    memset(&view,0,sizeof view); memset(&aircraft,0,sizeof aircraft);
    memset(&record,0,sizeof record); memset(&s,0,sizeof s); memset(&pose,0,sizeof pose);
    flight.commands=&commands; flight.viewed=&aircraft; view.flight=&flight;
    record.command_record=&aircraft; record.angle=0x1234;
    s.view=&view; s.records=&record; s.record_count=1; s.key_taken=&taken;
    commands.event_counter=commands.indexed.mode_gate=commands.indexed.mode=1;
    commands.pending_b=0x0200;
    assert(fa18_select_pending_command(&commands,&request));
    assert(request.action==COMMAND_CONTEXT_REFRESH && !commands.pending_b);
    assert(fa18_apply_context_input_command(&s,&request,NULL,NULL,&event));
    assert(view.detail_index==4 && view.emitted_requests==0x0200 && s.angle_history==0x1234);
    assert(flight.context_started==1 && !flight.pause && flight.command_word==2 &&
           s.view_request==0xff && commands.indexed.origin_gate_b==1);
    request=(CommandRequest){COMMAND_CONTEXT_SPECIAL,0x87654321,0,1,0,0};
    commands.indexed.pose_entry=0xff;
    pose.kind=FA18_CONTEXT_POSE_RECORD; pose.record=&record;
    aircraft.equipment_kind=0x20;
    record.inverse[0][0]=record.inverse[1][1]=record.inverse[2][2]=16;
    record.position[0]=100; record.position[1]=200; record.position[2]=300;
    assert(fa18_apply_context_input_command(&s,&request,&poses,NULL,&event));
    assert(taken==2 && s.origin_first==64 && view.origin_middle==247 && s.origin_third==252);
    assert(s.negated[0]==(uint32_t)-64 && s.negated[1]==(uint32_t)-247 &&
           s.negated[2]==(uint32_t)-252 && event==(uint32_t)-64 && !commands.indexed.origin_gate_b);
    /* A preset uses sign extension, SWAP and wrapped long arithmetic. */
    pose.kind=FA18_CONTEXT_POSE_PRESET;
    pose.preset[0]=-1; pose.preset[1]=2; pose.preset[3]=-3;
    pose.preset[4]=4; pose.preset[5]=-5; pose.grid_pair[0]=-6; pose.grid_pair[1]=7;
    assert(fa18_apply_context_input_command(&s,&request,&poses,NULL,&event));
    assert(s.origin_first==0xffffffc0u-512u && view.origin_middle==(uint32_t)-768 &&
           s.origin_third==0x00800000u+512u);
    request.action=COMMAND_MAP; request.raw_event=0x87654321;
    assert(fa18_apply_context_input_command(&s,&request,NULL,&ops,&event));
    assert(audio.calls==1 && audio.event==0x87654321 && event==0xabcd4321);
    assert(view.origin_middle==0x12345678 && s.negated[1]==0u-0x12345678 &&
           s.origin_first==0x10c00000 && s.origin_third==0x11400000 &&
           flight.pause==1 && !flight.context_started && s.pan==0x1c20 && !s.rotate);
    request.action=COMMAND_CONTEXT_REQUEST;
    s.recording_write=recording; s.recording_remaining=4;
    assert(fa18_apply_context_input_command(&s,&request,NULL,&ops,&event));
    assert(audio.calls==2 && event==0xabcd4321 && commands.indexed.origin_gate_a==0xff);
    assert(recording[0]==0xff && recording[1]==0xff && recording[2]==0xff && recording[3]==0xff);
    assert(fa18_apply_context_input_command(&s,&request,NULL,NULL,&event));
    assert(!commands.indexed.origin_gate_a && view.fire_state==0xfe && audio.calls==2);
    event=0xfeedface;
    assert(!fa18_apply_context_input_command(&s,&request,NULL,NULL,&event));
    assert(event==0xfeedface);
    pose.kind=FA18_CONTEXT_POSE_UNRESOLVED; request.action=COMMAND_CONTEXT_CALCULATION;
    assert(!fa18_apply_context_input_command(&s,&request,&poses,NULL,&event));
    assert(event==0xfeedface);
    assert(!fa18_is_context_input_command(COMMAND_GEAR));
    assert(!fa18_apply_context_input_command(NULL,&request,NULL,NULL,&event));
    return 0;
}
