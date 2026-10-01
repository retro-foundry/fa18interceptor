#ifndef FA18_OS_GRAPHICS_BLITTER_OWNERSHIP_H
#define FA18_OS_GRAPHICS_BLITTER_OWNERSHIP_H

#include <stdint.h>

int fa18_os_blitter_ownership_signature_matches(const uint8_t *rom);
int fa18_os_blitter_ownership_step(void);

#endif
