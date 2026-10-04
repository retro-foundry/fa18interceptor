#ifndef FA18_EXEC_MEMORY_ADAPTER_H
#define FA18_EXEC_MEMORY_ADAPTER_H
#include <stdint.h>
int fa18_os_exec_memory_signature_matches(const uint8_t *rom);
int fa18_os_exec_memory_step(void);
int fa18_os_exec_memory_enable_reference(void);
#endif
