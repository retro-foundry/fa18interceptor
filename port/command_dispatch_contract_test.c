#include "command_dispatch.h"
#include <assert.h>
#include <string.h>

typedef struct { unsigned count; int fail; int16_t carry; } Sound;
static int sound(void *context,FA18FlightCommandState *s,enum FlightCommandChild child,
                   const FA18FlightCommandChildInput *input,FlightCommandResult *result) {
    Sound *calls=context;
    (void)s;
    assert(child==FLIGHT_CHAFF_SOUND);
    ++calls->count; calls->carry=input->restore_event_word;
    result->event=0xabcd0000; result->carried_event_word=input->restore_event_word;
    return !calls->fail;
}
static void callback(void *context) { (void)context; assert(0); }
typedef struct { unsigned count; int fail_add; } Registry;
static int registration(void *context,FA18InputCallbackOperation operation,
                          unsigned kind,FA18NativeInputDescriptor *descriptor) {
    Registry *r=context;
    assert(kind==5 && descriptor);
    assert(operation==((r->count&1)?FA18_INPUT_CALLBACK_ADD:FA18_INPUT_CALLBACK_REMOVE));
    ++r->count;
    if(operation==FA18_INPUT_CALLBACK_ADD) {
        assert(descriptor->type==2 && !descriptor->priority && descriptor->callback==callback);
        return !r->fail_add;
    }
    return 1;
}

int main(void) {
    FA18CommandInput c={0}; FA18FlightCommandState f={0}; FA18ViewCommandState v={0};
    FA18ContextCommandState context={0}; FA18CommandQueue q={0};
    FA18FlightCommandRecord records[5]={{0}};
    FA18NativeInputDescriptor descriptor={0};
    Registry registry={0}; Sound calls={0};
    FA18InputCallbackRegistration input={&descriptor,"fixture name",callback,NULL,registration,&registry};
    FA18NativeCommandDispatcher d={&context,&q,&input,0};
    FA18ViewSpanOffsets spans={{0}};
    FA18FlightCommandOps flight_ops={sound,&calls};
    FA18NativeCommandOwners owners={NULL,&spans,NULL,NULL,NULL,&flight_ops,NULL};
    FA18NativeCommandOutcome result;
    uint8_t data[FA18_COMMAND_QUEUE_NEIGHBORS]={0},keys[FA18_COMMAND_KEY_TABLE_SIZE]={0};
    unsigned i;
    f.commands=&c; f.player=f.viewed=&records[0]; f.target=&records[4];
    for(i=0;i<3;++i) f.spawn_slots[i]=&records[i+1];
    v.flight=&f; context.view=&v;
    keys[0]=0x5a; keys[0x34]=0x6b; keys[0x12]=0x71;
    assert(fa18_initialize_command_queue(&q,&context,data,sizeof data,keys,sizeof keys));
    c.event_counter=c.indexed.mode_gate=c.indexed.mode=1;
    assert(fa18_dispatch_native_keyboard_command(&d,0x33,0x1234,&owners,&result));
    assert(result.action==COMMAND_CHAFF && result.completion==FA18_COMMAND_PUBLISHED);
    assert(calls.count==1 && calls.carry==0x1200 && result.event==0xabcd005a && q.raw[0]==0);
    q.taken=q.count=0; c.pending_a=4; /* first-word low byte bit 2 */
    assert(fa18_dispatch_native_pending_command(&d,0x1234,&owners,&result));
    assert(result.action==COMMAND_CHAFF && calls.carry==0x1234 && result.event==0xabcd006b);
    assert(!c.pending_a);

    q.taken=q.count=q.write_index=0; c.modifier=1; c.block_flags=0;
    assert(fa18_dispatch_native_keyboard_command(&d,0x12,0,&owners,&result));
    assert(result.action==COMMAND_EJECT && f.eject_flag==1 && f.redraw_e==8);
    assert(q.count==1 && q.raw[0]==0x12 && result.event==0x71 && !c.modifier);
    assert((f.emitted_requests&0x2000) && (c.block_flags&0x0a)==0x0a);
    c.pending_b=0x1000; q.taken=q.count=q.write_index=0; c.origin_mode=0;
    assert(fa18_dispatch_native_pending_command(&d,0,&owners,&result));
    assert(result.action==COMMAND_VIEW_ZERO && v.zoom_scale==0x80 && q.count==1);

    c.modifier=0; c.indexed.function_modifier=3; q.taken=q.count=0;
    assert(fa18_dispatch_native_keyboard_command(&d,0x60,0,&owners,&result));
    assert(result.completion==FA18_COMMAND_UNPUBLISHED && c.modifier==1 && !q.count);
    c.event_counter=0xff;
    assert(fa18_dispatch_native_keyboard_command(&d,0x33,0,&owners,&result));
    assert(result.action==COMMAND_COUNTER_WAIT && !c.event_counter && c.modifier==1);
    c.pending_a=0xf0;
    assert(fa18_dispatch_native_pending_command(&d,0,&owners,&result));
    assert(result.completion==FA18_COMMAND_INVALID_PENDING && d.error_code==0x33 && c.pending_a==0xf0);
    c.event_counter=1;
    assert(fa18_dispatch_native_keyboard_command(&d,0x46,0,&owners,&result));
    assert(result.completion==FA18_COMMAND_INPUT_RESET && registry.count==2 && !q.count);

    registry.fail_add=1; result=(FA18NativeCommandOutcome){COMMAND_QUEUE_ONLY,FA18_COMMAND_PUBLISHED,0xfeedface};
    assert(!fa18_dispatch_native_keyboard_command(&d,0x46,0,&owners,&result));
    assert(registry.count==4 && descriptor.type==2 && result.event==0xfeedface);
    calls.fail=1; c.modifier=0; c.block_flags=0;
    assert(!fa18_dispatch_native_keyboard_command(&d,0x33,0,&owners,&result));
    assert(!f.chaff_count && result.event==0xfeedface);
    d.input_registration=NULL;
    assert(!fa18_dispatch_native_keyboard_command(&d,0x46,0,&owners,&result));
    assert(!fa18_dispatch_native_pending_command(NULL,0,&owners,&result));
    q.commands=NULL;
    assert(!fa18_dispatch_native_keyboard_command(&d,0,0,&owners,&result));
    return 0;
}
