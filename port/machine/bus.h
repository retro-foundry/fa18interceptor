#ifndef FA18_MACHINE_BUS_H
#define FA18_MACHINE_BUS_H

/* CPU bus timing: DMA contention on the chip bus and CIA E-clock waits (see
 * bus.c). */

#include "machine.h"
#include <stdio.h>

/* Profiling only: origin of actual guest-memory API accesses, including
 * untimed game reads and writes. Diagnostic/loader accesses are host-owned. */
enum { FA18_ENGINE_HOST, FA18_ENGINE_INTERPRETED, FA18_ENGINE_GENERATED,
       FA18_ENGINE_RESIDUAL, FA18_ENGINE_PORT, FA18_ENGINE_OS,
       FA18_ENGINE_CHIPSET, FA18_ENGINE_COUNT };
typedef struct {
    uint64_t reads[FA18_ENGINE_COUNT], writes[FA18_ENGINE_COUNT];
    uint64_t blits, copper_instructions, bitplane_words, cia_events, interrupts;
    uint64_t service_steps, service_entries, port_steps, port_calls;
    uint64_t adapter_instructions;
} FA18EmulationMeter;
extern FA18EmulationMeter fa18_emulation_meter;
extern int fa18_meter_enabled, fa18_meter_engine;
void fa18_meter_start(int enabled);
void fa18_meter_access(uint32_t address, unsigned size, int write);
/* OS bridges which still execute a Musashi opcode count as residual work. */
void fa18_meter_os_opcode(void);
void fa18_meter_write_json(FILE *out, uint64_t frames);

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
/* Source-backed C service phase: preserve bus timing without reading an opcode
 * from the original ROM. `opcode` supplies timing classification only. */
void fa18_bus_begin_instruction(uint32_t pc, uint16_t opcode);
/* The previous instruction has completed and execution continues at
 * `pc`: charge a jump's deferred fetches. Called before chipset service. */
void fa18_bus_finish(uint32_t pc);
/* One CPU word access at `address`: charges its wait. */
void fa18_bus_access(uint32_t address);
/* A program word fetch at `address` (opcode or extension word). */
void fa18_bus_fetch(uint32_t address);
/* Optional source-instruction and chipset-event CSV observations. Configured
 * by FA18_BOUNDARY_TRACE (output path) and FA18_BOUNDARY_RANGE (hex LO-HI).
 * FA18_BOUNDARY_TRACE_MAX_MIB defaults to 1024; zero explicitly disables the
 * safety limit. An oversized partial trace is removed before the run aborts. */
void fa18_bus_trace_boundary(const char *kind, uint32_t source_pc);
int fa18_bus_trace_close(void);
/* CPU cycle of the next access (the beam position the CPU observes). */
int64_t fa18_bus_now(void);

#endif
