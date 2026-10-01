#ifndef FA18_OS_GRAPHICS_GLUE_H
#define FA18_OS_GRAPHICS_GLUE_H

#include <stdint.h>

/* Temporary CPU bridge for Kickstart 1.3's VBeamPos ROM leaf. */
int fa18_os_vbeam_signature_matches(const uint8_t *rom);
int fa18_os_vbeam_step(void);

#endif
