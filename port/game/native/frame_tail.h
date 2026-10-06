#ifndef FA18_NATIVE_FRAME_TAIL_H
#define FA18_NATIVE_FRAME_TAIL_H
/* C0F2DC cleanup before timer sampling; C0F386 overlays after the counter. */
void native_frame_selection_cleanup(void);
void native_frame_debug_overlay(void);
#endif
