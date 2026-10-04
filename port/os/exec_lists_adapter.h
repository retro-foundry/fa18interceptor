#ifndef FA18_EXEC_LISTS_ADAPTER_H
#define FA18_EXEC_LISTS_ADAPTER_H
#include <stdint.h>
int fa18_os_exec_lists_signature_matches(const uint8_t *rom);
int fa18_os_exec_lists_step(void);
#endif
