#ifndef FA18_SCENE_FIXED_POINT_STAGE_H
#define FA18_SCENE_FIXED_POINT_STAGE_H

#include <stdint.h>

#include "scene_component_magnitude.h"

typedef enum {
    FA18_SCENE_FIXED_POINT_MAGNITUDE = 0,
    FA18_SCENE_FIXED_POINT_C1D90A_BOUNDARY = 1
} FA18SceneFixedPointRoute;

typedef struct {
    int16_t offset_word;
    int32_t offset_long;
    int16_t offset_second_word;
    uint16_t variable_shift;
    uint8_t alternate_long_mode;
    int32_t alternate_long;
    int16_t component_0;
    int32_t component_1;
    int16_t component_2;
} FA18SceneFixedPointInput;

/* `$C1D91A-$C1D9D6`: combine the published offset packet with a component
 * triple, then enter the native `$C1D974` magnitude primitive. */
int fa18_run_scene_fixed_point_stage(const FA18SceneFixedPointInput *input,
                                     const FA18SceneMagnitudeTable *table,
                                     int16_t *result,
                                     FA18SceneFixedPointRoute *route);

#endif
