#include "scene_renderer_defaults.h"

int fa18_initialize_scene_renderer_defaults(FA18SceneRendererDefaults *defaults) {
    if (!defaults) return -1;
    *defaults = (FA18SceneRendererDefaults){
        {0x1c20, 0}, {0x00a8, 0x00fc, 0x0080}, 0x00a7, 0x0032, 0x0320
    };
    return 0;
}
