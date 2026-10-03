/* Negative proof only: introduce a live queue-byte difference while retaining
 * all original instruction steps, CPU outputs and custom write ordering. */
#define glue_C1017E_step fa18_original_C1017E_step
#include "../../port/game/glue/glue_menu_cold_step.c"
#undef glue_C1017E_step
#include "memory.h"
#include "globals.h"
int glue_C1017E_step(void) {
    uint32_t pc=REG_PC;
    int result=fa18_original_C1017E_step();
    if(result && pc==0xc1018eu) wr_u16(MESSAGE_QUEUE,rd_u16(MESSAGE_QUEUE)^1u);
    return result;
}
