#include "post_input_display_stages.h"

int fa18_bind_native_post_input_display(FA18NativePostInputDisplayStages *s,
                                         FA18CommandQueue *q) {
    return s && s->commands && s->viewport && s->countdown && s->callback &&
           s->auxiliary && q && q->commands==s->commands &&
           fa18_bind_command_queue_byte(q,0x34,s->auxiliary);
}
int fa18_finish_native_post_input_display(FA18NativePostInputDisplayStages *s,
                                           const FA18NativePostInputDisplayOps *ops) {
    if(!s || !s->countdown) return 0;
    if(*s->countdown<0x8000u) return fa18_clear_native_renderer(s->renderer);
    if(!ops || !ops->initialize_scene || !ops->initialize_scene(ops->context)) return 0;
    if(!s->commands || !s->viewport || !s->countdown || !s->callback) return 0;
    s->commands->indexed.recorder_mode=3;
    s->commands->origin_mode=0;
    *s->countdown=2;
    s->viewport->target=15;
    s->viewport->current=0;
    *s->callback=FA18_STAGE_C0FA4C;
    return 1;
}
int fa18_match_native_post_input_display(FA18NativePostInputDisplayStages *s) {
    if(!s || !s->countdown) return 0;
    if(*s->countdown<0x8000u) return 1;
    if(!s->auxiliary) return 0;
    *s->auxiliary=0;
    if(!s->viewport) return 0;
    if(s->viewport->current!=s->viewport->target) return 1;
    if(!s->callback) return 0;
    *s->countdown=2;
    *s->callback=FA18_STAGE_C0FA80;
    return 1;
}
int fa18_complete_native_post_input_display(FA18NativePostInputDisplayStages *s) {
    if(!s || !s->countdown) return 0;
    if(*s->countdown<0x8000u) return 1;
    if(!s->commands || !s->auxiliary || !s->callback) return 0;
    s->commands->indexed.origin_gate_a=0;
    *s->auxiliary=1;
    *s->callback=FA18_STAGE_C10C08;
    return 1;
}
