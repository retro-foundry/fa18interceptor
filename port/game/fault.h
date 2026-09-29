#ifndef FA18_GAME_FAULT_H
#define FA18_GAME_FAULT_H

#include <stdint.h>

/* The debugger hook the release build left empty ($C06C02). */
void fault_hook(void);

/* A fatal error: ERROR_CODE = `code`, then the hook, forever ($C1D5C6,
 * $C1D712). */
void fatal_error(uint16_t code);

#endif
