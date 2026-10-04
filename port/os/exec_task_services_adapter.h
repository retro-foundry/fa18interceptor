#ifndef FA18_EXEC_TASK_SERVICES_ADAPTER_H
#define FA18_EXEC_TASK_SERVICES_ADAPTER_H
#include <stdint.h>
int fa18_os_exec_messages_signature_matches(const uint8_t *rom);
int fa18_os_exec_signals_signature_matches(const uint8_t *rom);
int fa18_os_exec_task_protection_signature_matches(const uint8_t *rom);
int fa18_os_exec_messages_step(void);
int fa18_os_exec_signals_step(void);
int fa18_os_exec_task_protection_step(void);
/* Reference-runner activation only. Clean-start profiles enable proven ABI
 * registrations directly and do not inspect ROM signatures. */
int fa18_os_exec_task_services_enable_reference(void);
#endif
