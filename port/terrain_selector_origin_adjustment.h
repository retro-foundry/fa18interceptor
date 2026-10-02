#ifndef FA18_TERRAIN_SELECTOR_ORIGIN_ADJUSTMENT_H
#define FA18_TERRAIN_SELECTOR_ORIGIN_ADJUSTMENT_H

#include <stdint.h>

typedef int (*FA18TerrainSelectorOriginNormalize)(void *context, uint16_t length,
                                                int32_t candidate[3]);

typedef struct {
    int32_t magnitude;
    int32_t candidate[3];
    int32_t smoothed_delta[3];
    int32_t origin[3];
    int32_t negated_companion[3];
    uint16_t shift; /* caller-selected D4.W, saved around the C2574A child */
    FA18TerrainSelectorOriginNormalize normalize;
    void *normalize_context;
} FA18TerrainSelectorOriginAdjustmentState;

/* `$C29548-$C295D0`: reduce, scale, smooth, add, and publish the selector
 * origin candidate. `$C2574A` normalizes the reduced triple with length
 * 0x200; it changes that triple, while the caller's shift remains intact.
 * The complete game owner is port/game/selector_origin.c. */
int fa18_adjust_terrain_selector_origin(FA18TerrainSelectorOriginAdjustmentState *state);

#endif
