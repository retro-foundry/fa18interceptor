#ifndef FA18_NATIVE_SCENE_H
#define FA18_NATIVE_SCENE_H
#include "frontend.h"
/* Projection caller's depth-sort input: masked record Z in ordinary views,
 * final scaled view coefficient in independent views. */
uint32_t native_scene_project(void);
int native_scene_draw(NativeFrontend *game);
#endif
