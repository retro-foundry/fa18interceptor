#ifndef FA18_OS_EXEC_GLUE_H
#define FA18_OS_EXEC_GLUE_H

#include <stdint.h>

int fa18_os_exec_interrupt_signature_matches(const uint8_t *rom);
int fa18_os_exec_interrupt_step(void);
int fa18_os_exec_get_msg_signature_matches(const uint8_t *rom);
int fa18_os_exec_get_msg_step(void);

#endif
