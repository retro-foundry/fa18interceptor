#ifndef FA18_GLUE_HUD_PARENTS_H
#define FA18_GLUE_HUD_PARENTS_H
#include <stdint.h>
#define HP_GLUE(e) int glue_##e(void); int glue_##e##_complete_step(void); int glue_##e##_owns(uint32_t pc)
HP_GLUE(C30764); HP_GLUE(C309B6); HP_GLUE(C30B5C); HP_GLUE(C30D34);
HP_GLUE(C30F78); HP_GLUE(C3112A); HP_GLUE(C31A64); HP_GLUE(C31ACC);
#undef HP_GLUE
#endif
