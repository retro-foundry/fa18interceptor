#ifndef FA18_NATIVE_FRAME_TAIL_H
#define FA18_NATIVE_FRAME_TAIL_H
#include "frontend.h"
/* C0F2DC cleanup before timer sampling; C0F386 overlays after the counter. */
NativeInputReturn native_frame_selection_cleanup(NativeInputReturn prior);
/* Numeric drawing supersedes the prior result; an inactive pass preserves it. */
NativeInputReturn native_frame_debug_overlay(NativeInputReturn prior);
#endif
