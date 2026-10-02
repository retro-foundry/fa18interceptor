#ifndef FA18_GAME_PLACEMENT_ORDER_H
#define FA18_GAME_PLACEMENT_ORDER_H

#include "memory.h"

/* $C1E540-$C1EBAE: classify the selected placement-cache suffix against its
 * last eligible reference, then partition using the renderer workspace.
 * Includes both plane lists and indexed polygon/triangle lists. */
void order_placement_cache(void);

#endif
