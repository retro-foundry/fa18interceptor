#ifndef FA18_NATIVE_MODEL_H
#define FA18_NATIVE_MODEL_H
#include "../scene_placements.h"
/* C1EE14/C096CA descriptor rendering. Scratch is ordinary host storage;
 * streams contain original model data, never executable instructions. */
int native_model_draw(gaddr parameters, gaddr frame);
int32_t native_scene_placement(void *context, const ScenePlacementCall *call);
#endif
