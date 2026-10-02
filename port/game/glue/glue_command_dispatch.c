#include "glue.h"
#include "glue_child_call.h"
#include "glue_command_dispatch.h"
#include "glue_command_selection.h"
#include "glue_command_publication.h"
#include "glue_flight_commands.h"
#include "glue_view_commands.h"
#include "glue_indexed_commands.h"
#include "glue_context_commands.h"
#include "command_dispatch.h"

static int16_t carried_selection(void *context) { (void)context; return (int16_t)D(4); }
static void consume(void *context,enum CommandDispatchChild child) {
    static const struct { uint32_t entry,ret; } children[]={
        {0xc06c02,0xc1ac26},{0xc1748c,0xc06bf6},
        {0xc06c02,0xc06bfa},{0xc17456,0xc06c00}
    };
    (void)context; glue_complete_child(children[child].entry,children[child].ret);
}
static void outputs(void *context,enum CommandDispatchPhase phase,uint32_t value) {
    (void)context;
    if(phase==COMMAND_DISPATCH_ERROR) flags_logic_w(value);
    else D(0)=value;
}
static CommandDispatchHooks hooks(void) {
    CommandDispatchHooks result={
        glue_command_selection_hooks(),glue_command_publication_hooks(),
        glue_flight_command_hooks(),glue_view_command_hooks(),glue_indexed_command_hooks(),
        glue_context_command_hooks(),carried_selection,consume,outputs,NULL
    };
    return result;
}
int glue_C1AC28(void) {
    const CommandDispatchHooks h=hooks(); dispatch_pending_command(&h); return glue_return();
}
int glue_C1AD74(void) {
    const CommandDispatchHooks h=hooks(); dispatch_keyboard_command(rd_u32(A(7)+4),&h); return glue_return();
}
