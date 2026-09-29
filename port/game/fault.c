#include "fault.h"

#include "globals.h"
#include "memory.h"

void fault_hook(void) {
}

void fatal_error(uint16_t code) {
    for (;;) {
        wr_u16(ERROR_CODE, code);
        fault_hook();
    }
}
