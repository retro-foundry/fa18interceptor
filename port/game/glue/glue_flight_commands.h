#ifndef FA18_GLUE_FLIGHT_COMMANDS_H
#define FA18_GLUE_FLIGHT_COMMANDS_H
#include "flight_commands.h"
/* Internal flight-action body, ending before the owner's queue publication. */
uint32_t glue_execute_flight_command(const CommandRequest *request);
const FlightCommandHooks *glue_flight_command_hooks(void);
#endif
