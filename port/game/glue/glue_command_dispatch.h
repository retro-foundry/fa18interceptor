#ifndef FA18_GLUE_COMMAND_DISPATCH_H
#define FA18_GLUE_COMMAND_DISPATCH_H
#include <stdint.h>
int glue_C1AC28(void);
int glue_C1AD74(void);
int glue_C1AC28_step(void);
int glue_C1AD74_step(void);
int glue_C1AC28_owns(uint32_t pc);
int glue_C1AD74_owns(uint32_t pc);
#endif
