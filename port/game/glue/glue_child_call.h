#ifndef FA18_GLUE_CHILD_CALL_H
#define FA18_GLUE_CHILD_CALL_H
#include <stdint.h>
/* Whole-call CPU/RAM comparison only. Live source timing dispatches children
 * through the runtime between resumable parent instructions. */
int32_t glue_complete_child(uint32_t routine, uint32_t return_pc);
#endif
