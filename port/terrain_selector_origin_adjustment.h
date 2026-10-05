#ifndef FA18_TERRAIN_SELECTOR_ORIGIN_ADJUSTMENT_H
#define FA18_TERRAIN_SELECTOR_ORIGIN_ADJUSTMENT_H

#include <stdint.h>
#include "native_vector_math.h"

typedef struct {
    int32_t magnitude;
    int32_t *candidate;
    int32_t *smoothed_delta;
    int32_t *origin;
    int32_t *negated_companion;
    uint16_t shift; /* caller-selected D4.W, saved around the C2574A child */
    const FA18NativeVectorMath *vector_math;
} FA18TerrainSelectorOriginAdjustmentState;

/* `$C29548-$C295D0`: reduce, scale, smooth, add, and publish the selector
 * origin candidate. `$C2574A` normalizes the reduced triple with length
 * 0x200 through its actual native body and shared magnitude/output owners.
 * It changes that triple, while the caller's shift remains intact.
 * The complete game owner is port/game/selector_origin.c. */
int fa18_adjust_terrain_selector_origin(FA18TerrainSelectorOriginAdjustmentState *state);

#endif
