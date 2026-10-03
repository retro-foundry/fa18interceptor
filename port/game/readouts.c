/* The readout shares the complete source-backed main-loop sample owner. */
#include "readouts.h"

#include "main_loop_timers.h"

void update_readout(void) {
    sample_main_loop_readout(NULL);
}
