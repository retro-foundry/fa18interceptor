#ifndef AMIGA_EXEC_BOOTSTRAP_H
#define AMIGA_EXEC_BOOTSTRAP_H
#include "guest_memory.h"
/* An explicit initial process and verified library-vector profile. No ROM,
 * SDK, captured state, game placements, machine or CPU dependency. This is
 * process handoff construction, not a substitute for device/library startup. */
typedef struct { uint16_t offset; uint32_t target; } AmigaLibraryVector;
typedef struct {
    uint32_t exec_base,task,stack_lower,stack_upper,initial_sp,return_pc;
    uint32_t signal_allocated,trap_allocated,task_name_address;
    uint16_t negative_size,positive_size,version,revision,process_size;
    uint8_t process_signal_bit;
    const char *task_name;
    const AmigaLibraryVector *vectors;
    size_t vector_count;
} AmigaExecBootstrap;
/* Preflight ranges/vectors before modifying guest memory. The caller reserves
 * these regions outside asset allocations. Library vectors are six-byte JMPs;
 * targets are ABI identifiers and must be dispatched before opcode fetching.
 * The initial non-CLI process has no current-directory lock or error window. */
int amiga_exec_bootstrap(const AmigaGuestMemory *,const AmigaExecBootstrap *,
                          char *error,size_t error_size);
#endif
