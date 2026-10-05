#include "command_dispatch.h"

typedef struct {
    FA18NativeCommandDispatcher *state;
    const FA18FlightCommandOps *owner;
} FlightOwner;

static int dispatch_flight_child(void *context,FA18FlightCommandState *flight,
                                 enum FlightCommandChild child,
                                 const FA18FlightCommandChildInput *input,
                                 FlightCommandResult *result) {
    FlightOwner *bridge=context;
    if(flight!=bridge->state->context->view->flight) return 0;
    if(fa18_is_native_flight_child(child))
        return fa18_apply_native_flight_child(bridge->state,child,input,result);
    return bridge->owner && bridge->owner->consume &&
           bridge->owner->consume(bridge->owner->context,flight,child,input,result);
}

static int valid_dispatch(FA18NativeCommandDispatcher *s,
                          const FA18NativeCommandOwners *owners,
                          const FA18NativeCommandOutcome *outcome) {
    return s && owners && outcome && s->context && s->context->view &&
           s->context->view->flight && s->context->view->flight->commands &&
           s->queue && s->queue->commands==s->context->view->flight->commands &&
           s->context->key_taken==&s->queue->taken;
}

static int execute_native_request(FA18NativeCommandDispatcher *s,const CommandRequest *r,
                                  int16_t carry,const FA18NativeCommandOwners *owners,
                                  FA18NativeCommandOutcome *outcome) {
    FA18NativeCommandOutcome result={r->action,FA18_COMMAND_UNPUBLISHED,0};
    FlightOwner bridge={s,owners->flight};
    FA18FlightCommandOps flight_ops={dispatch_flight_child,&bridge};
    uint32_t event=r->raw_event;
    switch(r->action) {
    case COMMAND_PENDING_EMPTY: case COMMAND_COUNTER_WAIT: case COMMAND_FINISH_EVENT:
        *outcome=result; return 1;
    case COMMAND_INVALID_WORD:
        s->error_code=0x33; /* $C06C02 is an actual RTS in the release build. */
        result.completion=FA18_COMMAND_INVALID_PENDING; *outcome=result; return 1;
    case COMMAND_RESET_CONTEXT:
        if(!fa18_remove_native_input_callback(s->input_registration)) return 0;
        /* The intervening $C06C02 also has no game-state effects. */
        if(!fa18_install_native_input_callback(s->input_registration)) return 0;
        result.completion=FA18_COMMAND_INPUT_RESET; *outcome=result; return 1;
    case COMMAND_QUEUE_ONLY: break;
    default:
        if(fa18_is_flight_input_command(r->action)) {
            if(!fa18_apply_flight_input_command(s->context->view->flight,r,carry,&flight_ops,&event)) return 0;
        } else if(fa18_is_view_input_command(r->action)) {
            if(!fa18_apply_view_input_command(s->context->view,r,owners->spans,&event)) return 0;
        } else if(fa18_is_context_input_command(r->action)) {
            if(!fa18_apply_context_input_command(s->context,r,owners->context_poses,owners->context,&event)) return 0;
        } else {
            if(!fa18_apply_selected_indexed_command(s->queue->commands,r,carry,owners->indexed_poses,
                                                   owners->status_tone,owners->status_context,&event)) return 0;
        }
        break;
    }
    if(!fa18_publish_native_command(s->queue,event,&result.event)) return 0;
    result.completion=FA18_COMMAND_PUBLISHED;
    *outcome=result; return 1;
}

int fa18_dispatch_native_keyboard_command(FA18NativeCommandDispatcher *s,
                                           uint32_t event,int16_t carry,
                                           const FA18NativeCommandOwners *owners,
                                           FA18NativeCommandOutcome *outcome) {
    CommandRequest request;
    if(!valid_dispatch(s,owners,outcome) ||
       !fa18_select_keyboard_command_with_carry(s->queue->commands,event,&carry,&request)) return 0;
    return execute_native_request(s,&request,carry,owners,outcome);
}

int fa18_dispatch_native_pending_command(FA18NativeCommandDispatcher *s,int16_t carry,
                                          const FA18NativeCommandOwners *owners,
                                          FA18NativeCommandOutcome *outcome) {
    CommandRequest request;
    if(!valid_dispatch(s,owners,outcome) || !fa18_select_pending_command(s->queue->commands,&request)) return 0;
    return execute_native_request(s,&request,carry,owners,outcome);
}
