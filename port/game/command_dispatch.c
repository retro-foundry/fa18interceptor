/* C1AD74 and C1AC28: select one command, execute its action and publish it. */
#include "command_dispatch.h"
#include "globals.h"
#include <stdlib.h>

static void observe(const CommandDispatchHooks *h,enum CommandDispatchPhase phase,uint32_t value) {
    if(h->observe) h->observe(h->context,phase,value);
}
static void execute_request(const CommandRequest *request,const CommandDispatchHooks *h) {
    uint32_t event=request->raw_event;
    switch(request->action) {
    case COMMAND_PENDING_EMPTY: case COMMAND_COUNTER_WAIT: case COMMAND_FINISH_EVENT: return;
    case COMMAND_INVALID_WORD:
        wr_u16(ERROR_CODE,0x33); observe(h,COMMAND_DISPATCH_ERROR,0x33);
        h->consume(h->context,COMMAND_INVALID_INPUT_FAULT); return;
    case COMMAND_RESET_CONTEXT:
        h->consume(h->context,COMMAND_RESET_BEGIN);
        h->consume(h->context,COMMAND_RESET_FAULT);
        h->consume(h->context,COMMAND_RESET_FINISH); return;
    case COMMAND_QUEUE_ONLY: break;
    default:
        if(is_flight_command(request->action))
            event=execute_flight_command(request,h->carried_selection(h->context),h->flight);
        else if(is_view_command(request->action)) event=execute_view_command(request,h->view);
        else if(is_indexed_command(request->action))
            event=execute_indexed_command(request,h->carried_selection(h->context),h->indexed);
        else if(is_context_command(request->action)) event=execute_context_command(request,h->context_actions);
        else abort();
        break;
    }
    observe(h,COMMAND_DISPATCH_PUBLICATION,event);
    publish_command_event((uint8_t)event,h->publication);
}
static void check_hooks(const CommandDispatchHooks *h) {
    if(!h || !h->selection || !h->publication || !h->flight || !h->view ||
       !h->indexed || !h->context_actions || !h->carried_selection || !h->consume) abort();
}
void dispatch_keyboard_command(uint32_t raw,const CommandDispatchHooks *h) {
    CommandRequest request;
    check_hooks(h); request=select_keyboard_command(raw,h->selection); execute_request(&request,h);
}
void dispatch_pending_command(const CommandDispatchHooks *h) {
    CommandRequest request;
    check_hooks(h); request=select_pending_command(h->selection); execute_request(&request,h);
}
