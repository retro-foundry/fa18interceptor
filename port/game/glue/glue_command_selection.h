#ifndef FA18_GLUE_COMMAND_SELECTION_H
#define FA18_GLUE_COMMAND_SELECTION_H
#include "command_selection.h"
/* CPU adaptation of the dispatch policy component; no original routine is
 * registered until its action bodies and shared exit contracts are complete. */
CommandRequest glue_select_keyboard_command(void);
CommandRequest glue_select_pending_command(void);
uint32_t glue_command_action_pc(enum CommandAction action);
const CommandSelectionHooks *glue_command_selection_hooks(void);
#endif
