#ifndef FA18_NATIVE_VIEWPORT_H
#define FA18_NATIVE_VIEWPORT_H
#include "frontend.h"
/* C17456 installs C1718E as a vertical-blank server (Exec interrupt 5).
 * Its viewport/fade tail runs once per PAL tick, including frame waits. */
void native_viewport_tick(NativeFrontend *game);
#endif
