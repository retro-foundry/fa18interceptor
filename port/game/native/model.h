#ifndef FA18_NATIVE_MODEL_H
#define FA18_NATIVE_MODEL_H
#include "../scene_placements.h"
enum { NATIVE_SCENE_MODEL_FRAME = 0x4200 };
/* C1F074's early expiry returns the preceding -$7C output. This is native
 * renderer state, kept outside asset/game RAM until a model call consumes it.
 * Normalisation, depth sorting and map clipping publish actual source outputs. */
uint16_t native_model_retained_result(void);
void native_model_retain_result(uint16_t value);
/* C1EE14/C096CA descriptor rendering. Scratch is ordinary host storage;
 * streams contain original model data, never executable instructions. */
int native_model_draw(gaddr parameters, gaddr frame);
int32_t native_scene_placement(void *context, const ScenePlacementCall *call);
#endif
