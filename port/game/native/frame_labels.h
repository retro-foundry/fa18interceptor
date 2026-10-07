#ifndef FA18_NATIVE_FRAME_LABELS_H
#define FA18_NATIVE_FRAME_LABELS_H
#include <stdint.h>
#include "frontend.h"
#include "../text.h"
/* C2B3C2, called on both C0EFD4 branches before its debug overlays. */
NativeInputReturn native_frame_scene_labels(NativeInputReturn prior);
/* C32A44's shared grid/scene number layout; width is count minus one. */
TextDrawResult native_draw_position_number(int16_t x,int16_t y,int16_t number,uint16_t width);
#endif
