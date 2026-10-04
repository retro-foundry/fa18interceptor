#ifndef FA18_EXEC_SUPERVISOR_H
#define FA18_EXEC_SUPERVISOR_H
#include <stdint.h>
int fa18_os_exec_supervisor_signature_matches(const uint8_t *rom);
int fa18_os_exec_supervisor_step(void);
int fa18_os_exec_supervisor_enable_reference(void);
#endif
