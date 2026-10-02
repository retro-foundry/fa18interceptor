#ifndef FA18_COMMAND_DISPATCH_H
#define FA18_COMMAND_DISPATCH_H
#include "command_selection.h"
#include "command_publication.h"
#include "flight_commands.h"
#include "view_commands.h"
#include "indexed_commands.h"
#include "context_commands.h"
enum CommandDispatchChild {
    COMMAND_INVALID_INPUT_FAULT, COMMAND_RESET_BEGIN, COMMAND_RESET_FAULT, COMMAND_RESET_FINISH
};
enum CommandDispatchPhase { COMMAND_DISPATCH_ERROR, COMMAND_DISPATCH_PUBLICATION };
typedef struct {
    const CommandSelectionHooks *selection;
    const CommandPublicationHooks *publication;
    const FlightCommandHooks *flight;
    const ViewCommandHooks *view;
    const IndexedCommandHooks *indexed;
    const ContextCommandHooks *context_actions;
    int16_t (*carried_selection)(void *context);
    void (*consume)(void *context,enum CommandDispatchChild child);
    void (*observe)(void *context,enum CommandDispatchPhase phase,uint32_t value);
    void *context;
} CommandDispatchHooks;
/* Complete original owners, including shared actions and every exit. */
void dispatch_keyboard_command(uint32_t raw,const CommandDispatchHooks *hooks);
void dispatch_pending_command(const CommandDispatchHooks *hooks);
#endif
