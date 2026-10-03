#ifndef FA18_GLUE_FLIGHT_MOTION_HELPERS_H
#define FA18_GLUE_FLIGHT_MOTION_HELPERS_H
#include "glue.h"
#define MOTION_OWNER(e) int glue_##e(void); int glue_##e##_step(void); int glue_##e##_owns(uint32_t pc);
MOTION_OWNER(C26322)
MOTION_OWNER(C26352)
MOTION_OWNER(C26C72)
MOTION_OWNER(C26CC0)
MOTION_OWNER(C26D8A)
#undef MOTION_OWNER
#endif
