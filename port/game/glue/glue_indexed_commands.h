#ifndef FA18_GLUE_INDEXED_COMMANDS_H
#define FA18_GLUE_INDEXED_COMMANDS_H
#include "indexed_commands.h"
uint32_t glue_execute_indexed_command(const CommandRequest *request);
const IndexedCommandHooks *glue_indexed_command_hooks(void);
#endif
