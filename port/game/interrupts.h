#ifndef FA18_GAME_INTERRUPTS_H
#define FA18_GAME_INTERRUPTS_H

/* The game's interrupt server ($C06132), called by Kickstart with its data
 * in A6: counts the call and returns the data pointer. */

#include "memory.h"

gaddr count_interrupt(gaddr data);

#endif
