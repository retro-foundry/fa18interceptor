/* Test-only observation bound for original service loops on the frozen-clock
 * whole-call fixture. Budget exhaustion is an explicit failed proof, never a
 * replacement service return. Production bus code is included unchanged. */
#define fa18_bus_instruction fa18_original_bus_instruction
#include "../../port/machine/bus.c"
#undef fa18_bus_instruction
#include <stdio.h>
#include <stdlib.h>
static unsigned fixture_instructions;
static const char *fixture_phase;
void fa18_flight_markers_fixture_begin(const char *phase) {
    fixture_phase=phase;
    fixture_instructions=0;
}
void fa18_bus_instruction(void) {
    fa18_original_bus_instruction();
    if(fixture_phase && ++fixture_instructions>100000u) {
        fprintf(stderr,"flight-markers %s instruction budget exhausted at %06X; no completed proof\n",fixture_phase,REG_PC);
        exit(3);
    }
}
