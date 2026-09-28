#ifndef FA18_TERRAIN_SELECTOR_ORIGIN_ADJUSTMENT_H
#define FA18_TERRAIN_SELECTOR_ORIGIN_ADJUSTMENT_H

#include <stdint.h>

typedef int (*FA18TerrainSelectorOriginScale)(void *context, uint16_t input,
                                              int16_t *shift);

typedef struct {
    int32_t magnitude;
    int32_t candidate[3];
    int32_t smoothed_delta[3];
    int32_t origin[3];
    int32_t negated_companion[3];
    FA18TerrainSelectorOriginScale scale;
    void *scale_context;
} FA18TerrainSelectorOriginAdjustmentState;

/* `$C29548-$C295D0`: reduce, scale, smooth, add, and publish the selector
 * origin candidate. `$C2574A` remains a required caller-owned scale lookup. */
int fa18_adjust_terrain_selector_origin(FA18TerrainSelectorOriginAdjustmentState *state);

#endif
