#ifndef FA18_EXEC_SCHEDULER_ADAPTER_H
#define FA18_EXEC_SCHEDULER_ADAPTER_H
#include <stdint.h>
int fa18_os_exec_scheduler_signature_matches(const uint8_t *);
int fa18_os_exec_scheduler_enable_reference(void);
int fa18_os_exec_scheduler_step(void);
int fa18_os_exec_scheduler_requires_outer_dispatch(uint32_t pc);
#endif
