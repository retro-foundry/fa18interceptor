#include "scene_bootstrap_native.h"

static int shared_bootstrap(const FA18NativeSceneBootstrap *s) {
    FA18FlightCommandState *f;
    if(!s || !s->context || !s->context->view || !(f=s->context->view->flight) ||
       !f->commands || !s->player || s->player->flight!=f || !s->player->records ||
       s->player->records->input!=f->commands || !s->player->effects ||
       s->player->effects->context!=s->context || !s->startup || !s->viewed_word || !s->renderer) return 0;
    return f->player==s->player->records->aircraft &&
        s->context->records==s->player->records->geometry && s->context->record_count==16;
}
static int import_word(PortFieldByte *pair,uint16_t *owner) {
    uint8_t high,low;
    if(!owner || !port_read_field_byte(pair,&high) || !port_read_field_byte(pair+1,&low)) return 0;
    *owner=(uint16_t)(((unsigned)high<<8)|low);
    pair[0]=(PortFieldByte){.unsigned_word=owner,.shift=8};
    pair[1]=(PortFieldByte){.unsigned_word=owner};
    return 1;
}
static int import_byte(PortFieldByte *field,uint8_t *owner) {
    uint8_t value;
    if(!owner || !port_read_field_byte(field,&value)) return 0;
    *owner=value; *field=(PortFieldByte){.byte=owner}; return 1;
}
static int import_long(PortFieldByte *fields,uint32_t *owner) {
    uint32_t value=0; unsigned i; uint8_t byte;
    if(!owner) return 0;
    for(i=0;i<4;++i) { if(!port_read_field_byte(fields+i,&byte)) return 0; value=(value<<8)|byte; }
    *owner=value;
    for(i=0;i<4;++i) fields[i]=(PortFieldByte){.longword=owner,.shift=24-8*i};
    return 1;
}
int fa18_bind_native_scene_bootstrap(FA18NativeSceneBootstrap *s,FA18CommandQueue *q,
                                       PortFieldByte *words,size_t count) {
    FA18FlightCommandState *f; FA18ViewCommandState *v; size_t i;
    if(!shared_bootstrap(s) || !q || !words || count!=104 || !s->scene_limit ||
       !s->previous_scene_limit || !s->context_state || !s->menu_transition || !s->previous_state_byte ||
       !s->byte_458be || !s->menu_return_word || !s->word_4fda0 || !s->countdown ||
       !s->word_459a6 || !s->word_459a8 || !s->history_record || !s->readout_minimum ||
       !s->row_scales[0] || !s->row_scales[1] || !s->message_queue_first || !s->message_timer ||
       !s->readout_valid[0] || !s->readout_valid[1] || !s->reference_18 || !s->depth_values || s->depth_count<22) return 0;
    v=s->context->view; f=v->flight;
    if(q->commands!=f->commands) return 0;
    for(i=0;i<count;i+=2) if(!port_field_word_pair_valid(words+i)) return 0;
    if(!import_word(words+2,&f->spawn_gate) || !import_word(words+6,&f->command_word) ||
       !import_byte(words+12,&f->commands->indexed.cockpit_high_byte) ||
       !import_byte(words+13,&f->commands->indexed.cockpit_low_byte) ||
       !import_word(words+14,&s->player->effects->message_state) ||
       !import_word(words+24,&v->redraw_state_word) || !import_long(words+0x58,&v->redraw_state_long) ||
       !fa18_bind_native_viewed_record_word(s->viewed_word,s->player->records,f,words,count) ||
       !fa18_bind_native_startup_ranges(s->startup,q,words,count) ||
       !fa18_bind_native_scene_player(s->player,q) || !fa18_bind_native_renderer_clear(s->renderer,q) ||
       !fa18_bind_command_queue_byte(q,0xf4,s->previous_state_byte) ||
       !fa18_bind_command_queue_byte(q,0xf6,s->menu_transition)) return 0;
    return 1;
}
int fa18_bootstrap_native_scene(FA18NativeSceneBootstrap *s,
                                 const FA18NativeSceneBootstrapOps *ops,int16_t placement_word) {
    FA18ViewCommandState *v; FA18FlightCommandState *f;
    uint32_t position[3]; unsigned i;
    FA18ContextCommandChildInput input={0}; FA18ContextCommandChildResult result;
    if(!shared_bootstrap(s) || !s->startup->queue || s->startup->queue->commands!=s->context->view->flight->commands ||
       !s->startup->words || s->startup->words[0x1e].word_value!=&s->viewed_word->value ||
       s->viewed_word->flight!=s->context->view->flight || s->viewed_word->records!=s->player->records) return 0;
    v=s->context->view; f=v->flight;
    if(!fa18_clear_native_startup_ranges(s->startup) || !fa18_enable_native_startup_ranges(s->startup)) return 0;
    if(!s->scene_limit || !s->previous_scene_limit) return 0;
    *s->scene_limit=*s->previous_scene_limit;
    if(!s->menu_return_word) return 0;
    *s->menu_return_word=0;
    if(!s->word_4fda0) return 0;
    *s->word_4fda0=0;
    if(!s->message_queue_first) return 0;
    *s->message_queue_first=0;
    v->update_mask=0xff; f->commands->message_state=0xff;
    if(!s->context_state) return 0;
    *s->context_state=0xff;
    if(!s->countdown) return 0;
    *s->countdown=5;
    if(!s->message_timer) return 0;
    *s->message_timer=0x1b8;
    if(!fa18_clear_native_renderer(s->renderer) || !fa18_clear_native_bootstrap_records(s->player->records) ||
       !fa18_prepare_native_scene_player(s->player)) return 0;
    if(!s->word_459a6) return 0;
    *s->word_459a6=0x140;
    if(!s->word_459a8) return 0;
    *s->word_459a8=0x140;
    f->commands->indexed.pose_entry=0;
    if(!s->menu_transition) return 0;
    *s->menu_transition=1;
    if(!s->history_record) return 0;
    *s->history_record=0x800;
    if(!fa18_native_scene_start_position(position)) return 0;
    for(i=0;i<3;++i) input.position[i]=(int32_t)position[i];
    if(!fa18_apply_context_control_child(s->context,CONTEXT_COMMAND_SET_OBSERVER,&input,&result)) return 0;
    s->context->map_middle_cache=0x03000000;
    s->context->smoothed_delta=0; s->context->auxiliary_delta[0]=0; s->context->auxiliary_delta[1]=0;
    s->context->pan=0x1c20; s->context->rotate=0;
    v->line_last_row=0xa7; v->span_origin=0x32; v->span_origin_y=0x320;
    if(!s->readout_valid[0]) return 0;
    *s->readout_valid[0]=0xffffffff;
    if(!s->readout_valid[1]) return 0;
    *s->readout_valid[1]=0xffffffff;
    if(!s->previous_state_byte) return 0;
    *s->previous_state_byte=0xff;
    if(!s->byte_458be) return 0;
    *s->byte_458be=15;
    if(!s->readout_minimum) return 0;
    *s->readout_minimum=0x7fff;
    if(!s->reference_18) return 0;
    *s->reference_18=0x00800000;
    if(!s->row_scales[0]) return 0;
    *s->row_scales[0]=0xa8;
    if(!s->row_scales[1]) return 0;
    *s->row_scales[1]=0xfc;
    v->zoom_scale=0x80; v->zoom_flags=0x80;
    if(!s->depth_values || s->depth_count<22) return 0;
    for(i=0;i<22;++i) s->depth_values[i]=(uint16_t)i;
    if(!port_fill_field_bytes(s->startup->queue->slots+0x99,48,0x30) ||
       !port_fill_field_bytes(s->startup->queue->slots+0xa9,28,0x20)) return 0;
    if(!ops || !ops->place_view || !ops->place_view(ops->context,s,placement_word)) return 0;
    if(!ops->build_gates || !ops->build_gates(ops->context,s)) return 0;
    if(!ops->update_records || !ops->update_records(ops->context,s)) return 0;
    if(!ops->refresh_context || !ops->refresh_context(ops->context,s)) return 0;
    return 1;
}
int fa18_native_scene_bootstrap_callback(void *context) {
    FA18NativeSceneBootstrapCall *call=context;
    return call && fa18_bootstrap_native_scene(call->state,call->ops,call->placement_word);
}
