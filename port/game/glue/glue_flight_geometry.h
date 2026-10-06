#ifndef FA18_GLUE_FLIGHT_GEOMETRY_H
#define FA18_GLUE_FLIGHT_GEOMETRY_H
#include "glue.h"
int glue_C2651E_complete_step(void);
int glue_C2651E_owns(uint32_t pc);
int glue_C26EBE_complete_step(void);
int glue_C26EBE_owns(uint32_t pc);
int glue_C27456_complete_step(void);
int glue_C27456_owns(uint32_t pc);
int glue_complete_record_history(void);
int glue_complete_zone_exit(void);
int glue_complete_candidate_update(void);
int glue_complete_candidate_faces(void);
#endif
