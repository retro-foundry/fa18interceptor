#ifndef FA18_GLUE_CHILD_CALL_H
#define FA18_GLUE_CHILD_CALL_H
#include <stdint.h>
/* Whole-call CPU/RAM comparison only. Live source timing dispatches children
 * through the runtime between resumable parent instructions. */
int32_t glue_complete_child(uint32_t routine, uint32_t return_pc);
/* C0D730/C0DA38 may return directly through the enclosing LINK frame. */
int32_t glue_complete_child_or_frame_exit(uint32_t routine, uint32_t return_pc,
                                         uint32_t exit_pc, uint32_t exit_sp,
                                         int *frame_exited);
#endif
