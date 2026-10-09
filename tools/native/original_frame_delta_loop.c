/* Add an observation before the existing trace/input delivery without changing
 * loop counters, dispatch resumption checks, controls or scheduling. */
#define fa18_loop_iteration original_loop_iteration
#include "../../port/recomp/loop_input.c"
#undef fa18_loop_iteration
extern void original_frame_delta_entry(void);
void fa18_loop_iteration(void) {
    original_frame_delta_entry();
    original_loop_iteration();
}
