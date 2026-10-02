#ifndef FA18_GLUE_VIEW_COMMANDS_H
#define FA18_GLUE_VIEW_COMMANDS_H
#include "view_commands.h"
uint32_t glue_execute_view_command(const CommandRequest *request);
const ViewCommandHooks *glue_view_command_hooks(void);
#endif
