/* Independent whole-call source-child bridge; live steps use runtime dispatch. */
#include "glue.h"
#include "glue_child_call.h"
#include <stdio.h>
#include <stdlib.h>

extern int64_t fa18_next_event;

int32_t glue_complete_child(uint32_t routine, uint32_t ret) {
    uint32_t sp = A(7);
    int64_t event = fa18_next_event;
    /* This bridge is the whole-call CPU/RAM proof path. Source timing uses
     * separate resumable steps, so children/interrupts retain their runtime
     * dispatch contracts. Hold the child deadline just as the whole-call
     * reference holds it; restore the caller's deadline on return. This is
     * not the live step path. No handwritten glue invokes opcode handlers. */
    fa18_next_event = INT64_MAX;
    m68ki_push_32(ret); REG_PC = routine;
    for (;;) {
        uint32_t before_pc = REG_PC, before_sp = A(7);
        int result;
        if (REG_PC == ret && A(7) == sp) {
            fa18_next_event = event;
            return (int32_t)D(0);
        }
        result = fa18_recomp_call_dynamic();
        if (result == FA18_EXIT_INTERP ||
            (REG_PC == before_pc && A(7) == before_sp)) {
            fprintf(stderr, "whole-call child cannot complete at %06X\n", REG_PC);
            abort();
        }
    }
}
