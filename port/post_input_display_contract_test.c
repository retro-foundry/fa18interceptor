#include "post_input_display_stages.h"
#include "graphics_storage.h"
#include "post_input_tick.h"
#include <assert.h>
#include <string.h>

typedef struct { FA18NativePostInputDisplayStages *stage; unsigned calls; int complete; } SceneChild;
static int initialize_scene(void *context) {
    SceneChild *child=context;
    FA18NativePostInputDisplayStages *s=child->stage;
    assert(*s->countdown==0xffff && s->commands->indexed.recorder_mode==9);
    assert(*s->callback==FA18_STAGE_C0FA04);
    ++child->calls;
    *s->countdown=77; s->commands->origin_mode=5;
    s->viewport->current=7; s->viewport->target=8;
    return child->complete;
}
static void check_bytes(const uint8_t *bytes,size_t length,uint8_t value) {
    size_t i;
    for(i=0;i<length;++i) assert(bytes[i]==value);
}
/* A gate may be cleared by an earlier stream. Its fifth cursor advances only
 * while the live byte is set; a blanket five-plane memset changes this case. */
static void check_aliases(void) {
    static uint8_t bytes[10][8008];
    FA18NativeGraphicsPlane planes[10];
    FA18NativeGraphicsSetup setup={0};
    FA18NativeRendererClear clear={&setup,&planes[9],NULL};
    unsigned i;
    for(i=0;i<10;++i) planes[i]=(FA18NativeGraphicsPlane){bytes[i],sizeof bytes[i],0};
    for(i=0;i<9;++i) setup.source[i]=&planes[i];
    memset(bytes,0x5a,sizeof bytes); clear.fifth_buffer_used=bytes[0]+8;
    assert(fa18_clear_native_renderer(&clear));
    check_bytes(bytes[4],8,0); check_bytes(bytes[4]+8,8000,0x5a);
    for(i=0;i<10;++i) if(i!=4) {
        check_bytes(bytes[i],8000,0); check_bytes(bytes[i]+8000,8,0x5a);
    }
    memset(bytes,0x5a,sizeof bytes); clear.fifth_buffer_used=bytes[4]+8;
    assert(fa18_clear_native_renderer(&clear));
    check_bytes(bytes[4],12,0); check_bytes(bytes[4]+12,7996,0x5a);
    /* Bound failures retain actual preceding stores, including an entire A
     * phase before discovering the absent separate B buffer. */
    memset(bytes,0x5a,sizeof bytes); clear.additional_buffer=NULL;
    assert(!fa18_clear_native_renderer(&clear));
    check_bytes(bytes[0],8000,0); check_bytes(bytes[5],4,0);
    check_bytes(bytes[5]+4,8004,0x5a);
    clear.additional_buffer=&planes[9]; planes[1].byte_count=6;
    memset(bytes,0x5a,sizeof bytes); *clear.fifth_buffer_used=0;
    assert(!fa18_clear_native_renderer(&clear));
    check_bytes(bytes[0],8,0); check_bytes(bytes[1],4,0);
    check_bytes(bytes[1]+4,8004,0x5a);
}
int main(void) {
    FA18NativeGraphicsStorage storage={0};
    FA18NativeGraphicsSetup setup={0}; FA18NativeGraphicsSetupOps graphics_ops;
    FA18NativeInputDisplay display={0}; uint16_t palette[32]={0};
    uint8_t additional[8008],fifth=0;
    FA18NativeGraphicsPlane extra={additional,sizeof additional,0};
    FA18NativeRendererClear clear={&setup,&extra,&fifth};
    FA18CommandInput commands={0}; FA18FlightCommandState flight={0};
    FA18ViewCommandState view={0}; FA18ContextCommandState context={0};
    FA18CommandQueue queue={0}; FA18ViewportModeState viewport={0};
    FA18PostInputTickState tick={0};
    FA18NativePostInputDisplayStages stage={&commands,&viewport,&tick.countdown,
        &tick.callback,&tick.auxiliary_byte,&clear};
    SceneChild child={&stage,0,1}; FA18NativePostInputDisplayOps ops={initialize_scene,&child};
    uint8_t neighbors[FA18_COMMAND_QUEUE_NEIGHBORS]={0},keys[128]={0};
    uint32_t event; unsigned i;
    setup.display=&display; setup.initial_palette=palette;
    assert(fa18_bind_native_graphics_storage(&storage,&graphics_ops));
    assert(fa18_initialize_native_graphics(&setup,&graphics_ops));
    flight.commands=&commands; view.flight=&flight; context.view=&view;
    neighbors[0x75]=0; neighbors[0x34]=0x55;
    assert(fa18_initialize_command_queue(&queue,&context,neighbors,sizeof neighbors,keys,sizeof keys));
    assert(fa18_bind_native_renderer_clear(&clear,&queue));
    assert(fa18_bind_native_post_input_display(&stage,&queue));
    assert(queue.slots[0x75].byte==&fifth && queue.slots[0x34].byte==&tick.auxiliary_byte);
    assert(!fifth && tick.auxiliary_byte==0x55);
    for(i=0;i<5;++i) memset(storage.planes[i].bytes,0x5a,storage.planes[i].byte_count);
    memset(additional,0x5a,sizeof additional);
    /* Nonexpired entry uses the actual allocated shared lower-four planes.
     * No scene owner is required for that branch, nor an unused A4 span. */
    setup.source[4]=NULL; tick.countdown=0x7fff;
    assert(fa18_finish_native_post_input_display(&stage,NULL));
    for(i=0;i<4;++i) check_bytes(storage.planes[i].bytes,8000,0);
    check_bytes(storage.planes[4].bytes,8000,0x5a);
    check_bytes(additional,8000,0); check_bytes(additional+8000,8,0x5a);
    setup.source[4]=&storage.planes[4];
    /* An actual signed-index queue store changes the canonical fifth gate. */
    queue.write_index=0xf5;
    assert(fa18_publish_native_command(&queue,1,&event));
    assert(fifth==1);
    assert(fa18_finish_native_post_input_display(&stage,NULL));
    check_bytes(storage.planes[4].bytes,8000,0);
    tick.countdown=0xffff; tick.callback=FA18_STAGE_C0FA04;
    commands.indexed.recorder_mode=9;
    assert(fa18_finish_native_post_input_display(&stage,&ops));
    assert(child.calls==1 && commands.indexed.recorder_mode==3 && !commands.origin_mode);
    assert(tick.countdown==2 && !viewport.current && viewport.target==15);
    assert(tick.callback==FA18_STAGE_C0FA4C);
    tick.countdown=0x8000; tick.auxiliary_byte=7;
    assert(fa18_match_native_post_input_display(&stage));
    assert(!tick.auxiliary_byte && tick.countdown==0x8000 && tick.callback==FA18_STAGE_C0FA4C);
    viewport.current=15;
    assert(fa18_match_native_post_input_display(&stage));
    assert(tick.countdown==2 && tick.callback==FA18_STAGE_C0FA80);
    commands.indexed.origin_gate_a=0x80;
    assert(fa18_complete_native_post_input_display(&stage));
    assert(commands.indexed.origin_gate_a==0x80 && !tick.auxiliary_byte);
    tick.countdown=0xffff;
    assert(fa18_complete_native_post_input_display(&stage));
    assert(!commands.indexed.origin_gate_a && tick.auxiliary_byte==1 && tick.callback==FA18_STAGE_C10C08);
    /* The child's explicit incomplete result stops parent writes, preserving
     * writes the actual owner already performed. */
    tick.countdown=0xffff; tick.callback=FA18_STAGE_C0FA04;
    commands.indexed.recorder_mode=9; child.complete=0;
    assert(!fa18_finish_native_post_input_display(&stage,&ops));
    assert(child.calls==2 && tick.countdown==77 && commands.origin_mode==5);
    assert(commands.indexed.recorder_mode==9 && viewport.current==7 && viewport.target==8);
    assert(tick.callback==FA18_STAGE_C0FA04);
    check_aliases();
    return 0;
}
