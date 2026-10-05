/* Authority: original C1BEE8/C1BA86/C1B906 instruction graphs,
 * corroborated by game/context_publication.c and game/view_commands.c. */
#include "native_context_publication.h"

static int publication_valid(const FA18NativeContextPublication *s) {
    FA18FlightCommandState *f;
    return s && s->records && s->records->input && s->context && s->context->view &&
        (f=s->context->view->flight)!=NULL && f->commands==s->records->input &&
        s->context->records==s->records->geometry && s->context->record_count==16 &&
        s->queue && s->queue->commands==f->commands && s->context->key_taken==&s->queue->taken &&
        s->selection_marker && s->target_record && s->spans;
}
int fa18_publish_native_context_detail(FA18NativeContextPublication *s,uint32_t event,
                                        uint32_t *axis,uint32_t *published_event) {
    if(!publication_valid(s) || !axis || !published_event) return 0;
    *axis=4;
    return fa18_set_native_view_detail(s->context->view,4) &&
        fa18_publish_native_command_with_axis(s->queue,event,published_event,axis);
}
int fa18_publish_native_view_key(FA18NativeContextPublication *s,uint32_t event,
                                   uint32_t *axis,uint32_t *published_event) {
    FA18ViewCommandState *v; int mode;
    if(!publication_valid(s) || !axis || !published_event) return 0;
    v=s->context->view;
    if(!fa18_request_native_cockpit_redraw(v)) return 0;
    *axis=v->mode; mode=(int8_t)v->mode;
    v->line_last_row=mode>=3 && (mode<=9 || mode>=12)?
        (mode>4 && mode<8?0xb3:0xa7):0x90;
    return fa18_publish_native_command_with_axis(s->queue,event,published_event,axis);
}
int fa18_publish_native_zero_view(FA18NativeContextPublication *s,uint32_t event,
                                    uint32_t *axis,uint32_t *published_event) {
    if(!publication_valid(s) || !axis || !published_event ||
       !fa18_select_native_zero_view_mode(s->context->view,s->spans,axis)) return 0;
    return fa18_publish_native_command_with_axis(s->queue,event,published_event,axis);
}
int fa18_publish_native_context_record(FA18NativeContextPublication *s,uint32_t event,
                                         int16_t index,uint32_t *axis,uint32_t *published_event) {
    FA18ViewCommandState *v; FA18FlightCommandState *f;
    uint16_t offset; unsigned slot; uint8_t kind;
    if(!publication_valid(s) || !axis || !published_event) return 0;
    v=s->context->view; f=v->flight;
    v->update_mask=0xff; *s->selection_marker=0xffff; v->fire_state=0xfe;
    *s->target_record=(uint16_t)index;
    offset=(uint16_t)((uint16_t)index<<9); slot=offset/512;
    if(slot>=16) return 0;
    f->viewed=s->records->aircraft+slot;
    if(!fa18_set_native_zoom_maximum(v)) return 0;
    if(f->commands->origin_mode) {
        f->commands->origin_mode=s->context->view_request; v->redraw_first=3;
    } else {
        s->context->angle_history=s->records->geometry[slot].angle;
        kind=(uint8_t)(f->viewed->equipment_kind&0xf0u);
        *axis=(*axis&0xffffff00u)|kind;
        if(kind!=0x30) {
            /* C1B906 ends in the same publication tail; it is not a
             * COMMAND_VIEW_ZERO input and emits no request bit. */
            return fa18_publish_native_zero_view(s,event,axis,published_event);
        }
        v->span_origin=0x32; v->span_origin_y=0x320; v->mode=0;
        if(!fa18_publish_native_view_key(s,event,axis,&event)) return 0;
        v->line_last_row=0xa7;
    }
    return fa18_publish_native_command_with_axis(s->queue,event,published_event,axis);
}
