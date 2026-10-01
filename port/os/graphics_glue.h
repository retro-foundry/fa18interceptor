#ifndef FA18_OS_GRAPHICS_GLUE_H
#define FA18_OS_GRAPHICS_GLUE_H

#include <stdint.h>

/* Temporary CPU bridges for Kickstart 1.3 graphics.library ROM leaves. */
int fa18_os_vbeam_signature_matches(const uint8_t *rom);
int fa18_os_vbeam_step(void);
int fa18_os_wait_blit_signature_matches(const uint8_t *rom);
int fa18_os_wait_blit_step(void);

#endif
