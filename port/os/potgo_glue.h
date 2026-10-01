#ifndef FA18_OS_POTGO_GLUE_H
#define FA18_OS_POTGO_GLUE_H

#include <stdint.h>

int fa18_os_potgo_signature_matches(const uint8_t *rom);
int fa18_os_potgo_step(void);

#endif
