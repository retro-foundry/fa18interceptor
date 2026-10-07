/* C1AD74 and C1AC28: select one command, execute its action and publish it. */
#include "command_dispatch.h"
#include "globals.h"
#include <stdlib.h>

static void observe(const CommandDispatchHooks *h,enum CommandDispatchPhase phase,uint32_t value) {
    if(h->observe) h->observe(h->context,phase,value);
}
static CommandDispatchResult execute_request(const CommandRequest *request,const CommandDispatchHooks *h) {
    uint32_t event=request->raw_event;
    CommandDispatchResult result={.action=request->action};
    if(h->prepare_action) h->prepare_action(h->context,request);
    switch(request->action) {
    case COMMAND_PENDING_EMPTY: case COMMAND_COUNTER_WAIT: case COMMAND_FINISH_EVENT: return result;
    case COMMAND_INVALID_WORD:
        wr_u16(ERROR_CODE,0x33); observe(h,COMMAND_DISPATCH_ERROR,0x33);
        h->consume(h->context,COMMAND_INVALID_INPUT_FAULT); return result;
    case COMMAND_RESET_CONTEXT:
        h->consume(h->context,COMMAND_RESET_BEGIN);
        h->consume(h->context,COMMAND_RESET_FAULT);
        h->consume(h->context,COMMAND_RESET_FINISH); return result;
    case COMMAND_QUEUE_ONLY: break;
    default:
        if(is_flight_command(request->action)) {
            const FlightCommandExecution flight=execute_flight_command_result(request,h->carried_selection(h->context),h->flight);
            event=flight.event;result.flight_output=flight.output;
        }
        else if(is_view_command(request->action)) event=execute_view_command(request,h->view);
        else if(is_indexed_command(request->action))
            event=execute_indexed_command(request,h->carried_selection(h->context),h->indexed);
        else if(is_context_command(request->action)) event=execute_context_command(request,h->context_actions);
        else abort();
        break;
    }
    observe(h,COMMAND_DISPATCH_PUBLICATION,event);
    result.publication_ran=1;
    result.publication=publish_command_event_result((uint8_t)event,h->publication);
    return result;
}
static void check_hooks(const CommandDispatchHooks *h) {
    if(!h || !h->selection || !h->publication || !h->flight || !h->view ||
       !h->indexed || !h->context_actions || !h->carried_selection || !h->consume) abort();
}
void dispatch_keyboard_command(uint32_t raw,const CommandDispatchHooks *h) {
    dispatch_keyboard_command_result(raw,h);
}
CommandDispatchResult dispatch_keyboard_command_result(uint32_t raw,const CommandDispatchHooks *h) {
    CommandRequest request;
    check_hooks(h); request=select_keyboard_command(raw,h->selection); return execute_request(&request,h);
}
void dispatch_pending_command(const CommandDispatchHooks *h) {
    dispatch_pending_command_result(h);
}
CommandDispatchResult dispatch_pending_command_result(const CommandDispatchHooks *h) {
    CommandRequest request;
    check_hooks(h); request=select_pending_command(h->selection); return execute_request(&request,h);
}
