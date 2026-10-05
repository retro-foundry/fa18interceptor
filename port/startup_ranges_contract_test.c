#include "startup_ranges.h"
#include "post_input_display_stages.h"
#include <assert.h>
#include <string.h>

int main(void) {
    FA18CommandInput c={0}; FA18FlightCommandState f={0};
    FA18ViewCommandState v={0}; FA18ContextCommandState context={0};
    FA18CommandQueue q={0}; FA18NativeStartupRanges startup={0};
    uint16_t words[52]; PortFieldByte fields[FA18_STARTUP_WORD_BYTES];
    uint8_t data[FA18_COMMAND_QUEUE_NEIGHBORS],keys[128]={0},aux=0;
    unsigned i; uint32_t event;
    f.commands=&c; v.flight=&f; context.view=&v;
    memset(data,0x5a,sizeof data);
    assert(fa18_initialize_command_queue(&q,&context,data,sizeof data,keys,sizeof keys));
    for(i=0;i<52;++i) {
        words[i]=0xffff;
        fields[2*i]=(PortFieldByte){.unsigned_word=words+i,.shift=8};
        fields[2*i+1]=(PortFieldByte){.unsigned_word=words+i};
    }
    f.spawn_gate=0xffff;
    fields[2]=(PortFieldByte){.unsigned_word=&f.spawn_gate,.shift=8};
    fields[3]=(PortFieldByte){.unsigned_word=&f.spawn_gate};
    v.redraw_state_long=0xdeadbeef;
    for(i=0;i<4;++i) fields[0x58+i]=(PortFieldByte){.longword=&v.redraw_state_long,.shift=24-8*i};
    assert(!fa18_bind_native_startup_ranges(&startup,&q,fields,sizeof fields/sizeof fields[0]-1));
    assert(!startup.queue && !startup.words);
    assert(fa18_bind_native_startup_ranges(&startup,&q,fields,sizeof fields/sizeof fields[0]));
    assert(f.spawn_gate==0xffff && v.redraw_state_long==0xdeadbeef);
    /* Binding a later owner is visible to startup: no copied queue slots. */
    assert(fa18_bind_command_queue_byte(&q,0x34,&aux) && aux==0x5a);
    assert(fa18_clear_native_startup_ranges(&startup));
    assert(!context.recorder_on && !c.indexed.enable_gate && !c.indexed.mode_request);
    assert(!f.weapon_pause && !v.mode && !f.pause && !c.indexed.origin_gate_a && !aux);
    assert(!f.spawn_gate && !v.redraw_state_long);
    for(i=0;i<104;++i) { uint8_t byte; assert(port_read_field_byte(fields+i,&byte) && !byte); }
    assert(q.neighbors[0x2e]==0x5a && q.neighbors[0x64]==0x5a);
    assert(fa18_enable_native_startup_ranges(&startup));
    assert(c.origin_mode==1 && c.indexed.mode_gate==1);
    assert(q.neighbors[0x22]==0x5a && !context.recorder_on);
    for(i=0x23;i<0x2f;++i) { uint8_t byte; assert(port_read_field_byte(q.slots+i,&byte) && byte==1); }
    /* Actual publication reaches the very same newly cleared owner. */
    q.taken=0; q.count=0; q.write_index=(uint8_t)(0x34-128);
    assert(fa18_publish_native_command(&q,7,&event) && aux==7);
    fields[0]=(PortFieldByte){0};
    assert(!fa18_clear_native_startup_ranges(&startup) && !aux);
    assert(!fa18_enable_native_startup_ranges(NULL));
    return 0;
}
