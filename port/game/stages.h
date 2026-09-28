#ifndef FA18_GAME_STAGES_H
#define FA18_GAME_STAGES_H

#include "memory.h"

/* A stage with nothing to do (several update tables point here). */
void empty_stage(void);

/* Empty the list at LIST_BUFFER. */
void reset_list(void);

/* Count a byte timer down to zero; negative timers are stopped. */
void tick_timer(gaddr timer);

/* Handlers that report "nothing" (0). */
int zero_result(void);

#endif
