#ifndef FA18_SCENE_RENDERER_DEFAULTS_H
#define FA18_SCENE_RENDERER_DEFAULTS_H

#include <stdint.h>

typedef struct {
    int16_t matrix_input[2];
    int16_t matrix_row_scale[3];
    int16_t display_bound_y;
    int16_t display_vertical;
    int16_t display_horizontal;
} FA18SceneRendererDefaults;

/* `$C09010-$C09068` cold-boot stores used by the observed flight renderer:
 * `$C45A94/$96`, `$C45984/$86/$88`, and `$C45A3E/$40/$42`. */
int fa18_initialize_scene_renderer_defaults(FA18SceneRendererDefaults *defaults);

#endif
