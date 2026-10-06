/* Renderer state and blit helpers. */
#include "render_state.h"

#include "hardware.h"

void start_blit(uint16_t con0, uint32_t a, uint32_t b, uint32_t cd, uint16_t size) {
    custom_write(BLTCON0, con0);
    custom_write_ptr(BLTAPT, a);
    custom_write_ptr(BLTBPT, b);
    custom_write_ptr(BLTCPT, cd);
    custom_write_ptr(BLTDPT, cd);
    custom_write(BLTSIZE, size);
}

void restart_blit_cd(uint16_t con0, uint32_t cd, uint16_t size) {
    custom_write(BLTCON0, con0);
    custom_write_ptr(BLTCPT, cd);
    custom_write_ptr(BLTDPT, cd);
    custom_write(BLTSIZE, size);
}

void restart_blit_ad(uint16_t con0, uint32_t a, uint32_t d, uint16_t size) {
    custom_write(BLTCON0, con0);
    custom_write_ptr(BLTAPT, a);
    custom_write_ptr(BLTDPT, d);
    custom_write(BLTSIZE, size);
}
