#ifndef FA18_OS_GRAPHICS_WAIT_BOVP_H
#define FA18_OS_GRAPHICS_WAIT_BOVP_H

#include <stdint.h>

int fa18_os_wait_bovp_signature_matches(const uint8_t *rom);
int fa18_os_wait_bovp_step(void);

#endif
