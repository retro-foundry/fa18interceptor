#ifndef FA18_NATIVE_SETUP_H
#define FA18_NATIVE_SETUP_H
#include "frontend.h"
#include "../memory.h"
/* Source stage callbacks after the Free Flight code acknowledgement. */
enum NativeSetupStageResult { NATIVE_SETUP_UNHANDLED, NATIVE_SETUP_COMPLETE,
    NATIVE_SETUP_INPUT_PRESERVED };
int native_setup_stage(NativeFrontend *game,gaddr routine);
#endif
