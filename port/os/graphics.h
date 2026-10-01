#ifndef FA18_OS_GRAPHICS_H
#define FA18_OS_GRAPHICS_H

#include <stdint.h>

/* Finish Kickstart 1.3 graphics.library VBeamPos after its ASR.L #8. The
 * caller supplies the shifted VPOSR/VHPOSR longword; the ROM keeps 9 bits. */
uint16_t fa18_os_vbeam_row(uint32_t shifted_beam_position);
/* DMACONR's high byte reports BBUSY in bit 6. */
int fa18_os_blitter_busy(uint8_t dmaconr_high);
/* graphics.library's word-sized OwnBlitter/DisownBlitter nesting counter. */
uint16_t fa18_os_own_blitter_depth(uint16_t old_depth);
uint16_t fa18_os_disown_blitter_depth(uint16_t old_depth);

#endif
