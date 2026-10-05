#include "command_dispatch.h"

int fa18_is_native_flight_child(enum FlightCommandChild child) {
    return child==FLIGHT_EJECT_TOGGLE ||
           (child>=FLIGHT_SPACE_RELEASE && child<=FLIGHT_THROTTLE_MODE_RELEASE);
}

int fa18_apply_native_flight_child(FA18NativeCommandDispatcher *s,
                                   enum FlightCommandChild child,
                                   const FA18FlightCommandChildInput *input,
                                   FlightCommandResult *result) {
    FA18FlightCommandState *f;
    uint32_t event;
    if(!s || !s->context || !s->context->view || !s->context->view->flight ||
       !input || !result || !fa18_is_native_flight_child(child)) return 0;
    f=s->context->view->flight;
    if(child!=FLIGHT_EJECT_TOGGLE) return fa18_apply_flight_control_child(f,child,input,result);
    if(!s->queue || s->queue->commands!=f->commands || !f->commands) return 0;
    /* $C1C214 toggles a nonzero byte to zero, then tail-calls publication. */
    f->eject_flag=f->eject_flag?0:1;
    if(!fa18_publish_native_command(s->queue,input->event,&event)) return 0;
    result->event=event; result->carried_event_word=input->restore_event_word;
    return 1;
}
