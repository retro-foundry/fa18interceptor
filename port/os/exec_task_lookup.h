#ifndef FA18_EXEC_TASK_LOOKUP_H
#define FA18_EXEC_TASK_LOOKUP_H
#include <stdint.h>
int fa18_os_exec_task_lookup_signature_matches(const uint8_t *rom);
int fa18_os_exec_find_task_step(void);
int fa18_os_exec_find_name_step(void);
#endif
