#ifndef FA18_GLUE_MENU_SETUP_H
#define FA18_GLUE_MENU_SETUP_H
#include <stdint.h>
#define MENU_SETUP_ENTRY(e) int glue_##e(void); int glue_##e##_step(void); int glue_##e##_owns(uint32_t pc);
MENU_SETUP_ENTRY(C0FBE0)
MENU_SETUP_ENTRY(C17B96)
MENU_SETUP_ENTRY(C1082C)
MENU_SETUP_ENTRY(C11BB0)
MENU_SETUP_ENTRY(C24FA4)
#undef MENU_SETUP_ENTRY
#endif
