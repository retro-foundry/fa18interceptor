#include "fault.h"

#include "globals.h"
#include "memory.h"
#ifdef FA18_NATIVE
#include <stdio.h>
#include <stdlib.h>
#endif

void fault_hook(void) {
}

void fatal_error(uint16_t code) {
#ifdef FA18_NATIVE
    wr_u16(ERROR_CODE, code);
    fprintf(stderr, "native source fatal error: %04X\n", code);
    abort();
#else
    for (;;) {
        wr_u16(ERROR_CODE, code);
        fault_hook();
    }
#endif
}
