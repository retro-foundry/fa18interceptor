#ifndef FA18_GLUE_CONTEXT_COMMANDS_H
#define FA18_GLUE_CONTEXT_COMMANDS_H
#include "context_commands.h"
uint32_t glue_execute_context_command(const CommandRequest *request);
const ContextCommandHooks *glue_context_command_hooks(void);
#endif
