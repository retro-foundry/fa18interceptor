#ifndef FA18_NATIVE_SETUP_H
#define FA18_NATIVE_SETUP_H
#include "frontend.h"
#include "../memory.h"
/* Source stage callbacks after the Free Flight code acknowledgement. */
int native_setup_stage(NativeFrontend *game,gaddr routine);
#endif
