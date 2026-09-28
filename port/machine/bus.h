#ifndef FA18_MACHINE_BUS_H
#define FA18_MACHINE_BUS_H

/* CPU bus timing: DMA contention on the chip bus and CIA E-clock waits (see
 * bus.c). */

#include "machine.h"

extern int fa18_bus_timing; /* 0 while memory is accessed outside the CPU */

void fa18_bus_reset(void);
void fa18_cpu_timing_init(void);

/* Record the DMA slots of line `vpos`, which starts at CPU cycle `line_start`. */
void fa18_bus_line(FA18Machine *m, int vpos, int64_t line_start);

/* A blit starting at CPU cycle `start`: `words` repetitions of a cycle diagram
 * (1 = the step uses the bus, 0 = idle). Returns the CPU cycle it ends. */
int64_t fa18_bus_blit(int64_t start, const uint8_t *diagram, int steps_per_word, int64_t words);

/* Accesses restart (exception processing, chipset service). */
void fa18_bus_instruction(void);
/* The instruction at `pc` starts; the next access is its opcode fetch. */
void fa18_bus_begin(uint32_t pc);
/* The previous instruction has completed and execution continues at
 * `pc`: charge a jump's deferred fetches. Called before chipset service. */
void fa18_bus_finish(uint32_t pc);
/* One CPU word access at `address`: charges its wait. */
void fa18_bus_access(uint32_t address);
/* A program word fetch at `address` (opcode or extension word). */
void fa18_bus_fetch(uint32_t address);
/* CPU cycle of the next access (the beam position the CPU observes). */
int64_t fa18_bus_now(void);

#endif
