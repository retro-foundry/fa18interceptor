#ifndef FA18_GLUE_FLIGHT_DYNAMICS_H
#define FA18_GLUE_FLIGHT_DYNAMICS_H
#include "glue.h"
#define DYNAMICS_OWNER(e) int glue_##e(void); int glue_##e##_step(void); int glue_##e##_owns(uint32_t pc);
DYNAMICS_OWNER(C25B66)
DYNAMICS_OWNER(C266AE)
DYNAMICS_OWNER(C28996)
DYNAMICS_OWNER(C28B16)
#undef DYNAMICS_OWNER
int glue_C28B34_complete_step(void);
int glue_C28B34_owns(uint32_t pc);
int glue_complete_region_dispatch(void);
#endif
