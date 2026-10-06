#ifndef FA18_NATIVE_DISPLAY_H
#define FA18_NATIVE_DISPLAY_H
#include "frontend.h"
/* C15D96: select draw page -> C0EFD4 -> C1612C publication. */
void native_display_begin_frame(NativeFrontend *game);
void native_display_finish_frame(NativeFrontend *game);
int native_display_resume(NativeFrontend *game);
void native_display_read_pixels(NativeFrontend *game);
#endif
